#pragma once

#include "connection/flow/i_new_flow.hpp"

namespace sim {

class IPathChooser {
public:
    virtual std::shared_ptr<INewFlow> choose_flow();

    virtual const std::set<std::shared_ptr<INewFlow> > get_flows() const;
};

}  // namespace sim
