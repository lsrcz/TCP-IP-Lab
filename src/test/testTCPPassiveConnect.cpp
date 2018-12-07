#include <cassert>
#include <protocol/Router.h>
#include <protocol/api.h>
#include <protocol/socket.h>

int main() {
    initProtocol();
    setRouterRID(1);

    int dev;
    dev = addInterfaceWithIP("vb", "172.16.175.201", "255.255.255.0");
    addRouterDev(dev);

    int              fd  = __wrap_socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo* res = nullptr;
    __wrap_getaddrinfo("172.16.175.201", "7777", NULL, &res);
    assert(res != nullptr);
    sleep(3);
    __wrap_bind(fd, res->ai_addr, sizeof(sockaddr_in));
    __wrap_listen(fd, 1024);
    int fffd;
    /*while ((fffd = __wrap_accept(fd)> 0)
      printf("%d\n", fffd);*/
    pause();
    // __wrap_connect(fd, res->ai_addr, sizeof(sockaddr_in));

    sleep(10);

    freeaddrinfo(res);
}
