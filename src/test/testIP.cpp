#include <arpa/inet.h>
#include <netinet/in.h>
#include <protocol/api.h>
#include <protocol/device.h>
#include <protocol/frameDispatcher.h>
#include <protocol/ip.h>
#include <protocol/packetio.h>

int frame2IP(const void* buf, int len, int id) {
    printf("lenip: %d \n", len);
    for (int i = 0; i < len; ++i) {
        printf("%2x ", ((uint8_t*)buf)[i]);
    }
    printf("\n");
    return 1;
}

int testSendIPPacket() {
    struct in_addr src, dest;
    inet_aton("172.16.176.203", &src);
    inet_aton("172.16.176.1", &dest);
    uint8_t buf[100];
    for (int i = 0; i < 100; ++i) {
        buf[i] = i;
    }

    return sendIPPacket(src, dest, 254, buf, 70);
    return 1;
}

int testSendIPMulticast() {
    struct in_addr src, dest;
    inet_aton("172.16.175.203", &src);
    inet_aton("224.0.0.121", &dest);
    uint8_t buf[100];
    for (int i = 0; i < 100; ++i) {
        buf[i] = i;
    }

    return sendIPPacket(src, dest, 254, buf, 100);
}

void testRcvIP() {
    // setFrameReceiveCallback(defaultFrameReceiveCallback);

    addFrameDispatcher(ETHERTYPE_IP, frame2IP);
}

int main() {
    initProtocol();
    addInterfaceWithIP("v2p", "172.16.176.1", "255.255.255.0");
    addInterfaceWithIP("v1p", "172.16.176.203", "255.255.255.0");
    addInterfaceWithIP("ens33", "172.16.175.203", "255.255.255.0");
    testSendIPMulticast();
    testRcvIP();
    printf("%d\n", testSendIPPacket());
    // sleep(100);
}
