#include <protocol/socket.h>
#include <utils/errorutils.h>
#include <protocol/sockettype.h>


bool socket_t::sockaddr_in_iszero(sockaddr_in src) {
    return (src.sin_port == 0 && src.sin_addr.s_addr == 0);
}

socket_t::socket_t(socketController &sc) : sc(sc), t(*this) {
    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));
}

int socket_t::bind(const struct sockaddr_in *address) {
    std::lock_guard<std::mutex> lock(mu);
    if (!sockaddr_in_iszero(src))
        return EINVAL;
    src = *address;
    return 0;
}

int socket_t::connect(const sockaddr_in *address) {
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
    dst = *address;
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

    t.listen();
    listenFlag = true;
    return 0;
}

int socket_t::accept(struct sockaddr_in* addr) {
    // no EAGAIN, EBADF, EFAULT, EINTR, EMFILE
    std::lock_guard<std::mutex> lock(mu);
    std::unique_lock acclock(accmu);
    if (!listenFlag)
        return EINVAL;
    while (est_queue.empty()) {
        acccv.wait(acclock);
    }
    int fd = est_queue.front();
    est_queue.pop_front();
    return fd;
}

int socket_t::recv(const void* buf, int len) {
    return t.recv(buf, len);
}
