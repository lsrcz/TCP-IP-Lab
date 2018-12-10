#include <cassert>
#include <protocol/Router.h>
#include <protocol/api.h>
#include <protocol/socket.h>

int main() {
    initProtocol();
    setRouterRID(1);
    int dev;
    dev = addInterfaceWithIP("vd", "172.16.176.201", "255.255.255.0");
    addRouterDev(dev);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo *res = nullptr;
    getaddrinfo("172.16.176.201", "7200", NULL, &res);
    assert(res != nullptr);
    bind(fd, res->ai_addr, sizeof(sockaddr_in));
    freeaddrinfo(res);
    listen(fd, 1024);




    //getaddrinfo("172.16.176.201", "7269", NULL, &res);
    //assert(res != nullptr);

    //sleep(3);
    //connect(fd, res->ai_addr, sizeof(sockaddr_in));
    struct sockaddr_in ad;
    socklen_t l = sizeof(sockaddr_in);
    int connfd = accept(fd, (sockaddr*)&ad, &l);
    printf("\n\n accept %d \n\n", connfd);
    printf(" port: %x\n", htonl16(ad.sin_port));
    printf(" ip: %s\n", inet_ntoa(ad.sin_addr));

    uint8_t buffer[1000];
    for (int j = 0; j < 100; ++j) {
    for (int i = 0; i < 1000; ++i) {
        printf("i: %d\n", i);
        printf("rd: %lu\n", read(connfd, buffer, 1000));
        printf("buf: %d\n", buffer[0]);
        printf("buf: %d\n", buffer[1]);
        for (int t = 0; t < 1000; ++t)
            assert(buffer[t] == (uint8_t)(j * 1000000 + i * 1000 + t));
    }
    }

    pause();
    freeaddrinfo(res);
}
