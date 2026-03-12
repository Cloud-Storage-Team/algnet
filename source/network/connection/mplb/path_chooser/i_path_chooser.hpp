#pragma once

#include "network/connection/flow/i_new_flow.hpp"
#include "utils/id_table.hpp"

namespace sim {

class IPathChooser {
public:
    using FlowsTable = utils::IdTable<INewFlow>;

    virtual std::shared_ptr<INewFlow> choose_flow() = 0;

    virtual const FlowsTable& get_flows_table() const = 0;
};

}  // namespace sim
