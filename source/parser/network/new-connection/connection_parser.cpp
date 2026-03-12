#include "connection_parser.hpp"

#include "mplb/mplb_parser.hpp"
#include "network/connection/connection.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

std::shared_ptr<Connection> parse_connection(
    const ConfigNodeWithPreset& node, const utils::IdTable<IHost>& hosts_table);

std::shared_ptr<INewConnection> parse_i_connection(
    const ConfigNodeWithPreset& node,
    const utils::IdTable<IHost>& hosts_table) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "mplb-connection") {
        return parse_connection(node, hosts_table);
    }
    throw node.create_parsing_error(
        fmt::format("Unsupported connection type: {}", type));
}

std::shared_ptr<Connection> parse_connection(
    const ConfigNodeWithPreset& node,
    const utils::IdTable<IHost>& hosts_table) {
    Id connection_id = node.get_name_or_throw();

    Id sender_id =
        node["sender_id"].value_or_throw().as_or_throw<std::string>();
    Id receiver_id =
        node["receiver_id"].value_or_throw().as_or_throw<std::string>();

    std::shared_ptr<IHost> sender = hosts_table.get_or_throw(
        sender_id, node.create_parsing_error(fmt::format(
                       "Could not find sender with id {}", sender_id)));

    std::shared_ptr<IHost> receiver = hosts_table.get_or_throw(
        receiver_id, node.create_parsing_error(fmt::format(
                         "Could not find receiver with id {}", receiver_id)));

    std::shared_ptr<INewMPLB> mplb = parse_i_mplb(node["mplb"].value_or_throw(),
                                                  Endpoints{sender, receiver});

    return Connection::create_shared(std::move(connection_id), mplb);
}

}  // namespace sim
