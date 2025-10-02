#include "swift_cc_parser.hpp"

#include "parser/parse_utils.hpp"

namespace sim {
std::unique_ptr<TcpSwiftCC> SwiftCCParser::parse_swift_cc(
    const YAML::Node& node) {
    TimeNs a_base_target = parse_time(node["base_target"].as<std::string>());
    return std::make_unique<TcpSwiftCC>(a_base_target);
}
}  // namespace sim