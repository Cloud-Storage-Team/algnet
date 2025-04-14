#pragma once
#include "flow.hpp"
#include "device/sender.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IReceiver> a_receiver);

    void start(std::uint32_t time) final;
    Time try_to_generate() final;

    void update() final;
    std::shared_ptr<sim::ISender> get_sender() const final;
    std::shared_ptr<sim::IReceiver> get_receiver() const final;

private:
    std::shared_ptr<sim::IReceiver> m_receiver;
};

}  // namespace test
