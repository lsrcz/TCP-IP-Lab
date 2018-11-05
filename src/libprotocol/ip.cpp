#include <protocol/ip.h>
#include <protocol/device.h>
#include <utils/netutils.h>
#include <protocol/packetio.h>
#include <utils/logutils.h>
#include <net/ethernet.h>
#include <cstdint>
#include <cstring>
#include <map>
#include <arpa/inet.h>
#include <protocol/arp.h>

std::map<int, in_addr> ip_map;
std::map<in_addr_t, int> ip_map_inv;

int registerDeviceIP(const int device, const char* ip) {
    in_addr iip;
    if (inet_aton(ip, &iip) == 0) {
        std::string msg = ip + std::string(" is not a valid ip address.");
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    if (registerDeviceIP(device, iip) < 0) {
        return -1;
    }
    return 0;
}

int registerDeviceIP(const int device, const in_addr ip) {
    ip_map[device] = ip;
    ip_map_inv[ip.s_addr] = device;
    return 0;
}

void deleteDeviceIP(const int device) {
    auto iter = ip_map.find(device);
    if (iter == ip_map.end()) {
        return;
    }
    in_addr_t s_addr = iter->second.s_addr;
    ip_map.erase(iter);
    auto iter1 = ip_map_inv.find(s_addr);
    if (iter1 == ip_map_inv.end()) {
        // won't be executed
        return;
    }
    ip_map_inv.erase(iter1);
}

int getDeviceIP(const int device, in_addr* ip) {
    auto iter = ip_map.find(device);
    if (iter == ip_map.end()) {
        return -1;
    }
    *ip = iter->second;
    return 0;
}

int getIPDevice(const in_addr ip) {
    auto iter = ip_map_inv.find(ip.s_addr);
    if (iter == ip_map_inv.end()) {
        return -1;
    }
    return iter->second;
}

uint16_t ip_chksum(uint8_t* ptr) {
    uint32_t cksum = 0;
    for (int i = 0; i < 20; i += 2) {
        cksum += *(ptr + i + 1);
        cksum += *(ptr + i) << 8;
    }

    while (cksum > 0xffff) {
        cksum = (cksum >> 16) + (cksum & 0xffff);
    }
    return ~cksum;
}

union checksum_helper {
    uint32_t i;
    uint16_t s[2];
};

int sendIPPacket(const struct in_addr src, const struct in_addr dest,
                 int proto, const void *buf, int len) {
    ErrorBehavior eb("", false, true);
    struct ip hdr;
    hdr.ip_v = 4;
    hdr.ip_hl = 5;
    hdr.ip_tos = 0;
    hdr.ip_len = htonl16(len + 20);
    hdr.ip_id = 0;
    hdr.ip_off = htonl16(IP_DF);
    hdr.ip_ttl = 64;
    hdr.ip_p = proto;
    hdr.ip_sum = 0;
    hdr.ip_src = src;
    hdr.ip_dst = dest;
    checksum_helper ch;
    ch.i = 0;
    for (int i = 0; i < 10; ++i) {
        ch.i += *(((uint16_t*)&hdr) + i);
    }
    hdr.ip_sum = ~(ch.s[0] + ch.s[1]);
    //hdr.ip_sum = ip_chksum((uint8_t*)&hdr);
    int id;
    if ((id = getIPDevice(src)) < 0) {
        std::string msg = std::string("No device associates with ip address ") + inet_ntoa(src);
        eb.msg = msg.c_str();
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    uint8_t* ipbuf = (uint8_t*)malloc(len + 20);
    memcpy(ipbuf, &hdr, 20);
    memcpy(ipbuf + 20, buf, len);
    uint8_t mac[6];
    if (getDeviceMAC(id, mac) < 0) {
        eb.msg = "Can't get MAC address";
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    return sendFrame(ipbuf, len + 20, ETHERTYPE_IP, mac, id);
}

IPPacketReceiveCallback ipCallback  = NULL;
std::shared_mutex muIPCallback;

int setIPPacketReceiveCallback(IPPacketReceiveCallback callback) {
    std::unique_lock<std::shared_mutex> lock(muIPCallback);
    ipCallback = callback;
    return 0;
}


