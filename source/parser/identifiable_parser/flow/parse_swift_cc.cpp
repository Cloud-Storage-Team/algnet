// #include "flow/tcp/swift/swift_cc.hpp"
// #include "parse_tcp_flow.hpp"
// #include "parser/parse_utils.hpp"

// namespace sim {
// template <>
// TcpSwiftCC Parser<TcpFlow<TcpSwiftCC>>::parse_tcp_cc(
//     [[maybe_unused]] const YAML::Node& key_node,
//                      const YAML::Node& value_node) {
//     // Parse the Swift CC parameters from the YAML node
//     // TO-DO: Automatic calculation of base_target based on topology
//     TimeNs a_base_target = parse_time(value_node["cc"]["base_target"].as<std::string>());
//     return TcpSwiftCC(a_base_target);
// }

// }  // namespace sim
