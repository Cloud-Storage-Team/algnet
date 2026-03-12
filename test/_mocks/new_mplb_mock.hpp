#pragma once
#include <memory>
#include <set>
#include <vector>

#include "network/connection/mplb/i_new_mplb.hpp"

namespace test {
class MplbMock : public sim::INewMPLB {
public:
    MplbMock(SizeByte quota, bool a_send_immediately = true)
        : m_quota(quota), m_send_immediately(a_send_immediately) {}

    utils::StrExpected<void> send_data([[maybe_unused]] sim::Data data,
                                       OnDeliveryCallback callback) final {
        if (m_send_immediately) {
            callback();
        } else {
            m_callbacks.emplace_back(callback);
        }
        return {};
    }

    sim::MPLBContext get_context() const final {
        return sim::MPLBContext{SizeByte(0), SizeByte(0), m_quota};
    }

    void send_all_data() {
        for (size_t i = 0; i < m_callbacks.size(); i++) {
            m_callbacks[i]();
        }
        m_callbacks.clear();
    }

    sim::MetricsTable get_metrics_table() const final { return {}; }

    void write_inner_metrics(
        [[maybe_unused]] std::filesystem::path path) const final {}

private:
    SizeByte m_quota;
    std::vector<OnDeliveryCallback> m_callbacks;
    bool m_send_immediately;
};
}  // namespace test
