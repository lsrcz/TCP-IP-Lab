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
    getaddrinfo("172.16.175.201", "7777", NULL, &res);
    assert(res != nullptr);
    bind(fd, res->ai_addr, sizeof(sockaddr_in));
    listen(fd, 1024);
    int fffd;
    struct sockaddr_in ad;
    socklen_t l = sizeof(sockaddr_in);
    fffd = accept(fd, (sockaddr*)&ad, &l);
    printf("\n\n accept %d \n\n", fffd);
    printf(" port: %x\n", htonl16(ad.sin_port));
    printf(" ip: %s\n", inet_ntoa(ad.sin_addr));
    sleep(5);
    for (int i = 0; i < 30; ++i)
        printf("------------------ we will close -------------------\n");
    close(fffd);
    /*while ((fffd = __wrap_accept(fd)> 0)
      printf("%d\n", fffd);*/
    // __wrap_connect(fd, res->ai_addr, sizeof(sockaddr_in));
    sleep(10);
    freeaddrinfo(res);
    cleanUp();
}
