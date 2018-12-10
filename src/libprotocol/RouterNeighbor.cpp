#include <chrono>
#include <protocol/RouterNeighbor.h>
#include <string>
#include <utils/printutils.h>

RouterNeighbor::RouterNeighbor(uint16_t rid, in_addr ip) : rid(rid), ip(ip) {
    in_addr zip;
    memset(&zip, 0, sizeof(zip));
    dr = bdr = zip;
    state    = ONEWAY;
}

int RouterNeighbor::update(RouterPort& rp, const in_addr* dr,
                           const in_addr* bdr, const STATE* st) {
    if (isSelf)
        return 0;
    std::lock_guard<std::mutex> lock(mu);
    if (dr) {
        this->dr = *dr;
    }
    if (bdr) {
        this->bdr = *bdr;
    }
    if (st) {
        this->state = *st;
    }
    using namespace std::chrono_literals;
    std::function<void(void)> task = [& rp = rp, ip = ip]() {
        std::function<void(void)> innertask = [& rp = rp, ip = ip]() {
            rp.removeNeighbor(ip);
        };
        rp.addTask(std::move(innertask));
    };
    // std::function<void(void)> task1 = task;
    // std::invoke(task1);
    // rp.removeNeighbor(ip);
    timeout.setTimer(40s, std::move(task));
    return 0;
}

bool RouterNeighbor::operator<(const RouterNeighbor& rhs) const {
    return ip.s_addr < rhs.ip.s_addr;
}

bool RouterNeighbor::operator==(const RouterNeighbor& rhs) const {
    return ip.s_addr == rhs.ip.s_addr;
}

RouterNeighbor::RouterNeighbor(RouterNeighbor&& rhs)
    : rid(rhs.rid), ip(rhs.ip), dr(rhs.dr), bdr(rhs.bdr), state(rhs.state),
      timeout(std::move(rhs.timeout)), isSelf(rhs.isSelf) {}

RouterNeighbor& RouterNeighbor::operator=(RouterNeighbor&& rhs) {
    if (this == &rhs)
        return *this;
    rid     = rhs.rid;
    ip      = rhs.ip;
    dr      = rhs.dr;
    bdr     = rhs.bdr;
    state   = rhs.state;
    timeout = std::move(rhs.timeout);
    isSelf  = rhs.isSelf;
    return *this;
}

void RouterNeighbor::print() const {
    printf("Neighbor: ");
    printIP(&ip);
    if (isSelf)
        printf("(self)");
    printf("\n");
    printf("  state: ");
    if (state == ONEWAY) {
        printf("oneway\n");
    } else {
        printf("twoway\n");
    }
    printf("  idea of DR: ");
    printIP(&dr);
    printf("\n");
    printf("  idea of BDR: ");
    printIP(&bdr);
    printf("\n");
}
