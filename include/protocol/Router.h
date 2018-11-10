#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <arpa/inet.h>
#include <protocol/RouterPort.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <protocol/RouterTable.h>

class Router {
    std::map<int, RouterPort> devs;
    std::unique_ptr<uint16_t> rid;
    std::shared_mutex mu;
    std::thread linkstateThread;
    std::mutex lsmu;
    std::condition_variable lscv;
    RouterTable tbl;
 public:
    Router();
    ~Router();
    static const in_addr drip;
    static const in_addr allip;
    int addDevice(int dev);
    int controlPacketRecv(const void* buf, int len, int id);
    int linkstatePacketRecv(const void* buf, int len, int id);
    int sendLinkStatePacket();
    uint16_t getRID();
    void setRID(uint16_t);
};

#endif // ROUTER_H
