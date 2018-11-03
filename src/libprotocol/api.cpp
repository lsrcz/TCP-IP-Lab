#include <protocol/api.h>

void initProtocol() {
    setFrameReceiveCallback(defaultFrameReceiveCallback);
    addFrameDispatcher(ETHERTYPE_ARP, arpFrameReceiveCallback);
}

int addInterfaceWithIP(const std::string& name, in_addr ip) {
    int id;
    if ((id = addDevice(name.c_str())) < 0) {
        return -1;
    }
    registerDeviceIP(id, ip);
    return id;
}

int addInterfaceWithIP(const std::string& name, const std::string& ip) {
    int id;
    if ((id = addDevice(name.c_str())) < 0) {
        return -1;
    }
    if (registerDeviceIP(id, ip.c_str()) < 0) {
        removeDevice(id);
        return -1;
    }
    return id;
}
