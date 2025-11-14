#include "single_cc_mplb_parser.hpp"

#include "parser/parse_utils.hpp"
#include "new_i_mocks/cc_factory_mock.hpp"
#include "new_i_mocks/flow_factory_mock.hpp"

namespace sim {

std::shared_ptr<test::NewMPLBMock> SingleCCMplbParser::parse(
    const ConfigNode& node, std::shared_ptr<IHost> sender,
    std::shared_ptr<IHost> receiver) {
    ConfigNode flow_node = node["flow"].value_or_throw();
    auto flow_factory =
        std::make_unique<test::FlowFactoryMock>(flow_node.get_node(), sender, receiver);

    ConfigNode cc_node = node["cc"].value_or_throw();
    auto cc_factory = std::make_unique<test::TcpCCFactoryMock>(cc_node.get_node());

    return std::make_shared<test::NewMPLBMock>(sender, receiver, std::move(flow_factory),
                                               std::move(cc_factory));
}

}  // namespace sim
