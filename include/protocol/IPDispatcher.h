#ifndef IP_DISPATCHER_H
#define IP_DISPATCHER_H
#include <map>
#include <protocol/Router.h>
#include <protocol/ip.h>
#include <shared_mutex>

class IPPacketDispatcher {
    std::shared_mutex                          mu;
    std::map<uint8_t, IPPacketReceiveCallback> callbacks;

public:
    void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback);
    int  processPacket(const void* packet, int len, int id);
};

void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback);

int defaultIPPacketReceiveCallback(const void*, int, int);

#endif  // IP_DISPATCHER_H
