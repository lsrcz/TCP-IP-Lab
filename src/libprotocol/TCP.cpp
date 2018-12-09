#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <protocol/TCP.h>
#include <protocol/ip.h>
#include <protocol/sockettype.h>
#include <utils/netutils.h>
#include <utils/timeutils.h>

const double tcb::alpha = 0.8;
const double tcb::beta  = 1.4;
const double tcb::rtoubound = 60000; // 1 min
const double tcb::rtolbound = 1000; // 1 s

tcb::tcb(socket_t& socket) : socket(socket) {
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    state   = TCP_CLOSE;
    iss     = getTimeStamp() * 4;
    snd_una = snd_nxt = iss;
    srtt            = 0;
    rto             = rtolbound;
    rcv_wnd = 0xffff;
    mss = 1460;

    using namespace std::literals::chrono_literals;
    worker = std::thread([&]() {
            std::unique_lock<std::mutex> lock(timermu);
            while (!shouldStop) {
                sendCtrlBuf();
                timercv.wait_for(lock, 2ms);
                // sndTimer.setPeriodTimer(20ms, [&]() {
                sendCtrlBuf();
                {
                    std::unique_lock<std::mutex> lock1(mu);
                    if (state == TCP_TIME_WAIT) {
                        sendCtrlBuf();
                        while (statecv.wait_for(lock1, 240s) == std::cv_status::no_timeout);
                        state = TCP_CLOSE;
                        swcv.notify_all();
                        socket.setClose(true);
                    }
                    if (state == TCP_CLOSE || state == TCP_CLOSING) {
                        sendCtrlBuf();
                        statecv.wait_for(lock1, 1s); // shouldn't spin
                    }
                    if (state == TCP_FIN_WAIT2) {
                        sendCtrlBuf();
                        statecv.wait_for(lock1, 60s);
                        state = TCP_TIME_WAIT;
                        swcv.notify_all();
                        goto endloop;
                    }
                    sendCtrlBuf();
                }
                if (reset) {
                    goto endloop;
                } if (activeClose) {
                    while (true) {
                        std::unique_lock<std::mutex> lock(mu); // block all
                        /* if (state != TCP_ && state != TCP_CLOSE_WAIT && state != TCP_SYN_RECV)
                           break; */
                        // acts as normal
                        // send piggy back
                        sendCtrlBuf();
                        // TODO: send data
                        sendBuf();
                        if (needACK) {
                            printf("  ACKKKK   \n");
                            send(TH_ACK);
                            needACK = false;
                            sendCtrlBuf();
                        }
                        sendCtrlBuf();
                        if (!snd_buf.have()) {
                            printf("53state: %d\n", state);
                            printf("IN\n");
                            //if (state == TCP_ESTABLISHED || state == TCP_SYN_RECV) {
                            if (state == TCP_FIN_WAIT1) {
                                printf("FIN_WAIT1\n");
                                state = TCP_FIN_WAIT1;
                                decltype(1s) wait_time[] = { 1s, 1s, 2s, 4s, 8s };
                                int i = 0;
                                while (state == TCP_FIN_WAIT1) {
                                    seq_fin = snd_nxt;
                                    if (i != 0)
                                        snd_nxt = snd_nxt - 1;
                                    send(TH_FIN);
                                    sendCtrlBuf();
                                    statecv.wait_for(lock, wait_time[i]);
                                    // on an real Linux machine, it may retransmit the FIN, but it can
                                    // also be configured to not retransmit it
                                    // the behavior is not specified in RFC793
                                    // the orphan
                                    i++;
                                    printf("73state: %d\n", state);
                                    if (i == 5) {
                                        state = TCP_CLOSE;
                                        socket.setClose(true);
                                        swcv.notify_all();
                                        goto endloop;
                                    }
                                }
                                goto endloop;

                                //} else if (state == TCP_CLOSE_WAIT) {
                            } else if (state == TCP_LAST_ACK) {
                                state = TCP_LAST_ACK;
                                swcv.notify_all();
                                decltype(1s) wait_time[] = { 1s, 1s, 2s, 4s, 8s };
                                int i = 0;
                                while (state == TCP_LAST_ACK) {
                                    seq_fin = snd_nxt;
                                    if (i != 0)
                                        snd_nxt = snd_nxt - 1;
                                    send(TH_FIN);
                                    sendCtrlBuf();
                                    statecv.wait_for(lock, wait_time[i]);
                                    i++;
                                    if (i == 5) {
                                        state = TCP_CLOSE;
                                        socket.setClose(true);
                                        swcv.notify_all();
                                        goto endloop;
                                    }
                                }
                                goto endloop;
                            }
                        }
                    }
                    sendCtrlBuf();
                } else if (passiveClose) {
                    std::unique_lock<std::mutex> lock(mu); // block all
                    sendCtrlBuf();
                    statecv.wait(lock);
                    sendCtrlBuf();
                    printf("116state: %d\n", state);
                } else {
                    sendCtrlBuf();
                    sendBuf();
                    if (needACK) {
                        printf("  ACKKKK   \n");
                        send(TH_ACK);
                        needACK = false;
                        sendCtrlBuf();
                    }
                }
            endloop:
                int dummy;
            }
        });
}

