#include <protocol/RouterTable.h>
#include <bitset>
#include <iostream>
#include <deque>
#include <set>
#include <utils/printutils.h>

RouterInfo::RouterInfo(uint16_t rid, const std::vector<IP>& portInfo,
                       const std::vector<uint16_t>& neighborRouter, uint32_t timestamp)
    : rid(rid), portInfo(portInfo), neighborRouter(neighborRouter), timestamp(timestamp) {}

RouterInfo::RouterInfo(const RouterInfo& ri) {
    rid = ri.rid;
    portInfo = ri.portInfo;
    neighborRouter = ri.neighborRouter;
    timestamp = ri.timestamp;
}

RouterInfo& RouterInfo::operator=(const RouterInfo& ri) {
    rid = ri.rid;
    portInfo = ri.portInfo;
    neighborRouter = ri.neighborRouter;
    timestamp = ri.timestamp;
    return *this;
}

Huffman::Huffman() {
    root = std::make_shared<HuffmanNode>();
}

void Huffman::insert(IP ip, int dev) {
    std::bitset<32> ipst(htonl32(ip.ip.s_addr));
    std::bitset<32> smst(htonl32(ip.subnet_mask.s_addr));
    auto ptr = root;
    for (int i = 31; i >= 0; --i) {
        if (smst.test(i)) {
            if (ipst.test(i)) {
                if (!ptr->child[1]) {
                    ptr->child[1] = std::make_shared<HuffmanNode>();
                }
                ptr = ptr->child[1];
            } else {
                if (!ptr->child[0]) {
                    ptr->child[0] = std::make_shared<HuffmanNode>();
                }
                ptr = ptr->child[0];
            }
        } else {
            ptr->nexthop_dev = std::make_unique<int>(dev);
            return;
        }
    }
    ptr->nexthop_dev = std::make_unique<int>(dev);
}

int Huffman::query(IP ip) {
    std::bitset<32> ipst(htonl32(ip.ip.s_addr));
    std::bitset<32> smst(htonl32(ip.subnet_mask.s_addr));
    int next_hop = -1;
    auto ptr = root;
    for (int i = 31; i >= 0; --i) {
        if (smst.test(i)) {
            if (ipst.test(i)) {
                if (!ptr->child[1]) {
                    return next_hop;
                }
                ptr = ptr->child[1];
            } else {
                if (!ptr->child[0]) {
                    return next_hop;
                }
                ptr = ptr->child[0];
            }
        } else {
            return next_hop;
        }
        if (ptr->nexthop_dev)
            next_hop = *(ptr->nexthop_dev);
    }
    return next_hop;
}

void Huffman::printTreeInternal(std::shared_ptr<HuffmanNode> r, uint32_t t, int depth) {
    if (r->nexthop_dev) {
        in_addr ip;
        ip.s_addr = htonl32(t << (32 - depth));
        printf("%s/%d: %d\n", inet_ntoa(ip), depth, *(r->nexthop_dev));
    }
    for (int i = 0; i < 2; ++i) {
        if (r->child[i]) {
            printTreeInternal(r->child[i], t * 2 + i, depth + 1);
        }
    }
}

void Huffman::printTree() {
    printTreeInternal(root, 0, 0);
}

int RouterTable::update(const RouterInfo& ri) {
    std::unique_lock<std::shared_mutex> lock(mu);
    auto iter = graph.find(ri.rid);
    if (iter != graph.end()) {
        if (iter->second.timestamp - ri.timestamp < 40000) {
            LOG(INFO, "Ignore old packet");
            return -1;
        }
        LOG(DEBUG, "Update");
        iter->second = ri;
        return 0;
    }
    LOG(DEBUG, "Update");
    graph.try_emplace(ri.rid, ri);
    if (start != -1)
        recomputeDijkstra();
    return 0;
}

void RouterTable::updateStartPoint(int port, const std::set<uint16_t>& neighborVec) {
    std::unique_lock<std::shared_mutex> lock(mu);
    startpoint[port] = neighborVec;
    if (start != -1)
        recomputeDijkstra();
}

void RouterTable::recomputeDijkstra() {
    if (start == -1) {
        LOG(WARNING, "RID not set");
        return;
    }
    tree = Huffman();
    std::set<uint16_t> visited;
    visited.insert(start);
    std::deque<std::pair<int, uint16_t>> q;
    for (const auto& p: startpoint)
        for (uint16_t r: p.second)
            q.push_back(std::make_pair(p.first, r));
    while (!q.empty()) {
        auto t = q.front();
        q.pop_front(); 
        int port = t.first;
        uint16_t router = t.second;
        if (visited.find(router) != visited.end()) {
            continue;
        }
        visited.insert(router);
        auto iter = graph.find(router);
        if (iter == graph.end())
            continue;
        for (const IP& i: iter->second.portInfo) {
            tree.insert(i, port);
        }
        for (uint16_t n: iter->second.neighborRouter) {
            q.push_back(std::make_pair(port, n));
        }
    }
}

int RouterTable::query(IP ip) {
    std::shared_lock<std::shared_mutex> lock(mu);
    if (start == -1)
        return -1;
    return tree.query(ip);
}

void RouterTable::setStart(int self) {
    std::shared_lock<std::shared_mutex> lock(mu);
    start = self;
}

bool RouterTable::isNewer(uint16_t rid, uint32_t timestamp) {
    std::shared_lock<std::shared_mutex> lock(mu);
    auto iter = graph.find(rid);
    if (iter == graph.end())
        return true;
    return iter->second.timestamp - timestamp >= 40000;
}

void RouterTable::printRouterTable() {
    std::shared_lock<std::shared_mutex> lock(mu);
    printf("-------- routing table ---------\n");
    tree.printTree();
    printf("----- end of routing table -----\n");
}

void RouterTable::printGraph() {
    std::shared_lock<std::shared_mutex> lock(mu);
    printf("------------- graph -------------\n");
    printf("startpoint:\n");
    for (const auto& x: startpoint) {
        printf("  device: %d\n", x.first);
        printf("    adjwith:\n");
        for (uint16_t y: x.second) {
            printf("      %d\n", y);
        }
    }
    printf("nodes:\n");
    for (const auto& x: graph) {
        printf("  router ID: %d\n", x.first);
        printf("    ip:\n");
        for (const auto& y: x.second.portInfo) {
            printIP(&y);
            printf("\n");
        }
        printf("    adjwith:\n");
        for (const auto& y: x.second.neighborRouter) {
            printf("      %d\n", y);
        }
    }
    printf("---------- end of graph ---------\n");
}
