#ifndef ARP_H
#define ARP_H

int request_arp(in_addr ip, void* m);
int sendARPRequest(in_addr ip, int device);
int arpFrameReceiveCallback(const void* packet, int len, int id);

#endif