tcb::~tcb() {
    shouldStop = 1;
    statecv.notify_all();
    timercv.notify_all();
    worker.join();
}

tcphdr tcb::getHdr(uint8_t flags) {
    tcphdr hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.th_sport = src.sin_port;
    hdr.th_dport = dst.sin_port;
    printf("getHdr:   %u\n", (uint32_t)snd_nxt);
    hdr.th_seq   = htonl32(snd_nxt);
    hdr.th_ack   = htonl32(rcv_nxt);
    hdr.th_off   = 0;  // should be filled by tcp_cp
    hdr.th_flags = flags;
    hdr.th_win   = htonl16(rcv_wnd);
    // printf("getHdr:   %u\n", (uint32_t)hdr.th_win)
    hdr.th_urp   = 0;  // won't implement
    return hdr;
}

int tcb::setMSSOpt(uint8_t* buf) {
    buf[0]                  = 0x02;
    buf[1]                  = 0x04;
    *((uint16_t*)(&buf[2])) = htonl16(_GLIBCXX_HAVE_LC_MESSAGES);
    return 4;
}

int tcb::setTSOpt(uint8_t* buf, uint32_t acktimestamp) {
    buf[0]                = 8;
    buf[1]                = 10;
    *(uint32_t*)(buf + 2) = getTimeStamp();
    *(uint32_t*)(buf + 6) = acktimestamp;
    return 10;
}

int tcb::setWSOpt(uint8_t* buf, uint32_t ws) {
    buf[0] = 0x03;
    buf[1] = 0x03;
    buf[2] = ws;
    return 3;
}

int tcb::setEnd(uint8_t* buf) {
    buf[0] = 0x0;
    return 1;
}

// return the number of bytes in the buf
int tcb::setOption(uint8_t* buf, int flags, uint32_t acktimestamp = 0,
                   uint32_t ws = 0) {
    int b = 0;
    if (flags & TO_MSS) {
        b += setMSSOpt(buf + b);
    }
    /*
    if (flags & TO_TS) {
        b += setTSOpt(buf + b, acktimestamp);
    }
    if (flags & TO_WS) {
        b += setWSOpt(buf + b, ws);
    }
    if (flags & TO_SACK) {
        printf("Not implemented error\n");
        exit(0);
        }*/
    // no other options supported
    if (b != 0)
        b += setEnd(buf + b);
    for (int i = b; i < (b + 3) / 4 * 4; ++i)
        buf[i] = 0;
    b = (b + 3) / 4 * 4;
    return b;
}

