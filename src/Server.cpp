#include "Server.hpp"

ServerBase::ServerBase(std::uint32_t id)
    : id(id) { }

std::uint32_t ServerBase::GetID() const {
    return id;
}

std::uint64_t ServerBase::GetCurrentTime() const {
    return current_time_ns;
}

void ServerBase::SetCurrentTime(std::uint64_t new_time) {
    current_time_ns = new_time;
}

void ServerBase::IncreaseCurrentTime(std::uint64_t delta_time) {
    current_time_ns += delta_time;
}

ServerSender::ServerSender(std::uint64_t id, std::uint32_t distance_ns)
    : ServerBase(id), distance_ns(distance_ns) { }

std::uint64_t ServerSender::GetCWNDSize() const {
    return cwnd_size_packets;
}

std::uint32_t ServerSender::GetDistance() const {
    return distance_ns;
}