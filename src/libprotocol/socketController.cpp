#include <netinet/tcp.h>
#include <protocol/ip.h>
#include <protocol/socketController.h>
#include <utils/netutils.h>

socketController& socketController::getInstance() {
    static socketController* sc = nullptr;
    if (sc == nullptr) {
        sc = new socketController();
    }
    return *sc;
}

int socketController::socket() {
    int fd = 0;
    if (freelist.empty()) {
        fd = nxtfd++;
    }
    auto ret = fd2socket.try_emplace(fd, std::make_unique<socket_t>(*this));
    if (!ret.second) {
        nxtfd--;
        errno = ENOMEM;
        return -1;
    }
    return fd;
}

int socketController::connect(int socket, const struct sockaddr_in* address) {
    auto iter = fd2socket.find(socket);
    if (iter == fd2socket.end()) {
        errno = ENOTSOCK;
        return -1;
    }
    return iter->second->connect(address);
}

bool socketController::isListened(sockaddr_in s) {
    return listeningSet.find(s) != listeningSet.end();
}

bool socketController::isConnected(sockaddr_in s) {
    return connectedSet.find(s) != connectedSet.end();
}

int socketController::getFreshAddr(sockaddr_in* addr) {
    sockaddr_in s;
    memset(&s, 0, sizeof(s));
    s.sin_family = AF_INET;
    if (getSomeIP(&(s.sin_addr)) < 0)
        return -1;
    s.sin_port = nxtport;
    nxtport++;
    if (!(isListened(s) || isConnected(s)))
        goto ok;
    while (isListened(s) || isConnected(s)) {
        s.sin_port = nxtport;
        nxtport++;
    }
ok:
    *addr = s;
    return 0;
}

int socketController::recv(const void* buf, int len) {
    const ip*     iphdr = (const ip*)buf;
    uint8_t       iphl  = iphdr->ip_hl;
    const tcphdr* tcph  = (const tcphdr*)((uint8_t*)buf + iphl * 4);
    sockaddr_in   src, dst;
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
    src.sin_addr = iphdr->ip_src;
    src.sin_port = tcph->th_sport;
    dst.sin_addr = iphdr->ip_dst;
    dst.sin_port = tcph->th_dport;
    src.sin_family = AF_INET;
    dst.sin_family = AF_INET;
    if (tcpChksum(tcph, len - iphl * 4, src.sin_addr, dst.sin_addr) != 0) {
        return -1;
    }
    socket_t* listen_ptr = nullptr;
    for (auto& p : fd2socket) {
        auto s = p.second.get();
        if (dst == s->getSrc()) {
            if (src == s->getDst()) {
                return s->recv(buf, len);
            }
            if (s->isListen()) {
                listen_ptr = s;
            }
        }
    }
    if (listen_ptr != nullptr)
        return listen_ptr->recv(buf, len);
    return 0;
}

int TCPSegmentReceiveCallback(const void* buf, int len) {
    return socketController::getInstance().recv(buf, len);
}

int socketController::registerSocket(std::unique_ptr<socket_t> &&ptr) {
    int fd = 0;
    if (freelist.empty()) {
        fd = nxtfd++;
    }
    auto ret = fd2socket.try_emplace(fd, std::move(ptr));
    if (!ret.second) {
        nxtfd--;
        errno = ENOMEM;
        return -1;
    }
    return fd;
}

int socketController::bind(int socket, const struct sockaddr_in* addr) {
    auto iter = fd2socket.find(socket);
    if (iter == fd2socket.end()) {
        errno = ENOTSOCK;
        return -1;
    }
    if (iter->second->haveSrc()) {
        errno = EINVAL;
        return -1;
    }
    if (isConnected(*addr) || isListened(*addr)) {
        errno = EADDRINUSE;
        return -1;
    }
    if (iter->second->isListen() || iter->second->isConnect() || iter->second->isReset()) {
        errno = EINVAL;
        return -1;
    }
    iter->second->setSrc(*addr);
    return 0;
}

int socketController::listen(int socket, int backlog) {
    auto iter = fd2socket.find(socket);
    if (iter == fd2socket.end()) {
        errno = ENOTSOCK;
        return -1;
    }
    if (iter->second->isListen() || iter->second->isConnect() || iter->second->isReset()) {
        errno = EINVAL;
        return -1;
    }
    int ret = iter->second->listen(backlog);
    return ret + 1;
}
