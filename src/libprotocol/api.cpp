#include <protocol/api.h>
#include <arpa/inet.h>

void initProtocol() {
    setFrameReceiveCallback(defaultFrameReceiveCallback);
    addFrameDispatcher(ETHERTYPE_ARP, arpFrameReceiveCallback);
}

void removeInterfaceAndCleanup(int id) {
    removeDevice(id);
    deleteDeviceIP(id);
    deleteDeviceARP(id);
}

int addInterfaceWithIP(const std::string& name, in_addr ip) {
    int id;
    if ((id = addDevice(name.c_str())) < 0)
        return -1;
    if (sendGratuitousARPRequest(ip, id) == -1) {
        removeInterfaceAndCleanup(id);
        return -1;
    }
    registerDeviceIP(id, ip);
    return id;
}

int addInterfaceWithIP(const std::string& name, const std::string& ipstr) {
    in_addr ip;
    if (inet_aton(ipstr.c_str(), &ip) == 0) {
        std::string msg = "Invalid IP " + ipstr;
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    return addInterfaceWithIP(name, ip);
}
