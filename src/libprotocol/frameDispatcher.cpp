#include <protocol/frameDispatcher.h>
#include <utils/logutils.h>
#include <utils/netutils.h>

void FrameDispatcher::addDispatcher(int protocol, frameReceiveCallback c) {
    std::unique_lock<std::shared_mutex> lock(mu);
    char buf[200];
    snprintf(buf, 200, "Registered dispatcher for protocol id %d", protocol);
    LOG(INFO, buf);
    callbacks[protocol] = c;
}
int FrameDispatcher::processFrame(const void* frame, int len, int id) {
    std::shared_lock<std::shared_mutex> lock(mu);
    char buf[200];
    if (!ETHER_IS_VALID_LEN(len))
        return -2;
    uint16_t ether_type = htonl16(((struct ether_header *)frame)->ether_type);
    printf("len %d", len);
    if (callbacks.find(ether_type) == callbacks.end()) {
        snprintf(buf, 200, "Abandoned unknown frame for protocol 0x%4x", ether_type);
        LOG(INFO, buf);
        return -2;
    }
    return (*(callbacks.find(ether_type))->second)((uint8_t*)frame + ETHER_HDR_LEN, len - ETHER_HDR_LEN, id);
}

FrameDispatcher fd;

void addDispatcher(int protocol, frameReceiveCallback c) {
    fd.addDispatcher(protocol, c);
}
int defaultFrameReceiveCallback(const void* frame, int len, int id) {
    return fd.processFrame(frame, len, id);
}
