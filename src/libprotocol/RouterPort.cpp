#include <protocol/Router.h>
#include <protocol/RouterPort.h>
#include <protocol/RouterProtocol.h>
#include <protocol/ip.h>
#include <utils/errorutils.h>
#include <utils/netutils.h>
#include <utils/printutils.h>
#include <wrapper/pthread.h>
#include <wrapper/unix_file.h>

void RouterPort::resetDevice() {
    portState = UNINITIALIZED;
    destroyThreads();
    resetDRBDR();
    neighborTable.clear();
    taskQueue.clear();
}

RouterPort::~RouterPort() {
    destroyThreads();
}

void RouterPort::resetDRBDR() {
    memset(&dr, 0, sizeof(dr));
    memset(&bdr, 0, sizeof(bdr));
}
/*
void RouterPort::initBDUpdateThread() {
    if (bdUpdateThread.joinable()) {
        ErrorBehavior eb("The BD update thread was initialized before...this
should not happen", true, true); ERROR_WITH_BEHAVIOR(eb, exit(-1));
    }
    bdUpdateThread = std::thread([&]() {
            std::unique_lock<std::shared_mutex> lock(statemu);
            bdcv.wait(statemu);
            bdcv.wait_for(statemu, std::chrono::seconds(40));
            if (portState == ELECTING) {
                portState = DR;
            }
            hlcv.notify_all();
        });
}

void RouterPort::destroyBDUpdateThread() {
    ErrorBehavior eb("BD update thread was died", false, false);
    if (bdUpdateRoutine.joinable()) {
        pthread_t p = bdUpdateRoutine.native_handle();
        Pthread_cancel(p, eb);
        bdUpdateRoutine.join();
    } else {
        LOG(WARNING, "BD update thread is not running");
    }
    }*/

void RouterPort::initHelloThread() {
    if (helloThread.joinable()) {
        ErrorBehavior eb(
            "The hello thread was initialized before...this should not happen",
            true, true);
        ERROR_WITH_BEHAVIOR(eb, exit(-1));
    }
    helloThread = std::thread([&]() {
        std::unique_lock<std::mutex> lock(hlmu);
        while (!shouldStop) {
            if (hlcv.wait_for(hlmu, std::chrono::seconds(10))
                == std::cv_status::timeout) {
                this->sendHelloPacket();
            }
        }
    });
}

void RouterPort::destroyThreads() {
    shouldStop = true;
    if (helloThread.joinable()) {
        // pthread_t p = helloThread.native_handle();
        // Pthread_cancel(p, eb);
        hlcv.notify_all();
        helloThread.join();
    } else {
        LOG(WARNING, "Hello thread is not running");
    }
    if (taskThread.joinable()) {
        taskQueue.push([]() { return; });
        taskThread.join();
    } else {
        LOG(WARNING, "Task thread is not running");
    }
    shouldStop = false;
}

void RouterPort::initTaskThread() {
    if (taskThread.joinable()) {
        ErrorBehavior eb(
            "The task thread was initialized before...this should not happen",
            true, true);
        ERROR_WITH_BEHAVIOR(eb, exit(-1));
    }
    taskThread = std::thread([&]() {
        std::shared_ptr<std::function<void(void)>> task;
        while (!shouldStop) {
            task = taskQueue.waitAndPop();
            std::invoke(*task);
        }
    });
}

void RouterPort::printRouterStatus() {
    printf("--------- router port status --------\n");
    printf("state: ");
    switch (portState) {
    case UNINITIALIZED:
        printf("unintialized\n");
        break;
    case DOWN:
        printf("down\n");
        break;
    case STARTING:
        printf("starting\n");
        break;
    case DR:
        printf("designated router\n");
        break;
    case BDR:
        printf("backup designated router\n");
        break;
    case DROTHER:
        printf("not designated router\n");
        break;
    }
    printf("device: %d\n", device);
    printf("IP: ");
    printIP(&portIP.ip);
    printf("\n");
    printf("subnet mask: ");
    printIP(&portIP.subnet_mask);
    printf("\n");
    printf("idea of DR: ");
    printIP(&dr);
    printf("\n");
    printf("idea of bdr: ");
    printIP(&bdr);
    printf("\n");
    printf("num of neighbors: %ld\n", neighborTable.size());
    printf("neighbors list: \n");
    for (const auto& n : neighborTable) {
        n.print();
    }
    printf("-------------------------------------\n");
}

void RouterPort::calculateDRBDR() {
    // no lock
    auto iter = neighborTable.begin();
    dr        = iter->ip;
    bdr       = (++iter)->ip;
    /*for (const auto &n: neighborTable) {
        if (n.ip.s_addr > dr.s_addr) {
            bdr = dr;
            dr = n.ip;
        } else if (n.ip.s_addr > bdr.s_addr) {
            bdr = n.ip;
        }
        }*/
    if (dr.s_addr == portIP.ip.s_addr) {
        portState = DR;
    } else if (bdr.s_addr == portIP.ip.s_addr) {
        portState = BDR;
    } else {
        portState = DROTHER;
    }
}

