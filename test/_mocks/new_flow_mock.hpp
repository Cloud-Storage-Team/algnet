#pragma once

#include "connection/flow/i_new_flow.hpp"

namespace test {
class NewFlowMock : public sim::INewFlow {
public:
    NewFlowMock(bool a_send_emmediately = true)
        : m_send_immediately(a_send_emmediately),
          m_context{SizeByte(0),
                    SizeByte(0),
                    SizeByte(0),
                    std::nullopt,
                    std::nullopt,
                    utils::Statistics<TimeNs>(),
                    std::shared_ptr<sim::IHost>(nullptr),
                    std::shared_ptr<sim::IHost>(nullptr)} {}

    Id get_id() const final { return ""; }

    void send(std::vector<sim::PacketInfo> packets_info) final {
        for (sim::PacketInfo info : packets_info) {
            if (m_send_immediately) {
                info.callback(sim::PacketAckInfo{TimeNs(0), TimeNs(0), false});
            } else {
                m_sending_queue.emplace_back(info);
            }
        }
    }

    sim::FlowContext& get_mutable_context() { return m_context; }

    const sim::FlowContext& get_context() final { return m_context; }

private:
    bool m_send_immediately;
    std::vector<sim::PacketInfo> m_sending_queue;
    sim::FlowContext m_context;
};

}  // namespace test