int tcb::tcpCopyToBuf(uint8_t* buf, tcphdr t, const uint8_t* option, int optlen,
                      const uint8_t* data, int len) {
    tcphdr* hdr        = (tcphdr*)buf;
    int     tot_optlen = (optlen + 3) / 4 * 4;
    int     tot_len    = sizeof(t) + tot_optlen + len;
    t.th_off           = tot_optlen / 4 + 5;
    memset(buf, 0, tot_len + 1);  // +1 for chksum
    memcpy(buf, &t, sizeof(t));
    if (optlen != 0)
        memcpy(buf + sizeof(tcphdr), option, optlen);
    if (len != 0)
        memcpy(buf + sizeof(tcphdr) + tot_optlen, data, len);
    hdr->th_sum = htonl16(tcpChksum(buf, tot_len, src.sin_addr, dst.sin_addr));
    return tot_len;
}

int tcb::tcpCopyToBuf(uint8_t* buf, tcphdr t, uint8_t* option, int optlen) {
    return tcpCopyToBuf(buf, t, option, optlen, NULL, 0);
}

int tcb::tcpCopyToBuf(uint8_t* buf, tcphdr t) {
    return tcpCopyToBuf(buf, t, NULL, 0);
}

uint64_t tcb::getWindowSize() {
    // snd_wnd = 65535 << 6;
    // return std::make_pair(65535, 6);
    return rcv_wnd;
}

int tcb::send(int seg_type, uint32_t seq /* for rst, network*/) {
    // syn ack rst
    uint8_t buf[200];
    uint8_t optbuf[200];
    // auto    ws     = getWindowSize();
    tcphdr hdr;
    if (seg_type == TH_FIN)
        hdr = getHdr(TH_FIN | TH_ACK);
    // hdr = getHdr(TH_FIN | TH_ACK, ws);
    else
        hdr = getHdr(seg_type);
    //hdr = getHdr(seg_type, ws);
    int     optlen = 0;
    int     snd_n  = 0;
    if (seg_type == TH_SYN) {
        hdr.th_ack = 0;
        snd_n      = 1;
        optlen     = setOption(optbuf, TO_MSS | TO_TS, 0, 0);
    } else if (seg_type == (TH_SYN | TH_ACK)) {
        snd_n  = 1;
        optlen = setOption(optbuf, TO_MSS | TO_TS, 0, 0);
    } else if (seg_type == TH_ACK) {
        optlen = setOption(optbuf, TO_TS, 0, 0);
    } else if (seg_type == TH_RST) {
        hdr.th_seq = seq;
        hdr.th_ack = rcv_nxt;
        int totlen = tcpCopyToBuf(buf, hdr);
        return sendIPPacket(src.sin_addr, dst.sin_addr, IPPROTO_TCP, buf,
                            totlen);
    } else if (seg_type == TH_FIN) {
        snd_n = 1;
    } else if (seg_type == (TH_FIN | TH_ACK)) {
        snd_n = 1;
    }
    int totlen = tcpCopyToBuf(buf, hdr, optbuf, optlen);
    snd_nxt += snd_n;
    snd_ctrl_buf.put(buf, totlen);
    timercv.notify_all();
    return 0;
    // return sendIPPacket(src.sin_addr, dst.sin_addr, IPPROTO_TCP, buf, totlen);
}

int tcb::send(const void* buf, int len, uint32_t seq /* not network */) {
    uint8_t sendBuf[2000]; // always enough since we won't send TCP segments longer
    // than MSS, and MSS is limited.
    uint8_t optbuf[200];
    tcphdr hdr = getHdr(TH_ACK);
    hdr.th_seq = htonl32(seq);
    int optlen = setOption(optbuf, TO_TS, 0, 0);
    int totlen = tcpCopyToBuf(sendBuf, hdr, optbuf, optlen, (uint8_t*)buf, len);
    return sendIPPacket(src.sin_addr, dst.sin_addr, IPPROTO_TCP, sendBuf, totlen);
}

