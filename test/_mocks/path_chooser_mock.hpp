#pragma once
#include "connection/mplb/path_chooser/i_path_chooser.hpp"

namespace test {

class PathChooserMock : public sim::IPathChooser {
public:
    PathChooserMock(FlowsSet a_flows) : m_flows(std::move(a_flows)) {};

    virtual std::shared_ptr<sim::INewFlow> choose_flow() final {
        return m_flows.begin()->second;
    }

    virtual const FlowsSet& get_flows() const final { return m_flows; }

private:
    FlowsSet m_flows;
};

}  // namespace test
