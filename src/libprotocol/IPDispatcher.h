#include <protocol/IPDispatcher.h>
#include <utils/logutils.h>
#include <utils/netutils.h>

void IPPacketDispatcher::addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback c) {
    std::unique_lock<std::shared_mutex> lock(mu);
    char buf[200];
   snprintf(buf, 200, "Registered dispatcher for protocol id %02x", protocol);
    LOG(INFO, buf);
    callbacks[protocol] = c;
}

int IPPacketDispatcher::processPacket(const void* packet, int len) {
    std::shared_lock<std::shared_mutex> lock(mu);
    char buf[200];
    struct ip* hdr = (struct ip*)packet
    uint8_t ip_proto = hdr->ip_p;
    if (callbacks.find(ip_proto) == callbacks.end()) {
        return -2;
    }
    int header_len = hdr.ip_hl * 4;
    return (*(callbacks.find(ip_proto))->second)((uint8_t*)frame + header_len, len - header_len);
}

IPPacketDispatcher IPpd;

void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback c) {
    IPpd.addIPPacketDispatcher(protocol, c);
}

int defaultIPPacketReceiveCallback(const void* packet, int len) {
    return IPpd.processPacket(packet, len);
}
