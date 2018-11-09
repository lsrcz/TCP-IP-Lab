#include <protocol/Router.h>
#include <arpa/inet.h>
#include <cstdint>
#include <utils/netutils.h>
#include <utils/timeutils.h>
#include <utils/errorutils.h>
#include <utils/threadsafeQueue.h>
#include <protocol/ip.h>
#include <protocol/RouterProtocol.h>
#include <map>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <deque>

const in_addr Router::drip = std::invoke([](){
        in_addr ret;
        if (inet_aton("224.0.0.121", &ret) < -1) {
            printf("This should not happen");
            exit(-1);
        }
        return ret;
    });

const in_addr Router::allip = std::invoke([](){
        in_addr ret;
        if (inet_aton("224.0.0.120", &ret) < -1) {
            printf("This should not happen");
            exit(-1);
        }
        return ret;
    });

int Router::addDevice(int dev) {
    std::unique_lock<std::shared_mutex> lock(mu);
    if (devs.find(dev) != devs.end())
        return 0;
    char buf[200];
    snprintf(buf, 200, "Trying to add the device to router %d", dev);
    LOG(INFO, buf);
    ErrorBehavior eb("Failed to add device to the router", false, true);
    if (devs[dev].startDevice(dev) < 0) {
        auto iter = devs.find(dev);
        if (iter == devs.end()) {
            devs.erase(iter);
            ERROR_WITH_BEHAVIOR(eb, return -1);
        }
    }
    snprintf(buf, 200, "The device %d was added to router", dev);
    return 0;
}

int Router::controlPacketRecv(const void* buf, int len, int id) {
    std::shared_lock<std::shared_mutex> lock(mu);
    ErrorBehavior eb("", false, true);
    auto iter = devs.find(id);
    char errbuf[200];
    if (iter == devs.end()) {
        snprintf(errbuf, 200, "Device %d is not connected to a port of the router", id);
        eb.msg = errbuf;
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    RouterPort& port = iter->second;
    if (port.getState() == RouterPort::UNINITIALIZED) {
        snprintf(errbuf, 200, "Device %d is down as a port of the router", id);
        eb.msg = errbuf;
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    if (len < ROUTER_HEADER_LEN) {
        eb.msg = "Broken route control packet";
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    const ip* iphdr = (const ip*)buf;
    int ip_header_len = iphdr->ip_hl * 4;
    const RouterHeader *header = (const RouterHeader*)(((uint8_t*)buf) + ip_header_len);
    uint16_t route_packet_len = htonl16(header->len);
    printf("%d, %d\n", ip_header_len, route_packet_len);
    if (htonl16(header->len) + ip_header_len != len) {
        eb.msg = "Route control packet length error";
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    if (chksum((uint8_t*)header, route_packet_len) != 0) {
        eb.msg = "Wrong route control packet checksum";
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    snprintf(errbuf, 200, "Control packet header verified on %d", id);
    LOG(DEBUG, errbuf);
    // should be well-formed
    if (header->type == HELLO) {
        return port.recvHelloPacket(buf, len);
    } else  {
        eb.msg = "Not implemented error";
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    return 0;
}
