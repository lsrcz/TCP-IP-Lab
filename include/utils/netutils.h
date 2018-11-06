#ifndef NETUTILS_H
#define NETUTILS_H
#include <cstdint>
#include <cstring>
#include <string>

union htonl_helper {
    uint64_t i64;
    uint32_t i32[2];
    uint16_t i16[4];
    uint8_t i8[8];
};

inline
uint16_t htonl16(uint16_t i16) {
    htonl_helper h;
    h.i16[0] = i16;
    uint8_t tmp = h.i8[0];
    h.i8[0] = h.i8[1];
    h.i8[1] = tmp;
    return h.i16[0];
}

inline
uint32_t htonl32(uint32_t i32) {
    htonl_helper h;
    h.i32[0] = i32;
    uint16_t tmp = h.i16[0];
    h.i16[0] = htonl16(h.i16[1]);
    h.i16[1] = htonl16(tmp);
    return h.i32[0];
}

inline
uint64_t htonl64(uint64_t i64) {
    htonl_helper h;
    h.i64 = i64;
    uint32_t tmp = h.i32[0];
    h.i32[0] = htonl32(h.i32[1]);
    h.i32[1] = htonl32(tmp);
    return h.i64;
}

inline
bool isHexPoint(char c) {
    if ('0' <= c && c <= '9')
        return true;
    if ('a' <= c && c <= 'f')
        return true;
    if ('A' <= c && c <= 'F')
        return true;
    return false;
}

inline
uint8_t chartobyte(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

inline
uint8_t stringtobyte(char *buf) {
    // caller check if buf contains a valid number
    return chartobyte(buf[0]) * 16 + chartobyte(buf[1]);
}
struct MAC {
    uint8_t mac[6];
    inline
    bool operator==(const MAC& rhs) const {
        for (int i = 0; i < 6; ++i) {
            if (mac[i] != rhs.mac[i])
                return false;
        }
        return true;
    }
    inline
    bool operator!=(const MAC& rhs) const {
        return !(*this == rhs);
    }
    inline
    bool operator<(const MAC& rhs) const {
        for (int i = 0; i < 6; ++i) {
            if (mac[i] < rhs.mac[i]) {
                return true;
            } else if (mac[i] > rhs.mac[i]) {
                return false;
            }
        }
        return false;
    }
    inline
    std::string toString() const {
        char buf[20];
        snprintf(buf, 20, "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return buf;
    }
};

struct IP {
    in_addr ip;
    in_addr subnet_mask;
    inline
    IP(in_addr ip, in_addr subnet_mask)
        : ip(ip), subnet_mask(subnet_mask) {}
    inline
    IP() {}
};

#endif // NETUTILS_H
