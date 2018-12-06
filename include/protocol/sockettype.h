#ifndef SRC_SOCKET_H
#define SRC_SOCKET_H

#include <list>
#include <mutex>
#include <protocol/TCP.h>
#include <protocol/socketController.h>
#include <set>

class socketController;
class socket_t {
    std::mutex        mu;
    socketController& sc;
    tcb               t;
    sockaddr_in       src;
    sockaddr_in       dst;
    bool              listenFlag  = false;
    bool              connectFlag = false;
    bool              resetFlag   = false;

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
    int bind(const struct sockaddr_in* address);
    int connect(const sockaddr_in* address);
    int listen(int backlog);
    int accept(struct sockaddr_in* addr);
    int recv(const void* buf, int len);

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
    inline sockaddr_in getSrc() {
        return src;
    }
    inline sockaddr_in getDst() {
        return dst;
    }
};

#endif  // SRC_SOCKET_H
