#pragma once
#include "flow/i_flow.hpp"
#include "packet.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IHost> a_receiver);

    void update(sim::Packet packet) final;
    SizeByte get_sending_quota() const final;
    SizeByte get_packet_size() const final; 
    void send_data(SizeByte data) final;
    std::shared_ptr<sim::IConnection> get_conn() const final;
    virtual SizeByte get_delivered_data_size() const final;
    virtual TimeNs get_fct() const final;

    std::shared_ptr<sim::IHost> get_sender() const final;
    std::shared_ptr<sim::IHost> get_receiver() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IHost> m_receiver;
    SizeByte m_packet_size;
};

}  // namespace test
