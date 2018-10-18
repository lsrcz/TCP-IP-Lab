#include <protocol/device.h>
#include <pcap/pcap.h>
#include <cstdio>
#include <testDevice.h>
#include <vector>
#include <string>


int testManageDevice() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevsp;
    pcap_findalldevs(&alldevsp , errbuf);
    pcap_if_t *first = alldevsp;
    std::vector<std::string> succ;
    while (first != NULL) {
        int t = addDevice(first->name);
        if (t == -1) {
            printf("Failed to add the device %s\n", first->name);
        } else {
            printf("Success to add the device %s, id is %d\n", first->name, t);
            succ.push_back(first->name);
        }
        first = first->next;
    }
    if (succ.empty()) {
        printf("Error: No devices available\n");
        return -1;
    }
    pcap_freealldevs(alldevsp);
    int id = 0;
    for (const std::string&devicename : succ) {
        int fid = findDevice(devicename.c_str());
        if (fid != id++) {
            printf("Error id %d for %s\n", fid, devicename.c_str());
            return -1;
        } else {
            printf("Success id %d for %s\n", fid, devicename.c_str());
        }
    }

    if (succ.size() >= 1) {
        int ret = removeDevice(succ[0].c_str());
        if (ret != -1) {
            printf("Success to remove the device %s\n", succ[0].c_str());
        } else {
            printf("Failed to remove the device %s\n", succ[0].c_str());
            return -1;
        }
        if (succ.size() >= 2) {
            removeDevice(1);
            if (ret != -1) {
                printf("Success to remove the device at 1\n");
            } else {
                printf("Failed to remove the device at 1\n");
                return -1;
            }
            int t = addDevice(succ[1].c_str());
            if (t == 0) {
                printf("Success to add the device %s, id is %d\n", succ[1].c_str(), t);
            } else {
                printf("Failed to add the device %s, return value is %d\n", succ[1].c_str(), t);
                return -1;
            }
            t = addDevice(succ[0].c_str());
            if (t == 1) {
                printf("Success to add the device %s, id is %d\n", succ[0].c_str(), t);
            } else {
                printf("Failed to add the device %s, return value is %d\n", succ[0].c_str(), t);
                return -1;
            }
        }
    }
    for (std::string devicename : succ) {
        if (removeDevice(devicename.c_str()) != -1) {
            printf("Success to remove the device %s\n", devicename.c_str());
        } else {
            printf("Failed to remove the device %s\n", devicename.c_str());
            return -1;
        }
    }

    int dd = addDevice(succ[0].c_str());
    if (dd == -1) {
        printf("Failed to add the device %s, return value is %d\n", succ[0].c_str(), dd);
        return -1;
    }
    if (removeDevice(succ[0].c_str()) == -1) {
        printf("Failed to remove the device %s\n", succ[0].c_str());
        return -1;
    }

    return 0;
}

int testMac() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevsp;
    pcap_findalldevs(&alldevsp , errbuf);
    pcap_if_t *first = alldevsp;
    std::vector<std::pair<int, std::string>> succ;
    while (first != NULL) {
        int t = addDevice(first->name);
        if (t != -1) {
            succ.push_back(std::make_pair(t, std::string(first->name)));
        }
        first = first->next;
    }
    if (succ.empty()) {
        printf("Error: No devices available\n");
        return -1;
    }
    pcap_freealldevs(alldevsp);

    unsigned char *buf = (unsigned char*)malloc(6);
    int oknum = 0;
    for (auto& p : succ) {
        if (getDeviceMac(p.first, buf) < 0) {
            printf("No MAC address available for %s\n", p.second.c_str());
        } else {
            printf("MAC address of %s: ", p.second.c_str());
            for (int i = 0; i < 5; ++i) {
                printf("%.2x:", buf[i]);
            }
            printf("%.2x\n", buf[5]);
            oknum++;
        }
    }

    free(buf);
    removeAllDevice();
    return (oknum > 0);
}

int testDevice() {
    if (testManageDevice() < 0)
        return -1;
    if (testMac() < 0)
        return -1;
    return 1;
}
