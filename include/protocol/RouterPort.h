#ifndef ROUTER_PORT_H
#define ROUTER_PORT_H

#include <set>
#include <utils/netutils.h>
#include <shared_mutex>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <protocol/RouterNeighbor.h>

class RouterNeighbor;
class RouterPort {
public:
    enum STATE {
        UNINITIALIZED,
        DOWN,
        STARTING,
        DR,
        BDR,
        DROTHER
    };
private:
    STATE portState;

    int device;
    IP portIP;
    std::set<RouterNeighbor> neighborTable;
    std::shared_mutex ntmu; // neighborTable

    in_addr dr;
    in_addr bdr;
    void resetDRBDR();
    void calculateDRBDR();

    std::condition_variable_any bdcv;
    std::mutex hlmu; // for synchronize
    std::condition_variable_any hlcv;
    std::shared_mutex statemu; // portState, dr, bdr

    //std::thread bdUpdateThread;
    //void initBDUpdateThread();
    //void destroyBDUpdateThread();
    std::thread helloThread;
    void initHelloThread();
    void destroyHelloThread();
    threadsafeQueue<std::function<void(void)>> taskQueue;
    std::thread taskThread;
    void initTaskThread();
    void destroyTaskThread();
    void resetDevice();
    void printRouterStatus();
 public:
    RouterPort();
    int startDevice(int id);
    int sendHelloPacket();
    int recvHelloPacket(const void* packet, int len);
    STATE getState();
    void removeNeighbor(in_addr ip);
    void addTask(std::function<void(void)> func);
};

#endif // ROUTER_PORT_H
