#include "Flow.hpp"
#include "NetworkSimulator.hpp"

#include <utility>

Flow::Flow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver, std::uint64_t packet_generation_interval_ns):
        m_sender(std::move(sender)),
        m_receiver(std::move(receiver)),
        m_packet_generation_interval_ns(packet_generation_interval_ns) { }

std::uint32_t Flow::GetSenderID() const {
    return m_sender->id;
}

std::uint32_t Flow::GetReceiverID() const {
    return m_receiver->id;
}

void Flow::Send() {
    Packet p(m_sender->id, m_receiver->id, 0, false);

    NetworkSimulator::Schedule(m_packet_generation_interval_ns, [this, p]() {
        m_sender->ProcessPacket(p);
        Send();
    });
}
