#include "mplb_parser.hpp"

#include "metricable-cc/metricable_cc_parser.hpp"
#include "network/connection/mplb/single_cc/single_cc_mplb.hpp"
#include "parser/parse_utils.hpp"
#include "path-chooser/path_chooser_parser.hpp"

namespace sim {

std::shared_ptr<SingleCCMplb> parse_single_cc_mplb(
    const ConfigNodeWithPreset& node, Endpoints endpoints);

std::shared_ptr<INewMPLB> parse_i_mplb(const ConfigNodeWithPreset& node,
                                       Endpoints endpoints) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "single-cc") {
        return parse_single_cc_mplb(node, std::move(endpoints));
    }
    throw node.create_parsing_error(
        fmt::format("Unexpected mplb type: {}", type));
}

SingleCCMetricsFilters parse_metrics_filters(
    const ConfigNodeWithPresetExpected& node) {
    SingleCCMetricsFilters filters = SingleCCMplb::DEFAULT_METRICS_FILTERS;
    if (const auto& fairness_node = node["fairness"]; fairness_node) {
        filters.fairness = fairness_node.value().as_or_throw<bool>();
    }
    return filters;
}

std::shared_ptr<SingleCCMplb> parse_single_cc_mplb(
    const ConfigNodeWithPreset& node, Endpoints endpoints) {
    const ConfigNodeWithPreset& cc_node = node["cc"].value_or_throw();
    MetricableCC cc = parse_metricable_cc(cc_node);

    const ConfigNodeWithPreset& path_chooser_node =
        node["path-chooser"].value_or_throw();
    std::unique_ptr<IPathChooser> path_chooser =
        parse_i_path_chooser(path_chooser_node, std::move(endpoints));

    SizeByte packet_size =
        parse_size(node["packet-size"].value_or_throw().get_node());

    SingleCCMetricsFilters metrics_filters =
        parse_metrics_filters(node["metrics_filters"]);

    return SingleCCMplb::create_shared(std::move(cc), std::move(path_chooser),
                                       packet_size, metrics_filters);
}

}  // namespace sim
