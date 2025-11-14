#include "parser/simulation/connection/new_connection_parser.hpp"

#include "parser/simulation/connection/mplb/new_mplb_parser.hpp"

#include "new_i_mocks/new_conn_mock.hpp"
#include "new_i_mocks/new_mplb_mock.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<INewConnection> NewConnectionParser::parse_i_connection(
    const ConfigNode& node) {
    return parse_connection(node);
}

std::shared_ptr<INewConnection> NewConnectionParser::parse_connection(
    const ConfigNode& node) {
    Id conn_id = node.get_name_or_throw();

    Id sender_id = node["sender_id"].value_or_throw().as_or_throw<Id>();

    Id receiver_id = node["receiver_id"].value_or_throw().as_or_throw<Id>();

    std::shared_ptr<IHost> sender_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(sender_id);

    std::shared_ptr<IHost> receiver_ptr =
        IdentifierFactory::get_instance().get_object<IHost>(receiver_id);

    ConfigNode mplb_node = node["mplb"].value_or_throw();

    std::shared_ptr<INewMPLB> mplb =
        NewMplbParser::parse_i_mplb(mplb_node, sender_ptr, receiver_ptr);

    auto conn = std::make_shared<test::NewConnectionMock>(conn_id, sender_ptr,
                                                          receiver_ptr, mplb);

    auto& idf = IdentifierFactory::get_instance();
    idf.add_object(conn);

    return conn;
}

}  // namespace sim
