#pragma once
#include <type_traits>

#include "metrics_multi_id_table.hpp"
#include "utils/identifier_factory.hpp"

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
        for (auto [metric_name, metric_storage] : object_metrics_table) {
            result[std::move(metric_name)][id] = metric_storage;
        }
    }
    return result;
}

}  // namespace sim