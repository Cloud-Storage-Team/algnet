#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "key_generator.hpp"
#include "number_manager.hpp"
#include "packet.hpp"
#include "server.hpp"
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
    std::uint32_t numberOfQueues;
    std::vector<std::queue<PacketHeader>> queueVector;
    std::vector<int> queuePauseCounter;
    std::uint32_t maxQueueSize;
    std::uint32_t thresholdQueueSize;
    NumberManager numberManager;
    std::unordered_map<std::size_t, FlowInfo> table;
    std::unordered_map<std::size_t, int> pauseCounter;
    std::uint64_t last_process_time_ns = 0;
    std::uint64_t process_time = 5;
    std::uint32_t offset = 0;

    int getQueue();
    void returnQueue(int number);
    void pause(int queueNumber);
    void resume(int queueNumber);

    void sendPause(PacketHeader &packet);
    void sendResume(PacketHeader &packet);

  public:
    BFCSwitch(std::uint32_t numberOfQueues = 8, std::uint32_t maxQueueSize = 32,
              std::uint32_t thresholdQueueSize = 16);
    std::uint64_t SendPackets(std::uint64_t current_time_ns,
                              PriorityQueueWrapper &packets_wrapped);
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                       PriorityQueueWrapper &packets_wrapped);
};
