#include <ranges>

#include "packet_storage.hpp"
#include "utils/safe_matplot.hpp"

namespace sim {
PacketStorage::PacketStorage(std::string a_metric_name,
                                std::string a_filter)
    : metric_name(std::move(a_metric_name)), m_filter(a_filter) {}

void PacketStorage::add_record(Id id, Packet packet) {
    auto it = m_storage.find(id);
    if (it == m_storage.end()) {
        std::string filename = get_metrics_filename(id);
        if (!std::regex_match(filename, m_filter)) {
            m_storage[id] = std::nullopt;
        } else {
            std::vector<Packet> new_storage;
            new_storage.push_back(packet);
            m_storage.emplace(std::move(id), std::move(new_storage));
        }
    } else if (it->second.has_value()) {
        it->second->push_back(packet);
    }
}

void PacketStorage::export_to_file(std::filesystem::path path, std::vector<Packet> device_data) const {
    utils::create_all_directories(path);
    std::ofstream output_file(path);
    if (!output_file) {
        throw std::runtime_error("Failed to create file for metric values");
    }

    for (const auto& packet : device_data) {
        output_file << packet.to_string() << std::endl;
        output_file << packet.get_telemetry_string() << std::endl;
        output_file << std::endl;
    }

    output_file.close();
}

void PacketStorage::export_to_files(
    std::filesystem::path output_dir_path) const {
    for (auto& [id, values] : data()) {
        export_to_file(output_dir_path / get_metrics_filename(id), values);
    }
}

std::unordered_map<Id, std::vector<Packet>> PacketStorage::data() const {
    std::unordered_map<Id, std::vector<Packet>> result;
    result.reserve(m_storage.size());
    for (auto [id, maybe_storage] : m_storage) {
        if (maybe_storage) {
            result.emplace(std::move(id), maybe_storage.value());
        }
    }
    return result;
}
std::string PacketStorage::get_metrics_filename(Id id) const {
    return fmt::format("{}/device_{}.txt", metric_name, id);
}

}  // namespace sim