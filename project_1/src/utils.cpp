#include "utils.hpp"

Server::Server(std::uint32_t id, std::uint32_t distance_ms) : id(id), distance_ms(distance_ms) { }

std::uint32_t Server::GetID() const {
    return id;
}

std::uint32_t Server::GetDistance() const {
    return distance_ms;
}

PackageHeader::PackageHeader(std::uint32_t sender_id, std::uint32_t receiver_id, std::uint32_t time, std::uint32_t total_number, std::uint32_t index)
    : sender_id(sender_id), receiver_id(receiver_id), sending_time(time), total_number_of_packages(total_number), package_index(index) { }

void PackageHeader::SetRTT(std::uint32_t rtt) {
    RTT = rtt;
}

std::uint32_t PackageHeader::GetRTT() const {
    return RTT;
}

std::uint32_t PackageHeader::GetSendingTime() const {
    return sending_time;
}

std::uint32_t PackageHeader::GetPackageIndex() const {
    return package_index;
}

std::uint32_t PackageHeader::GetSenderID() const {
    return sender_id;
}

bool PackageHeader::operator<(const PackageHeader &other) const {
    return RTT < other.GetRTT();
}

Event::Event(std::uint32_t id, Server& initiator, event_type type, std::uint32_t packages_number)
    : id(id), server_initiator(initiator), type(type), packages_number(packages_number) { }

std::uint32_t Event::GetID() const {
    return id;
}

