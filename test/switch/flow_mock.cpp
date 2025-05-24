#include "flow_mock.hpp"

namespace test {

FlowMock::FlowMock(std::shared_ptr<sim::IReceiver> m_receiver)
    : m_receiver(m_receiver) {}

void FlowMock::start() {}
Time FlowMock::create_new_data_packet() {
    return 1;
};
Time FlowMock::put_data_to_device() {
    return 1;
};

void FlowMock::update(sim::Packet packet, sim::DeviceType type) {
    
};
std::shared_ptr<sim::ISender> FlowMock::get_sender() const {
    return nullptr;
}
std::shared_ptr<sim::IReceiver> FlowMock::get_receiver() const {
    return m_receiver.lock();
}

Id FlowMock::get_id() const { return 42; }

}  // namespace test
