#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <protocol/TCP.h>
#include <protocol/ip.h>
#include <protocol/sockettype.h>
#include <utils/netutils.h>
#include <utils/timeutils.h>

const float tcb::alpha = 0.8;
const float tcb::beta  = 1.4;

tcb::tcb(socket_t& socket) : socket(socket) {
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    state   = TCP_CLOSE;
    iss     = getTimeStamp();
    snd_una = snd_nxt = iss;
    t_srtt            = 0;
    t_rto             = 1;

    using namespace std::literals::chrono_literals;
    sndTimer.setPeriodTimer(20ms, [&]() {
            {
                std::unique_lock<std::mutex> lock1(mu);
                printf("28state: %d\n", state);
                if (state == TCP_TIME_WAIT) {
                    while (statecv.wait_for(lock1, 15s) == std::cv_status::no_timeout);
                    state = TCP_CLOSE;
                    socket.setClose(true);
                }
                if (state == TCP_CLOSE || state == TCP_CLOSING)
                    statecv.wait_for(lock1, 10s); // shouldn't spin
                if (state == TCP_FIN_WAIT2) {
                    sendCtrlBuf();
                    statecv.wait_for(lock1, 15s);
                    state = TCP_TIME_WAIT;
                    return;
                }
            }
            printf("41state: %d\n", state);
            if (reset) {
                return;
            } if (activeClose) {
                sendCtrlBuf();
                while (true) {
                    std::unique_lock<std::mutex> lock(mu); // block all
                    /* if (state != TCP_ && state != TCP_CLOSE_WAIT && state != TCP_SYN_RECV)
                       break; */
                    // acts as normal
                    // send piggy back
                    // TODO: send data
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
                                    return;
                                }
                            }
                            return;

                            //} else if (state == TCP_CLOSE_WAIT) {
                        } else if (state == TCP_LAST_ACK) {
                            state = TCP_LAST_ACK;
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
                                    return;
                                }
                            }
                            return;
                        }
                    }
                }
                sendCtrlBuf();
            } else if (passiveClose) {
                std::unique_lock<std::mutex> lock(mu); // block all
                sendCtrlBuf();
                statecv.wait(lock);
            } else {
                sendCtrlBuf();
            }
        });
}

tcphdr tcb::getHdr(uint8_t flags, uint16_t win) {
    tcphdr hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.th_sport = src.sin_port;
    hdr.th_dport = dst.sin_port;
    hdr.th_seq   = htonl32(snd_nxt);
    hdr.th_ack   = htonl32(rcv_nxt);
    hdr.th_off   = 0;  // should be filled by tcp_cp
    hdr.th_flags = flags;
    hdr.th_win   = win;
    hdr.th_urp   = 0;  // won't implement
    return hdr;
}

int tcb::setMSSOpt(uint8_t* buf) {
    buf[0]                  = 0x02;
    buf[1]                  = 0x04;
    *((uint16_t*)(&buf[2])) = htonl16(1460);
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
    if (flags & TO_TS) {
        b += setTSOpt(buf + b, acktimestamp);
    }
    if (flags & TO_WS) {
        b += setWSOpt(buf + b, ws);
    }
    if (flags & TO_SACK) {
        printf("Not implemented error\n");
        exit(0);
    }
    b += setEnd(buf + b);
    for (int i = b; i < (b + 3) / 4 * 4; ++i)
        buf[i] = 0;
    b = (b + 3) / 4 * 4;
    return b;
}

