#ifndef TCP_H
#define TCP_H
#include <atomic>
#include <deque>
#include <list>
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
    std::thread             worker;
    std::mutex              timermu;
    std::condition_variable timercv;
    bool                    shouldStop = 0;  // only used in destructor

    friend class socket_t;
    friend class socketController;
    socket_t&                        socket;
    int                              state;
    tcpBuffer                        rcv_buf;
    tcpBuffer                        snd_buf;
    tcpBuffer                        snd_ctrl_buf;
    std::mutex                       rtxmu;
    std::condition_variable          rtxcv;
    std::list<tcpBufferWithTimeItem> rtx_buf;
    std::mutex                       mu;
    std::condition_variable          statecv;
    // only set once now
    // no need to protect
    int reset        = 0;
    int activeClose  = 0;
    int passiveClose = 0;
    int flush        = 0;

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
    uint16_t mss;

    // sliding window
    std::mutex                               swmu;
    std::condition_variable_any              swcv;
    tcpSeq                                   bufferBegin;
    std::deque<uint8_t>                      data;
    std::deque<uint8_t>                      valid;
    std::list<std::pair<uint32_t, uint32_t>> hole;
    bool                                     needACK = false;
    std::atomic<bool>                        ackSeen;
    std::atomic<uint64_t>                    lastACKTime;
    uint64_t                                 lastzerowintime;

    // transmit timing
    double              srtt;
    double              rto;
    static const double alpha;
    static const double beta;
    static const double rtoubound;
    static const double rtolbound;

    // congestion control
    tcpSeq           lastack;
    std::atomic<int> ackcounting;
    std::mutex       ccmu;
    double           cwnd;
    uint32_t         ssthresh;

    tcphdr getHdr(uint8_t flags);  //, uint16_t win);
    int    setMSSOpt(uint8_t* buf);
    int    setTSOpt(uint8_t* buf, uint32_t acktimestamp);
    int    setWSOpt(uint8_t* buf, uint32_t ws);
    int    setEnd(uint8_t* buf);
    int setOption(uint8_t* buf, int flags, uint32_t acktimestamp, uint32_t ws);
    int tcpCopyToBuf(uint8_t* buf, tcphdr t, const uint8_t* option, int optlen,
                     const uint8_t* data, int len);
    int tcpCopyToBuf(uint8_t* buf, tcphdr t, uint8_t* option, int optlen);
    int tcpCopyToBuf(uint8_t* buf, tcphdr t);
    uint64_t getWindowSize();
    int      send(int type, uint32_t seq = 0);
    int      send(const void* buf, int len, uint32_t seq);
    int      recv(const void* buf, int len);
    void     sendCtrlBuf();
    void     copyData(const uint8_t* buf, uint32_t len, uint32_t seq);
    void     updateHole(uint32_t seq, uint32_t seq_end);
    int      sendBuf();
    void     processACK(uint32_t ack);

public:
    tcb(socket_t& socket);
    ~tcb();
    int connect();
    int listen();
    int recv(void* buf, int len);
    int close();
    int abort();
    int write(const void* buf, size_t len);
    int read(void* buf, size_t len);
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
