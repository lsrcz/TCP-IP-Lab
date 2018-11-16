#ifndef FRAME_DISPATCHER_H
#define FRAME_DISPATCHER_H
#include <map>
#include <protocol/packetio.h>
#include <shared_mutex>

class FrameDispatcher {
    std::shared_mutex                        mu;
    std::map<uint16_t, frameReceiveCallback> callbacks;

public:
    void addFrameDispatcher(uint16_t protocol, frameReceiveCallback);
    int  processFrame(const void* frame, int len, int id);
};

void addFrameDispatcher(uint16_t protocol, frameReceiveCallback c);

int defaultFrameReceiveCallback(const void*, int, int);

#endif  // FRAME_DISPATCHER_H
