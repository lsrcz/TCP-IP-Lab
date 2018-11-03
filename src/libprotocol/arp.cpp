#include <map>
#include <cstring>
#include <arpa/inet.h>
#include <protocol/device.h>
#include <protocol/ip.h>
#include <protocol/packetio.h>
#include <net/ethernet.h>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>
#include <protocol/arp.h>

struct MAC {
    uint8_t mac[6];
    bool operator==(const MAC& rhs) {
        for (int i = 0; i < 6; ++i) {
            if (mac[i] != rhs.mac[i])
                return false;
        }
        return true;
    }
    std::string toString() {
        char buf[20];
        snprintf(buf, 20, "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return buf;
    }
};

std::map<int, std::map<in_addr_t, MAC>> arp_cache;
std::shared_mutex arp_mu;
std::condition_variable_any arp_cv;

int request_arp(in_addr ip, void* m) {
    std::shared_lock<std::shared_mutex> lock(arp_mu);
    in_addr_t uip = ip.s_addr;
    for (auto &p : arp_cache) {
        auto iter = p.second.find(uip);
        if (iter != p.second.end()) {
            memcpy(m, &(iter->second), 6);
            return 0;
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (sendARPRequest(ip, 0) < 0)
            continue;
        if (arp_cv.wait_for(arp_mu, std::chrono::seconds(1), [uip]{
                    for (auto &p : arp_cache) {
                        auto iter = p.second.find(uip);
                        if (iter != p.second.end()) {
                            return true;
                        }
                    }
                    return false;
                })) {
            for (auto &p : arp_cache) {
                auto iter = p.second.find(uip);
                if (iter != p.second.end()) {
                    memcpy(m, &(iter->second), 6);
                    return 0;
                }
            }
            break;
        }
    }
    return -1;
}

struct arpPacket {
    uint16_t hrd;
    uint16_t pro;
    uint8_t hln;
    uint8_t pln;
    uint16_t op;
    MAC sendermac;
    in_addr senderip;
    MAC destmac;
    in_addr destip;
} __attribute__((packed));

int sendARPRequest(in_addr ip, int device) {
    arpPacket ap;
    ap.hrd = 0x0100;
    ap.pro = 0x0008;
    ap.hln = 6;
    ap.pln = 4;
    ap.op = 0x0100;
    if (getDeviceMAC(device, &ap.sendermac) == -1) {
        return -1;
    }
    if (getDeviceIP(device, &ap.senderip) == -1) {
        return -1;
    }
    uint8_t broadcastMAC[] = {0xff,0xff,0xff,0xff,0xff,0xff}; 
    memcpy(&ap.destmac, broadcastMAC, 6);
    ap.destip = ip;
    if (sendFrame(&ap, 28, ETHERTYPE_ARP, broadcastMAC, device) < 0)
        return -1;
    return 0;
}

int arpFrameReceiveCallback(const void* packet, int len, int id) {
    std::unique_lock<std::shared_mutex> lock(arp_mu);
    ErrorBehavior eb("ARP incoming packet processing failed", false, true);
    LOG(INFO, "Receiving ARP packet on device");
    arpPacket *ap = (arpPacket*)packet;
    if (ap->hrd == 0x0100 && ap->hln == 6) {
        if (ap->pro == 0x0008 && ap->pln == 4) {
            bool merge_flag = false;
            auto iter = arp_cache[id].find(ap->senderip.s_addr);
            if (iter != arp_cache[id].end()) {
                std::string msg = std::string("Update ARP cache for ") +
                    inet_ntoa(ap->senderip) + " to " + ap->sendermac.toString();
                LOG(INFO, msg);
                iter->second = ap->sendermac;
                arp_cv.notify_all();
                merge_flag = true;
            }
            in_addr ip;
            if (getDeviceIP(id, &ip) < 0) {
                ERROR_WITH_BEHAVIOR(eb, return -1);
            }
            if (ip.s_addr == ap->destip.s_addr) {
                if (!merge_flag) {
                    std::string msg = std::string("Set ARP cache for ") +
                        inet_ntoa(ap->senderip) + " to " + ap->sendermac.toString();
                    LOG(INFO, msg);
                    arp_cache[id][ap->senderip.s_addr] = ap->sendermac;
                    arp_cv.notify_all();
                }
                if (ap->op == 0x0100) {
                    in_addr tmp = ap->destip;
                    ap->destip = ap->senderip;
                    ap->senderip = tmp;
                    ap->destmac = ap->sendermac;
                    MAC m;
                    if (getDeviceMAC(id, &m) < 0) {
                        ERROR_WITH_BEHAVIOR(eb, return -1);
                    }
                    ap->sendermac = m;
                    ap->op = 0x0200;
                    if (sendFrame(ap, 28, ETHERTYPE_ARP, &(ap->destmac), id) < 0) {
                        ERROR_WITH_BEHAVIOR(eb, return -1);
                    }
                }
            }
        } else {
            LOG(INFO, "Ignore ARP with unknown protocols");
        }
    } else {
        LOG(INFO, "Ignore ARP with unknown hardware types");
    }
    return 0;
}
