#pragma once
#include "parser/config_reader/config_node.hpp"
#include "new_i_mocks/new_mplb_mock.hpp"

namespace sim {

class SingleCCMplbParser {
public:
    static std::shared_ptr<test::NewMPLBMock> parse(
        const ConfigNode& node, std::shared_ptr<IHost> sender,
        std::shared_ptr<IHost> receiver);
};

}  // namespace sim
