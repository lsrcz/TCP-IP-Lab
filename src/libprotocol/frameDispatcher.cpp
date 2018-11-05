#include <protocol/frameDispatcher.h>
#include <utils/logutils.h>
#include <utils/netutils.h>

void FrameDispatcher::addFrameDispatcher(uint16_t protocol, frameReceiveCallback c) {
    std::unique_lock<std::shared_mutex> lock(mu);
    char buf[200];
    snprintf(buf, 200, "Registered dispatcher for protocol id 0x%04x", protocol);
    LOG(INFO, buf);
    callbacks[protocol] = c;
}
int FrameDispatcher::processFrame(const void* frame, int len, int id) {
    std::shared_lock<std::shared_mutex> lock(mu);
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
