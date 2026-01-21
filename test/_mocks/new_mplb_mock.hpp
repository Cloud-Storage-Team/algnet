#pragma once
#include <vector>

#include "connection/mplb/i_new_mplb.hpp"

namespace test {
class MplbMock : public sim::INewMPLB {
public:
    MplbMock(SizeByte quota, bool a_send_immideatly = true)
        : m_context{{}, SizeByte(0), SizeByte(0), quota},
          m_send_immediatly(a_send_immideatly) {}

    utils::StrExpected<void> send_data([[maybe_unused]] sim::Data data,
                                       OnDeliveryCallback callback) final {
        if (m_send_immediatly) {
            callback();
        } else {
            m_callbacks.emplace_back(callback);
        }
        return {};
    }

    const sim::MPLBContext& get_context() const final { return m_context; }

    void send_all_data() {
        for (auto callback : m_callbacks) {
            callback();
        }
        m_callbacks.clear();
    }

private:
    sim::MPLBContext m_context;
    std::vector<OnDeliveryCallback> m_callbacks;
    bool m_send_immediatly;
};
}  // namespace test
