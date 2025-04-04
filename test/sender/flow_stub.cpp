#include "flow_stub.hpp"

namespace test {
FlowStub::FlowStub(std::shared_ptr<sim::ISender> a_sender, std::shared_ptr<sim::IReceiver> a_receiver)
    : m_sender(a_sender), m_receiver(a_receiver), cnt(0) {}

void FlowStub::start(std::uint32_t time) {}
bool FlowStub::try_to_generate(std::uint32_t packet_size) { return false; }

void FlowStub::update() {
    cnt++;
}
int FlowStub::get_cnt() {
    return cnt;
}
std::shared_ptr<sim::ISender> FlowStub::get_source() const {
    return m_sender;
}
std::shared_ptr<sim::IReceiver> FlowStub::get_destination() const {
    return m_receiver;
}

}  // namespace test
