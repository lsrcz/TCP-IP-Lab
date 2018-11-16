#ifndef ROUTER_PROTOCOL_H
#define ROUTER_PROTOCOL_H
#include <arpa/inet.h>
#include <type_traits>
#include <utils/errorutils.h>

enum routerPacketType : uint8_t { HELLO, LINKSTATE, ACK };

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
 * |          checksum       |        router ID        |
 * +------------+------------+------------+------------+
 */
struct RouterHeader {
    routerPacketType type;
    uint8_t          padding;
    uint16_t         len;
    in_addr          ip;
    in_addr          subnetMask;
    uint16_t         checksum;
    uint16_t         rid;
    int              setIPByID(int id);
};
static_assert(sizeof(RouterHeader) == ROUTER_HEADER_LEN,
              "Header length mismatch");
static_assert(std::is_standard_layout_v<RouterHeader>,
              "Router header is not in standard layout");

/**
 * hello packet
 * +------------+------------+------------+------------+
 * |                 designated router                 |
 * +------------+------------+------------+------------+
 * |             backup deginated router               |
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
 * |                    neighbor ip                    |
 * +------------+------------+------------+------------+
 * |        router ID        |
 * +------------+------------+
 */
struct HelloPacket {
    RouterHeader hdr;
    in_addr      dr;
    in_addr      bdr;
};
#define HELLO_HEADER_LEN 24
static_assert(sizeof(HelloPacket) == HELLO_HEADER_LEN,
              "Hello packet length mismatch");
static_assert(std::is_standard_layout_v<HelloPacket>,
              "Hello Packet is not in standard layout");

struct helloNeighborInformation {
    in_addr  ip;
    uint16_t rid;
} __attribute__((packed));

/**
 * linkstate packet
 * +------------+------------+------------+------------+
 * |        router ID        |         padding         |
 * +------------+------------+------------+------------+
 * |      num of ports       |     num of neighbors    |
 * +------------+------------+------------+------------+
 * |                     timestamp                     |
 * +------------+------------+------------+------------+
 * |                 port information                  |
 * +--                                               --+
 * |                       ...                         |
 * +--                                               --+
 * |                                                   |
 * +------------+------------+------------+------------+
 * |   neighbor information  |                         |
 * +--                                               --+
 * |                       ...                         |
 * +--                                               --+
 * |                                                   |
 * +------------+------------+------------+------------+
 *
 * port information
 * +------------+------------+------------+------------+
 * |                      port IP                      |
 * +------------+------------+------------+------------+
 * |                 port subnet mask                  |
 * +------------+------------+------------+------------+
 *
 * neighbor information
 * +------------+------------+
 * |        router ID        |
 * +------------+------------+
 */

struct LinkstatePacket {
    RouterHeader hdr;
    uint16_t     rid;
    uint16_t     padding;
    uint16_t     nop;
    uint16_t     non;
    uint32_t     timestamp;
};

#define LINKSTATE_HEADER_LEN 28
static_assert(sizeof(LinkstatePacket) == LINKSTATE_HEADER_LEN,
              "Linkstate packet length mismatch");
static_assert(std::is_standard_layout_v<LinkstatePacket>,
              "Linkstate Packet is not in standard layout");

#endif
