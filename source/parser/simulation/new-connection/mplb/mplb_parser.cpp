#include "mplb_parser.hpp"

#include "connection/mplb/single_cc/single_cc_mplb.hpp"
#include "parser/parse_utils.hpp"
#include "parser/simulation/connection/flow/tcp/cc/tcp_cc_parser.hpp"
#include "path-chooser/path_chooser_parser.hpp"

namespace sim {

std::shared_ptr<SingleCCMplb> parse_single_cc_mplb(
    const ConfigNodeWithPreset& node, Endpoints endpoints);

std::shared_ptr<SingleCCMplb> parse_i_mplb(const ConfigNodeWithPreset& node,
                                           Endpoints endpoints) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "single-cc") {
        return parse_single_cc_mplb(node, std::move(endpoints));
    }
    throw node.create_parsing_error(
        fmt::format("Unexpected mplb type: {}", type));
}

std::shared_ptr<SingleCCMplb> parse_single_cc_mplb(
    const ConfigNodeWithPreset& node, Endpoints endpoints) {
    // TODO: replace ConfigNode with ConfigNodeWithPreset inside CC parser
    ConfigNode cc_node = node["cc"].value_or_throw().get_node();
    std::unique_ptr<ITcpCC> cc = TcpCCParser::parse_i_tcp_cc(cc_node);

    const ConfigNodeWithPreset& path_chooser_node =
        node["path-chooser"].value_or_throw();
    std::unique_ptr<IPathChooser> path_chooser =
        parse_i_path_chooser(path_chooser_node, std::move(endpoints));

    SizeByte packet_size =
        parse_size(node["packet-size"].value_or_throw().get_node());

    return SingleCCMplb::create_shared(std::move(cc), std::move(path_chooser),
                                       packet_size);
}

}  // namespace sim
