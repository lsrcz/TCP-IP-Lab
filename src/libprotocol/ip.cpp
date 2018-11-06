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

std::map<int, IP> ip_map;
std::map<in_addr_t, int> ip_map_inv;

int registerDeviceIP(const int device, const char* ip, const char* subnet_mask) {
    in_addr iip;
    if (inet_aton(ip, &iip) == 0) {
        std::string msg = ip + std::string(" is not a valid ip address.");
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    in_addr ssm;
    if (inet_aton(subnet_mask, &ssm) == 0) {
        std::string msg = subnet_mask + std::string(" is not a valid subnet mask");
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    if (registerDeviceIP(device, iip, ssm) < 0) {
        return -1;
    }
    return 0;
}

int registerDeviceIP(const int device, const in_addr ip, const in_addr subnet_mask) {
    ip_map[device] = {ip, subnet_mask};
    ip_map_inv[ip.s_addr] = device;
    return 0;
}

void deleteDeviceIP(const int device) {
    auto iter = ip_map.find(device);
    if (iter == ip_map.end()) {
        return;
    }
    in_addr_t s_addr = iter->second.ip.s_addr;
    ip_map.erase(iter);
    auto iter1 = ip_map_inv.find(s_addr);
    if (iter1 == ip_map_inv.end()) {
        // won't be executed
        return;
    }
    ip_map_inv.erase(iter1);
}

int getDeviceIP(const int device, IP* ip) {
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

bool isMulticastIP(const struct in_addr src) {
    uint8_t fst = *((const uint8_t*)&src);
    return fst >= 224 && fst <= 239;
}

int sendIPPacket(const struct in_addr src, const struct in_addr dest,
                 int proto, const void *buf, int len) {
    bool isMulticast = isMulticastIP(dest);


    ErrorBehavior eb("", false, true);
    struct ip hdr;
    hdr.ip_v = 4;
    hdr.ip_hl = 5;
    hdr.ip_tos = 0;
    hdr.ip_len = htonl16(len + 20);
    hdr.ip_id = 0;
    hdr.ip_off = htonl16(IP_DF);
    if (isMulticast) {
        hdr.ip_ttl = 1;
    } else {
        hdr.ip_ttl = 64;
    }
    hdr.ip_p = proto;
    hdr.ip_sum = 0;
    hdr.ip_src = src;
    hdr.ip_dst = dest;
    hdr.ip_sum = htonl16(chksum((uint8_t*)&hdr));
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

    if (isMulticast) {
        for (int i = 0; i < 3; ++i) {
            mac[i + 3] = ((const uint8_t*)&dest)[i + 1];
        }
        mac[0] = 0x01;
        mac[1] = 0;
        mac[2] = 0xC2;
        mac[3] &= 0x7f;
    } else {
        if (request_arp(dest, mac) < 0) {
            eb.msg = "Can't get MAC address";
            ERROR_WITH_BEHAVIOR(eb, return -1);
        }
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


