#include <protocol/frameDispatcher.h>
#include <protocol/device.h>
#include <utils/logutils.h>
#include <utils/netutils.h>
#include <utils/printutils.h>

void FrameDispatcher::addFrameDispatcher(uint16_t protocol, frameReceiveCallback c) {
    std::unique_lock<std::shared_mutex> lock(mu);
    char buf[200];
    snprintf(buf, 200, "Registered dispatcher for protocol id 0x%04x", protocol);
    LOG(INFO, buf);
    callbacks[protocol] = c;
}
int FrameDispatcher::processFrame(const void* frame, int len, int id) {
    LOG(DEBUG, "Frame received");
    std::shared_lock<std::shared_mutex> lock(mu);

    int ok = 1;
    for (int i = 0; i < 6; ++i) {
        if (((uint8_t*)frame)[i] != 0xff) {
            ok = 0;
            break;
        }
    }
    if (ok == 0) {
        ok = 1;
        for (int i = 0; i < 6; ++i) {
            uint8_t m[6];
            if (getDeviceMAC(id, m) < 0)
                return -1;
            if (((uint8_t*)frame)[i] != m[i]) {
                ok = 0;
                break;
            }
        }
    }
    if (ok == 0) {
        uint8_t* m = (uint8_t*)frame;
        if (m[0] == 0x01 && m[1] == 0 && m[2] == 0xC2 && (m[3] & 0x80) == 0)
            ok = 1;
    }
    if (ok == 0) {
        return -1;
    }
    LOG(DEBUG, "Frame verified");

    char buf[200];
    if (len >= ETHER_MAX_LEN)
        return -2;
    uint16_t ether_type = htonl16(((struct ether_header *)frame)->ether_type);
    if (callbacks.find(ether_type) == callbacks.end()) {
        return -2;
    }
    return (*(callbacks.find(ether_type))->second)((uint8_t*)frame + ETHER_HDR_LEN, len - ETHER_HDR_LEN, id);
}

FrameDispatcher fd;

void addFrameDispatcher(uint16_t protocol, frameReceiveCallback c) {
    fd.addFrameDispatcher(protocol, c);
}
int defaultFrameReceiveCallback(const void* frame, int len, int id) {
    return fd.processFrame(frame, len, id);
}
