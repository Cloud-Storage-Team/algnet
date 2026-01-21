#pragma once
#include <vector>

#include "connection/mplb/i_new_mplb.hpp"

namespace test {
class MplbMock : public sim::INewMPLB {
public:
    MplbMock(SizeByte quota, bool a_send_immideatly = true)
        : m_quota(quota), m_send_immediatly(a_send_immideatly) {}

    utils::StrExpected<void> send_data([[maybe_unused]] sim::Data data,
                                       OnDeliveryCallback callback) final {
        if (m_send_immediatly) {
            callback();
        } else {
            m_callbacks.emplace_back(callback);
        }
        return {};
    }

    sim::MPLBContext get_context() const final {
        return sim::MPLBContext{m_flows, SizeByte(0), SizeByte(0), m_quota};
    }

    void send_all_data() {
        for (auto callback : m_callbacks) {
            callback();
        }
        m_callbacks.clear();
    }

private:
    std::set<std::shared_ptr<sim::INewFlow> > m_flows;
    SizeByte m_quota;
    std::vector<OnDeliveryCallback> m_callbacks;
    bool m_send_immediatly;
};
}  // namespace test
