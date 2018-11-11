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
    std::shared_lock<std::shared_mutex> lock(mu);
    char buf[200];
    struct ip* hdr = (struct ip*)packet;
    in_addr dest = hdr->ip_dst;
    int packet_len = htonl16(hdr->ip_len);
    int header_len = hdr->ip_hl * 4;
    if (chksum((uint8_t*)hdr, header_len) != 0) {
        ErrorBehavior eb("Wrong ip packet checksum", false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    if (dest.s_addr == router.drip.s_addr || dest.s_addr == router.allip.s_addr || hdr->ip_p == 254) {
        char buf[200];
        return router.controlPacketRecv(packet, packet_len, id);
    } else if (isLocalIP(dest)) {
        uint8_t ip_proto = hdr->ip_p;
        if (callbacks.find(ip_proto) == callbacks.end()) {
            return -2;
        }
        return (*(callbacks.find(ip_proto))->second)((uint8_t*)packet + header_len, packet_len - header_len);
    } else {
        return router.otherPacketRecv(packet, packet_len, id);
    }
    return 0;
}

IPPacketDispatcher IPpd;

void addIPPacketDispatcher(uint8_t protocol, IPPacketReceiveCallback c) {
    IPpd.addIPPacketDispatcher(protocol, c);
}

int defaultIPPacketReceiveCallback(const void* packet, int len, int id) {
    return IPpd.processPacket(packet, len, id);
}
