#include "tahoe_cc_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {
std::unique_ptr<TcpTahoeCC> TahoeCCParser::parse_tahoe_cc(
    const ConfigNode& node) {
    auto double_parser = [](const ConfigNode& double_node) {
        return double_node.as<double>().value_or_throw();
    };

    double start_cwnd = node["start_cwnd"].apply_or_default<double>(
        double_parser, TcpTahoeCC::DEFAULT_START_CWND);
    double sstresh = node["sstresh"].apply_or_default<double>(
        double_parser, TcpTahoeCC::DEFAULT_SSTRESH);
    return std::make_unique<TcpTahoeCC>(start_cwnd, sstresh);
}
}  // namespace sim