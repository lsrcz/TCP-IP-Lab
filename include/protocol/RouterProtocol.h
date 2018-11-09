#ifndef ROUTER_PROTOCOL_H
#define ROUTER_PROTOCOL_H
#include <type_traits>
#include <arpa/inet.h>
#include <utils/errorutils.h>

enum routerPacketType: uint8_t {
    HELLO,
    LINKSTATE,
    ACK
};

#define ROUTER_HEADER_LEN 16
#define ROUTER_PROTO_NUM 254

/**
 * packet header
 * +------------+------------+------------+------------+
 * |    type    |  padding   |          length         |
 * +------------+------------+------------+------------+
 * |                        ip                         |
 * +------------+------------+------------+------------+
 * |                   subnet mask                     |
 * +------------+------------+------------+------------+
 * |          checksum       |         padding         |
 * +------------+------------+------------+------------+
 */
struct RouterHeader {
    routerPacketType type;
    uint8_t padding;
    uint16_t len;
    in_addr ip;
    in_addr subnetMask;
    uint16_t checksum;
    uint16_t padding2;
    int setIPByID(int id);
};
static_assert(sizeof(RouterHeader) == ROUTER_HEADER_LEN, "Header length mismatch");
static_assert(std::is_standard_layout_v<RouterHeader>, "Router header is not in standard layout");

/**
 * hello packet
 * +------------+------------+------------+------------+
 * |                 designated router                 |
 * +------------+------------+------------+------------+
 * |             backup deginated router               |
 * +------------+------------+------------+------------+
 * |                  neighbor ip                      |
 * +--                                               --+
 * |                       ...                         |
 * +--                                               --+
 * |                                                   |
 * +------------+------------+------------+------------+
 */
struct HelloPacket {
    RouterHeader hdr;
    in_addr dr;
    in_addr bdr;
};
#define HELLO_HEADER_LEN 24
static_assert(sizeof(HelloPacket) == HELLO_HEADER_LEN, "Hello packet length mismatch");
static_assert(std::is_standard_layout_v<HelloPacket>, "Hello Packet is not in standard layout");

/**
 * linkstate packet
 * +------------+------------+------------+------------+
 * |                      node ip                      |
 * +------------+------------+------------+------------+
 * |                  node subnet mask                 |
 * +------------+------------+------------+------------+
 * |                     timestamp                     |
 * +------------+------------+------------+------------+
 * |         length          |         checksum        |
 * +------------+------------+------------+------------+
 * |                neighbor information               |
 * +--                                               --+
 * |                       ...                         |
 * +--                                               --+
 * |                                                   |
 * +------------+------------+------------+------------+
 *
 * neighbor information
 * +------------+------------+------------+------------+
 * |                     neighbor ip                   |
 * +------------+------------+------------+------------+
 * |                neighbor subnet mask               |
 * +------------+------------+------------+------------+
 */

struct LinkstatePacket {
    RouterHeader hdr;
    in_addr ip;
    in_addr subnet_mask;
    uint32_t timestamp;
    uint16_t length;
    uint16_t checksum;
};
#define LINKSTATE_HEADER_LEN 32
static_assert(sizeof(LinkstatePacket) == LINKSTATE_HEADER_LEN, "Linkstate packet length mismatch");
static_assert(std::is_standard_layout_v<LinkstatePacket>, "Linkstate Packet is not in standard layout");




#endif
