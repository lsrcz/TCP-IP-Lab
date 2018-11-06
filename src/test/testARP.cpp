#include <protocol/api.h>
#include <arpa/inet.h>

int main() {
    initProtocol();
    if (addInterfaceWithIP("ens33", "172.16.175.1", "255.255.255.0") == 0) {
        printf("Should not add ens33 with 172.16.175.1\n");
    }
    if (addInterfaceWithIP("ens33", "172.16.175.203", "255.255.255.0") == -1) {
        printf("Should be able to add ens33 with 172.16.175.203\n");
    }
    int id1 = addInterfaceWithIP("lo", "127.0.0.1", "255.0.0.0");
    in_addr ip;
    inet_aton("127.0.0.1", &ip);
    uint8_t buf[6];
    request_arp(ip, buf);
    for (int i = 0; i < 6; ++i) {
        if (i != 0)
            printf(":");
        printf("%02x", buf[i]);
    }
    printf("\n");
    inet_aton("172.16.175.2", &ip);
    request_arp(ip, buf);
    for (int i = 0; i < 6; ++i) {
        if (i != 0)
            printf(":");
        printf("%02x", buf[i]);
    }
    printf("\n");
    sleep(10);
}
