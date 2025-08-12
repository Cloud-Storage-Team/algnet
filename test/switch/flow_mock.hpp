#pragma once
#include "flow/i_flow.hpp"
#include "packet.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IHost> a_receiver);

    void update(sim::Packet packet) final;
    std::uint32_t get_sending_quota() const final;
    void send_packet() final;
    std::shared_ptr<sim::IConnection> get_conn() const final;
    virtual SizeByte get_delivered_data_size() const final;

    std::shared_ptr<sim::IHost> get_sender() const final;
    std::shared_ptr<sim::IHost> get_receiver() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IHost> m_receiver;
};

}  // namespace test
