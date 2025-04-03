#pragma once

#include "flow.hpp"

namespace test_receiver {

class FlowMock : public sim::IFlow {
public:
    FlowMock(std::shared_ptr<sim::ISender> a_sender,
             std::shared_ptr<sim::IReceiver> a_receiver)
        : m_sender(a_sender), m_receiver(a_receiver) {}

    ~FlowMock() = default;

    void start(std::uint32_t time) {}
    bool try_to_generate(std::uint32_t packet_size) { return false; }

    void update() {}
    std::shared_ptr<sim::ISender> get_source() const { return m_sender; }
    std::shared_ptr<sim::IReceiver> get_destination() const {
        return m_receiver;
    }

private:
    std::shared_ptr<sim::ISender> m_sender;
    std::shared_ptr<sim::IReceiver> m_receiver;
};

}  // namespace test_receiver
