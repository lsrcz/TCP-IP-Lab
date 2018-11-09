#include <protocol/device.h>
#include <protocol/packetio.h>
#include <utils/netutils.h>
#include <utils/logutils.h>
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
#include <wrapper/unix_file.h>


extern frameReceiveCallback frCallback;
extern std::shared_mutex muFrCallback;

struct handler_arg {
    uint8_t mac[6];
    pcap_t* pcap;
    int id;
};

void pcapHandler(u_char* user, const struct pcap_pkthdr *h, const u_char *byte) {
    handler_arg* arg = (handler_arg*)user;
    int id = arg->id;
    char msgBuf[1000];
    if (h->len != h->caplen) {
        sprintf(msgBuf, "Abandon partial packet on id %d.", id);
        //logPrint(WARNING, msgBuf);
        return;
    }
    std::shared_lock<std::shared_mutex> lock(muFrCallback);
    if (frCallback != NULL) {
        if (frCallback(byte, h->caplen, id) < 0) {
            sprintf(msgBuf, "Frame receive callback failed on %d.", id);
            //logPrint(ERROR, msgBuf);
        }
    }
}

/**
 * class device_t
 */

void device_t::startListening() {
    if (isListening())
        return;
    LOG(INFO, std::string("Start listening on device ") + name);
    handler_arg ha;
    ha.id = id;
    ha.pcap = pcap.get();
    memcpy(ha.mac, mac, 6);
    listening_thread = std::thread([](handler_arg ha) {
            pcap_loop(ha.pcap, -1, pcapHandler, (u_char *)&ha);
        }, ha);
}

void device_t::stopListening() {
    if (!isListening())
        return;
    LOG(INFO, std::string("Stop listening on device ") + name);
    pthread_t pt = listening_thread.native_handle();
    Pthread_cancel(pt);
    listening_thread.join();
}

bool device_t::init() {
    ErrorBehavior eb = ErrorBehavior("Initialization failed");
    eb.setHaveAction();
    std::string dir("/sys/class/net/");
    dir += name + '/';

    eb.setSysError();
    Access(dir.c_str(), F_OK, eb, return false);

    // open link
    char errbuf[PCAP_ERRBUF_SIZE];

    eb.setPcapError();
    pcap_t *pcap_dev = Pcap_create(name.c_str(), errbuf, eb, return false);
    std::unique_ptr<pcap_t,std::function<void(pcap_t*)>> ppcap_dev(pcap_dev, [](pcap_t *p) {pcap_close(p);});
    Pcap_activate(ppcap_dev.get(), eb, return false);

    int *dlt_p;
    int dlt_num = Pcap_list_datalinks(ppcap_dev.get(), &dlt_p, eb, return false);
    bool support_ether = false;
    for (int i = 0; i < dlt_num; ++i) {
        if (dlt_p[i] == DLT_EN10MB) {
            support_ether = true;
            break;
        }
    }
    pcap_free_datalinks(dlt_p);
    if (!support_ether) {
            return false;
    }
    pcap = std::move(ppcap_dev);

    // get MAC
    dir += "address";
    eb.setSysError();
    Access(dir.c_str(), F_OK, eb, return false);
    int fd = Open(dir.c_str(), O_RDONLY, eb, printf("ok\n");return false;1);
    char buf[20];
    memset(buf, 0, 20);

    Read(fd, buf, 17, eb, Close(fd);return false);
    for (int i = 0; i < 6; ++i) {
        if (!(isHexPoint(buf[i * 3]) && isHexPoint(buf[i * 3 + 1]))) {
            ERROR_WITH_BEHAVIOR(eb, return false);
        }
        uint8_t t = stringtobyte(buf + i * 3);
        mac[i] = t;
    }

    startListening();
    eb.setUserError();
    if (!isListening()) {
        ERROR_WITH_BEHAVIOR(eb, return false);
    }
    is_inited = true;
    return true;
}

std::set<int> device_t::avaliable_id = {0};

device_t::device_t(device_t&& rhs) {
    id = rhs.id;
    rhs.ownid = false;
    ownid = true;
    name = std::move(rhs.name);
    is_inited = rhs.is_inited;
    pcap = std::move(rhs.pcap);
    memcpy(mac, rhs.mac, 6);
    listening_thread = std::move(rhs.listening_thread);
}

device_t& device_t::operator=(device_t&& rhs) {
    if (this == &rhs)
        return *this;
    id = rhs.id;
    rhs.ownid = false;
    ownid = true;
    name = std::move(rhs.name);
    is_inited = rhs.is_inited;
    pcap = std::move(rhs.pcap);
    memcpy(mac, rhs.mac, 6);
    listening_thread = std::move(rhs.listening_thread);
    return *this;
}

