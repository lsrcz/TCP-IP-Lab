#include <protocol/device.h>
#include <protocol/packetio.h>
#include <net/ethernet.h>
#include <utils/netutils.h>
#include <cstring>
#include <cstdlib>
#include <cstdint>

/** 
 * @brief Encapsule some data into an Ethernet II frame and send it.
 *
 * @param buf Pointer to the payload.
 * @param len Length of the payload.
 * @param ethtype EtherType field value of this frame.
 * @param destmac MAC address of the destination.
 * @param id ID of the device(returned by `addDevice`) to send on.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
int sendFrame(const void* buf, int len, 
              int ethtype, const void* destmac, int id) {
    if (!ETHER_IS_VALID_LEN(len + ETHER_HDR_LEN)) {
        LOG(ERROR, "Invalid packet length");
        return -1;
    }
    uint8_t framebuf[ETHER_MAX_LEN];
    struct ether_header eh;
    memcpy(eh.ether_dhost, destmac, 6);
    if (getDeviceMac(id, eh.ether_shost) < 0) {
        return -1;
    }
    eh.ether_type = htonl16((uint16_t)ethtype);
    memcpy(framebuf, &eh, ETHER_HDR_LEN);
    memcpy(framebuf + ETHER_HDR_LEN, buf, len);
    return sendPacketOnDevice(id, framebuf, len + ETHER_HDR_LEN);
}

/** 
 * @brief Process a frame upon receiving it.
 *
 * @param buf Pointer to the frame.
 * @param len Length of the frame.
 * @param id ID of the device(returned by `addDevice`) receiving current 
 * frame.
 * @return 0 on success, -1 on error.
 * @see addDevice
 */
typedef int (*frameReceiveCallback)(const void*, int, int);

frameReceiveCallback frCallback = NULL;
std::shared_mutex muFrCallback;

/**
 * @brief Register a callback function to be called each time an Ethernet II 
 * frame was received.
 *
 * @param callback The callback function.
 * @return 0 on success, -1 on error.
 * @see frameReceiveCallback
 */
int setFrameReceiveCallback(frameReceiveCallback callback) {
    std::unique_lock<std::shared_mutex> lock(muFrCallback);
    frCallback = callback;
    return 0;
}

