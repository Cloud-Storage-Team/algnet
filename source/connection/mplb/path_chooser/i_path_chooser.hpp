#pragma once

#include "connection/flow/i_new_flow.hpp"

namespace sim {

class IPathChooser {
public:
    virtual std::shared_ptr<INewFlow> choose_flow() = 0;

    using FlowsSet = std::set<std::shared_ptr<INewFlow> >;

    virtual const FlowsSet& get_flows() const = 0;
};

}  // namespace sim