int tcb::sendBuf() {
    uint64_t lastchk = 0;
    uint64_t newchk;
    while (snd_buf.have()) {
        std::unique_lock<std::mutex> lock(rtxmu);
        newchk = getTimeStamp();
        if (newchk <= lastchk + 2 * srtt) {// if more than a srtt, recheck rtx buf
            lastchk = newchk;
            goto skip;
        }
        for (auto & b : rtx_buf) {
            //if (snd_wnd == 0)
            //    printf("zero window\n");
            uint64_t t = getTimeStamp();
            if (b.lastSendTime + rto * b.p < t) {
                // retransmission
                b.lastSendTime = t;
                b.retransCount++;
                b.p *= 2;
                send(b.buf, b.len, b.seq);
            }
        }
    skip:
        if (snd_nxt >= snd_una + snd_wnd) {
            if (snd_wnd == 0) {
            static bool a = true;
            if (a) {
                printf("snd_nxt: %d\n", (uint32_t)snd_nxt);
                printf("snd_una: %d\n", (uint32_t)snd_una);
                printf("snd_wnd: %ld\n", snd_wnd);
                a = false;
            }
            }
            return 0;
        }
        printf("ok to send\n");
        printf("snd_nxt: %d\n", (uint32_t)snd_nxt);
        printf("snd_una: %d\n", (uint32_t)snd_una);
        printf("snd_wnd: %ld\n", snd_wnd);
        tcpBufferItem i = snd_buf.get();
        rtx_buf.emplace_back(i.buf, i.len, snd_nxt, getTimeStamp());
        send(i.buf, i.len, snd_nxt);
        snd_nxt = snd_nxt + i.len;
        needACK = false;
    }
    return 0;
}

int tcb::connect() {
    std::unique_lock<std::mutex> lock(mu);
    if (send(TH_SYN) < 0) {
        // no route
        return EAGAIN;
    }
    state = TCP_SYN_SENT;
    statecv.notify_all();
    int i = 0;
    using namespace std::literals::chrono_literals;
    decltype(1s) gap[] = { 1s, 1s, 1s, 2s, 4s, 8s, 16s, 32s };
    auto*        x     = begin(gap);
    while (state == TCP_SYN_SENT || state == TCP_SYN_RECV) {
        if (x == end(gap))
            return ETIMEDOUT;
        statecv.wait_for(lock, *x, [&](){
                return state != TCP_SYN_SENT && state != TCP_SYN_RECV;
            });
        if (state != TCP_SYN_SENT && state != TCP_SYN_RECV)
            break;
        printf("                            %ld                   \n", x - begin(gap));
        snd_nxt = snd_nxt - 1;
        printf("SYN retransmission\n");
        send(TH_SYN);
        statecv.notify_all();
        x++;
    }
    if (state == TCP_CLOSE) {
        if (reset == 1) {
            return ECONNRESET;
        } else {
            assert(false);
        }
    }
    statecv.notify_all();
    return 0;
}

int tcb::write(const void *buf, size_t nbyte) {
    std::lock_guard<std::mutex> lock(mu);
    // TODO: sent no mss know
    if (state == TCP_SYN_SENT ||
               state == TCP_SYN_RECV ||
               state == TCP_ESTABLISHED ||
               state == TCP_CLOSE_WAIT) {
        size_t pos = 0;
        for (; pos + mss <= nbyte; pos += mss) {
            snd_buf.put((uint8_t*)buf + pos, mss);
        }
        if (pos + mss == nbyte)
            return 0;
        snd_buf.put((uint8_t*)buf + pos, nbyte - pos);
        return 0;
    } else {
        errno = ENOTCONN;
        return -1;
    }
}

int tcb::read(void *buf, size_t nbyte) {
    // TODO: fin case not handled
    std::unique_lock<std::mutex> lock1(swmu);
    if (state == TCP_CLOSE ||
        state == TCP_CLOSING ||
        state == TCP_LAST_ACK ||
        state == TCP_TIME_WAIT) {
        errno = ENOTCONN;
        return -1;
    }
    if (state == TCP_CLOSE_WAIT)
        goto closewait;
    if ((uint32_t)rcv_nxt - bufferBegin >= nbyte) {
        // ok to read
        goto ok;
    } else {
        swcv.wait(lock1, [&](){
                if (state == TCP_CLOSE ||
                    state == TCP_CLOSE_WAIT ||
                    state == TCP_CLOSING ||
                    state == TCP_LAST_ACK ||
                    state == TCP_TIME_WAIT)
                    return true;
                return (uint32_t)rcv_nxt - bufferBegin >= nbyte;
            });
        if (state == TCP_CLOSE_WAIT) {
            goto closewait;
        }
        if (state == TCP_CLOSING || state == TCP_LAST_ACK || state == TCP_TIME_WAIT) {
            errno = ENOTCONN;
            return -1;
        }
        goto ok;
    }
 closewait:
    if (bufferBegin == rcv_nxt - 1) {
        errno = ENOTCONN;
        return -1;
    }
    if ((uint32_t)rcv_nxt - 1 - bufferBegin <= nbyte)
        nbyte = rcv_nxt - 1 - bufferBegin;
 ok:
    std::copy_n(data.begin(), nbyte, (uint8_t*)buf);
    for (size_t i = 0; i < nbyte; ++i) {
        data.pop_front();
        valid.pop_front();
    }
    bufferBegin = bufferBegin + nbyte;
    rcv_wnd = rcv_wnd + nbyte;
    return nbyte;
}

