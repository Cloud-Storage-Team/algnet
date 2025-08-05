
#include "hasher.hpp"

#include <spdlog/fmt/fmt.h>

#include "scheduler.hpp"

namespace sim {

std::uint32_t RandomHasher::get_hash(Packet) {
    return static_cast<std::uint32_t>(std::rand());
};

static Id get_flow_id(IFlow* flow_ptr) {
    return (flow_ptr == nullptr ? "" : flow_ptr->get_id());
}

std::uint32_t ECMPHasher::get_hash(Packet packet) {
    std::string flow_id_str = get_flow_id(packet.flow);

    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {}", flow_id_str, packet.source_id, packet.dest_id);
    return static_cast<uint32_t>(hasher(header_str));
};

SaltECMPHasher::SaltECMPHasher(Id a_device_id)
    : m_device_id(std::move(a_device_id)) {}

std::uint32_t SaltECMPHasher::get_hash(Packet packet) {
    std::string flow_id_str = get_flow_id(packet.flow);

    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {} {}", flow_id_str, packet.source_id,
                    packet.dest_id, m_device_id);
    return static_cast<uint32_t>(hasher(header_str));
}

FLowletHasher::FLowletHasher(TimeNs a_flowlet_threshold)
    : m_flowlet_threshold(a_flowlet_threshold) {}

static inline std::uint32_t sum_with_overflow(std::uint32_t first,
                                              std::uint32_t second) {
    std::uint32_t max = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t second_to_max = max - second;
    if (first > second_to_max) {
        return first - second_to_max;
    }
    return first + second;
}

std::uint32_t FLowletHasher::get_hash(Packet packet) {
    std::uint32_t ecmp_hash = m_ecmp_hasher.get_hash(packet);

    Id flow_id = get_flow_id(packet.flow);
    TimeNs curr_time = Scheduler::get_instance().get_current_time();
    auto it = m_flow_table.find(flow_id);

    if (it == m_flow_table.end()) {
        m_flow_table[flow_id] = {curr_time, 0};
        return ecmp_hash;
    }

    auto& [last_seen, shift] = it->second;
    TimeNs elapced_from_last_seen = curr_time - last_seen;

    if (elapced_from_last_seen > m_flowlet_threshold) {
        shift++;
    }
    last_seen = curr_time;

    return sum_with_overflow(ecmp_hash, shift);
}

std::uint32_t SymmetricHasher::get_hash(Packet packet) {
    std::hash<std::string> hasher;

    std::string combined_id_str =
        std::to_string(hasher(packet.source_id) ^ hasher(packet.dest_id));
    std::string flow_id_str = get_flow_id(packet.flow);

    std::string header_str = fmt::format("{} {}", flow_id_str, combined_id_str);
    return static_cast<uint32_t>(hasher(header_str));
};

}  // namespace sim
