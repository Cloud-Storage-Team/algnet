#pragma once

#include "switch.hpp"

class Switch : public NetworkSwitch {
private:
    std::uint64_t process_time_ns = 0;
    
public:
    Switch() = default;
    explicit Switch(std::uint64_t max_buffer_size_bit, std::uint64_t process_time_ns = 0): process_time_ns(process_time_ns) {
        this->max_buffer_size_bit = max_buffer_size_bit;
    };

    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final {
        while (!buffer.empty() && buffer.front().GetSendingTime() <= current_time_ns) {
            current_buffer_size_bit -= buffer.front().GetSize();
            buffer.pop();
        }

        // TODO: maybe should add some check that sending time of elements in queue is decreasing 
        if (current_buffer_size_bit + packet.GetSize() <= max_buffer_size_bit) {
            current_buffer_size_bit += packet.GetSize();
            std::cout << "Got packet on switch: " << packet.GetSendingTime() << std::endl;
            packet.SetSendingTime(current_time_ns + process_time_ns);
            buffer.push(packet);
            packets_wrapped.push(RoutingPacket(packet, GetNextElement(packet.GetDestinationID())));
            std::cout << "Finish of processing is at: " << packet.GetSendingTime() << std::endl << std::endl;
        } else {
            std::cout << "Dropped packet on switch: " << packet.GetSendingTime() << std::endl << std::endl;
            return;
        }
    };
};
