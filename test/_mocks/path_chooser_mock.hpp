#pragma once
#include "connection/mplb/path_chooser/i_path_chooser.hpp"

namespace test {

class PathChooserMock : public sim::IPathChooser {
public:
    PathChooserMock(FlowsTable a_flows) : m_flows(std::move(a_flows)) {};

    virtual std::shared_ptr<sim::INewFlow> choose_flow() final {
        return m_flows.begin()->second;
    }

    virtual const FlowsTable& get_flows_table() const final { return m_flows; }

private:
    FlowsTable m_flows;
};

}  // namespace test
