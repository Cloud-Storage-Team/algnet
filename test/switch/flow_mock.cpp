#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IHost> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::update([[maybe_unused]] sim::Packet packet) {};

std::uint32_t FlowMock::get_sending_quota() const { return 1; }
void FlowMock::send_packet() {}
SizeByte FlowMock::get_delivered_data_size() const { return SizeByte(0); }
TimeNs FlowMock::get_fct() const { return TimeNs(0); }

const sim::FlowFlagsManager& FlowMock::get_flag_mamager() const {
    return m_flag_manager;
}

std::shared_ptr<sim::IHost> FlowMock::get_sender() const { return nullptr; }
std::shared_ptr<sim::IHost> FlowMock::get_receiver() const {
    return m_receiver.lock();
}

Id FlowMock::get_id() const { return ""; }

}  // namespace test
