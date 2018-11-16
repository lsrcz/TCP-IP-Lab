#ifndef ROUTER_PORT_H
#define ROUTER_PORT_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <protocol/RouterNeighbor.h>
#include <set>
#include <shared_mutex>
#include <thread>
#include <utils/netutils.h>
#include <utils/threadsafeQueue.h>
#include <vector>

class Router;
class RouterNeighbor;
class RouterPort {
public:
    enum STATE { UNINITIALIZED, DOWN, STARTING, DR, BDR, DROTHER };

private:
    Router& router;
    STATE   portState;

    int                      device;
    IP                       portIP;
    std::set<RouterNeighbor> neighborTable;
    std::shared_mutex        ntmu;  // neighborTable

    in_addr dr;
    in_addr bdr;
    void    resetDRBDR();
    void    calculateDRBDR();

    std::condition_variable_any bdcv;
    std::mutex                  hlmu;  // for synchronize
    std::condition_variable_any hlcv;
    std::shared_mutex           statemu;  // portState, dr, bdr

    // std::thread bdUpdateThread;
    // void initBDUpdateThread();
    // void destroyBDUpdateThread();
    std::thread                                helloThread;
    void                                       initHelloThread();
    void                                       destroyHelloThread();
    threadsafeQueue<std::function<void(void)>> taskQueue;
    std::thread                                taskThread;
    void                                       initTaskThread();
    void                                       destroyTaskThread();
    void                                       resetDevice();
    void                                       printRouterStatus();

public:
    RouterPort(Router& router);
    int                startDevice(int id);
    int                sendHelloPacket();
    int                recvHelloPacket(const void* packet, int len);
    STATE              getState();
    void               removeNeighbor(in_addr ip);
    void               addTask(std::function<void(void)> func);
    std::set<IP>       getNeighborInformation() const;
    int                sendLinkStatePacket(const std::vector<IP>&,
                                           const std::vector<uint16_t>&);
    IP                 getIP();
    std::set<uint16_t> getNeighborRID();
    std::set<std::pair<uint16_t, in_addr>> getNeighborRIDIP();
};

#endif  // ROUTER_PORT_H
