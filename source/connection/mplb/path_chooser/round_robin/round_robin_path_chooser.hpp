#pragma once
#include "../i_path_chooser.hpp"
#include "utils/loop_iterator.hpp"

namespace sim {

class RoundRobinPathChooser : public IPathChooser {
public:
    explicit RoundRobinPathChooser(utils::IdTable<INewFlow> a_flows);

    virtual std::shared_ptr<INewFlow> choose_flow() final;

    virtual const FlowsSet& get_flows() const final;

private:
    FlowsSet m_flows;
    LoopIterator<FlowsSet::iterator> m_next_flow;
};

}  // namespace sim
