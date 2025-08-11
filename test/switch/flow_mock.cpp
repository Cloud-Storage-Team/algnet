#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IHost> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::update([[maybe_unused]] sim::Packet packet) {};

bool FlowMock::can_send() const { return true; }
void FlowMock::send_packet() {}
void FlowMock::set_conn([[maybe_unused]] std::shared_ptr<sim::IConnection> connection) {}
std::shared_ptr<sim::IConnection> FlowMock::get_conn() const {
    return nullptr;
}
SizeByte FlowMock::get_delivered_data_size() const { return SizeByte(0); }

std::shared_ptr<sim::IHost> FlowMock::get_sender() const { return nullptr; }
std::shared_ptr<sim::IHost> FlowMock::get_receiver() const {
    return m_receiver.lock();
}

Id FlowMock::get_id() const { return ""; }

}  // namespace test
