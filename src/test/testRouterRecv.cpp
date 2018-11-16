#include <map>
#include <protocol/IPDispatcher.h>
#include <protocol/api.h>
#include <string>
#include <utils/netutils.h>

int receiveIPPacket(const void* frame, int len) {
    printf("len: %d\n", len);
    for (int i = 0; i < len; ++i) {
        printf("%02x ", ((char*)frame)[i]);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    initProtocol();
    setRouterRID(4);
    int dev = addInterfaceWithIP("v3ap", "192.168.3.1", "255.255.255.0");
    addRouterDev(dev);
    addIPPacketDispatcher(253, receiveIPPacket);
    while (true) {
        sleep(100);
    }
}
