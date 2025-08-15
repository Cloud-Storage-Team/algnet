#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IHost> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::update([[maybe_unused]] sim::Packet packet) {};

std::uint32_t FlowMock::get_sending_quota() const { return 1; }
void FlowMock::send_packet() {}
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
