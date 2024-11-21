#include "number_manager.hpp"
#include "packet.hpp"
#include "key_generator.hpp"
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>

class FlowInfo {
public:
    int size; 
    int qAssignment; 
    FlowInfo() : size(0), qAssignment(-1) {}
};

class FlowTable {
private:
    int numberOfQueues = 8;
    std::vector<std::queue<PacketHeader>> queueVector = std::vector<std::queue<PacketHeader>>(numberOfQueues);
    std::uint32_t maxQueueLength = 8;

    NumberManager numberManager = NumberManager(numberOfQueues);
    std::unordered_map<std::string, FlowInfo> table;
    std::unordered_map<std::string, int> pauseCounter;



public:
    void enqueue(PacketHeader& packet) {
        std::string key = generateKey(packet.GetSourceID(), packet.GetDestinationID()); //TODO +egress Port
        bool reassignedQueue = false;

        if (table[key].size == 0) {
            reassignedQueue = true;
        }
        table[key].size += 1;

        if (reassignedQueue) {
            table[key].qAssignment = getQueue();
        }
        packet.qAssignment = table[key].qAssignment;
    }

    void dequeue(PacketHeader& packet) {
        std::string key = generateKey(packet.GetSourceID(), packet.GetDestinationID()); //TODO +egress Port

        table[key].size -= 1;

        returnQueue(table[key].qAssignment);
    }

    void enqueue2(PacketHeader& packet) {
        std::string key = generateKey(packet.upstreamQ); //TODO +ingress Port

        if (queueVector[packet.qAssignment].size() > maxQueueLength) { 
            packet.counterInc = true;
            pauseCounter[key] += 1;

            if (pauseCounter[key] == 1) {
                //Pasue the queue at upstream
                sendPause(packet); //also can work on key with port
            }
        }
    }

    void dequeue2(PacketHeader& packet) {
        std::string key = generateKey(packet.upstreamQ); //+ingress port

        if (packet.counterInc) {
            pauseCounter[key] -= 1;

            if (pauseCounter[key] == 0) {
                //Resume the queue at upstream
                sendResume(packet); //also can work on key with port
            }
        }
    }

    int getQueue() {
        return numberManager.getNumber();
    }
    void returnQueue(int number) {
        return numberManager.returnNumber(number);
    }

    void sendPause(PacketHeader& packet) {
        //TODO    
    }

    void sendResume(PacketHeader& packet) {
        //TODO
    }
};
