#ifndef ROUTER_NEIGHBOR_H
#define ROUTER_NEIGHBOR_H

#include <protocol/RouterPort.h>
#include <thread>
#include <utils/threadsafeQueue.h>
#include <utils/timeutils.h>

class RouterPort;

struct RouterNeighbor {
    uint16_t rid;
    in_addr  ip;
    in_addr  dr;
    in_addr  bdr;
    enum STATE : uint32_t { ONEWAY, TWOWAY } state;
    std::mutex mu;
    Timer      timeout;
    bool       isSelf = false;
    RouterNeighbor(uint16_t, in_addr);
    int  update(RouterPort& rp, const in_addr* dr, const in_addr* bdr,
                const STATE* st);
    bool operator<(const RouterNeighbor& rhs) const;
    bool operator==(const RouterNeighbor& rhs) const;
    RouterNeighbor(RouterNeighbor&& rhs);
    RouterNeighbor& operator=(RouterNeighbor&& rhs);
    void            print() const;
};

#endif  // ROUTER_NEIGHBOR_H
