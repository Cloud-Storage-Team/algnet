#include "Flow.hpp"
#include "NetworkSimulator.hpp"

#include <utility>

Flow::Flow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver, std::uint64_t packet_generation_interval_ns):
    sender_(std::move(sender)),
    receiver_(std::move(receiver)),
    packet_generation_interval_ns_(packet_generation_interval_ns),
    flow_id_(++last_flow_id_) { }

void Flow::Send() 
{
    Packet p(sender_->GetId(), receiver_->GetId(), 0, false , flow_id_);

    NetworkSimulator::NetworkSimulator::Schedule(packet_generation_interval_ns_, [this, p]() 
    {
        sender_->ProcessPacket(p);
        Send();
    });
}