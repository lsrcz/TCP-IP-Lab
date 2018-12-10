#ifndef SRC_SOCKET_CONTROLLER_H
#define SRC_SOCKET_CONTROLLER_H

#include <list>
#include <map>
#include <protocol/sockettype.h>
#include <set>
#include <memory>

class socket_t;
class socketController {
    bool shouldComputeChecksum = true;
    std::set<sockaddr_in>   listeningSet;
    std::set<sockaddr_in>   connectedSet;
    int                     nxtport = 16384;
    std::list<int>          freeportlist;
    int                     nxtfd = 1024;
    std::list<int>          freelist;
    std::map<int, std::unique_ptr<socket_t>> fd2socket;
    std::mutex closingmu;
    std::condition_variable closingcv;
    std::set<int> closing;
    std::mutex needClosemu;
    std::set<int> needClose;
    std::thread worker;
    socketController();
    bool shouldStop = false; // only true when the destructor is called

public:
    static socketController& getInstance();
    int                      getFreshAddr(sockaddr_in* addr);
    bool                     isConnected(sockaddr_in);
    bool                     isListened(sockaddr_in);
    int                      socket();
    int connect(int socket, const struct sockaddr_in* address);
    int recv(const void* buf, int len);
    int registerSocket(std::unique_ptr<socket_t> &&ptr);
    int bind(int socket, const struct sockaddr_in* addr);
    int listen(int socket, int backlog);
    int close(int fd);
    int accept(int fd, struct sockaddr* address, socklen_t *addrlen);
    int write(int fd, const void *buf, size_t nbyte);
    int read(int fd, void *buf, size_t nbyte);
    bool isSocket(int fd);
    sockaddr_in getSocketPeerAddr(int fd);
    void cleanup();
    ~socketController();
    inline
    void disableChecksum() {
        shouldComputeChecksum = false;
    }
    inline
    void enableChecksum() {
        shouldComputeChecksum = true;
    }
};

int TCPSegmentReceiveCallback(const void* buf, int len);
#endif  // SRC_SOCKET_CONTROLLER_H
