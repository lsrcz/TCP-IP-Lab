#include <protocol/socketController.h>
#include <protocol/ip.h>
#include <utils/netutils.h>
#include <netinet/tcp.h>

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
    auto ret = fd2socket.try_emplace(fd, *this);
    if (!ret.second) {
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
    return iter->second.connect(address);
}


bool socketController::isListened( sockaddr_in s) {
    return listeningSet.find(s) != listeningSet.end();
}

bool socketController::isConnected( sockaddr_in s) {
    return connectedSet.find(s) != connectedSet.end();
}

int socketController::getFreshAddr(sockaddr_in *addr) {
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
    const ip* iphdr = (const ip*)buf;
    uint8_t iphl = iphdr->ip_hl;
    const tcphdr* tcph = (const tcphdr*)((uint8_t*)buf + iphl * 4);
    sockaddr_in src, dst;
    src.sin_addr = iphdr->ip_src;
    src.sin_port = tcph->th_sport;
    dst.sin_addr = iphdr->ip_dst;
    dst.sin_port = tcph->th_dport;
    if (tcpChksum(tcph, len - iphl * 4, src.sin_addr, dst.sin_addr) != 0) {
        return -1;
    }
    for (auto &p : fd2socket) {
        auto &s = p.second;
        if (src == s.getDst()) {
            if (dst == s.getSrc()) {
                return s.recv(buf, len);
            }
            if (s.isListen()) {
                return s.recv(buf, len);
            }
        }
    }
    return 0;
}

int TCPSegmentReceiveCallback(const void* buf, int len) {
    return socketController::getInstance().recv(buf, len);
}
