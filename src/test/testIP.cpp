#include <protocol/device.h>
#include <protocol/ip.h>
#include <netinet/in.h>

int testSendIPPacket() {
    addDevice("ens33");
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

int main() {
    testSendIPPacket();
}
