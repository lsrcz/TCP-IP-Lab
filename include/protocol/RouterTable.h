#ifndef ROUTER_TABLE_H
#define ROUTER_TABLE_H

#include <memory>
#include <vector>
#include <utils/netutils.h>
#include <map>
#include <set>
#include <shared_mutex>

struct RouterInfo {
    uint16_t rid;
    std::vector<IP> portInfo;
    std::vector<uint16_t> neighborRouter;
    uint32_t timestamp;
    RouterInfo(uint16_t rid, const std::vector<IP>& portInfo,
               const std::vector<uint16_t>& neighborRouter, uint32_t timestamp);
    RouterInfo(const RouterInfo& ri);
    RouterInfo& operator=(const RouterInfo& ri);
};

struct HuffmanNode {
    std::shared_ptr<HuffmanNode> child[2];
    std::unique_ptr<in_addr> nexthop;
    std::unique_ptr<int> port;
};

class Huffman {
    std::shared_ptr<HuffmanNode> root;
    void printTreeInternal(std::shared_ptr<HuffmanNode> r, uint32_t t, int depth);
 public:
    Huffman();
    void insert(IP ip, in_addr nexthop, int port);
    int query(in_addr ip, in_addr *nexthop);
    void printTree();
};


class RouterTable {
    int start = -1;
    std::map<uint16_t, RouterInfo> graph;
    std::map<int, std::set<std::pair<uint16_t, in_addr>>> startpoint;
    Huffman tree;
    std::shared_mutex mu;
 public:
    void printRouterTable();
    void printGraph();
    void setStart(int self);
    int update(const RouterInfo&);
    void updateStartPoint(int, const std::set<std::pair<uint16_t, in_addr>>& neighborVec);
    int query(in_addr ip, in_addr* nexthop);
    void recomputeDijkstra();
    bool isNewer(uint16_t rid, uint32_t timestamp);
};


#endif // ROUTER_TABLE_H
