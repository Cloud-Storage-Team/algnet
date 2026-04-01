#pragma once

#include "network/connection/flow/i_flow.hpp"

namespace test {
class FlowMock : public sim::IFlow {
public:
    FlowMock(bool a_send_emmediately = true)
        : m_send_immediately(a_send_emmediately),
          m_context{sim::FlowFourTuple(
              sim::Endpoints(std::shared_ptr<sim::IHost>(nullptr),
                             std::shared_ptr<sim::IHost>(nullptr)))} {}

    const Id& get_id() const final { return m_id; }

    void send(std::vector<sim::PacketInfo> packets_info) final {
        for (auto& info : packets_info) {
            if (m_send_immediately) {
                info.callback(sim::PacketAckInfo{TimeNs(0), TimeNs(0), false});
            } else {
                m_sending_queue.emplace_back(info);
            }
        }
    }

    sim::FlowContext& get_mutable_context() { return m_context; }

    const sim::FlowContext& get_context() const final { return m_context; }

    virtual sim::MetricsTable get_metrics_table() const final {
        return sim::MetricsTable{};
    }

    // Put metrics of all inner objects to given directory
    virtual void write_inner_metrics(
        [[maybe_unused]] std::filesystem::path output_dir) const final {}

private:
    bool m_send_immediately;
    std::vector<sim::PacketInfo> m_sending_queue;
    sim::FlowContext m_context;
    Id m_id = "";
};

}  // namespace test
