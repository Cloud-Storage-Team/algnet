#include "tahoe_cc_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {
std::unique_ptr<TcpTahoeCC> TahoeCCParser::parse_tahoe_cc(
    const ConfigNode& node) {
    double start_cwnd = simple_parse_with_default(
        node, "start_cwnd", TcpTahoeCC::DEFAULT_START_CWND);
    double sstresh =
        simple_parse_with_default(node, "sstresh", TcpTahoeCC::DEFAULT_SSTRESH);
    return std::make_unique<TcpTahoeCC>(start_cwnd, sstresh);
}
}  // namespace sim