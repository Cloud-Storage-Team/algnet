#pragma once
#include <spdlog/fmt/fmt.h>

#include <regex>
#include <type_traits>

#include "metrics_storage.hpp"

namespace sim {
class MultiIdMetricsStorage {
public:
    MultiIdMetricsStorage(std::string a_metric_name);

    void add_record(Id id, TimeNs time, double value);
    void export_to_files(std::filesystem::path output_dir_path) const;

    std::unordered_map<Id, MetricsStorage> data() const;

    void set_filter(std::string filter);

private:
    std::string get_metrics_filename(Id id) const;

    std::string metric_name;
    // If m_storage does not contain some id, there was no check is metrics file
    // name for id correspond to m_filter
    // If m_storage[id] = std::nullopt, this check was failed
    // Otherwice, check was succseed
    std::unordered_map<Id, std::optional<MetricsStorage> > m_storage;
    std::regex m_filter;
};
}  // namespace sim