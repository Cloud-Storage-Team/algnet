#include "i_new_flow.hpp"

namespace sim {

class INewFlowFactory {
    virtual std::shared_ptr<INewFlow> create_flow() = 0;
};

}  // namespace sim
