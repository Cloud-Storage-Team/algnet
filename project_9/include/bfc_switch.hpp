#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "key_generator.hpp"
#include "number_manager.hpp"
#include "packet.hpp"
#include "switch.hpp"

class FlowInfo {
  public:
    int size;
    int qAssignment;
    FlowInfo() : size(0), qAssignment(-1) {
    }
};

class BFCSwitch : public NetworkSwitch {
  private:
    int numberOfQueues = 8;
    std::vector<std::queue<PacketHeader>> queueVector;
    std::vector<int> queuePauseCounter;
    std::uint32_t maxQueueSize = 32;
    std::uint32_t thresholdQueueSize = 16;
    NumberManager numberManager;
    std::unordered_map<std::string, FlowInfo> table;
    std::unordered_map<std::string, int> pauseCounter;
    std::uint32_t last_process_time = 0;
    std::uint32_t offset = 0;

    int getQueue();
    void returnQueue(int number);
    void pause(int queueNumber);
    void resume(int queueNumber);

    void sendPause(PacketHeader &packet);
    void sendResume(PacketHeader &packet);

  public:
    BFCSwitch();
    std::uint64_t SendPackets(std::uint64_t current_time_ns,
                              PriorityQueueWrapper &packets_wrapped);
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                       PriorityQueueWrapper &packets_wrapped);
};
