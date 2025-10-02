#include "swift_cc_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {
std::unique_ptr<TcpSwiftCC> SwiftCCParser::parse_swift_cc(
    const YAML::Node& node) {
    double start_cwnd = simple_parse_with_default(
        node, "start_cwnd", TcpSwiftCC::DEFAULT_START_CWND);
    double ai = simple_parse_with_default(node, "ai", TcpSwiftCC::DEFAULT_AI);
    double md = simple_parse_with_default(node, "md", TcpSwiftCC::DEFAULT_MD);
    double max_mdf =
        simple_parse_with_default(node, "max_mdf", TcpSwiftCC::DEFAULT_MAX_MDF);
    double fs_range = simple_parse_with_default(node, "fs_range",
                                                TcpSwiftCC::DEFAULT_FS_RANGE);
    double fs_min_cwnd = simple_parse_with_default(
        node, "fs_min_cwnd", TcpSwiftCC::DEFAULT_FS_MIN_CWND);
    double fs_max_cwnd = simple_parse_with_default(
        node, "fs_max_cwnd", TcpSwiftCC::DEFAULT_FS_MAX_CWND);

    TimeNs base_target = parse_time(node["base_target"].as<std::string>());

    return std::make_unique<TcpSwiftCC>(base_target, start_cwnd, ai, md,
                                        max_mdf, fs_range, fs_min_cwnd,
                                        fs_max_cwnd);
}
}  // namespace sim