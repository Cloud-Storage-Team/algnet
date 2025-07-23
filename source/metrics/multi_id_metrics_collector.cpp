#include <ranges>

#include "multi_id_metrics_storage.hpp"

namespace sim {
MultiIdMetricsStorage::MultiIdMetricsStorage(std::string a_metric_name,
                                             std::string a_filter)
    : metric_name(std::move(a_metric_name)), m_filter(a_filter) {}

void MultiIdMetricsStorage::add_record(Id id, TimeNs time, double value) {
    auto it = m_storage.find(id);
    if (it == m_storage.end()) {
        std::string filename = get_metrics_filename(id);
        if (!std::regex_match(filename, m_filter)) {
            m_storage[id] = std::nullopt;
        } else {
            MetricsStorage new_storage;
            new_storage.add_record(time, value);
            m_storage.emplace(std::move(id), std::move(new_storage));
        }
    } else if (it->second.has_value()) {
        it->second->add_record(time, value);
    }
}

void MultiIdMetricsStorage::export_to_files(
    std::filesystem::path output_dir_path) const {
    for (auto& [id, values] : m_storage) {
        if (values) {
            values->export_to_file(output_dir_path / get_metrics_filename(id));
        }
    }
}

std::unordered_map<Id, MetricsStorage> MultiIdMetricsStorage::data() const {
    std::unordered_map<Id, MetricsStorage> result;
    for (auto [id, maybe_storage] : m_storage) {
        if (maybe_storage) {
            result[id] = maybe_storage.value();
        }
    }
    return result;
}
std::string MultiIdMetricsStorage::get_metrics_filename(Id id) const {
    return fmt::format("{}/{}.txt", metric_name, id);
}

}  // namespace sim