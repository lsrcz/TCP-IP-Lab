#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <arpa/inet.h>
#include <protocol/RouterPort.h>

class Router {
    std::map<int, RouterPort> devs;
    std::shared_mutex mu;
 public:
    static const in_addr drip;
    static const in_addr allip;
    int addDevice(int dev);
    int controlPacketRecv(const void* buf, int len, int id);
};

#endif // ROUTER_H
