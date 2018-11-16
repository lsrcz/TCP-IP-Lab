#ifndef API_H
#define API_H

#include <protocol/arp.h>
#include <protocol/device.h>
#include <protocol/frameDispatcher.h>
#include <protocol/ip.h>
#include <protocol/packetio.h>
#include <string>

void initProtocol();
int  addInterfaceWithIP(const std::string& name, in_addr ip,
                        in_addr subnet_mask);
int  addInterfaceWithIP(const std::string& name, const std::string& ipstr,
                        const std::string& subnet_mask);

#endif
