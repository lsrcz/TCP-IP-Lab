#include <protocol/device.h>
#include <protocol/ip.h>
#include <protocol/packetio.h>
#include <protocol/frameDispatcher.h>
#include <protocol/api.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    inet_aton("172.16.175.203", &src);
    inet_aton("172.16.175.1", &dest);
    uint8_t buf[100];
    for (int i = 0; i < 100; ++i) {
        buf[i] = i;
    }
    sendIPPacket(src, dest, 254, buf, 70);
    return 1;
}

void testRcvIP() {
    //setFrameReceiveCallback(defaultFrameReceiveCallback);
    
    //addFrameDispatcher(ETHERTYPE_IP, frame2IP);
    //sleep(20); 
}

int main() {
    initProtocol();
    addInterfaceWithIP("ens33", "172.16.175.203");
    testSendIPPacket();
    testRcvIP();
}
