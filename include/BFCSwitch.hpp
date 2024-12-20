#pragma once

#include "Switch.hpp"
#include "Packet.hpp"
#include "BFCQNumberManger.hpp"
#include "BFCKeyGenerator.hpp"
#include <unordered_map>
#include "NetworkDevice.hpp"

class FlowInfo {
  public:
    int size;
    int qAssignment;
    FlowInfo() : size(0), qAssignment(-1) {
    }
};

class BFCSwitch: public Switch {
  public:
    BFCSwitch(std::uint64_t processing_delay_ns)
        : Switch(processing_delay_ns) {}

    std::uint32_t numberOfQueues = 8;
    std::vector<std::queue<Packet>> queueVector = std::vector<std::queue<Packet>>(numberOfQueues);
    std::vector<int> queuePauseCounter = std::vector<int>(numberOfQueues, 0);
    std::uint32_t maxQueueSize = 32;
    std::uint32_t thresholdQueueSize = 16;
    QNumberManager numberManager = QNumberManager(numberOfQueues);
    std::unordered_map<std::size_t, FlowInfo> table;
    std::unordered_map<std::size_t, int> pauseCounter;
    std::uint64_t last_process_time_ns = 0;
    std::uint32_t offset = 0;

    int getQueue();
    void returnQueue(int number);
    void pause(int queueNumber);
    void resume(int queueNumber);

    void sendPause(Packet &packet);
    void sendResume(Packet &packet);

    void SelfProcessHandler();
    void ProcessPacket(Packet p);
};