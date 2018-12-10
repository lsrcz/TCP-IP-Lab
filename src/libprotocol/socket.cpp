#include <protocol/socket.h>
#include <protocol/sockettype.h>
#include <utils/errorutils.h>

bool socket_t::sockaddr_in_iszero(sockaddr_in src) {
    return (src.sin_port == 0 && src.sin_addr.s_addr == 0);
}

socket_t::socket_t(socketController& sc) : sc(sc), t(*this) {
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
}

socket_t::socket_t(socketController& sc, sockaddr_in src, sockaddr_in dst)
    : sc(sc), t(*this), src(src), dst(dst) {
    setSrc(src);
    setDst(dst);
}

int socket_t::bind(const struct sockaddr_in* address) {
    std::lock_guard<std::mutex> lock(mu);
    if (!sockaddr_in_iszero(src))
        return EINVAL;
    src = *address;
    return 0;
}

int socket_t::connect(const sockaddr_in* address) {
    std::lock_guard<std::mutex> lock(mu);
    if (connectFlag) {
        errno = EISCONN;
        return -1;
    }
    {
        std::lock_guard<std::mutex> lock_guard(t.mu);
        if (sockaddr_in_iszero(src)) {
            if (sc.getFreshAddr(&src) == -1) {
                errno = EADDRNOTAVAIL;
                return -1;
            }
        }
        if (sc.isListened(src)) {
            errno = EADDRINUSE;
            return -1;
        }
    }
    dst   = *address;
    t.src = src;
    t.dst = dst;
    return t.connect();
}

int socket_t::listen(int backlog) {
    std::lock_guard<std::mutex> lock(mu);
    if (sc.isListened(t.src)) {
        errno = EADDRINUSE;
        return -1;
    }
    if (connectFlag || resetFlag) {
        errno = EINVAL;
        return -1;
    }
    if (backlog < 0) {
        errno = EINVAL;
        return -1;
    }

    this->backlog = backlog;
    t.listen();
    listenFlag = true;
    return 0;
}

int socket_t::accept(struct sockaddr_in* addr, socklen_t addrlen) {
    // no EAGAIN, EBADF, EFAULT, EINTR, EMFILE
    std::unique_lock<std::mutex> lock(mu);
    std::unique_lock             acclock(accmu);
    if (!listenFlag)
        return EINVAL;
    while (est_queue.empty()) {
        lock.unlock();
        acccv.wait(acclock);
    }
    lock.lock();
    int fd = est_queue.front();
    est_queue.pop_front();
    struct sockaddr_in local_addr;
    if (addr != nullptr) {
        local_addr = sc.getSocketPeerAddr(fd);
        memcpy(addr, &local_addr, addrlen);
    }
    return fd;
}

int socket_t::recv(const void* buf, int len) {
    return t.recv(buf, len);
}

int socket_t::genConnectFD(sockaddr_in src, sockaddr_in dst, tcpSeq rcv_nxt,
                           tcpSeq irs) {
    auto ptr    = std::make_unique<socket_t>(sc, src, dst);
    auto rawptr = ptr.get();
    ptr->lock();
    ptr->father        = this;  // so, the child should be disabled early
    ptr->t.state       = TCP_SYN_RECV;
    ptr->t.irs         = irs;
    ptr->t.rcv_nxt     = rcv_nxt;
    ptr->t.bufferBegin = rcv_nxt;
    int fd             = sc.registerSocket(std::move(ptr));
    syn_queue.push_back(fd);
    rawptr->unlock();
    rawptr->t.send(TH_ACK | TH_SYN);
    return fd;
}

int socket_t::write(const void* buf, size_t nbyte) {
    if (isListen()) {
        errno = ENOTCONN;
        return -1;
    }
    if (isReset()) {
        errno = ECONNRESET;
        return -1;
    }
    return t.write(buf, nbyte);
}

int socket_t::read(void* buf, size_t nbyte) {
    if (isListen()) {
        errno = ENOTCONN;
        return -1;
    }
    if (isReset()) {
        errno = ECONNRESET;
        return -1;
    }
    return t.read(buf, nbyte);
}

int socket_t::close() {
    return t.close();
}

int socket_t::getFD() {
    return fd;
}

int socket_t::notifyEstFather() {
    if (father == nullptr)
        return 0;
    if (father->estQueueIsFull())
        return -1;
    father->moveConnFromSynToEst(fd);
    return 0;
}
