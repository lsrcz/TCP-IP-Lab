#ifndef SRC_TCPBUFFER_H
#define SRC_TCPBUFFER_H

#include <cstdint>
#include <deque>
#include <mutex>
#include <list>
#include <protocol/TCPSeq.h>

struct tcpBufferItem {
    uint8_t* buf;
    int      len;
    tcpBufferItem(const uint8_t* buf, int len);
    tcpBufferItem(const tcpBufferItem& rhs);
    tcpBufferItem(tcpBufferItem&& rhs);
    tcpBufferItem& operator=(const tcpBufferItem& rhs);
    tcpBufferItem& operator=(tcpBufferItem&& rhs);
    ~tcpBufferItem();
};

class tcpBuffer {
    std::deque<tcpBufferItem> buf;
    std::mutex                mu;
    int                       len;
    bool                      have_nolock();

public:
    bool have();
    tcpBuffer();
    tcpBufferItem get();
    void          put(const uint8_t* b, int len);
    void putback(tcpBufferItem i);
};

struct tcpBufferWithTimeItem {
    uint8_t* buf;
    int len;
    tcpSeq seq;
    uint64_t firstSendTime;
    uint64_t lastSendTime;
    int retransCount = 0;
    int p = 1;
    tcpBufferWithTimeItem(const uint8_t *buf, int len, tcpSeq seq, uint64_t sendTime);
    tcpBufferWithTimeItem(const tcpBufferWithTimeItem& rhs);
    tcpBufferWithTimeItem(tcpBufferWithTimeItem&& rhs);
    tcpBufferWithTimeItem& operator=(const tcpBufferWithTimeItem& rhs);
    tcpBufferWithTimeItem& operator=(tcpBufferWithTimeItem&& rhs);
    ~tcpBufferWithTimeItem();
};

class tcpBufferWithTime {
    std::mutex mu;
    bool have_nolock();
 public:
    bool have();
    tcpBufferWithTime();
    void clearBefore(tcpSeq s);
    std::list<tcpBufferWithTimeItem> &raw();
};

#endif  // SRC_TCP_BUFFER_H
