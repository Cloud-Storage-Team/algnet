#pragma once
#include "connection/mplb/path_chooser/i_path_chooser.hpp"

namespace test {

class PathChooserMock : public sim::IPathChooser {
public:
    PathChooserMock(std::set<std::shared_ptr<sim::INewFlow> > a_flows)
        : m_flows(a_flows) {};

    virtual std::shared_ptr<sim::INewFlow> choose_flow() final {
        return *m_flows.begin();
    }

    virtual const std::set<std::shared_ptr<sim::INewFlow> > get_flows()
        const final {
        return m_flows; 
    }

private:
    std::set<std::shared_ptr<sim::INewFlow> > m_flows;
};

}  // namespace test
