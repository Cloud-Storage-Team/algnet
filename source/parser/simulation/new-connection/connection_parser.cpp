#include "connection_parser.hpp"

#include "connection/new_connection.hpp"
#include "mplb/mplb_parser.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

std::shared_ptr<NewConnection> parse_connection(
    const ConfigNodeWithPreset& node);

std::shared_ptr<INewConnection> parse_i_connection(
    const ConfigNodeWithPreset& node) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "mplb-connection") {
        return parse_connection(node);
    }
    throw node.create_parsing_error(
        fmt::format("Unsupported connection type: {}", type));
}

std::shared_ptr<NewConnection> parse_connection(
    const ConfigNodeWithPreset& node) {
    Id connection_id = node.get_name_or_throw();

    Id sender_id =
        node["sender_id"].value_or_throw().as_or_throw<std::string>();
    Id receiver_id =
        node["receiver_id"].value_or_throw().as_or_throw<std::string>();

    std::shared_ptr<IHost> sender =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    if (sender == nullptr) {
        throw node.create_parsing_error(
            fmt::format("Could not find sender with id {}", sender_id));
    }

    std::shared_ptr<IHost> receiver =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

    if (receiver == nullptr) {
        throw node.create_parsing_error(
            fmt::format("Could not find receiver with id {}", receiver_id));
    }

    std::shared_ptr<INewMPLB> mplb =
        parse_i_mplb(node, Endpoints{sender, receiver});

    return NewConnection::create_shared(std::move(connection_id), mplb);
}

}  // namespace sim
