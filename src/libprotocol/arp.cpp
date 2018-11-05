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
    bool operator!=(const MAC& rhs) {
        return !(*this == rhs);
    }
    std::string toString() {
        char buf[20];
        snprintf(buf, 20, "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return buf;
    }
};

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

std::map<int, std::map<in_addr_t, MAC>> arp_cache;
std::shared_mutex arp_mu;
std::condition_variable_any arp_cv;

void deleteDeviceARP(int id) {
    auto iter = arp_cache.find(id);
    if (iter == arp_cache.end())
        return;
    arp_cache.erase(iter);
}

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
        std::vector<int> devices = getAllDevice();
        if (devices.empty()) {
            ErrorBehavior eb("No device avaliable", false, true);
            ERROR_WITH_BEHAVIOR(eb, return -1);
        }
        for (int i: devices) {
            sendARPRequest(ip, i);
        }
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
    std::string msg = std::string("Unable to get the MAC address for ") + inet_ntoa(ip);
    ErrorBehavior eb(msg.c_str(), false, true);
    ERROR_WITH_BEHAVIOR(eb, return -1);
    return -1;
}

int sendGratuitousARPRequest(in_addr ip, int device) {
    std::shared_lock<std::shared_mutex> lock(arp_mu);
    char buf[12];
    snprintf(buf, 12, "%d", device);
    std::string msg = std::string("Sending gratuitous ARP request for ") + inet_ntoa(ip) + " on device " + buf;
    LOG(INFO, msg);
    arpPacket ap;
    ap.hrd = 0x0100;
    ap.pro = 0x0008;
    ap.hln = 6;
    ap.pln = 4;
    ap.op = 0x0100;
    if (getDeviceMAC(device, &ap.sendermac) == -1) {
        return -1;
    }
    if (getDeviceMAC(device, &ap.destmac) == -1) {
        return -1;
    }
    ap.senderip = ip;
    ap.destip = ip;
    uint8_t broadcastMAC[] = {0xff,0xff,0xff,0xff,0xff,0xff}; 
    if (sendFrame(&ap, 28, ETHERTYPE_ARP, broadcastMAC, device) < 0)
        return -1;
    in_addr_t uip = ip.s_addr;
    if (arp_cv.wait_for(arp_mu, std::chrono::seconds(1), [uip,device,ap]{
                auto iter = arp_cache[device].find(uip);
                if (iter != arp_cache[device].end()) {
                    if (iter->second == ap.sendermac)
                        return false;
                    return true;
                }
                return false;
            })) {
        std::string msg = std::string("IP is in use ") + inet_ntoa(ip);
        ErrorBehavior eb(msg.c_str(), false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
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
                MAC m;
                if (getDeviceMAC(id, &m) < 0) {
                    ERROR_WITH_BEHAVIOR(eb, return -1);
                }
                if (ap->senderip.s_addr == ap->destip.s_addr &&
                    ap->sendermac == ap->destmac && ap->destmac == m) {
                    LOG(INFO, "Ignore gratuitous ARP");
                    return 0;
                } else if (ap->senderip.s_addr == ap->destip.s_addr && ap->destmac == m && ap->sendermac != ap->destmac) {
                    LOG(WARNING, "Gratuitous ARP reply detected");
                    arp_cache[id][ap->senderip.s_addr] = ap->sendermac;
                    arp_cv.notify_all();
                }
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
