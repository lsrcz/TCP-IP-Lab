#include <map>
#include <protocol/IPDispatcher.h>
#include <protocol/api.h>
#include <string>
#include <utils/netutils.h>

int sendIPPacket() {
    struct in_addr src, dest;
    inet_aton("192.168.0.1", &src);
    inet_aton("192.168.3.1", &dest);
    uint8_t buf[100];
    for (int i = 0; i < 100; ++i) {
        buf[i] = i;
    }
    return sendIPPacket(src, dest, 253, buf, 100);
}

int main(int argc, char* argv[]) {
    initProtocol();
    setRouterRID(0);
    int dev = addInterfaceWithIP("v1p", "192.168.0.1", "255.255.255.0");
    addRouterDev(dev);
    while (true) {
        sleep(2);
        sendIPPacket();
    }
}
