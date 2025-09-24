#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IHost> a_receiver, SizeByte packet_size,
                   SizeByte a_sending_quota, TimeNs a_last_rtt)
    : m_receiver(std::move(a_receiver)),
      m_packet_size(packet_size),
      m_sending_quota(a_sending_quota),
      m_last_rtt(a_last_rtt) {}

void FlowMock::update([[maybe_unused]] sim::Packet packet) {};

SizeByte FlowMock::get_sending_quota() const { return m_sending_quota; }
TimeNs FlowMock::get_last_rtt() const { return m_last_rtt; }
void FlowMock::send_data(SizeByte data) {
    if (data > m_sending_quota) {
        throw std::runtime_error("FlowMock::send_data: data > sending_quota");
    }
    m_sending_quota -= data;
}
std::shared_ptr<sim::IConnection> FlowMock::get_conn() const { return nullptr; }
SizeByte FlowMock::get_delivered_data_size() const { return SizeByte(0); }
TimeNs FlowMock::get_fct() const { return TimeNs(0); }

std::shared_ptr<sim::IHost> FlowMock::get_sender() const { return nullptr; }
std::shared_ptr<sim::IHost> FlowMock::get_receiver() const {
    return m_receiver.lock();
}

Id FlowMock::get_id() const { return ""; }

void FlowMock::set_sending_quota(SizeByte quota) { m_sending_quota = quota; }
void FlowMock::set_last_rtt(TimeNs rtt) { m_last_rtt = rtt; }

SizeByte FlowMock::get_packet_size() const { return m_packet_size; }

}  // namespace test