int tcb::tcpCopyToBuf(uint8_t* buf, tcphdr t, uint8_t* option, int optlen,
                      uint8_t* data, int len) {
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

std::pair<int, int> tcb::getWindowSize() {
    snd_wnd = 65535 << 6;
    return std::make_pair(65535, 6);
}

int tcb::send(int seg_type, uint32_t seq /* for rst, network*/) {
    // syn ack rst
    uint8_t buf[200];
    uint8_t optbuf[200];
    auto    ws     = getWindowSize();
    tcphdr hdr;
    if (seg_type == TH_FIN)
        hdr = getHdr(TH_FIN | TH_ACK, ws.first);
    else
        hdr = getHdr(seg_type, ws.first);
    int     optlen = 0;
    int     snd_n  = 0;
    if (seg_type == TH_SYN) {
        hdr.th_ack = 0;
        snd_n      = 1;
        optlen     = setOption(optbuf, TO_MSS | TO_TS, 0, ws.second);
    } else if (seg_type == (TH_SYN | TH_ACK)) {
        snd_n  = 1;
        optlen = setOption(optbuf, TO_MSS | TO_TS, 0, ws.second);
    } else if (seg_type == TH_ACK) {
        optlen = setOption(optbuf, TO_TS, 0, ws.second);
    } else if (seg_type == TH_RST) {
        hdr.th_seq = seq;
        hdr.th_ack = rcv_nxt;
        int totlen = tcpCopyToBuf(buf, hdr);
        return sendIPPacket(src.sin_addr, dst.sin_addr, IPPROTO_TCP, buf,
                            totlen);
    } else if (seg_type == TH_FIN) {
        // not implemented
    } else if (seg_type == (TH_FIN | TH_ACK)) {
        // not implemented
    }
    int totlen = tcpCopyToBuf(buf, hdr, optbuf, optlen);
    if (seg_type != TH_ACK)
        snd_nxt += 1;
    snd_ctrl_buf.put(buf, totlen);
    return 0;
    // return sendIPPacket(src.sin_addr, dst.sin_addr, IPPROTO_TCP, buf, totlen);
}

int tcb::send(const tcpBufferItem& buf, int type) {
    return 1;
}

int tcb::connect() {
    std::unique_lock<std::mutex> lock(mu);
    if (send(TH_SYN) < 0) {
        // no route
        return EAGAIN;
    }
    state = TCP_SYN_SENT;
    int i = 0;
    using namespace std::literals::chrono_literals;
    decltype(1s) gap[] = { 1s, 1s, 1s, 2s, 4s, 8s, 16s, 32s };
    auto*        x     = begin(gap);
    while (state == TCP_SYN_SENT || state == TCP_SYN_RECV) {
        if (x == end(gap))
            return ETIMEDOUT;
        statecv.wait_for(lock, *x);
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

int tcb::listen() {
    std::lock_guard<std::mutex> lock(mu);
    assert(state == TCP_CLOSE);
    state = TCP_LISTEN;
    return 0;
}

int tcb::recv(const void* buf, int len) {
    std::unique_lock<std::mutex> lock(mu);
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
                statecv.notify_all();
                return 0;
            }
        }
        if (hdr->th_flags & TH_RST)
            return 0;
        if (hdr->th_flags & TH_SYN) {
            rcv_nxt = htonl32(hdr->th_seq) + 1;
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
        const uint8_t *data = (const uint8_t *)hdr + doff;
        uint32_t seq = htonl32(hdr->th_seq);
        bool ok = false;
        if (segmentlen == 0) {
            if (rcv_wnd == 0 && seq == rcv_nxt)
                ok = true;
            if (rcv_nxt <= seq && seq < rcv_nxt + rcv_wnd)
                ok = true;
        } else {
            if (rcv_nxt <= seq && seq < rcv_nxt + rcv_wnd)
                ok = true;
            if (rcv_nxt <= seq + segmentlen - 1 && seq + segmentlen - 1 < rcv_nxt + rcv_wnd)
                ok = true;
        }
        if (!ok) {
            send(TH_ACK);
            return 0;
        }
        // second check the RST bit
        if (hdr->th_flags & TH_RST) {
            state = TCP_CLOSE;
            reset = 1;
            socket.setReset(true);
            return 0;
        }

        // fourth, check the SYN bit
        if (hdr->th_flags & TH_SYN) {
            send(TH_RST);
            state = TCP_CLOSE;
            reset = 1;
            socket.setReset(true);
            flush = 1;
            return 0;
        }

        // fifth, check the ACK field
        if (hdr->th_flags & TH_ACK) {
            uint32_t ack = htonl32(hdr->th_ack);
            uint32_t seq = htonl32(hdr->th_seq);
            uint16_t win = htonl16(hdr->th_win);
            if (state == TCP_SYN_RECV) {
                if (snd_una <= ack && ack <= snd_nxt) {
                    state = TCP_ESTABLISHED;
                } else {
                    send(TH_RST, hdr->th_ack);
                    return 0;
                }
            } else if (state == TCP_ESTABLISHED || state == TCP_FIN_WAIT1 || state == TCP_FIN_WAIT2 || state == TCP_CLOSE_WAIT || state == TCP_CLOSING) {
                if (snd_una < ack && ack <= snd_nxt) {
                    snd_una = ack;
                    // TODO: remove unacknowledged
                    // removeUnack();
                    if (snd_wl1 < seq || (snd_wl1 == seq && snd_wl2 <= ack)) {
                        snd_wnd = win;
                        snd_wl1 = seq;
                        snd_wl2 = ack;
                    }
                    if (state == TCP_FIN_WAIT1) {
                        if (ack == seq_fin + 1) {
                            state = TCP_FIN_WAIT2;
                            if (hdr->th_flags & TH_FIN)
                                state = TCP_CLOSING;
                            statecv.notify_all();
                        }
                    }
                    if (state == TCP_CLOSING) {
                        if (ack == seq_fin + 1) {
                            state = TCP_TIME_WAIT;
                            statecv.notify_all();
                        }
                    }
                } else if (ack > snd_nxt) {
                    send(TH_ACK);
                    return 0;
                }
            } else if (state == TCP_LAST_ACK) {
                if (ack == seq_fin) {
                    state = TCP_CLOSE;
                    socket.setClose(true);
                    statecv.notify_all();
                }
            } else if (state == TCP_TIME_WAIT) {
                send(TH_ACK);
                statecv.notify_all();
            }
        } else {
            return 0;
        }

        if (hdr->th_flags & TH_FIN) {
            rcv_nxt += 1;
        }
        // TODO: process the segment text
        if (hdr->th_flags & TH_FIN) {
            if (state == TCP_CLOSE || state == TCP_LISTEN || state == TCP_SYN_SENT)
                return 0;
            if (state == TCP_SYN_RECV || state == TCP_ESTABLISHED) {
                passiveClose = 1;
                send(TH_ACK);
                state = TCP_CLOSE_WAIT;
            } else if (state == TCP_FIN_WAIT1) {
                // before
            } else if (state == TCP_FIN_WAIT2) {
                state = TCP_TIME_WAIT;
            } else if (state == TCP_TIME_WAIT) {
                statecv.notify_all();
            } else {
                // do nothing
            }
        }
    }
    return 0;
}

int tcb::close() {
    std::unique_lock<std::mutex> lock(mu);
    if (state == TCP_CLOSE) {
        return -1;
    } else if (state == TCP_LISTEN || state == TCP_SYN_SENT ) {
        state = TCP_CLOSE;
        socket.setClose(true);
    } else if (state == TCP_SYN_RECV) {
        state = TCP_FIN_WAIT1;
        activeClose = 1;
    } else if (state == TCP_ESTABLISHED) {
        state = TCP_FIN_WAIT1;
        activeClose = 1;
    } else if (state == TCP_FIN_WAIT1 || state == TCP_FIN_WAIT2) {
        return 0;
    } else if (state == TCP_CLOSE_WAIT) {
        state = TCP_LAST_ACK;
        activeClose = 1;
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
