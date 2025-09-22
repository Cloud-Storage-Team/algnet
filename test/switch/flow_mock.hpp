#pragma once
#include "flow/i_flow.hpp"
#include "packet.hpp"

namespace test {

// It is used for testing mplb algorithms
struct FlowStat {
    SizeByte sending_quota = SizeByte(0);
    TimeNs last_rtt = TimeNs(0);
};

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IHost> a_receiver,
             SizeByte a_packet_size = SizeByte(64),
             FlowStat a_stat = {SizeByte(0), TimeNs(0)});

    void update(sim::Packet packet) final;
    SizeByte get_sending_quota() const final;
    TimeNs get_last_rtt() const final;
    void send_data(SizeByte data) final;
    std::shared_ptr<sim::IConnection> get_conn() const final;
    virtual SizeByte get_delivered_data_size() const final;
    virtual TimeNs get_fct() const final;

    std::shared_ptr<sim::IHost> get_sender() const final;
    std::shared_ptr<sim::IHost> get_receiver() const final;

    Id get_id() const final;

    void set_flow_stat(FlowStat stat);
    SizeByte get_packet_size() const;

private:
    std::weak_ptr<sim::IHost> m_receiver;
    SizeByte m_packet_size;
    FlowStat m_stat;
};

}  // namespace test
