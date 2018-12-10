#ifndef ROUTER_H
#define ROUTER_H

#include <arpa/inet.h>
#include <condition_variable>
#include <map>
#include <mutex>
#include <protocol/RouterPort.h>
#include <protocol/RouterTable.h>
#include <thread>

class Router {
    friend class RouterPort;
    std::map<int, RouterPort> devs;
    std::unique_ptr<uint16_t> rid;
    std::shared_mutex         mu;
    std::thread               linkstateThread;
    std::mutex                lsmu;
    std::condition_variable   lscv;
    RouterTable               tbl;

public:
    Router();
    void clearRouter();
    ~Router();
    static const in_addr drip;
    static const in_addr allip;
    int                  addDevice(int dev);
    int                  controlPacketRecv(const void* buf, int len, int id);
    int                  otherPacketRecv(const void* buf, int len, int id);
    int                  linkstatePacketRecv(const void* buf, int len, int id);
    int                  sendLinkStatePacket();
    uint16_t             getRID();
    void                 setRID(uint16_t);
    int                  query(in_addr ip, in_addr* nexthop);
};

extern Router router;
int           addRouterDev(int dev);
void          setRouterRID(uint16_t rid);
void          cleanupRouter();

#endif  // ROUTER_H
