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
#include <wrapper/pthread.h>

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
    if (!rid) {
        ErrorBehavior eb("Router not initialized", false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    std::unique_lock<std::shared_mutex> lock(mu);
    auto iter = devs.find(dev);
    if (devs.find(dev) != devs.end())
        return 0;
    char buf[200];
    snprintf(buf, 200, "Trying to add the device to router %d", dev);
    LOG(INFO, buf);
    ErrorBehavior eb("Failed to add device to the router", false, true);
    auto itert = devs.try_emplace(dev, *this);
    if (!itert.second) {
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    if (itert.first->second.startDevice(dev) < 0) {
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
    LOG(DEBUG, "Control packet");
    if (!rid) {
        ErrorBehavior eb("Router not initialized", false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
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
        if (port.recvHelloPacket(buf, len) < 0) {
            return -1;
        }
        for (auto x: port.getNeighborRID()) {
            printf("rid: %d\n", x);
        }
        tbl.updateStartPoint(id, port.getNeighborRID());
    } else {
        return linkstatePacketRecv(buf, len, id);
    }
    return 0;
}

int Router::linkstatePacketRecv(const void* buf, int len, int id) {
    LOG(INFO, "Link state packet received");
    const ip* ipheader = (const ip*)buf;
    int ip_header_len = ipheader->ip_hl * 4;
    const RouterHeader *rtheader = (const RouterHeader*)(((uint8_t*)buf) + ip_header_len);
    const LinkstatePacket *lkheader = (const LinkstatePacket*)rtheader;
    uint16_t rid = htonl16(lkheader->rid);
    uint16_t nop = htonl16(lkheader->nop);
    uint16_t non = htonl16(lkheader->non);
    uint32_t timestamp = htonl32(lkheader->timestamp);

    IP* portList = (IP*)(lkheader + 1);
    uint16_t* neighborList = (uint16_t*)(portList + nop);

    printf("Before\n");
    tbl.printRouterTable();
    tbl.printGraph();
    if (!tbl.isNewer(rid, timestamp)) {
        LOG(INFO, "Ignore old packet");
        return 0;
    }

    std::vector<IP> portInfo(portList, portList + nop);
    std::vector<uint16_t> neighborInfo(neighborList, neighborList + non);
    for (auto i: portInfo) {
        printIP(&i.ip);
        printf("/");
        printIP(&i.subnet_mask);
        printf("\n\n");
    }
    for (auto i: neighborInfo) {
        printf("%d\n", i);
    }
    RouterInfo ri(rid, portInfo, neighborInfo, timestamp);
    tbl.update(ri);
    printf("After\n");
    tbl.printRouterTable();
    tbl.printGraph();

    for (auto& d: devs) {
        if (d.first != id) {
            IP i = d.second.getIP();
            len = htonl16(rtheader->len);
            uint8_t buf[len];
            RouterHeader *rh = (RouterHeader*)buf;
            memcpy(buf, rh, len);
            rh->ip = i.ip;
            rh->subnetMask = i.subnet_mask;
            rh->checksum = 0;
            rh->rid = htonl16(rid);
            rh->checksum = htonl16(chksum((uint8_t*)rh, len));
            sendIPPacket(d.second.getIP().ip, allip, ROUTER_PROTO_NUM, lkheader, len);
        }
    }

    return 0;
}

int Router::sendLinkStatePacket() {
    LOG(INFO, "Trying to send link state packet");
    if (!rid) {
        ErrorBehavior eb("Router not initialized", false, true);
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    std::unique_lock<std::shared_mutex> lock(mu);
    std::vector<IP> vec;
    for (auto &d: devs) {
        vec.push_back(d.second.getIP());
    }
    std::set<uint16_t> s;
    for (auto &d: devs) {
        for (uint16_t n: d.second.getNeighborRID()) {
            s.insert(n);
        }
    }
    std::vector<uint16_t> v;
    for (uint16_t n: s) {
        v.push_back(n);
    }

    for (auto &d: devs) {
        if (d.second.sendLinkStatePacket(vec, v) < 0) {
            char buf[200];
            snprintf(buf, 200, "Error sending link state packet on device %d", d.first);
            ErrorBehavior eb(buf,false,false);
            ERROR_WITH_BEHAVIOR(eb,return -1;);
        }
    }
    LOG(INFO, "Link state packet sent");
    return 0;
}

Router::Router() {
    linkstateThread = std::thread( [&]() {
            std::unique_lock<std::mutex> lock(lsmu);
            while (true) {
                lscv.wait_for(lock, std::chrono::seconds(20));
                this->sendLinkStatePacket();
            }
        });
}

Router::~Router() {
    if (linkstateThread.joinable()) {
        pthread_t p = linkstateThread.native_handle();
        Pthread_cancel(p);
        linkstateThread.join();
    }
}

uint16_t Router::getRID() {
    if (rid)
        return *rid;
    else
        return 0xffff;
}

void Router::setRID(uint16_t rid) {
    tbl.setStart(rid);
    this->rid = std::make_unique<uint16_t>(rid);
}
