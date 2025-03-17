#pragma once
#include "flow.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IReceiver> a_receiver);

    void start(std::uint32_t time) final;
    bool try_to_generate(std::uint32_t packet_size) final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update() final;
    std::shared_ptr<sim::IReceiver> get_destination() const final;

private:
    std::shared_ptr<sim::IReceiver> m_receiver;
};

}  // namespace test
