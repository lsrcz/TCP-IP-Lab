#ifndef API_H
#define API_H

#include <protocol/device.h>
#include <protocol/packetio.h>
#include <protocol/frameDispatcher.h>
#include <protocol/ip.h>
#include <protocol/arp.h>
#include <string>

void initProtocol();
int addInterfaceWithIP(const std::string& name, in_addr ip);
int addInterfaceWithIP(const std::string& name, const std::string& ip);

#endif
