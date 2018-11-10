#ifndef IP_DISPATCHER_H
#define IP_DISPATCHER_H
#include <map>
#include <shared_mutex>
#include <protocol/ip.h>
#include <protocol/Router.h>

class IPPacketDispatcher {
    std::shared_mutex mu;
    std::map<uint8_t, IPPacketReceiveCallback> callbacks;
    Router router;
 public:
    void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback);
    int processPacket(const void* packet, int len, int id);
    int addRouterDev(int dev);
    void setRouterRID(uint16_t rid);
};

void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback);

int defaultIPPacketReceiveCallback(const void*, int, int);

int addRouterDev(int dev);

void setRouterRID(uint16_t rid);

#endif // IP_DISPATCHER_H
