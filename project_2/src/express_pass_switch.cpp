#include "express_pass_switch.hpp"

ExpressPassSwitch::ExpressPassSwitch(std::uint64_t credit_rate_limit_ns, std::uint64_t process_time_ns, std::uint8_t max_credit_buffer_size): 
    process_time_ns(process_time_ns),
    max_credit_buffer_size(max_credit_buffer_size),
    credit_rate_limit_ns(credit_rate_limit_ns) {};

void ExpressPassSwitch::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) {
    if (packet.GetFlag(0) == 0) {
        packets_wrapped.push(RoutingPacket(packet, GetNextElement(packet.destination_id)));
        return;
    }

    while (!credit_buffer.empty() && credit_buffer.top().sending_time <= current_time_ns) {
        credit_buffer.pop();
    }

    // std::cout << "Current time: " << current_time_ns << " ";
    if (credit_buffer.size() < max_credit_buffer_size) {
        // std::cout << "Got: " << packet << std::endl;
        last_credit_process_time_ns = std::max(last_credit_process_time_ns, current_time_ns) + process_time_ns;
        packet.sending_time = last_credit_process_time_ns;
        last_credit_process_time_ns += credit_rate_limit_ns;
        credit_buffer.push(packet);
        packets_wrapped.push(RoutingPacket(packet, GetNextElement(packet.destination_id)));
        // std::cout << "Put into queue: " << packet << std::endl;
    } else {
        // std::cout << "Dropped: " << packet << std::endl;
        return;
    }
}