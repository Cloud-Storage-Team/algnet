#include "Flow.hpp"
#include "NetworkSimulator.hpp"

#include <utility>

Flow::Flow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver, std::uint64_t packet_generation_interval_ns):
        m_sender(std::move(sender)),
        m_receiver(std::move(receiver)),
        m_packet_generation_interval_ns(packet_generation_interval_ns) { }
