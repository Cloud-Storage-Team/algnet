#pragma once

#include "i_new_flow.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace sim {

class INewFlowFactory {
public:
    virtual std::shared_ptr<INewFlow> create_flow(INewMPLB& mplb) = 0;
};

}  // namespace sim
