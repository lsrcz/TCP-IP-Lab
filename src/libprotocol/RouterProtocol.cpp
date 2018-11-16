#include <protocol/RouterProtocol.h>
#include <protocol/ip.h>

int RouterHeader::setIPByID(int id) {
    IP ip;
    if (getDeviceIP(id, &ip) < 0) {
        ErrorBehavior eb("No device found", false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    this->ip   = ip.ip;
    subnetMask = ip.subnet_mask;
    return 0;
}
