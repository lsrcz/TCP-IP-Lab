#include <cassert>
#include <cstdlib>
#include <cstring>
#include <protocol/tcpBuffer.h>
#include <utils/logutils.h>

tcpBufferItem::tcpBufferItem(const uint8_t* buf, int len) {
    this->buf = new uint8_t[len];
    memcpy(this->buf, buf, len);
    this->len = len;
}

tcpBufferItem::tcpBufferItem(const tcpBufferItem& rhs) {
    buf = new uint8_t[rhs.len];
    memcpy(buf, rhs.buf, rhs.len);
    len = rhs.len;
}

tcpBufferItem::tcpBufferItem(tcpBufferItem&& rhs) {
    buf     = rhs.buf;
    rhs.buf = nullptr;
    len     = rhs.len;
    rhs.len = 0;
}

tcpBufferItem& tcpBufferItem::operator=(const tcpBufferItem& rhs) {
    if (this == &rhs)
        return *this;
    if (buf) {
        delete[] buf;
    }
    buf = new uint8_t[rhs.len];
    memcpy(buf, rhs.buf, rhs.len);
    len = rhs.len;
    return *this;
}

tcpBufferItem& tcpBufferItem::operator=(tcpBufferItem&& rhs) {
    if (this == &rhs)
        return *this;
    if (buf)
        delete[] buf;
    buf     = rhs.buf;
    rhs.buf = nullptr;
    len     = rhs.len;
    rhs.len = 0;
    return *this;
}

tcpBufferItem::~tcpBufferItem() {
    if (buf) {
        delete[] buf;
    }
}

tcpBuffer::tcpBuffer() : len(0) {}

bool tcpBuffer::have() {
    std::lock_guard<std::mutex> lock(mu);
    return len != 0;
}

bool tcpBuffer::have_nolock() {
    return len != 0;
}

tcpBufferItem tcpBuffer::get() {
    std::lock_guard<std::mutex> lock(mu);
    assert(have_nolock());
    len -= buf.front().len;
    tcpBufferItem ret = buf.front();
    buf.pop_front();
    return ret;
}

void tcpBuffer::put(const uint8_t* b, int len) {
    std::lock_guard<std::mutex> lock(mu);
    this->len += len;
    buf.emplace_back(b, len);
}
