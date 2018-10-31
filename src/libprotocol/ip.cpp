#include <protocol/ip.h>
#include <protocol/device.h>
#include <utils/netutils.h>
#include <protocol/packetio.h>
#include <utils/logutils.h>
#include <net/ethernet.h>
#include <cstdint>
#include <cstring>

bool IPInitFlag = false;

union checksum_helper {
    uint32_t i;
    uint16_t s[2];
};

int sendIPPacket(const struct in_addr src, const struct in_addr dest,
                 int proto, const void *buf, int len) {
    struct ip hdr;
    hdr.ip_v = 4;
    hdr.ip_hl = 5;
    hdr.ip_tos = 0;
    hdr.ip_len = htonl16(len + 20);
    hdr.ip_id = 0;
    hdr.ip_off = htonl16(IP_DF);
    hdr.ip_ttl = 64;
    hdr.ip_p = proto;
    hdr.ip_sum = 0;
    hdr.ip_src = src;
    hdr.ip_dst = dest;
    checksum_helper ch;
    ch.i = 0;
    for (int i = 0; i < 10; ++i) {
        ch.i += *((uint16_t*)&hdr);
    }
    hdr.ip_sum = htonl16(ch.s[0] + ch.s[1]);
    int id = getFirstDevice();
    if (id < 0) {
        //logPrint(ERROR, "No available devices.");
        return -1;
    }
    uint8_t* ipbuf = (uint8_t*)malloc(len + 20);
    memcpy(ipbuf, &hdr, 20);
    memcpy(ipbuf + 20, buf, len);
    uint8_t mac[6] = {0x00,0x50,0x56,0xc0,0x00,0x08};
    return sendFrame(ipbuf, len + 20, ETHERTYPE_IP, mac, id);
}

IPPacketReceiveCallback IPCallbackVector[IPPROTO_MAX];
pthread_rwlock_t *IPCallbackVectorRwlock;

int setIPPacketReceiveCallback(int protocol, IPPacketReceiveCallback callback) {
    if (protocol >= IPPROTO_MAX || protocol < 0)
        return -1;
    pthread_rwlock_wrlock(IPCallbackVectorRwlock);
    IPCallbackVector[protocol] = callback;
    pthread_rwlock_unlock(IPCallbackVectorRwlock);
    return 0;
}

int defaultFrameReceiveCallback(const void* buf, int len, int id) {
    printf("Packet of length %d received on id %d\n", len, id);
    return 0;
}

int IPInit() {
    if (!packetioIsInit()) {
        if (packetioInit() != 0) {
            //logPrint(FATAL, "Unable to init IP, packetio init failed.");
            return -1;
        }
    }
    if (IPIsInit())
        return 0;
    if (pthread_rwlock_init(IPCallbackVectorRwlock, NULL) != 0) {
        //logPrint(FATAL, "Unable to init IP, rwlock init failed.");
        return -1;
    }
    return setFrameReceiveCallback(defaultFrameReceiveCallback);
}

bool IPIsInit() {
    return IPInitFlag;
}
