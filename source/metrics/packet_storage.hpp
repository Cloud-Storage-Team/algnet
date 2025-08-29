#pragma once
#include <spdlog/fmt/fmt.h>

#include <regex>
#include <type_traits>

#include "metrics_storage.hpp"
#include "packet.hpp"

namespace sim {

class PacketStorage {
public:
    // TODO: think about getting list of metric names for packet and working with all packet metrics from the same storage
    PacketStorage(std::string a_metric_name, std::string a_filter);

    void add_record(Id id, Packet packet);
    void export_to_file(std::filesystem::path output_dir_path, std::vector<Packet>) const;
    void export_to_files(std::filesystem::path output_dir_path) const;

    std::unordered_map<Id, std::vector<Packet>> data() const;

private:
    std::string get_metrics_filename(Id id) const;

    std::string metric_name;
    // If m_storage does not contain some id, there was no check is metrics file
    // name for id correspond to m_filter
    // If m_storage[id] = std::nullopt, this check was failed
    // Otherwice, check was succseed
    std::unordered_map<Id, std::optional<std::vector<Packet>> > m_storage;
    std::regex m_filter;
};
}  // namespace sim