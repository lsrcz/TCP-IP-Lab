#include <protocol/device.h>
#include <protocol/packetio.h>
#include <net/ethernet.h>
#include <protocol/netutils.h>
#include <cstring>
#include <cstdlib>
#include <cstdint>

bool packetioInitFlag = false;
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
    uint8_t* framebuf = (uint8_t*)malloc(len + 14);
    memcpy(framebuf, destmac, 6);
    if (getDeviceMac(id, (uint8_t*)framebuf + 6) < 0) {
        return -1;
    }
    uint16_t ethtype_bigendian = htonl16((uint16_t)ethtype);
    *((uint16_t*)&framebuf[12]) = ethtype_bigendian;
    memcpy(framebuf + 14, buf, len);
    return sendPacketOnDevice(id, framebuf, len + 14);
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
pthread_rwlock_t *frCallbackRwlock;

/**
 * @brief Register a callback function to be called each time an Ethernet II 
 * frame was received.
 *
 * @param callback The callback function.
 * @return 0 on success, -1 on error.
 * @see frameReceiveCallback
 */
int setFrameReceiveCallback(frameReceiveCallback callback) {
    frCallback = callback;
    return 0;
}

int packetioInit() {
    if (packetioInitFlag)
        return 0;
    if (pthread_rwlock_init(frCallbackRwlock, NULL) != 0) {
        logPrint(FATAL, "Unable to init packetio, rwlock init failed.");
        return -1;
    }
    return 0;
}

bool packetioIsInit() {
    return packetioInitFlag;
}
