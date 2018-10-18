#include <protocol/device.h>
#include <protocol/packetio.h>
#include <protocol/utils.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <deque>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pcap.h>

std::unordered_map<std::string, int> devices_id;
std::vector<pcap_t*> devices_pcap;
std::vector<std::string> devices_name;
std::deque<int> freed_pos;

/**
 * Add a device to the library for sending/receiving packets. 
 *
 * @param device Name of network device to send/receive packet on.
 * @return An non-negative _device-ID_ on success, -1 on error.
 */
int addDevice(const char* device) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap_dev = pcap_create(device, errbuf);
    if (pcap_dev == NULL)
        return -1;
    if (pcap_activate(pcap_dev) != 0)
        return -1;
    int device_id = devices_pcap.size();
    if (freed_pos.size() > 0) {
        device_id = freed_pos.front();
        freed_pos.pop_front();
        devices_pcap[device_id] = pcap_dev;
        devices_name[device_id] = device;
    } else {
        devices_pcap.push_back(pcap_dev);
        devices_name.push_back(device);
    }
    devices_id[device] = device_id;
    return device_id;
}

/**
 * Find a device added by `addDevice`.
 *
 * @param device Name of the network device.
 * @return An non-negative _device-ID_ on success, -1 if no such device 
 * was found.
 */
int findDevice(const char* device) {
    auto device_iter = devices_id.find(device);
    if (device_iter != devices_id.end())
        return device_iter->second;
    return -1;
}

int removeDevice(const char* device) {
    auto device_iter = devices_id.find(device);
    if (device_iter == devices_id.end())
        return -1;
    int dd = device_iter->second;
    devices_id.erase(device_iter);
    pcap_close(devices_pcap[dd]);
    devices_pcap[dd] = NULL;
    freed_pos.push_back(dd);
    return 0;
}

int removeDevice(int device_descriptor) {
    return removeDevice(devices_name[device_descriptor].c_str());
}

int removeAllDevice() {
    for (size_t i = 0; i < devices_pcap.size(); ++i) {
        if (devices_pcap[i] != NULL) {
            if (removeDevice(i) < 0)
                return -1;
        }
    }
    return 0;
}

int sendPacketOnDevice(int device_descriptor, const void *buf, int size) {
    if (devices_pcap[device_descriptor] == NULL)
        return -1;
    int ret = pcap_inject(devices_pcap[device_descriptor], buf, size);
    if (ret != size)
        return -1;
    return 0;
}

int getDeviceMac(int device_descriptor, void *buf) {
    if (device_descriptor < 0 || (size_t)device_descriptor >= devices_pcap.size() || devices_pcap[device_descriptor] == NULL) {
        return -1;
    }
    pcap_t* p = devices_pcap[device_descriptor];
    int socketfd = pcap_get_selectable_fd(p);
    if (socketfd < 0) {
        return -1;
    }
    struct ifreq ifr;
    strcpy(ifr.ifr_name, devices_name[device_descriptor].c_str());
    if (ioctl(socketfd, SIOCGIFHWADDR, &ifr) == -1) {
        return -1;
    }
    memcpy(buf, ifr.ifr_hwaddr.sa_data, 6);
    return 0;
}

/** 
 * @brief Encapsule some data into an Ethernet II frame and send it.
 *
 * @param buf Pointer to the payload.
 * @param len Length of the payload.
 * @param ethtype EtherType field value of this frame.
 * @param destmac MAC address of the destination.
 * @param id ID of the device(returned by `addDevice`) to send on.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
/*
int sendFrame(const void* buf, int len, 
              int ethtype, const void* destmac, int id) {
    const void* frameBuf = malloc(18 + len);
    
    }*/

/** 
 * @brief Process a frame upon receiving it.
 *
 * @param buf Pointer to the frame.
 * @param len Length of the frame.
 * @param id ID of the device(returned by `addDevice`) receiving current 
 * frame.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
/*
  typedef int (*frameReceiveCallback)(const void*, int, int);*/

/**
 * @brief Register a callback function to be called each time an Ethernet II 
 * frame was received.
 *
 * @param callback The callback function.
 * @return 0 on success, -1 on error.
 * @see frameReceiveCallback
 */
/*
  int setFrameReceiveCallback(frameReceiveCallback callback);*/
