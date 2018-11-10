#ifndef ROUTER_TABLE_H
#define ROUTER_TABLE_H

#include <memory>
#include <vector>
#include <utils/netutils.h>
#include <map>
#include <set>

struct RouterInfo {
    uint16_t rid;
    std::vector<IP> portInfo;
    std::vector<uint16_t> neighborRouter;
    RouterInfo(uint16_t rid, const std::vector<IP>& portInfo,
               const std::vector<uint16_t>& neighborRouter);
    RouterInfo(const RouterInfo& ri) {
        rid = ri.rid;
        portInfo = ri.portInfo;
        neighborRouter = ri.neighborRouter;
    }
};

struct HuffmanNode {
    std::shared_ptr<HuffmanNode> child[2];
    std::unique_ptr<int> nexthop_dev;
};

class Huffman {
    std::shared_ptr<HuffmanNode> root;
    void printTreeInternal(std::shared_ptr<HuffmanNode> r, std::string s);
 public:
    Huffman();
    void insert(IP ip, int dev);
    int query(IP ip);
    void printTree();
};


class RouterTable {
    int start;
    std::map<uint16_t, RouterInfo> graph;
    std::map<int, std::vector<uint16_t>> startpoint;
    Huffman tree;
    void recomputeDijkstra();
 public:
    RouterTable(int);
    void update(const RouterInfo&);
    void updateStartPoint(int, const std::vector<uint16_t>& neighborVec);
    int query(IP ip);
};


#endif // ROUTER_TABLE_H
