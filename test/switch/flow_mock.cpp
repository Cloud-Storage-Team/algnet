#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IHost> m_receiver)
    : m_receiver(m_receiver), m_packet_size(SizeByte(64)) {}

void FlowMock::update([[maybe_unused]] sim::Packet packet) {};

SizeByte FlowMock::get_sending_quota() const { return m_packet_size; }
SizeByte FlowMock::get_packet_size() const { return m_packet_size; }
void FlowMock::send_data([[maybe_unused]] SizeByte data) {}
std::shared_ptr<sim::IConnection> FlowMock::get_conn() const { return nullptr; }
SizeByte FlowMock::get_delivered_data_size() const { return SizeByte(0); }
TimeNs FlowMock::get_fct() const { return TimeNs(0); }

std::shared_ptr<sim::IHost> FlowMock::get_sender() const { return nullptr; }
std::shared_ptr<sim::IHost> FlowMock::get_receiver() const {
    return m_receiver.lock();
}

Id FlowMock::get_id() const { return ""; }

}  // namespace test
