#include "connection_parser.hpp"

#include "mplb/mplb_parser.hpp"
#include "network/connection/connection.hpp"
#include "rdma_connection_parser.hpp"
#include "utils/identifiable.hpp"

namespace sim {

std::shared_ptr<Connection> parse_mplb_connection(
    std::shared_ptr<IHost> sender, std::shared_ptr<IHost> receiver,
    const ConfigNodeWithPreset& node, const Id& connection_id);

std::shared_ptr<IConnection> parse_i_connection(
    std::shared_ptr<IHost> sender, std::shared_ptr<IHost> receiver,
    const ConfigNodeWithPreset& params) {
    return parse_i_connection(
        sender, receiver, params,
        fmt::format("{}--{}", sender->get_id(), receiver->get_id()));
}

std::shared_ptr<IConnection> parse_i_connection(
    std::shared_ptr<IHost> sender, std::shared_ptr<IHost> receiver,
    const ConfigNodeWithPreset& params, const Id& connection_id) {
    std::string type =
        params["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "mplb-connection") {
        return parse_mplb_connection(sender, receiver, params, connection_id);
    } else if (type == "rdma") {
        return parse_rdma_connection(sender, receiver, params, connection_id);
    }
    throw params.create_parsing_error(
        fmt::format("Unsupported connection type: {}", type));
}

std::shared_ptr<IConnection> parse_i_connection(
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

    return parse_i_connection(sender, receiver, node, node.get_name_or_throw());
}

std::shared_ptr<Connection> parse_mplb_connection(
    std::shared_ptr<IHost> sender, std::shared_ptr<IHost> receiver,
    const ConfigNodeWithPreset& node, const Id& connection_id) {
    std::shared_ptr<IMPLB> mplb = parse_i_mplb(node["mplb"].value_or_throw(),
                                               Endpoints{sender, receiver});

    return Connection::create_shared(connection_id, mplb);
}

}  // namespace sim
