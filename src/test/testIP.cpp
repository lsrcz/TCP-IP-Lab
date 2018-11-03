#include <protocol/device.h>
#include <protocol/ip.h>
#include <protocol/packetio.h>
#include <protocol/frameDispatcher.h>
#include <netinet/in.h>

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
    src.s_addr = (1 << 24) + (175 << 16) + (16 << 8) + 172;
    dest.s_addr = (203 << 24) + (175 << 16) + (16 << 8) + 172;
    uint8_t buf[100];
    for (int i = 0; i < 100; ++i) {
        buf[i] = i;
    }
    sendIPPacket(src, dest, IPPROTO_ICMP, buf, 100);
    return 1;
}

void testRcvIP() {
    setFrameReceiveCallback(defaultFrameReceiveCallback);
    addFrameDispatcher(ETHERTYPE_IP, frame2IP);
    sleep(20); 
}

int main() {
    addDevice("lo");
    testSendIPPacket();
    testRcvIP();
}
