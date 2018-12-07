#ifndef TCP_SEQ_H
#define TCP_SEQ_H

class tcpSeq {
    uint32_t base;

public:
    inline tcpSeq(uint32_t base) : base(base) {}
    inline tcpSeq() : base(0) {}
    /*inline bool operator<(const tcpSeq& rhs) {
        return (int)(base - rhs.base) < 0;
    }
    inline bool operator==(const tcpSeq& rhs) {
        return (int)(base - rhs.base) == 0;
    }
    inline bool operator<=(const tcpSeq& rhs) {
        return (*this < rhs) || (*this == rhs);
    }
    inline bool operator>(const tcpSeq& rhs) {
        return !(*this <= rhs);
    }
    inline bool operator>=(const tcpSeq& rhs) {
        return !(*this < rhs);
    }
    inline bool operator!=(const tcpSeq& rhs) {
        return !(*this == rhs);
        }*/
    inline tcpSeq& operator+=(uint32_t p) {
        base += p;
        return *this;
    }
    inline operator uint32_t() {
        return (uint32_t)base;
    }
};

#endif  // TCP_SEQ_H
