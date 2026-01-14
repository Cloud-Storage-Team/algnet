#pragma once
#include "connection/mplb/i_new_mplb.hpp"

namespace test {
class MplbMock : public sim::INewMPLB {
public:
    MplbMock(SizeByte quota) : m_context{{}, SizeByte(0), SizeByte(0), quota} {}

    utils::StrExpected<void> send_data([[maybe_unused]] sim::Data data,
                                       OnDeliveryCallback callback) final {
        callback();
        return {};
    }

    const sim::MPLBContext& get_context() const final { return m_context; }

private:
    sim::MPLBContext m_context;
};
}  // namespace test