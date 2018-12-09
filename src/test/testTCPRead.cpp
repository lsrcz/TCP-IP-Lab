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
    getaddrinfo("172.16.176.202", "7233", NULL, &res);
    assert(res != nullptr);
    bind(fd, res->ai_addr, sizeof(sockaddr_in));
    freeaddrinfo(res);



    getaddrinfo("172.16.176.201", "7228", NULL, &res);
    assert(res != nullptr);

    sleep(3);
    connect(fd, res->ai_addr, sizeof(sockaddr_in));

    uint8_t buffer[1000];
    for (int i = 0; i < 66; ++i) {
        printf("i: %d\n", i);
        printf("rd: %lu\n", read(fd, buffer, 1000));
        printf("buf: %d\n", buffer[0]);
        printf("buf: %d\n", buffer[1]);
        assert(buffer[0] == (uint8_t)(i * 1000));
    }

    pause();
    freeaddrinfo(res);
}
