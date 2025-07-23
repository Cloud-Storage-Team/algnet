#pragma once
#include <filesystem>
#include <map>
#include <optional>
#include <regex>
#include <utility>

#include "link/packet_queue/link_queue.hpp"
#include "metrics_storage.hpp"
#include "types.hpp"

namespace sim {
class LinksQueueSizeStorage {
public:
    LinksQueueSizeStorage(std::string filter);

    void add_record(Id id, LinkQueueType type, TimeNs time, double value);
    void export_to_files(std::filesystem::path output_dir_path) const;
    void draw_plots(std::filesystem::path output_dir_path) const;

    std::map<std::pair<Id, LinkQueueType>, MetricsStorage> data() const;

private:
    std::string get_metrics_filename(Id id) const;

    // If m_storage does not contain some id, there was no check is metrics file
    // name for id correspond to m_filter
    // If m_storage[id] = std::nullopt, this check was failed
    // Otherwice, check was succseed
    std::map<std::pair<Id, LinkQueueType>, std::optional<MetricsStorage>>
        m_storage;

    std::regex m_filter;
};
}  // namespace sim