/**
 * class device_list_t
 */

auto device_list_t::getDeviceIter(std::string name) {
    return std::find_if(devices.begin(), devices.end(),
                        [&name](const device_t& d) {
                            return d.name == name;
                        });
}
auto device_list_t::getDeviceIter(int id) {
    return std::find_if(devices.begin(), devices.end(),
                        [id](const device_t& d) {
                            return d.id == id;
                        });
}
int device_list_t::addDevice(const std::string& device) {
    std::unique_lock<std::shared_mutex> lock(mu);
    device_t d(device);
    if (!d.init())
        return -1;
    int id = d.id;
    MAC m;
    memcpy(&m, d.mac, 6);
    mac_addr[id] = m;
    devices.push_back(std::move(d));
    return id;
}
int device_list_t::findDevice(const std::string &name) {
    std::shared_lock<std::shared_mutex> lock(mu);
    auto iter = getDeviceIter(name);
    if (iter == devices.end())
        return -1;
    return iter->id;
}
int device_list_t::getFirstDevice() {
    std::shared_lock<std::shared_mutex> lock(mu);
    if (devices.empty())
        return -1;
    return devices.front().id;
}
std::vector<int> device_list_t::getAllDevice() {
    std::shared_lock<std::shared_mutex> lock(mu);
    std::vector<int> v;
    for (const auto &d : devices) {
        v.push_back(d.id);
    }
    return v;
}
int device_list_t::removeDevice(const std::string &name) {
    auto iter = getDeviceIter(name);
    if (iter == devices.end())
        return -1;
    devices.erase(iter);
    return 0;
}
int device_list_t::removeDevice(int id) {
    std::unique_lock<std::shared_mutex> lock(mu);
    auto iter = getDeviceIter(id);
    if (iter == devices.end())
        return -1;
    devices.erase(iter);
    return 0;
}
void device_list_t::removeAllDevice() {
    std::unique_lock<std::shared_mutex> lock(mu);
    devices.clear();
}
int device_list_t::sendPacketOnDevice(int id, const void *buf, int size) {
    std::shared_lock<std::shared_mutex> lock(mu);
    auto iter = getDeviceIter(id);
    if (iter == devices.end())
        return -1;
    return iter->sendPacket(buf, size);
}
int device_list_t::getDeviceMAC(int id, void *buf) {
    std::shared_lock<std::shared_mutex> lock(mu);
    auto iter = mac_addr.find(id);
    if (iter == mac_addr.end())
        return -1;
    memcpy(buf, &(iter->second), 6);
    return 0;
}


device_list_t devices;

std::vector<std::string> listAvailableDevices() {
    DIR *net = Opendir("/sys/class/net");
    struct dirent *d;
    auto ret = std::vector<std::string>();
    while ((d = readdir(net)) != NULL) {
        const char *name = d->d_name;
        if (name[0] != 0 && name[0] != '.') {
            ret.emplace_back(name);
        }
    }
    return ret;
}

int addDevice(const char* device) {
    LOG(INFO, std::string("Trying to add device ") + device);
    int id;
    if ((id = devices.addDevice(device)) < 0)
        return -1;
    char buf[300];
    snprintf(buf, 300, "Device %s was added, id is %d", device, id);
    LOG(INFO, buf);
    return id;
}

int findDevice(const char* device) {
    return devices.findDevice(device);
}

int getFirstDevice() {
    return devices.getFirstDevice();
}

std::vector<int> getAllDevice() {
    return devices.getAllDevice();
}

int removeDevice(const char* device) {
    LOG(INFO, std::string("Trying to remove device ") + device);
    int ret = devices.removeDevice(device);
    if (ret == -1)
        return -1;
    LOG(INFO, std::string("Device ") + device + " was removed");
    return ret;
}

int removeDevice(int id) {
    char buf[200];
    sprintf(buf, "Trying to remove device of id %d", id);
    LOG(INFO, buf);
    int ret = devices.removeDevice(id);
    if (ret == -1)
        return -1;
    sprintf(buf, "Device of id %d was removed", id);
    LOG(INFO, buf);
    return devices.removeDevice(id);
}

void removeAllDevice() {
    LOG(INFO, "Trying to remove all devices");
    devices.removeAllDevice();
    LOG(INFO, "All devices are removed");
}

int sendPacketOnDevice(int id, const void *buf, int size) {
    return devices.sendPacketOnDevice(id, buf, size);
}

int getDeviceMAC(int id, void *buf) {
    return devices.getDeviceMAC(id, buf);
}
