#include <protocol/api.h>
#include <arpa/inet.h>

int main() {
    initProtocol();
    int id = addInterfaceWithIP("ens33", "172.16.175.203");
    in_addr ip;
    inet_aton("172.16.175.1", &ip);
    uint8_t buf[6];
    sleep(10);
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