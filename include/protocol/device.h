/**
 * @file device.h
 * @brief Library supporting network device management.
 */
#ifndef DEVICE_H
#define DEVICE_H

#include <cstddef>
#include <cstring>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <net/if.h>
#include <netinet/in.h>
#include <pcap.h>
#include <pcap/pcap.h>
#include <set>
#include <shared_mutex>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <utils/errorutils.h>
#include <utils/logutils.h>
#include <utils/netutils.h>
#include <vector>
#include <wrapper/pcap.h>
#include <wrapper/pthread.h>
#include <wrapper/unix_file.h>

struct device_t {
    static std::set<int> avaliable_id;
    int                  id;
    bool                 ownid;
    std::string          name;
    bool                 is_inited = false;

    std::unique_ptr<pcap_t, std::function<void(pcap_t*)>> pcap;
    uint8_t                                               mac[6];
    std::thread                                           listening_thread;

    inline device_t(std::string name)
        : id(*avaliable_id.begin()), ownid(true), name(name) {
        avaliable_id.erase(avaliable_id.begin());
        if (avaliable_id.empty())
            avaliable_id.insert(id + 1);
    }
    inline ~device_t() {
        if (ownid)
            avaliable_id.insert(id);
        stopListening();
    }
    void        startListening();
    void        stopListening();
    inline bool isListening() const {
        return listening_thread.joinable();
    }

    bool init();
    device_t(device_t&& rhs);
    device_t&   operator=(device_t&& rhs);
    inline bool operator==(const device_t& rhs) {
        return id == rhs.id;
    }
    int sendPacket(const void* buf, int size) {
        std::string str = std::string("Send packet on " + name + " failed");
        char        ebbuf[200];
        strcpy(ebbuf, str.c_str());
        ErrorBehavior eb(ebbuf, false, true);
        Pcap_inject(pcap.get(), buf, size, eb, return -1);
        return 0;
    }
};

class device_list_t {
    std::map<int, MAC>        mac_addr;
    std::list<device_t>       devices;
    mutable std::shared_mutex mu;
    auto                      getDeviceIter(std::string name);
    auto                      getDeviceIter(int id);

public:
    int              addDevice(const std::string& name);
    int              findDevice(const std::string& name);
    int              getFirstDevice();
    std::vector<int> getAllDevice();
    int              removeDevice(const std::string& name);
    int              removeDevice(int id);
    void             removeAllDevice();
    int              sendPacketOnDevice(int id, const void* buf, int size);
    int              getDeviceMAC(int id, void* buf);
};

std::vector<std::string> listAvailableDevices();

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

std::vector<int> getAllDevice();

int removeDevice(const char* device);

int removeDevice(int device_descriptor);

void removeAllDevice();

int sendPacketOnDevice(int device_descriptor, const void* buf, int size);

int getDeviceMAC(int device_descriptor, void* buf);
#endif  // DEVICE_H
