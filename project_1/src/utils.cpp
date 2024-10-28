#include "utils.hpp"

std::uint64_t GetCurrentTimeInMicroseconds() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

ServerBase::ServerBase(std::uint32_t id) : id(id) { }

std::uint32_t ServerBase::GetID() const {
    return id;
}

ServerSender::ServerSender(std::uint32_t id, std::uint32_t distance_ms) : ServerBase(id), distance_μs(distance_ms) { }

std::uint32_t ServerSender::GetDistance() const {
    return distance_μs;
}

PackageHeader::PackageHeader(std::uint64_t sender_id, std::uint64_t receiver_id, std::uint64_t time, std::uint32_t index)
    : sender_id(sender_id), receiver_id(receiver_id), sending_time(time), package_index(index) { }

void PackageHeader::SetRTT(std::uint32_t rtt) {
    RTT = rtt;
}

std::uint32_t PackageHeader::GetRTT() const {
    return RTT;
}

std::uint64_t PackageHeader::GetSendingTime() const {
    return sending_time;
}

std::uint32_t PackageHeader::GetPackageIndex() const {
    return package_index;
}

std::uint64_t PackageHeader::GetSenderID() const {
    return sender_id;
}

bool PackageHeader::operator<(const PackageHeader &other) const {
    return RTT < other.GetRTT();
}

Event::Event(std::uint64_t id, ServerBase& initiator, event_type type, std::uint32_t packages_number)
    : id(id), server_initiator(initiator), type(type), packages_number(packages_number) { }

std::uint32_t Event::GetID() const {
    return id;
}

std::ostream& operator<<(std::ostream& out, const Event& event) {
    out << "[EVENT] ID: " << event.id
        << ". Initiator ID: " << event.server_initiator.GetID()
        << ". Type: " << (event.type == 0 ? "SEND_DATA" : "ACKNOWLEDGEMENT") 
        << ". Number of packages: " << event.packages_number << ".\n";
    
    return out;
}