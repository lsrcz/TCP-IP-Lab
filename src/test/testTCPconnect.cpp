#include <cassert>
#include <protocol/Router.h>
#include <protocol/api.h>
#include <protocol/socket.h>

int main() {
    initProtocol();
    setRouterRID(1);

    int dev;
    dev = addInterfaceWithIP("va", "172.16.175.202", "255.255.255.0");
    addRouterDev(dev);

    int              fd  = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo* res = nullptr;
    getaddrinfo("172.16.175.201", "7777", NULL, &res);
    assert(res != nullptr);
    connect(fd, res->ai_addr, sizeof(sockaddr_in));

    close(fd);


    freeaddrinfo(res);
    sleep(20);
    cleanUp();
}
