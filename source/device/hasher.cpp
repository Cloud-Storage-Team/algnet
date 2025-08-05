
#include "hasher.hpp"

#include <spdlog/fmt/fmt.h>

#include "scheduler.hpp"

namespace sim {

std::uint32_t RandomHasher::get_hash(Packet, Id) {
    return static_cast<std::uint32_t>(std::rand());
};

std::uint32_t ECMPHasher::get_hash(Packet packet, Id) {
    std::string flow_id_str =
        ((packet.flow == nullptr) ? "0" : packet.flow->get_id());

    std::hash<std::string> hasher;
    std::string header_str =
        fmt::format("{} {} {}", flow_id_str, packet.source_id, packet.dest_id);
    return static_cast<uint32_t>(hasher(header_str));
};

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

std::uint32_t FLowletHasher::get_hash(Packet packet, Id device_id) {
    std::uint32_t ecmp_hash =
        m_ecmp_hasher.get_hash(packet, std::move(device_id));

    Id flow_id = packet.flow->get_id();
    TimeNs curr_time = Scheduler::get_instance().get_current_time();
    auto last_record_it = m_last_record.find(flow_id);

    if (last_record_it == m_last_record.end()) {
        m_last_record[flow_id] = curr_time;
        m_flow_to_shift[flow_id] = 0;
        return ecmp_hash;
    }

    TimeNs& last_seen = last_record_it->second;
    TimeNs elapced_from_last_seen = curr_time - last_seen;
    last_seen = curr_time;

    std::uint32_t& shift = m_flow_to_shift[flow_id];
    if (elapced_from_last_seen > m_flowlet_threshold) {
        shift++;
    }
    return sum_with_overflow(ecmp_hash, shift);
}

std::uint32_t SymmetricHasher::get_hash(Packet packet, Id) {
    std::hash<std::string> hasher;

    std::string combined_id_str =
        std::to_string(hasher(packet.source_id) ^ hasher(packet.dest_id));
    std::string flow_id_str =
        ((packet.flow == nullptr) ? "0" : packet.flow->get_id());

    std::string header_str = fmt::format("{} {}", flow_id_str, combined_id_str);
    return static_cast<uint32_t>(hasher(header_str));
};

}  // namespace sim