RouterPort::RouterPort(Router& router) : router(router) {}

int RouterPort::startDevice(int id) {
    std::unique_lock<std::shared_mutex> lockst(statemu);
    char                                buf[200];
    snprintf(buf, 200, "Failed to start the router port %d", id);
    ErrorBehavior eb(buf, false, true);
    device = id;
    if (getDeviceIP(device, &portIP) < 0) {
        ERROR_WITH_BEHAVIOR(eb, return -1);
    }
    std::unique_lock<std::shared_mutex> lock(ntmu);
    resetDevice();
    RouterNeighbor n(router.getRID(), portIP.ip);
    n.isSelf = true;
    n.state  = RouterNeighbor::TWOWAY;
    neighborTable.insert(std::move(n));
    initHelloThread();
    initTaskThread();
    resetDRBDR();
    portState = DOWN;
    return 0;
}

int RouterPort::sendHelloPacket() {
    LOG(INFO, "Trying to send hello packet");
    if (portState == UNINITIALIZED) {
        LOG(ERROR, "Port unintialized");
        return -1;
    }
    RouterHeader hdr;
    hdr.type     = HELLO;
    hdr.checksum = 0;
    if (hdr.setIPByID(device) < 0) {
        LOG(ERROR, "Cannot get IP");
        return -1;
    }
    in_addr dest;
    inet_aton("224.0.0.120", &dest);
    uint16_t len = HELLO_HEADER_LEN + neighborTable.size() * 6;
    hdr.len      = htonl16(len);
    hdr.rid      = htonl16(router.getRID());
    uint8_t buf[hdr.len];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &hdr, ROUTER_HEADER_LEN);
    memcpy(buf + ROUTER_HEADER_LEN, &dr, 4);
    memcpy(buf + ROUTER_HEADER_LEN + 4, &bdr, 4);
    int                       i = 0;
    helloNeighborInformation* neighbor_info =
        (helloNeighborInformation*)(buf + HELLO_HEADER_LEN);
    for (auto const& n : neighborTable) {
        neighbor_info[i].ip  = n.ip;
        neighbor_info[i].rid = htonl16(n.rid);
        ++i;
    }
    hdr.checksum = htonl16(chksum(buf, len));
    memcpy(buf, &hdr, ROUTER_HEADER_LEN);
    char errbuf[200];
    snprintf(errbuf, 200, "Neighbor table size: %ld, len: %d",
             neighborTable.size(), len);
    LOG(INFO, errbuf);
    return sendIPPacket(hdr.ip, dest, ROUTER_PROTO_NUM, buf, len);
}

int RouterPort::recvHelloPacket(const void* packet, int len) {
    if (portState == UNINITIALIZED) {
        return -1;
    }
    const ip*           ipheader      = (const ip*)packet;
    int                 ip_header_len = ipheader->ip_hl * 4;
    const RouterHeader* header =
        (const RouterHeader*)(((uint8_t*)packet) + ip_header_len);
    const HelloPacket* hheader =
        (const HelloPacket*)(((uint8_t*)packet) + ip_header_len);
    uint16_t hellopacketLen = htonl16(header->len);
    uint16_t numOfList      = (hellopacketLen - HELLO_HEADER_LEN) / 6;
    helloNeighborInformation* neighborListHello =
        (helloNeighborInformation*)(((uint8_t*)header) + HELLO_HEADER_LEN);
    LOG(INFO, "Hello packet received");
    if (header->subnetMask.s_addr != portIP.subnet_mask.s_addr) {
        char buf[200];
        snprintf(buf, 200, "Hello packet for different subnet on %d", device);
        LOG(WARNING, buf);
        return 0;
    } else {
        in_addr               from = header->ip;
        uint16_t              rid  = htonl16(header->rid);
        RouterNeighbor        n(rid, from);
        RouterNeighbor::STATE state = RouterNeighbor::ONEWAY;
        {
            std::unique_lock<std::shared_mutex> lock(ntmu);

            for (int i = 0; i < numOfList; ++i) {
                RouterNeighbor n1(htonl16(neighborListHello[i].rid),
                                  neighborListHello[i].ip);
                if (n1.ip.s_addr == portIP.ip.s_addr) {
                    state = RouterNeighbor::TWOWAY;
                } else {
                    auto iter = neighborTable.find(n1);
                    if (iter == neighborTable.end()) {
                        RouterNeighbor::STATE s = RouterNeighbor::ONEWAY;
                        n1.update(*this, NULL, NULL, &s);
                        neighborTable.insert(std::move(n1));
                    }
                }
            }
        }

        if (portState == DOWN) {
            std::unique_lock<std::shared_mutex> lock1(ntmu);
            n.update(*this, &hheader->dr, &hheader->bdr, &state);
            neighborTable.insert(std::move(n));
            bool all = true;
            for (const auto& n : neighborTable) {
                all = all && (n.state == RouterNeighbor::TWOWAY);
            }
            std::unique_lock<std::shared_mutex> lock(statemu);
            if (all) {
                calculateDRBDR();
                // bdUpdateThread = std::thread(bdUpdateRoutine);
            } else {
                portState = STARTING;
            }
        } else {
            std::unique_lock<std::shared_mutex> lock1(ntmu);
            auto                                iter = neighborTable.find(n);
            if (iter != neighborTable.end()) {
                neighborTable.erase(iter);
            }
            n.update(*this, &hheader->dr, &hheader->bdr, &state);
            neighborTable.insert(std::move(n));

            bool all = true;
            for (const auto& n : neighborTable) {
                all = all && (n.state == RouterNeighbor::TWOWAY);
            }
            if (all) {
                std::unique_lock<std::shared_mutex> lock(statemu);
                calculateDRBDR();
                // bdUpdateThread = std::thread(bdUpdateRoutine);
            }
        }
    }
    return 0;
}

