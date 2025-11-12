#include "i_tcp_cc.hpp"

namespace sim {

enum CCScope {
    CCPerFlow,
    CCShared
};

class ITcpCCFactory {
public:
    virtual std::unique_ptr<ITcpCC> create_cc() = 0;
};

}  // namespace sim