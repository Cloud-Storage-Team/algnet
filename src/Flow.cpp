#include "Flow.hpp"
#include "NetworkSimulator.hpp"

#include <utility>

Flow::Flow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver, std::uint64_t packet_generation_interval_ns):
    sender(std::move(sender)),
    receiver(std::move(receiver)),
    packet_generation_interval_ns(packet_generation_interval_ns) { }

void Flow::Send() {
    Packet p(sender->id, receiver->id, 0, false);

    NetworkSimulator::Schedule(packet_generation_interval_ns, [this, p]() {
        sender->ProcessPacket(p);
        Send();
    });

}
