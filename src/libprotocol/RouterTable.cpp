#include <protocol/RouterTable.h>
#include <bitset>
#include <iostream>
#include <deque>
#include <set>
#include <utils/printutils.h>

RouterInfo::RouterInfo(uint16_t rid, const std::vector<IP>& portInfo,
                       const std::vector<uint16_t>& neighborRouter)
    : rid(rid), portInfo(portInfo), neighborRouter(neighborRouter) {}

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

void Huffman::printTreeInternal(std::shared_ptr<HuffmanNode> r, std::string s) {
    if (r->nexthop_dev) {
        printf("%s: %d\n", s.c_str(), *(r->nexthop_dev));
    }
    for (int i = 0; i < 2; ++i) {
        if (r->child[i]) {
            s.push_back(i + '0');
            printTreeInternal(r->child[i], s);
            s.pop_back();
        }
    }
}

void Huffman::printTree() {
    printTreeInternal(root, "");
}

void RouterTable::update(const RouterInfo& ri) {
    graph.try_emplace(ri.rid, ri);
    recomputeDijkstra();
}

void RouterTable::updateStartPoint(int port, const std::vector<uint16_t>& neighborVec) {
    startpoint[port] = neighborVec;
    recomputeDijkstra();
}

void RouterTable::recomputeDijkstra() {
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
        for (IP i: iter->second.portInfo) {
            tree.insert(i, port);
        }
        for (uint16_t n: iter->second.neighborRouter) {
            q.push_back(std::make_pair(port, n));
        }
    }
}

int RouterTable::query(IP ip) {
    return tree.query(ip);
}

RouterTable::RouterTable(int self) {
    start = self;
}
