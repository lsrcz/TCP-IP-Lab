#ifndef SRC_SOCKET_CONTROLLER_H
#define SRC_SOCKET_CONTROLLER_H

#include <list>
#include <map>
#include <protocol/sockettype.h>
#include <set>

class socket_t;
class socketController {
    std::set<sockaddr_in>   listeningSet;
    std::set<sockaddr_in>   connectedSet;
    int                     nxtport = 16384;
    std::list<int>          freeportlist;
    int                     nxtfd = 1024;
    std::list<int>          freelist;
    std::map<int, socket_t> fd2socket;

public:
    static socketController& getInstance();
    int                      getFreshAddr(sockaddr_in* addr);
    bool                     isConnected(sockaddr_in);
    bool                     isListened(sockaddr_in);
    int                      socket();
    int connect(int socket, const struct sockaddr_in* address);
    int recv(const void* buf, int len);
};

int TCPSegmentReceiveCallback(const void* buf, int len);
#endif  // SRC_SOCKET_CONTROLLER_H
