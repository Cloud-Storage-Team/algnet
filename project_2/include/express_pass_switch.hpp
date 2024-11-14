#pragma once

#include "switch.hpp"

class ExpressPassSwitch : public NetworkSwitch {
private:
    // Queue for credit packets
    std::priority_queue<PacketHeader> credit_buffer{};
    std::uint64_t process_time_ns = 0;
    std::uint64_t credit_rate_limit_ns = 100;
    std::uint64_t last_credit_process_time_ns = 0;
    std::uint8_t max_credit_buffer_size = 8;
    
public:
    ExpressPassSwitch() = default;
    explicit ExpressPassSwitch(std::uint64_t credit_rate_limit_ns, std::uint64_t process_time_ns = 0, std::uint8_t max_credit_buffer_size = 8);

    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final;
};
