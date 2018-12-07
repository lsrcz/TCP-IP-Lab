#ifndef SRC_SOCKET_H
#define SRC_SOCKET_H

#include <list>
#include <mutex>
#include <protocol/TCP.h>
#include <protocol/socketController.h>
#include <set>
#include <condition_variable>

class socketController;
class socket_t {
    friend class socketController;
    std::mutex        mu;
    socketController& sc;
    tcb               t;
    sockaddr_in       src;
    sockaddr_in       dst;
    bool              listenFlag  = false;
    bool              connectFlag = false;
    bool              resetFlag   = false;
    bool              closeFlag   = false;

    /* listen */
    /* connfd */
    socket_t* father = nullptr;
    /* listenfd */
    std::list<int>          syn_queue;
    std::list<int>          est_queue;
    int                     backlog = 0;
    std::mutex              accmu;
    std::condition_variable acccv;

    static bool sockaddr_in_iszero(sockaddr_in x);


public:
    socket_t(socketController& sc);
    socket_t(socketController& sc, sockaddr_in src, sockaddr_in dst);
    int bind(const struct sockaddr_in* address);
    int connect(const sockaddr_in* address);
    int listen(int backlog);
    int accept(struct sockaddr_in* addr);
    int recv(const void* buf, int len);
    int close();
    int genConnectFD(sockaddr_in src, sockaddr_in dst, tcpSeq rcv_nxt, tcpSeq irs);
    inline void setSrc(sockaddr_in x) {
        src = x;
        t.src = x;
    }
    inline void setDst(sockaddr_in x) {
        dst = x;
        t.dst = x;
    }

public:
    inline bool isListen() {
        return listenFlag;
    }
    inline void setListen(bool f) {
        listenFlag = f;
    }
    inline bool isConnect() {
        return connectFlag;
    }
    inline void setConnect(bool f) {
        connectFlag = f;
    }
    inline bool isReset() {
        return resetFlag;
    }
    inline void setReset(int f) {
        resetFlag = f;
    }
    inline bool isClose() {
        return closeFlag;
    }
    inline void setClose(bool f) {
        closeFlag = f;
    }
    inline sockaddr_in getSrc() {
        return src;
    }
    inline sockaddr_in getDst() {
        return dst;
    }
    inline bool haveSrc() {
        return !sockaddr_in_iszero(src);
    }
    inline bool haveDst() {
        return !sockaddr_in_iszero(dst);
    }
};

#endif  // SRC_SOCKET_H
