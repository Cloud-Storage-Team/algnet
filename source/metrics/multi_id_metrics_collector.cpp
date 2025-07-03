#include "multi_id_metrics_storage.hpp"

namespace sim {
MultiIdMetricsStorage::MultiIdMetricsStorage(std::string a_metric_name)
    : metric_name(std::move(a_metric_name)) {}

void MultiIdMetricsStorage::add_record(Id id, Time time, double value) {
    if (std::regex_match(get_metrics_filename(id), m_filter)) {
        m_storage[id].add_record(time, value);
    }
}

void MultiIdMetricsStorage::export_to_files(
    std::filesystem::path output_dir_path) const {
    for (auto& [id, values] : m_storage) {
        values.export_to_file(output_dir_path / get_metrics_filename(id));
    }
}

const std::unordered_map<Id, MetricsStorage>& MultiIdMetricsStorage::data()
    const {
    return m_storage;
}

void MultiIdMetricsStorage::set_filter(std::string filter) {
    m_filter = std::regex(filter);
}

std::string MultiIdMetricsStorage::get_metrics_filename(Id id) const {
    return fmt::format("{}/{}.txt", metric_name, id);
}

}  // namespace sim
