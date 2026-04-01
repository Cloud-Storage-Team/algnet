#pragma once
#include <concepts>
#include <filesystem>
#include <type_traits>

#include "metrics_multi_id_table.hpp"
#include "utils/identifiable.hpp"

namespace sim {

template <typename T>
concept IdentifiableAndMetricable =
    std::derived_from<T, Identifiable> && std::derived_from<T, IMetricable>;

template <IdentifiableAndMetricable T>
MetricsMultiIdTable collect_and_combine_metrics(
    const utils::IdTable<T>& table) {
    MetricsMultiIdTable result;
    for (const auto& [id, metricable] : table) {
        MetricsTable object_metrics_table = metricable->get_metrics_table();
        for (const auto& [metric_name, metric_storage] : object_metrics_table) {
            result[metric_name][id] = metric_storage;
        }
    }
    return result;
}

template <IdentifiableAndMetricable T>
void collect_and_save_all_metrics(const utils::IdTable<T>& id_table,
                                  std::filesystem::path output_dir) {
    MetricsMultiIdTable metrics_multi_id_table =
        collect_and_combine_metrics(id_table);

    metrics_multi_id_table.draw_plots(output_dir);
    for (const auto& [id, object] : id_table) {
        std::filesystem::path flow_path = output_dir / id;

        object->write_inner_metrics(std::move(flow_path));
    }
}

}  // namespace sim