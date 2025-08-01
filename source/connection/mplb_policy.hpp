#pragma once

#include <memory>
#include <vector>

#include "flow/i_flow.hpp"
#include "types.hpp"
#include "packet.hpp"

namespace sim {

class MPLBPolicy {
public:
    virtual ~MPLBPolicy() = default;

    virtual std::shared_ptr<IFlow>
    select_flow(const std::vector<std::shared_ptr<IFlow>>& flows) = 0;

    virtual TimeNs next_pacing_delay(const std::shared_ptr<IFlow>&) const {
        return TimeNs(0);
    }

    virtual void on_ack(const std::shared_ptr<IFlow>&, const Packet&) {}
};

} // namespace sim
