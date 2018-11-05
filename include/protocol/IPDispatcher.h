#ifndef IP_DISPATCHER_H
#define IP_DISPATCHER_H
#include <map>
#include <shared_mutex>
#include <protocol/ip.h>

class IPPacketDispatcher {
    std::shared_mutex mu;
    std::map<uint8_t, IPPacketReceiveCallback> callbacks;
 public:
    void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback);
    int processPacket(const void* packet, int len);
}

void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback);

int defaultIPPacketReceiveCallback(const void*, int);

#endif // IP_DISPATCHER_H
