#include <cassert>
#include <protocol/Router.h>
#include <protocol/api.h>
#include <protocol/socket.h>

uint8_t buffer[100000000];
int     main() {
    initProtocol();
    setRouterRID(2);
    int dev;
    dev = addInterfaceWithIP("vc", "172.16.176.202", "255.255.255.0");
    addRouterDev(dev);

    int              fd  = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo* res = nullptr;
    getaddrinfo("172.16.176.202", "3230", NULL, &res);
    assert(res != nullptr);
    bind(fd, res->ai_addr, sizeof(sockaddr_in));
    freeaddrinfo(res);
    getaddrinfo("172.16.176.201", "7200", NULL, &res);
    assert(res != nullptr);
    connect(fd, res->ai_addr, sizeof(sockaddr_in));
    for (int i = 0; i < 100000000; ++i) {
        buffer[i] = (uint8_t)i;
    }
    printf("prepare to write\n");
    printf("%lu\n", write(fd, buffer, 100000000));
    pause();

    freeaddrinfo(res);
    cleanUp();
}