RouterPort::STATE RouterPort::getState() {
    return portState;
}

void RouterPort::removeNeighbor(in_addr ip) {
    std::unique_lock<std::shared_mutex> lock(ntmu);
    RouterNeighbor                      n(0, ip);
    auto                                iter = neighborTable.find(n);
    if (iter != neighborTable.end()) {
        neighborTable.erase(iter);
        lock.unlock();
        router.tbl.updateStartPoint(device, getNeighborRIDIP());
    }
}

void RouterPort::addTask(std::function<void(void)> func) {
    taskQueue.push(std::move(func));
}

std::set<IP> RouterPort::getNeighborInformation() const {
    std::set<IP> ret;
    for (const auto& n : neighborTable) {
        ret.insert({ n.ip, portIP.subnet_mask });
    }
    return ret;
}

int RouterPort::sendLinkStatePacket(const std::vector<IP>&       portInfo,
                                    const std::vector<uint16_t>& neighborInfo) {
    LinkstatePacket lp;
    RouterHeader&   hdr = lp.hdr;
    hdr.type            = LINKSTATE;
    uint16_t len =
        LINKSTATE_HEADER_LEN + portInfo.size() * 8 + neighborInfo.size() * 2;
    hdr.len        = htonl16(len);
    hdr.ip         = portIP.ip;
    hdr.subnetMask = portIP.subnet_mask;
    hdr.checksum   = 0;
    lp.rid         = htonl16(router.getRID());
    lp.timestamp   = htonl32((uint32_t)getTimeStamp());
    lp.nop         = htonl16((uint16_t)portInfo.size());
    lp.non         = htonl16((uint16_t)neighborInfo.size());

    uint8_t buf[len];
    memcpy(buf, &lp, LINKSTATE_HEADER_LEN);
    memcpy(buf + LINKSTATE_HEADER_LEN, portInfo.data(),
           portInfo.size() * sizeof(IP));
    memcpy(buf + LINKSTATE_HEADER_LEN + portInfo.size() * 8,
           neighborInfo.data(), neighborInfo.size() * sizeof(uint16_t));
    ((RouterHeader*)buf)->checksum = htonl16(chksum(buf, len));
    // originally designed for broadcast network
    /*in_addr dest;
      {
      std::shared_lock<std::shared_mutex> lock(stmu);
      if (state == DROTHER)
      inet_aton("224.0.0.121", &dest);
      else
      inet_aton("224.0.0.120", &dest);
      }*/
    in_addr dest;
    inet_aton("224.0.0.120", &dest);
    sendIPPacket(hdr.ip, dest, ROUTER_PROTO_NUM, buf, len);
    return 0;
}

IP RouterPort::getIP() {
    return portIP;
}

std::set<uint16_t> RouterPort::getNeighborRID() {
    std::set<uint16_t>                  ret;
    std::shared_lock<std::shared_mutex> lock(ntmu);
    for (auto& n : neighborTable) {
        ret.insert(n.rid);
    }
    return ret;
}

std::set<std::pair<uint16_t, in_addr>> RouterPort::getNeighborRIDIP() {
    std::set<std::pair<uint16_t, in_addr>> ret;
    std::shared_lock<std::shared_mutex>    lock(ntmu);
    for (auto& n : neighborTable) {
        ret.insert(std::make_pair(n.rid, n.ip));
    }
    return ret;
}
