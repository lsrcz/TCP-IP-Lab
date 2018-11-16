#include <cstdint>
#include <cstdio>
#include <protocol/device.h>
#include <protocol/packetio.h>
#include <utils/logutils.h>

int testSendFrame() {
    const uint8_t buf[10]    = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int           dd         = addDevice("ens33");
    const uint8_t destmac[6] = { 0x00, 0x50, 0x56, 0xc0, 0x00, 0x08 };
    int           ret        = sendFrame(buf, 10, 0x0800, destmac, dd);
    if (ret < 0) {
        printf("Failed to send a frame\n");
        return -1;
    } else {
        printf("Successed to send a frame\n");
    }
    removeDevice(dd);
    return 0;
}

int printAllPacketCallback(const void* packet, int len, int user) {
    if (*((uint8_t*)packet + 23) == 1) {
        char buf[100000];
        sprintf(buf, "ICMP packet of length %d received on id %d.", len, user);
        // logPrint(DEBUG, buf);
        for (int i = 0; i < len; ++i) {
            if (i != 0)
                printf(" ");
            printf("%.2x", *((uint8_t*)packet + i));
        }
        printf("\n");
    }
    return 0;
}

int testRecvFrame() {
    setFrameReceiveCallback(printAllPacketCallback);
    int id = addDevice("ens33");
    sleep(20);
    removeDevice(id);
    return 0;
}

int main() {
    // logPrint(DEBUG, "test send frame");
    if (testSendFrame() < 0)
        return -1;
    testRecvFrame();
    return 0;
}
