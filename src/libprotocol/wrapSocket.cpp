#include <arpa/inet.h>
#include <protocol/TCP.h>
#include <protocol/socket.h>
#include <protocol/socketController.h>
#include <utils/errorutils.h>
#include <utils/netutils.h>
#include <wrapper/unix.h>
#include <wrapper/unix_file.h>

int __wrap_socket(int domain, int type, int protocol) {
    if (domain != AF_INET) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    if (type != SOCK_STREAM) {
        errno = EPROTONOSUPPORT;
        return -1;
    }
    if (protocol != 0 && protocol != IPPROTO_TCP) {
        errno = EPROTONOSUPPORT;
        return -1;
    }
    return socketController::getInstance().socket();
}

int __wrap_bind(int socket, const struct sockaddr* address, socklen_t address_len) {
    const struct sockaddr_in* addrin = (const struct sockaddr_in*)address;
    if (addrin->sin_family != AF_INET) {
        errno = EINVAL;
        return -1;
    }
    if (address_len != sizeof(sockaddr_in)) {
        errno = EINVAL;
        return -1;
    }
    return socketController::getInstance().bind(socket, addrin);
}

int __wrap_listen(int socket, int backlog) {
    return socketController::getInstance().listen(socket, backlog);
}

int __wrap_connect(int socket, const struct sockaddr* address,
                   socklen_t address_len) {
    if (address_len != sizeof(sockaddr_in)) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    if (address->sa_family != AF_INET) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    const struct sockaddr_in* addrin = (const struct sockaddr_in*)address;
    return socketController::getInstance().connect(socket, addrin);
}

int __wrap_accept(int socket, struct sockaddr* address, socklen_t *addrlen) {
    int fd = socketController::getInstance().accept(socket, address, addrlen);
    return fd;
}

ssize_t __wrap_read(int fd, void *buf, size_t nbyte) {
    if (!socketController::getInstance().isSocket(fd))
        return __real_read(fd, buf, nbyte);
    return socketController::getInstance().read(fd, buf, nbyte);
}

ssize_t __wrap_write(int fd, void *buf, size_t nbyte) {
    if (!socketController::getInstance().isSocket(fd))
        return __real_write(fd, buf, nbyte);
    return socketController::getInstance().write(fd, buf, nbyte);
}

int __wrap_close(int fd) {
    if (!socketController::getInstance().isSocket(fd))
        return __real_close(fd);
    // TODO:
    return socketController::getInstance().close(fd);
}

static int serviceToPort(const char* service) {
    if (strlen(service) > 5)
        return -1;
    int ret = 0;
    for (int i = 0; service[i] != 0; ++i) {
        ret *= 10;
        char s = service[i];
        if (s < '0' || s > '9')
            return -1;
        ret += (s - '0');
    }
    if (ret >= 65536)
        return -1;
    return ret;
}

int __wrap_getaddrinfo(const char* node, const char* service,
                       const struct addrinfo* hints, struct addrinfo** res) {
    printf("%d\n", serviceToPort(service));
    if (hints != NULL) {
        if ((hints->ai_family != AF_INET) && (hints->ai_family != AF_UNSPEC)) {
            return EAI_ADDRFAMILY;
        }
        if ((hints->ai_socktype != SOCK_STREAM) && (hints->ai_socktype != 0)) {
            return EAI_SOCKTYPE;
        }
        if ((hints->ai_protocol != IPPROTO_TCP) && (hints->ai_protocol != 0)) {
            return EAI_SERVICE;
        }
        if (hints->ai_flags != 0) {
            return EAI_BADFLAGS;
        }
    }
    if (node == NULL && service == NULL)
        return EAI_NONAME;
    ErrorBehavior eb("Out of memory", true, true);
    addrinfo* r    = (addrinfo*)Malloc(sizeof(addrinfo), eb, return EAI_MEMORY);
    r->ai_flags    = 0;
    r->ai_family   = AF_INET;
    r->ai_socktype = SOCK_STREAM;
    r->ai_protocol = IPPROTO_TCP;
    r->ai_addrlen  = sizeof(sockaddr_in);
    r->ai_addr =
        (sockaddr*)Malloc(sizeof(sockaddr_in), eb, free(r); return EAI_MEMORY);
    memset(r->ai_addr, 0, sizeof(sockaddr_in));
    ((sockaddr_in*)(r->ai_addr))->sin_family = AF_INET;
    if (service != NULL) {
        if ((((sockaddr_in*)(r->ai_addr))->sin_port =
                 htonl16(serviceToPort(service)))
            < 0) {
            free(r);
            return EAI_NONAME;
        }
    }
    if (node != NULL) {
        if (inet_aton(node, &((sockaddr_in*)(r->ai_addr))->sin_addr) < 0) {
            free(r);
            return EAI_NONAME;
        }
    }
    r->ai_canonname = NULL;
    r->ai_next      = NULL;
    *res            = r;
    return 0;
}
