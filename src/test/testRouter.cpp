#include <protocol/api.h>
#include <protocol/IPDispatcher.h>
#include <utils/netutils.h>
#include <map>
#include <string>

int main(int argc, char*argv[]) {
    initProtocol();
    int rid;
    sscanf(argv[1], "%d", &rid);
    setRouterRID(rid);
    for (int i = 0; i * 2 + 2 < argc; ++i) {
        int dev;
        if ((dev = addInterfaceWithIP(argv[2 + i * 2], argv[3 + i * 2], "255.255.255.0")) < 0) {
            return -1;
        }
        addRouterDev(dev);
    }
    while (true) {
        sleep(100);
    }
}
