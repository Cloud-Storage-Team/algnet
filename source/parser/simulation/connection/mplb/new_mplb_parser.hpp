#pragma once
#include <spdlog/fmt/fmt.h>
#include <memory>

#include "connection/mplb/i_new_mplb.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class NewMplbParser {
public:
    static std::shared_ptr<INewMPLB> parse_i_mplb(
        const ConfigNode& node, std::shared_ptr<IHost> sender,
        std::shared_ptr<IHost> receiver);
private:
    static std::shared_ptr<INewMPLB> parse_mplb(
        const ConfigNode& node, std::shared_ptr<IHost> sender,
        std::shared_ptr<IHost> receiver);
};

}  // namespace sim
