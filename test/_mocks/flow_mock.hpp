#pragma once
#include "connection/flow/i_flow.hpp"
#include "packet.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IHost> a_receiver,
             SizeByte a_packet_size = SizeByte(64),
             SizeByte a_sending_quota = SizeByte(0),
             TimeNs a_last_rtt = TimeNs(0));

    void update(sim::Packet packet) final;
    SizeByte get_packet_size() const final;
    SizeByte get_sending_quota() const final;
    void send_data(SizeByte data) final;

    virtual SizeByte get_total_data_size_added_from_conn() const final;
    virtual SizeByte get_delivered_data_size() const final;
    virtual SizeByte get_sent_data_size() const final;
    virtual uint32_t retransmit_count() const final;
    virtual TimeNs get_fct() const final;
    virtual std::optional<TimeNs> get_last_rtt() const;
    const sim::BaseFlagManager& get_flag_manager() const final;

    std::shared_ptr<sim::IHost> get_sender() const final;
    std::shared_ptr<sim::IHost> get_receiver() const final;

    Id get_id() const final;
    void set_sending_quota(SizeByte quota);
    void set_last_rtt(std::optional<TimeNs> rtt);

private:
    std::weak_ptr<sim::IHost> m_receiver;
    sim::BaseFlagManager m_flag_manager;
    SizeByte m_packet_size;
    SizeByte m_sending_quota;
    std::optional<TimeNs> m_last_rtt;
};

}  // namespace test
