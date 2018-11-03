#ifndef ARP_H
#define ARP_H

void deleteDeviceARP(int id);
int request_arp(in_addr ip, void* m);
int sendGratuitousARPRequest(in_addr ip, int device);
int arpFrameReceiveCallback(const void* packet, int len, int id);

#endif
