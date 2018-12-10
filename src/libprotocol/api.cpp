#include <arpa/inet.h>
#include <protocol/IPDispatcher.h>
#include <protocol/api.h>
#include <protocol/socketController.h>
#include <protocol/Router.h>

void initProtocol() {
    setFrameReceiveCallback(defaultFrameReceiveCallback);
    addFrameDispatcher(ETHERTYPE_ARP, arpFrameReceiveCallback);
    addFrameDispatcher(ETHERTYPE_IP, defaultIPPacketReceiveCallback);
    addIPPacketDispatcher(IPPROTO_TCP, TCPSegmentReceiveCallback);
}

void cleanUp() {
    socketController::getInstance().cleanup();
    cleanupRouter();
}

void removeInterfaceAndCleanup(int id) {
    removeDevice(id);
    deleteDeviceIP(id);
    deleteDeviceARP(id);
}

int addInterfaceWithIP(const std::string& name, in_addr ip,
                       in_addr subnet_mask) {
    int id;
    if ((id = addDevice(name.c_str())) < 0)
        return -1;
    if (sendGratuitousARPRequest(ip, id) == -1) {
        removeInterfaceAndCleanup(id);
        return -1;
    }
    registerDeviceIP(id, ip, subnet_mask);
    return id;
}

int addInterfaceWithIP(const std::string& name, const std::string& ipstr,
                       const std::string& subnet_mask) {
    in_addr ip;
    if (inet_aton(ipstr.c_str(), &ip) == 0) {
        std::string   msg = "Invalid IP " + ipstr;
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    in_addr ssm;
    if (inet_aton(subnet_mask.c_str(), &ssm) == 0) {
        std::string   msg = "Invalid subnet mask " + subnet_mask;
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    return addInterfaceWithIP(name, ip, ssm);
}
