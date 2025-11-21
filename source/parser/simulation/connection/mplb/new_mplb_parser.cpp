#include "new_mplb_parser.hpp"

#include "new_i_mocks/cc_factory_mock.hpp"
#include "new_i_mocks/new_conn_mock.hpp"
#include "new_i_mocks/new_mplb_mock.hpp"
#include "new_i_mocks/tcp_flow_factory_mock.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<INewMPLB> NewMplbParser::parse_i_mplb(
    const ConfigNode& node, std::shared_ptr<IHost> sender,
    std::shared_ptr<IHost> receiver) {
    return parse_mplb(node, sender, receiver);
}

std::shared_ptr<INewMPLB> NewMplbParser::parse_mplb(
    const ConfigNode& node, std::shared_ptr<IHost> sender,
    std::shared_ptr<IHost> receiver) {
    MPLBType mplb_type;
    std::string type =
        node["type"].value_or_throw().as_or_throw<std::string>();

    if (type == "single-cc") {
        mplb_type = MPLBType::SINGLE_CC;
    } else {
        throw std::runtime_error("Unknown MPLB type: " + type);
    }

    ConfigNode flow_node = node["flow"].value_or_throw();
    std::string flow_type =
        flow_node["type"].value_or_throw().as_or_throw<std::string>();

    std::unique_ptr<sim::INewFlowFactory> flow_factory;

    if (flow_type == "tcp") {
        test::TcpFlowFactoryParams params{.sender = sender,
                                          .receiver = receiver};
        flow_factory = std::make_unique<test::TcpFlowFactoryMock>(params);
    } else {
        throw std::runtime_error("Unsupported flow type for single-cc mplb: " +
                                 flow_type);
    }

    std::unique_ptr<sim::ITcpCCFactory> cc_factory;

    ConfigNode cc_node = node["cc"].value_or_throw();
    std::string cc_type =
        cc_node["type"].value_or_throw().as_or_throw<std::string>();

    // TODO: expand with other cc types (need add corresponding factories)
    if (cc_type == "basic") {
        auto cc_factory = std::make_unique<test::TcpCCFactoryMock>();
    } else {
        throw std::runtime_error("Unsupported cc type");
    }
    return std::make_shared<test::NewMPLBMock>(mplb_type);
}

}  // namespace sim
