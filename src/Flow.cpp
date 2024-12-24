#include "Flow.hpp"
#include "NetworkSimulator.hpp"

void Flow::Send() {
    Packet p(m_sender->id, m_receiver->id, 0, false);

    NetworkSimulator::Schedule(m_packet_generation_interval_ns, [this, p]() {
        m_sender->ProcessPacket(p);
        Send();
    });

}
