#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "connection/i_new_connection.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class NewConnectionParser {
public:
    static std::shared_ptr<INewConnection> parse_i_connection(
        const ConfigNode& node);

private:
    static std::shared_ptr<INewConnection> parse_connection(
        const ConfigNode& node);
};

}  // namespace sim
