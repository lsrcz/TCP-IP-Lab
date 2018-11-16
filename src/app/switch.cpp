#include <map>
#include <protocol/api.h>
#include <string>
#include <utils/netutils.h>
#include <utils/printutils.h>

std::map<MAC, std::pair<bool, int>> mac_cache;

int addSwitchPort(const std::string& name) {
    int id;
    if ((id = addDevice(name.c_str())) < 0) {
        LOG(WARNING, "Failed to add device " + name);
        return -1;
    }
    MAC m;
    if (getDeviceMAC(id, &m.mac) < 0) {
        LOG(WARNING, "Failed to get MAC of " + name);
        removeDevice(id);
        return -1;
    }
    printf("%s: %d\n", name.c_str(), id);
    mac_cache[m] = std::make_pair(true, id);
    return 0;
}

int switchReceiveCallback(const void* buf, int len, int id) {
    printIncomingFrame(buf, len);
    if (!ETHER_IS_VALID_LEN(len + ETHER_CRC_LEN)) {
        return -1;
    }
    MAC dst;
    memcpy(dst.mac, buf, 6);
    MAC src;
    memcpy(src.mac, (uint8_t*)buf + 6, 6);
    mac_cache[src] = std::make_pair(false, id);

    auto iter = mac_cache.find(dst);
    if (iter == mac_cache.end()) {
        // cache miss, or multicast
        for (int d : getAllDevice()) {
            if (d != id) {
                if (sendPacketOnDevice(d, buf, len) < 0) {
                    char buf[200];
                    snprintf(buf, 200, "Failed to send fram on device %d", id);
                    LOG(WARNING, buf);
                }
            }
        }
    } else {
        if (iter->second.first) {
            // do nothing
        } else {
            if (sendPacketOnDevice(iter->second.second, buf, len)) {
                char buf[200];
                snprintf(buf, 200, "Failed to send fram on device %d", id);
                LOG(WARNING, buf);
            }
        }
    }
    return 0;
}

int main() {
    setFrameReceiveCallback(switchReceiveCallback);
    for (const std::string& s : listAvailableDevices()) {
        addSwitchPort(s);
    }
    while (true) {
        sleep(10);
        printf("MAC cache:\n");
        printf("%5s%12s%18s\n", "port", "MAC", "internal");
        for (auto t : mac_cache) {
            printf("%6d%20s%6s\n", t.second.second, t.first.toString().c_str(),
                   t.second.first ? "T" : "F");
        }
    }
}
