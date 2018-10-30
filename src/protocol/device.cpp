#include <protocol/device.h>
#include <protocol/packetio.h>
#include <protocol/netutils.h>
#include <protocol/logutils.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <list>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pcap/pcap.h>
#include <pthread.h>

std::list<device_t> devices;

extern frameReceiveCallback frCallback;

struct handler_arg {
    uint8_t mac[6];
    pcap_t* pcap;
    int id;
};

void pcapHandler(u_char* user, const struct pcap_pkthdr *h, const u_char *byte) {
    handler_arg* arg = (handler_arg*)user;
    int id = arg->id;
    char msgBuf[1000];
    int ok = 1;
    for (int i = 0; i < 6; ++i) {
        if (byte[i] != 0xff) {
            ok = 0;
            break;
        }
    }
    if (ok == 0) {
        ok = 1;
        for (int i = 0; i < 6; ++i) {
            if (byte[i] != arg->mac[i]) {
                ok = 0;
                break;
            }
        }
    }
    if (ok == 0) {
        return;
    }
    if (h->len != h->caplen) {
        sprintf(msgBuf, "Abandon partial packet on id %d.", id);
        logPrint(WARNING, msgBuf);
        return;
    }
    if (frCallback(byte, h->caplen, id) < 0) {
        sprintf(msgBuf, "Frame receive callback failed on %d.", id);
        logPrint(ERROR, msgBuf);
    }
}

void* listening_handler(void* arg) {
    handler_arg* device = (handler_arg*)arg;
    pcap_loop(device->pcap, -1, pcapHandler, (u_char *)arg);
    free(arg);
    return NULL;
}

int device_t::avaliable_id = 0;
device_t::device_t(std::string name, pcap_t* pcap)
    : name(name), id(avaliable_id++), pcap(pcap) {
    int socketfd = pcap_get_selectable_fd(pcap);
    if (socketfd < 0) {
        logPrint(ERROR, std::string("Can't get the MAC address of ") + name + ".");
        return;
    }
    struct ifreq ifr;
    strcpy(ifr.ifr_name, name.c_str());
    if (ioctl(socketfd, SIOCGIFHWADDR, &ifr) == -1) {
        logPrint(ERROR, std::string("Can't get the MAC address of ") + name + ".");
        return;
    }
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);

    handler_arg* arg = (handler_arg*)malloc(sizeof(handler_arg));
    arg->pcap = pcap;
    arg->id = id;
    memcpy(&(arg->mac), mac, 6);
    if (pthread_create(&listening_thread, NULL, listening_handler, arg) != 0) {
        logPrint(ERROR, std::string("Failed to create the listening thread for ") + name + ".");
        return;
    }
    holding_thread = 1;

    is_properly_inited = 1;
}
device_t::device_t(device_t&& rhs) {
    name = std::move(rhs.name);
    id = rhs.id;
    pcap = rhs.pcap;
    rhs.pcap = NULL;
    memcpy(mac, rhs.mac, 6);
    listening_thread = rhs.listening_thread;
    holding_thread = rhs.holding_thread;
    rhs.holding_thread = 0;
    is_properly_inited = rhs.is_properly_inited;
}
device_t& device_t::operator=(device_t&& rhs) {
    if (this == &rhs)
        return *this;
    name = std::move(rhs.name);
    id = rhs.id;
    pcap = rhs.pcap;
    rhs.pcap = NULL;
    memcpy(mac, rhs.mac, 6);
    listening_thread = rhs.listening_thread;
    holding_thread = rhs.holding_thread;
    rhs.holding_thread = 0;
    is_properly_inited = rhs.is_properly_inited;
    return *this;
}
device_t::~device_t() {
    if (holding_thread) {
        logPrint(INFO, std::string("Destroying listening thread for ") + name + ".");
        pthread_cancel(listening_thread);
        pthread_join(listening_thread, NULL);
    }
    if (pcap != NULL) {
        logPrint(INFO, std::string("Destroying pcap handler for ") + name + ".");
        pcap_close(pcap);
    }
}

/**
 * Add a device to the library for sending/receiving packets. 
 *
 * @param device Name of network device to send/receive packet on.
 * @return An non-negative _device-ID_ on success, -1 on error.
 */
int addDevice(const char* device) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap_dev = pcap_create(device, errbuf);
    if (pcap_dev == NULL) {
        logPrint(ERROR, std::string("Failed to open the device ") + device + ".");
        return -1;
    }
    if (pcap_activate(pcap_dev) != 0) {
        pcap_close(pcap_dev);
        logPrint(ERROR, std::string("Failed to activate the device ") + device + ".");
        return -1;
    }
    int *dlt_p;
    int dlt_num = pcap_list_datalinks(pcap_dev, &dlt_p);
    int support_ether = 0;
    for (int i = 0; i < dlt_num; ++i) {
        if (dlt_p[i] == DLT_EN10MB) {
            support_ether = 1;
            break;
        }
    }
    pcap_free_datalinks(dlt_p);
    if (support_ether == 0) {
        pcap_close(pcap_dev);
        logPrint(ERROR, std::string("The device ") + device + " doesn't support ethernet.");
        return -1;
    }
    devices.push_back({device, pcap_dev});
    if (devices.back().is_properly_inited == 0) {
        devices.pop_back();
        return -1;
    }
    return devices.back().id;
}

std::list<device_t>::iterator getDeviceStruct(int id) {
    for (auto iter =  devices.begin(); iter != devices.end(); ++iter)
        if (iter->id == id)
            return iter;
    return devices.end();
}

std::list<device_t>::iterator getDeviceStruct(const std::string& name) {
    for (auto iter = devices.begin(); iter != devices.end(); ++iter)
        if (iter->name == name)
            return iter;
    return devices.end();
}

/**
 * Find a device added by `addDevice`.
 *
 * @param device Name of the network device.
 * @return An non-negative _device-ID_ on success, -1 if no such device 
 * was found.
 */
int findDevice(const char* device) {
    auto iter = getDeviceStruct(device);
    if (iter == devices.end())
        return -1;
    return iter->id;
}

int getFirstDevice() {
    if (devices.empty())
        return -1;
    return devices.front().id;
}

int removeDevice(const char* device) {
    auto iter = getDeviceStruct(device);
    if (iter == devices.end())
        return -1;
    devices.erase(iter);
    return 0;
}

int removeDevice(int id) {
    auto iter = getDeviceStruct(id);
    if (iter == devices.end())
        return -1;
    devices.erase(iter);
    return 0;
}

void removeAllDevice() {
    devices.clear();
}

int sendPacketOnDevice(int id, const void *buf, int size) {
    auto iter = getDeviceStruct(id);
    if (iter == devices.end())
        return -1;
    int ret = pcap_inject(iter->pcap, buf, size);
    if (ret != size)
        return -1;
    return 0;
}

int getDeviceMac(int id, void *buf) {
    if (id < 0)
        return -1;
    auto iter = getDeviceStruct(id);
    if (iter == devices.end())
        return -1;
    memcpy(buf, iter->mac, 6);
    return 1;
}
