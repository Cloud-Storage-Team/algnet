#pragma once
#include <spdlog/fmt/fmt.h>

#include <regex>
#include <type_traits>

#include "metrics_storage.hpp"

namespace sim {
class MultiIdMetricsStorage {
public:
    MultiIdMetricsStorage(std::string a_metric_name);

    void add_record(Id id, Time time, double value);
    void export_to_files(std::filesystem::path output_dir_path) const;

    const std::unordered_map<Id, MetricsStorage>& data() const;

    void set_filter(std::string filter);

private:
    std::string get_metrics_filename(Id id) const;

    std::string metric_name;
    std::unordered_map<Id, MetricsStorage> m_storage;
    std::regex m_filter;
};
}  // namespace sim