#pragma once
#include "flow/flow.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IHost> a_receiver);

    void start() final;

    Time create_new_data_packet() final;

    void update(sim::Packet packet, sim::DeviceType type) final;
    std::uint32_t get_updates_number() const;

    std::shared_ptr<sim::IHost> get_sender() const final;
    std::shared_ptr<sim::IHost> get_receiver() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IHost> m_receiver;
};

}  // namespace test
