#include <protocol/IPDispatcher.h>
#include <utils/logutils.h>
#include <utils/netutils.h>
#include <protocol/ip.h>

void IPPacketDispatcher::addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback c) {
    std::unique_lock<std::shared_mutex> lock(mu);
    char buf[200];
   snprintf(buf, 200, "Registered dispatcher for protocol id %02x", protocol);
    LOG(INFO, buf);
    callbacks[protocol] = c;
}

int IPPacketDispatcher::processPacket(const void* packet, int len, int id) {
    LOG(DEBUG, "Packet received");
    std::shared_lock<std::shared_mutex> lock(mu);
    char buf[200];
    struct ip* hdr = (struct ip*)packet;
    in_addr dest = hdr->ip_dst;
    int packet_len = htonl16(hdr->ip_len);
    int header_len = hdr->ip_hl * 4;
    if (dest.s_addr == router.drip.s_addr || dest.s_addr == router.allip.s_addr) {
        // TODO: forward to router
        char buf[200];
        snprintf(buf, 200, "Router control packet received on %d", id);
        LOG(DEBUG, buf);
        return router.controlPacketRecv(packet, packet_len, id);
    } else if (isLocalIP(dest)) {
        // local processing
        uint8_t ip_proto = hdr->ip_p;
        if (callbacks.find(ip_proto) == callbacks.end()) {
            return -2;
        }
        return (*(callbacks.find(ip_proto))->second)((uint8_t*)packet + header_len, packet_len - header_len, id);
    } else {
        // TODO: forward to router
    }
    return 0;
}

int IPPacketDispatcher::addRouterDev(int dev) {
    return router.addDevice(dev);
}

IPPacketDispatcher IPpd;

void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback c) {
    IPpd.addIPPacketDispatcher(protocol, c);
}

int defaultIPPacketReceiveCallback(const void* packet, int len, int id) {
    return IPpd.processPacket(packet, len, id);
}

int addRouterDev(int dev) {
    return IPpd.addRouterDev(dev);
}
