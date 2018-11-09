#include <protocol/api.h>
#include <protocol/IPDispatcher.h>
#include <utils/netutils.h>
#include <map>
#include <string>

int main(int argc, char*argv[]) {
    initProtocol();
    if (addInterfaceWithIP(argv[1], argv[2], "255.255.255.0") < 0) {
        return -1;
    }
    addRouterDev(0);
    sleep(200);
}
