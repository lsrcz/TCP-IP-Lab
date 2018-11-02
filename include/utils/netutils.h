#ifndef NETUTILS_H
#define NETUTILS_H
#include <cstdint>

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
#endif // NETUTILS_H
