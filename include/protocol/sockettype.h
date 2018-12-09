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
    uint32_t                     backlog = 0;
    std::mutex              accmu;
    std::condition_variable acccv;

    int fd;

    static bool sockaddr_in_iszero(sockaddr_in x);


public:
    socket_t(socketController& sc);
    socket_t(socketController& sc, sockaddr_in src, sockaddr_in dst);
    int bind(const struct sockaddr_in* address);
    int connect(const sockaddr_in* address);
    int listen(int backlog);
    int recv(const void* buf, int len);
    int accept(sockaddr_in*addr, socklen_t len);
    int close();
    int write(void *buf, size_t nbyte);
    int read(void *buf, size_t nbyte);
    int genConnectFD(sockaddr_in src, sockaddr_in dst, tcpSeq rcv_nxt, tcpSeq irs);
    int getFD();
    int notifyEstFather();
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
    inline bool synQueueIsFull() {
        std::unique_lock<std::mutex> lock(accmu);
        const int syn_backlog = 256; // set as Linux
        return syn_queue.size() >= syn_backlog;
    }
    inline bool estQueueIsFull() {
        std::unique_lock<std::mutex> lock(accmu);
        return syn_queue.size() >= backlog;
    }
    void moveConnFromSynToEst(int fd) {
        std::unique_lock<std::mutex> lock(accmu);
        auto iter = std::find(syn_queue.begin(), syn_queue.end(), fd);
        if (iter == syn_queue.end())
            return;
        syn_queue.erase(iter);
        est_queue.push_back(*iter);
        acccv.notify_all();
    }
    inline void lock() {
        mu.lock();
    }
    inline void unlock() {
        mu.unlock();
    }
};

#endif  // SRC_SOCKET_H
