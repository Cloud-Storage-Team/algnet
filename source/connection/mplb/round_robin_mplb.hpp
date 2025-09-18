#pragma once

#include <memory>
#include <set>

#include "connection/mplb/i_mplb.hpp"
#include "flow/i_flow.hpp"
#include "utils/loop_iterator.hpp"

namespace sim {

class RoundRobinMPLB final : public IMPLB {
public:
    void add_flow(const std::shared_ptr<IFlow>& flow,
                  FlowSample initial_sample) override;

    void remove_flow(const std::shared_ptr<IFlow>& flow) override;

    std::shared_ptr<IFlow> select_flow() override;

    void notify_packet_confirmed(const std::shared_ptr<IFlow>& flow) override;

    void clear_flows() override;

private:
    using FlowPtr = std::shared_ptr<IFlow>;
    using FlowTable = std::map<FlowPtr, FlowSample, std::owner_less<FlowPtr>>;

    FlowTable m_flows;
    LoopIterator<FlowTable::iterator> m_current_flow;
};

}  // namespace sim
