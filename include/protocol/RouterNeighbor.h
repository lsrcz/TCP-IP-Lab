#ifndef ROUTER_NEIGHBOR_H
#define ROUTER_NEIGHBOR_H

#include <utils/timeutils.h>
#include <utils/threadsafeQueue.h>
#include <protocol/RouterPort.h>
#include <thread>

class RouterPort;

struct RouterNeighbor {
    in_addr ip;
    in_addr dr;
    in_addr bdr;
    enum STATE: uint32_t {
        ONEWAY,
        TWOWAY
    } state;
    std::mutex mu;
    Timer timeout;
    bool isSelf = false;
    RouterNeighbor(in_addr);
    int update(RouterPort& rp, const in_addr* dr, const in_addr* bdr, const STATE* st);
    bool operator<(const RouterNeighbor& rhs) const;
    bool operator==(const RouterNeighbor& rhs) const;
    RouterNeighbor(RouterNeighbor&& rhs);
    RouterNeighbor& operator=(RouterNeighbor&& rhs);
    void print() const;
};

#endif // ROUTER_NEIGHBOR_H
