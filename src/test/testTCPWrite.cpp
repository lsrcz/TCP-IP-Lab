#include <cassert>
#include <protocol/Router.h>
#include <protocol/api.h>
#include <protocol/socket.h>

int main() {
    initProtocol();
    setRouterRID(1);

    int dev;
    dev = addInterfaceWithIP("va", "172.16.176.202", "255.255.255.0");
    addRouterDev(dev);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo *res = nullptr;
    getaddrinfo("172.16.176.202", "3233", NULL, &res);
    assert(res != nullptr);
    bind(fd, res->ai_addr, sizeof(sockaddr_in));
    freeaddrinfo(res);
    getaddrinfo("172.16.176.201", "7288", NULL, &res);
    assert(res != nullptr);
    connect(fd, res->ai_addr, sizeof(sockaddr_in));
    uint8_t buffer[1000000];
    for (int i = 0; i < 1000000; ++i) {
        buffer[i] = (uint8_t)i;
    }
    printf("%lu\n", write(fd, buffer, 1000000));
    pause();
    freeaddrinfo(res);

}
