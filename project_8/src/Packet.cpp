#include "Packet.hpp"
#include <cstdint>

Packet::Packet(std::uint64_t src_id, std::uint64_t dst_id, std::uint64_t sending_time_ns, bool is_ack, std::uint32_t flow_id) 
: source_id_(src_id) ,
  destination_id_(dst_id) ,
  sending_time_ns_(sending_time_ns),
  is_ack_(is_ack),
  flow_id_(flow_id_)
{
}

std::uint64_t Packet::GetSourceID() const
{
    return source_id_;
}

std::uint64_t Packet::GetDestinationID() const 
{
    return destination_id_;
}

std::uint64_t Packet::GetSendingTime() const 
{
    return sending_time_ns_;
}

std::uint32_t Packet::GetTransmittedDataSize() const 
{
    return data_size_bytes_;
}

std::uint64_t Packet::GetFlowId() const
{
    return flow_id_;
}

bool Packet::IsAck() const 
{
    return is_ack_;
}

void Packet::SetAck(bool value)
{
    is_ack_=value;
}