int tcb::listen() {
    std::lock_guard<std::mutex> lock(mu);
    assert(state == TCP_CLOSE);
    state = TCP_LISTEN;
    return 0;
}

int tcb::recv(const void* buf, int len) {
    std::unique_lock<std::mutex> lock(mu);
    std::lock_guard<std::mutex> lock1(swmu);
    printf("state: %d\n", state);
    const ip*                   iphdr = (const ip*)buf;
    uint8_t                     iphl  = iphdr->ip_hl;
    const tcphdr*               hdr = (const tcphdr*)((uint8_t*)buf + iphl * 4);
    if (state == TCP_CLOSE) {
        if (hdr->th_flags & TH_RST)
            return 0;
        return send(TH_RST, hdr->ack);
    } else if (state == TCP_SYN_SENT) {
        if (hdr->th_flags & TH_ACK) {
            tcpSeq ack = htonl32(hdr->th_ack);
            if (ack <= iss || ack > snd_nxt) {
                if (hdr->th_flags & TH_RST)
                    return 0;
                return send(TH_RST, hdr->th_ack);
            }
            if (snd_una > ack || ack > snd_nxt)
                return 0;
            if (hdr->th_flags & TH_RST) {
                state = TCP_CLOSE;
                socket.setReset(true);
                reset = 1;
                swcv.notify_all();
                statecv.notify_all();
                return 0;
            }
        }
        if (hdr->th_flags & TH_RST)
            return 0;
        if (hdr->th_flags & TH_SYN) {
            rcv_nxt = htonl32(hdr->th_seq) + 1;
            bufferBegin = rcv_nxt;
            irs     = htonl32(hdr->th_seq);
            if (hdr->th_flags & TH_ACK)
                snd_una = htonl32(hdr->th_ack);
            if (snd_una > iss) {
                state = TCP_ESTABLISHED;
                send(TH_ACK);
            } else {
                state = TCP_SYN_RECV;
                send(TH_ACK);
            }
            if (hdr->th_flags & TH_ACK) {
                snd_wnd = htonl16(hdr->th_win);
                snd_wl1 = htonl32(hdr->th_seq);
                snd_wl2 = htonl32(hdr->th_ack);
            }
        }
    } else if (state == TCP_LISTEN) {
        // for listenfd
        // TODO: new socket
        // TODO: add to queue
        if (hdr->th_flags & TH_RST)
            return 0;
        if (hdr->th_flags & TH_ACK)
            return send(TH_RST, hdr->th_ack);
        if (hdr->th_flags & TH_SYN) {
            // rcv_nxt = htonl32(hdr->th_seq) + 1;
            // irs     = htonl32(hdr->th_seq);
            // TODO: RFC 793, p66: any other control or text should be queued for processing later
            if (socket.synQueueIsFull())
                return 0;
            sockaddr_in src;
            sockaddr_in dst;
            memset(&src, 0, sizeof(src));
            memset(&src, 0, sizeof(dst));
            src.sin_family = AF_INET;
            src.sin_addr = iphdr->ip_src;
            src.sin_port = hdr->th_sport;
            dst.sin_family = AF_INET;
            dst.sin_addr = iphdr->ip_dst;
            dst.sin_port = hdr->th_dport;
            int fd = socket.genConnectFD(dst, src, htonl32(hdr->th_seq) + 1, htonl32(hdr->th_seq));
            return fd;
            // TODO
        }
        // TODO
    } else {
        int doff = hdr->th_off * 4;
        int segmentlen = len - iphl * 4 - doff;
        if (hdr->th_flags & (TH_FIN | TH_SYN))
            segmentlen += 1;
        const uint8_t *data = (const uint8_t *)hdr + doff;
        uint32_t seq = htonl32(hdr->th_seq);
        bool ok = false;
        if (segmentlen == 0) {
            if (rcv_wnd == 0 && seq == rcv_nxt)
                ok = true;
            if (rcv_nxt <= seq && seq < rcv_nxt + rcv_wnd)
                ok = true;
        } else {
            if (rcv_nxt <= seq + segmentlen - 1 && seq + segmentlen - 1 < rcv_nxt + rcv_wnd)
                ok = true;
        }
        if (!ok) {
            printf("%u\n", (uint32_t)rcv_nxt);
            printf("%u\n", seq);
            printf("%u\n", (uint32_t)rcv_wnd);
            printf("not ok");
            send(TH_ACK);
            return 0;
        }
        printf("%u\n", (uint32_t)rcv_nxt);
        printf("%u\n", seq);
        printf("%u\n", (uint32_t)rcv_wnd);
        printf("ok");
        // second check the RST bit
        if (hdr->th_flags & TH_RST) {
            state = TCP_CLOSE;
            reset = 1;
            socket.setReset(true);
            swcv.notify_all();
            return 0;
        }

        // fourth, check the SYN bit
        if (hdr->th_flags & TH_SYN) {
            send(TH_RST);
            state = TCP_CLOSE;
            reset = 1;
            socket.setReset(true);
            flush = 1;
            swcv.notify_all();
            return 0;
        }

        // fifth, check the ACK field
        if (hdr->th_flags & TH_ACK) {
            uint32_t ack = htonl32(hdr->th_ack);
            printf("ACK %u recv \n", ack);
            uint32_t seq = htonl32(hdr->th_seq);
            uint16_t win = htonl16(hdr->th_win);
            if (state == TCP_SYN_RECV) {
                if (snd_una <= ack && ack <= snd_nxt) {
                    if (hdr->th_flags & TH_ACK) {
                        snd_wnd = htonl16(hdr->th_win);
                        snd_wl1 = htonl32(hdr->th_seq);
                        snd_wl2 = htonl32(hdr->th_ack);
                    }
                    if (socket.notifyEstFather() < 0)
                        return 0;
                    state = TCP_ESTABLISHED;
                } else {
                    send(TH_RST, hdr->th_ack);
                    return 0;
                }
            } else if (state == TCP_ESTABLISHED || state == TCP_FIN_WAIT1 || state == TCP_FIN_WAIT2 || state == TCP_CLOSE_WAIT || state == TCP_CLOSING) {
                printf("ACK %u recv \n", ack);
                if (snd_una <= ack && ack <= snd_nxt) {
                    // TODO: remove unacknowledged
                    // removeUnack();
                    processACK(ack);
                    if (snd_wl1 < seq || (snd_wl1 == seq && snd_wl2 <= ack)) {
                        printf("Window update: %d\n", win);
                        snd_wnd = win;
                        snd_wl1 = seq;
                        snd_wl2 = ack;
                    }
                    if (state == TCP_FIN_WAIT1) {
                        if (ack == seq_fin + 1) {
                            state = TCP_FIN_WAIT2;
                            if (hdr->th_flags & TH_FIN) {
                                state = TCP_CLOSING;
                                swcv.notify_all();
                            }
                            statecv.notify_all();
                        }
                    }
                    if (state == TCP_CLOSING) {
                        if (ack == seq_fin + 1) {
                            state = TCP_TIME_WAIT;
                            swcv.notify_all();
                            statecv.notify_all();
                        }
                    }
                } else if (ack > snd_nxt) {
                    send(TH_ACK);
                    return 0;
                }
            } else if (state == TCP_LAST_ACK) {
                if (ack == seq_fin + 1) {
                    state = TCP_CLOSE;
                    socket.setClose(true);
                    swcv.notify_all();
                    statecv.notify_all();
                }
            } else if (state == TCP_TIME_WAIT) {
                send(TH_ACK);
                statecv.notify_all();
            }
        } else {
            return 0;
        }


        printf("segmentlen: %u\n", segmentlen);
        // process the segment text
        if (state == TCP_ESTABLISHED ||
            state == TCP_FIN_WAIT1 ||
            state == TCP_FIN_WAIT2) {
            if (segmentlen != 0 && !(hdr->th_flags & TH_FIN)) {
                copyData(data, segmentlen, seq);
                needACK = true;
                statecv.notify_all();
                timercv.notify_all();
            }
        }
        if (hdr->th_flags & TH_FIN) {
            rcv_nxt += 1;
        }


        if (hdr->th_flags & TH_FIN) {
            if (state == TCP_CLOSE || state == TCP_LISTEN || state == TCP_SYN_SENT)
                return 0;
            if (state == TCP_SYN_RECV || state == TCP_ESTABLISHED) {
                printf("FIN received\n");
                passiveClose = 1;
                send(TH_ACK);
                state = TCP_CLOSE_WAIT;
                swcv.notify_all();
                statecv.notify_all();
            } else if (state == TCP_FIN_WAIT1) {
                // before
            } else if (state == TCP_FIN_WAIT2) {
                printf("---should ack---");
                send(TH_ACK);
                state = TCP_TIME_WAIT;
                swcv.notify_all();
                statecv.notify_all();
            } else if (state == TCP_TIME_WAIT) {
                statecv.notify_all();
            } else {
                // do nothing
            }
        }
        printf("finish\n");
    }
    return 0;
}

