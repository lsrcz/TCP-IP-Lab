#ifndef SRC_TCPBUFFER_H
#define SRC_TCPBUFFER_H

#include <cstdint>
#include <deque>
#include <mutex>

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
};

#endif  // SRC_TCP_BUFFER_H
