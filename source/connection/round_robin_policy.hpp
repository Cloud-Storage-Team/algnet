#pragma once

#include <cstddef>

#include "mplb_policy.hpp"

namespace sim {

class RoundRobinPolicy : public MPLBPolicy {
public:
    RoundRobinPolicy();
    ~RoundRobinPolicy() override = default;

    std::shared_ptr<IFlow>
    select_flow(const std::vector<std::shared_ptr<IFlow>>& flows) override;

private:
    std::size_t m_next{0};
};

}  // namespace sim