void tcb::updateHole(uint32_t seq, uint32_t seq_end) {
    if (hole.empty())
        return;
    auto iter = hole.begin();
    auto iternxt = std::next(iter);
    while (true) {
        auto &h = *iter;
        if (h.second <= seq)
            goto nextiter;
        if (h.first >= seq_end)
            break;
        if (h.first < seq) {
            if (h.second <= seq_end) {
                h.second = seq;
            } else if (h.second > seq_end) {
                hole.insert(iternxt, std::make_pair(seq_end, h.second));
                h.second = seq;
            }
        } else {
            if (h.second <= seq_end) {
                hole.erase(iter);
            } else if (h.second > seq_end) {
                h.first = seq_end;
            }
        }
    nextiter:
        iter = iternxt;
        if (iter == hole.end())
            break;
        iternxt = std::next(iter);
    }
}

void tcb::copyData(const uint8_t* buf, uint32_t len, uint32_t seq) {
    assert(len != 1);
    uint32_t seq_end = seq + len;
    uint32_t bbegin = bufferBegin;
    uint32_t bend = bufferBegin + data.size();
    uint32_t seqrel = seq - bbegin;
    uint32_t seq_endrel = seq_end - bbegin;
    updateHole(seq, seq_end);
    if (seq_end <= bend) {
        std::copy_n(buf, len, data.begin() + seqrel);
        std::fill_n(valid.begin() + seqrel, len, 1);
    } else if (seq >= bend) {
        auto dataiter = std::back_inserter(data);
        auto validiter = std::back_inserter(valid);
        std::fill_n(dataiter, seq - bend, 0);
        std::fill_n(validiter, seq - bend, 0);
        std::copy_n(buf, len, dataiter);
        std::fill_n(validiter, len, 1);
        if (hole.empty() || hole.back().second != bend) {
            if (bend != seq)
                hole.push_back(std::make_pair(bend, seq));
        } else {
            hole.back().second = seq;
        }
    } else if (seq < bend && seq_end > bend) {
        std::copy_n(buf, bend - seq, data.begin() + seqrel);
        std::fill_n(valid.begin() + seq, bend - seq, 1);
        auto dataiter = std::back_inserter(data);
        auto validiter = std::back_inserter(valid);
        std::copy_n(buf + bend - seq, seq_end - bend, dataiter);
        std::fill_n(validiter, seq_end - bend, 1);
    }
    uint32_t old_rcv_nxt = rcv_nxt;
    printf("Holes:\n");
    for (auto &x : hole) {
        printf("%u,%u\n", x.first, x.second);
    }
    if (hole.empty()) {
        printf("%u, ", (uint32_t)rcv_nxt);
        printf("%u, %lu\n", (uint32_t)bufferBegin, data.size());
        rcv_nxt = bufferBegin + data.size();
    } else {
        rcv_nxt = hole.front().first;
        printf("rcv_nxt: %u\n", (uint32_t)rcv_nxt);
    }
    if (rcv_nxt != old_rcv_nxt)
        swcv.notify_all();
    rcv_wnd = rcv_wnd + old_rcv_nxt - rcv_nxt;
}

