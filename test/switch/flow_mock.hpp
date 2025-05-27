#pragma once
#include "flow.hpp"
#include "device/sender.hpp"

namespace test {

class FlowMock : public sim::IFlow {
public:
    ~FlowMock() = default;
    FlowMock(std::shared_ptr<sim::IReceiver> a_receiver);

    void start() final;

    Time create_new_data_packet() final;
    Time put_data_to_device() final;

    void update(sim::Packet packet, sim::DeviceType type) final;
    std::uint32_t get_updates_number() const;

    std::shared_ptr<sim::ISender> get_sender() const final;
    std::shared_ptr<sim::IReceiver> get_receiver() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IReceiver> m_receiver;
};

}  // namespace test
