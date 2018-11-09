#ifndef PRINTUTILS_H
#define PRINTUTILS_H
#include <protocol/ip.h>
#include <cstdio>
#include <protocol/RouterProtocol.h>

inline
void printMAC(const void* buf) {
    const uint8_t* macbuf = (const uint8_t*)buf;
    for (int i = 0; i < 6; ++i) {
        if (i != 0)
            printf(":");
        printf("%02x", macbuf[i]);
    }
}

inline
void printIP(const void* buf) {
    const uint8_t* ipbuf = (const uint8_t*)buf;
    for (int i = 0; i < 4; ++i) {
        if (i != 0)
            printf(".");
        printf("%d", ipbuf[i]);
    }
}

inline
void printRoutePacket(const void* buf, int len) {
    printf("type: router\n");
    RouterHeader* hdr = (RouterHeader*)buf;
    printf("real length: %d\n", len);
    printf("length:%d\n", htonl16(hdr->len));
    printf("IP: ");
    printIP(&hdr->ip);
    printf("\n");
    printf("subnet mask: ");
    printIP(&hdr->subnetMask);
    printf("\n");
    if (hdr->type == HELLO) {
        printf("type: HELLO\n");
        int list_len = (htonl16(hdr->len) - HELLO_HEADER_LEN) / 4;
        in_addr* ip_list = (in_addr*)(((HelloPacket*)hdr) + 1);
        printf("list len: %d\n", list_len);
        printf("list: \n");
        for (int i = 0; i < list_len; ++i) {
            printf("  ");
            printIP(ip_list + i);
            printf("\n");
        }
    } else if (hdr->type == LINKSTATE) {
        printf("type: LINKSTATE\n");
        LinkstatePacket* packet = (LinkstatePacket*)hdr;
        int list_len = (htonl16(hdr->len) - LINKSTATE_HEADER_LEN) / 8;
        IP* ip_list = (IP*)(packet + 1);
        printf("node ip: ");
        printIP(&packet->ip);
        printf("\n");
        printf("node subnet mask: ");
        printIP(&packet->subnet_mask);
        printf("\n");
        printf("timestamp: %d\n", htonl32(packet->timestamp));
        printf("list len: %d\n", list_len);
        printf("list: \n");
        for (int i = 0; i < list_len; ++i) {
            printf("  ");
            printIP(ip_list + i);
            printf("/");
            printIP(((in_addr*)(ip_list + i)) + 1);
            printf("\n");
        }
    }
}

inline
void printIPPacket(const void* buf, int len) {
    printf("type: IP\n");
    const ip* hdr = (const ip*)buf;
    int packet_len = htonl16(hdr->ip_len);
    int header_len = hdr->ip_hl * 4;
    printf("real length: %d\n", packet_len);
    printf("length: %d\n", htonl16(((const uint16_t*)buf)[1]));
    printf("src IP: ");
    printIP(((uint8_t*)buf) + 12);
    printf("\n");
    printIP(((uint8_t*)buf) + 16);
    printf("\n");
    uint8_t protocol = ((uint8_t*)buf)[9];
    if (protocol == ROUTER_PROTO_NUM) {
        printRoutePacket((uint8_t*)buf + header_len, packet_len - header_len);
    } else {
        printf("type: Unknown(");
        printf("%d", protocol);
        printf(")\n");
    }
}

inline
void printIncomingFrame(const void* buf, int len) {
    printf("Incoming frame received\n");
    printf("length: %d\n", len);
    printf("dest MAC: ");
    printMAC(buf);
    printf("\n");
    printf("src MAC: ");
    printMAC(((uint8_t*)buf) + 6);
    printf("\n");
    uint16_t type = htonl16(((uint16_t*)buf)[6]);
    if (type == 0x0800) {
        printIPPacket(((uint8_t*)buf) + 14, len - 14);
    } else {
        printf("type: Unknown(");
        printf("%04x", type);
        printf(")\n");
    }
}


#endif // PRINTUTILS_H