int tcb::close() {
    std::unique_lock<std::mutex> lock(mu);
    if (state == TCP_CLOSE) {
        return -1;
    } else if (state == TCP_LISTEN || state == TCP_SYN_SENT ) {
        state = TCP_CLOSE;
        swcv.notify_all();
        socket.setClose(true);
    } else if (state == TCP_SYN_RECV) {
        state = TCP_FIN_WAIT1;
        activeClose = 1;
        statecv.notify_all();
    } else if (state == TCP_ESTABLISHED) {
        state = TCP_FIN_WAIT1;
        activeClose = 1;
        statecv.notify_all();
    } else if (state == TCP_FIN_WAIT1 || state == TCP_FIN_WAIT2) {
        return 0;
    } else if (state == TCP_CLOSE_WAIT) {
        state = TCP_LAST_ACK;
        swcv.notify_all();
        activeClose = 1;
        statecv.notify_all();
    } else {
        return -1;
    }
    return 0;
}

int tcpChksum(const void* ptr, int len, in_addr src, in_addr dst) {
    uint8_t     buf[len + sizeof(psu_header) + 10];
    psu_header* h = (psu_header*)buf;
    h->src        = src;
    h->dst        = dst;
    h->padding    = 0;
    h->protocol   = 6;
    h->len        = htonl16(len);
    memcpy(buf + sizeof(psu_header), ptr, len);
    if (len % 2 == 1) {
        buf[sizeof(psu_header) + len] = 0;
        return chksum(buf, len + sizeof(psu_header) + 1);
    }
    return chksum(buf, len + sizeof(psu_header));
}

