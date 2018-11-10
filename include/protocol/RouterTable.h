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
    uint32_t timestamp;
    RouterInfo(uint16_t rid, const std::vector<IP>& portInfo,
               const std::vector<uint16_t>& neighborRouter, uint32_t timestamp);
    RouterInfo(const RouterInfo& ri);
    RouterInfo& operator=(const RouterInfo& ri);
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
    std::shared_mutex mu;
 public:
    RouterTable(int);
    int update(const RouterInfo&);
    void updateStartPoint(int, const std::vector<uint16_t>& neighborVec);
    int query(IP ip);
};


#endif // ROUTER_TABLE_H
