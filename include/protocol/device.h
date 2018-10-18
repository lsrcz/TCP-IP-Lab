/**
 * @file device.h
 * @brief Library supporting network device management.
 */

#include <cstddef>

/**
 * Add a device to the library for sending/receiving packets. 
 *
 * @param device Name of network device to send/receive packet on.
 * @return An non-negative _device-ID_ on success, -1 on error.
 */
int addDevice(const char* device);

/**
 * Find a device added by `addDevice`.
 *
 * @param device Name of the network device.
 * @return An non-negative _device-ID_ on success, -1 if no such device 
 * was found.
 */
int findDevice(const char* device);

int removeDevice(const char* device);

int removeDevice(int device_descriptor);

int removeAllDevice();

int sendPacketOnDevice(int device_descriptor, const void *buf, int size);

int getDeviceMac(int device_descriptor, void *buf); 
