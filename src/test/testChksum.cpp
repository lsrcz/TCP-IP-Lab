#include <utils/netutils.h>

int main() {
    uint8_t buf[160];
    for (int i = 0; i < 32; ++i) {
        buf[i] = i;
    }
    for (int i = 34; i < 160; ++i) {
        buf[i] = i;
    }
    ((uint16_t*)buf)[16] = htonl16(chksum(buf, 160));
    for (int i = 0; i < 160; ++i) {
        printf("%d,", buf[i]);
    }
    printf("%d\n", chksum(buf, 160));
}
