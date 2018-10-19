#include <protocol/device.h>
#include <protocol/packetio.h>
#include <test.h>
#include <cstdint>
#include <cstdio>

int testSendFrame() {
    const uint8_t buf[10] = {1,2,3,4,5,6,7,8,9,10};
    int dd = addDevice("ens33");
    const uint8_t destmac[6] = {0x00,0x50,0x56,0xc0,0x00,0x08};
    int ret = sendFrame(buf, 10, 0x0800, destmac, dd);
    if (ret < 0) {
        printf("Failed to send a frame\n");
        return -1;
    } else {
        printf("Successed to send a frame\n");
    }
    return 0;
}

int main() {
    if (testSendFrame() < 0)
        return -1;
    return 0;
}