void tcb::sendCtrlBuf() {
    while (snd_ctrl_buf.have()) {
        auto bi = snd_ctrl_buf.get();
        sendIPPacket(src.sin_addr, dst.sin_addr, IPPROTO_TCP, bi.buf, bi.len);
    }
}

void tcb::processACK(uint32_t ack) {
    printf("ACK %u recv \n", ack);
    std::lock_guard<std::mutex> lock(rtxmu);
    snd_una = ack;
    if (rtx_buf.empty())
        return;
    printf("ACK %u recv \n", ack);
    auto iter = rtx_buf.begin();
    auto iternxt = std::next(iter);
    bool lasttime = false;
    while (true) {
        if (iternxt == rtx_buf.end())
            lasttime = true;
        tcpBufferWithTimeItem &t = *iter;
        printf("p: %u, %d, %d\n", (uint32_t)t.seq, t.len, ack);
        if (t.seq + t.len == ack) {
            /// TODO: cal RTT
            uint64_t rtt = getTimeStamp() - t.firstSendTime;
            srtt = (alpha * srtt) + (1 - alpha * rtt);
            rto = std::min(rtoubound, std::max(rtolbound, beta * srtt));
            printf("remove: %u, %d, %d\n", (uint32_t)t.seq, t.len, ack);
            rtx_buf.erase(iter);
        } else if (t.seq + t.len <= ack) {
            printf("remove: %u, %d, %d\n", (uint32_t)t.seq, t.len, ack);
            rtx_buf.erase(iter);
        }
        if (lasttime)
            break;
        iter = iternxt;
        iternxt = std::next(iter);
    }

    rtxcv.notify_all();
}
