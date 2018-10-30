/**
 * @file device.h
 * @brief Library supporting network device management.
 */
#ifndef DEVICE_H
#define DEVICE_H

#include <pcap/pcap.h>
#include <utils/logutils.h>
#include <cstddef>
#include <string>
#include <list>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>
#include <pcap.h>
#include <cstring>

struct device_t {
    static int avaliable_id;
    int is_properly_inited = 0;
    std::string name;
    int id;
    pcap_t* pcap;
    uint8_t mac[6];
    pthread_t listening_thread;
    int holding_thread = 0;
    inline
    device_t() : name(""), id(-1), pcap(NULL) {}
    device_t(std::string name, pcap_t* pcap);
    device_t(device_t&& rhs);
    device_t& operator=(device_t&& rhs);
    inline
    bool operator==(const device_t& rhs) {
        return id == rhs.id;
    }
    ~device_t(); 
};

extern std::list<device_t> devices;

/**
 * Add a device to the library for sending/receiving packets. 
 *
 * @param device Name of network device to send/receive packet on.
 * @return An non-negative _device-ID_ on success, -1 on error.
 */
int addDevice(const char* device);

/**
 * Find a device added by `addDevice`.
 *
 * @param device Name of the network device.
 * @return An non-negative _device-ID_ on success, -1 if no such device 
 * was found.
 */
int findDevice(const char* device);

int getFirstDevice();

int removeDevice(const char* device);

int removeDevice(int device_descriptor);

void removeAllDevice();

int sendPacketOnDevice(int device_descriptor, const void *buf, int size);

int getDeviceMac(int device_descriptor, void *buf); 
#endif // DEVICE_H
