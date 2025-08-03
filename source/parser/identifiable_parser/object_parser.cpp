// #include "parser/identifiable_parser/object_parser.hpp"

// #include "flow/tcp/tcp_flow.hpp"
// #include "device/host.hpp"
// #include "link/link.hpp"
// #include "device/switch.hpp"

// namespace sim {

//     static std::shared_ptr<ISwitch> parse_switch(const YAML::Node& key_node,
//                                            const YAML::Node& value_node) {
//         std::string type = value_node["type"].as<std::string>();
//         if (type == "tcp") {
//             return Parser<TcpFlow>::parse_object(key_node, value_node);
//         }
//         throw "wow";
//     }

//     static std::shared_ptr<IFlow> parse_flow(const YAML::Node& key_node,
//                                            const YAML::Node& value_node) {
//         return Parser<Switch>::parse_object(key_node, value_node);
//     }

//     static std::shared_ptr<IHost> parse_host(const YAML::Node& key_node,
//                                            const YAML::Node& value_node) {
//         return Parser<Host>::parse_object(key_node, value_node);
//     }

//     static std::shared_ptr<ILink> parse_link(const YAML::Node& key_node,
//                                            const YAML::Node& value_node) {
//         return Parser<Switch>::parse_object(key_node, value_node);
//     }

// }  // namespace sim
