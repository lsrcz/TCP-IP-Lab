#include <protocol/api.h>
#include <protocol/socket.h>
#include <cassert>
#include <protocol/Router.h>

int main() {
    initProtocol();
    setRouterRID(1);

    int dev;
    dev = addInterfaceWithIP("ens33", "172.16.175.202", "255.255.255.0");
    addRouterDev(dev);

    int fd = __wrap_socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo* res = nullptr;
    __wrap_getaddrinfo("172.16.175.1", "7777", NULL, &res);
    assert(res != nullptr);
    sleep(3);
    __wrap_connect(fd, res->ai_addr, sizeof(sockaddr_in));

    sleep(10);


    freeaddrinfo(res);
}
