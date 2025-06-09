#pragma once

#include "flow/interfaces/i_flow.hpp"

namespace sim {

class ITcpFlow : public IFlow {
public:
    virtual ~ITcpFlow() = default;
    virtual double get_cwnd() const = 0;
};

}  // namespace sim
