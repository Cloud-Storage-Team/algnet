#pragma once
#include "flow/i_flow.hpp"
#include "packet.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IHost> a_receiver);

    void start() final;

    void update(sim::Packet packet, sim::DeviceType type) final;

    std::shared_ptr<sim::IHost> get_sender() const final;
    std::shared_ptr<sim::IHost> get_receiver() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IHost> m_receiver;
};

}  // namespace test
