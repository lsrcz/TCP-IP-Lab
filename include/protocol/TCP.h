#ifndef TCP_H
#define TCP_H
#include <deque>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <protocol/TCPSeq.h>
#include <protocol/tcpBuffer.h>
#include <shared_mutex>
#include <sys/socket.h>
#include <thread>
#include <utils/lockutils.h>
#include <utils/timeutils.h>

class socket_t;
class socketController;

class tcb {
    Timer sndTimer;

    friend class socket_t;
    friend class socketController;
    socket_t&               socket;
    int                     state;
    tcpBuffer               rcv_buf;
    tcpBuffer               snd_buf;
    tcpBuffer               snd_ctrl_buf;
    std::mutex              mu;
    std::condition_variable statecv;
    int                     reset = 0;
    int                     activeClose = 0;
    int                     passiveClose = 0;
    int flush = 0;

    sockaddr_in src;
    sockaddr_in dst;

    tcpSeq   snd_una;
    tcpSeq   snd_nxt;
    tcpSeq   snd_wl1;
    tcpSeq   snd_wl2;
    tcpSeq   iss;
    uint64_t snd_wnd;

    tcpSeq   rcv_nxt;
    tcpSeq   irs;
    uint64_t rcv_wnd;

    tcpSeq   seq_fin;

    // sliding window
    std::deque<uint8_t> data;
    std::deque<uint8_t> valid;

    // transmit timing
    float              t_srtt;
    float              t_rto;
    static const float alpha;
    static const float beta;

    tcphdr getHdr(uint8_t flags, uint16_t win);
    int    setMSSOpt(uint8_t* buf);
    int    setTSOpt(uint8_t* buf, uint32_t acktimestamp);
    int    setWSOpt(uint8_t* buf, uint32_t ws);
    int    setEnd(uint8_t* buf);
    int setOption(uint8_t* buf, int flags, uint32_t acktimestamp, uint32_t ws);
    int tcpCopyToBuf(uint8_t* buf, tcphdr t, uint8_t* option, int optlen,
                     uint8_t* data, int len);
    int tcpCopyToBuf(uint8_t* buf, tcphdr t, uint8_t* option, int optlen);
    int tcpCopyToBuf(uint8_t* buf, tcphdr t);
    std::pair<int, int> getWindowSize();
    int                 send(int type, uint32_t seq = 0);
    int                 send(const tcpBufferItem& buf, int type);
    int                 recv(const void* buf, int len);
    void sendCtrlBuf();

public:
    tcb(socket_t& socket);
    int connect();
    int listen();
    int recv(void* buf, int len);
    int close();
    int abort();
};

enum TCPOPT : uint32_t { TO_MSS = 1, TO_TS = 2, TO_WS = 4, TO_SACK = 8 };

struct psu_header {
    in_addr  src;
    in_addr  dst;
    uint8_t  padding;
    uint8_t  protocol;  // should be 6
    uint16_t len;
};

int tcpChksum(const void* ptr, int len, in_addr src, in_addr dst);

#endif  // TCP_H
