#include "i_tcp_cc.hpp"

namespace sim {

class ITcpCCFactory {
public:
    virtual std::unique_ptr<ITcpCC> create_cc() = 0;
};

}  // namespace sim