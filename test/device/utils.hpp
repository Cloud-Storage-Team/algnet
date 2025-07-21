#pragma once
#include <gtest/gtest.h>

#include "device/routing_module.hpp"
#include "link/i_link.hpp"
#include "packet.hpp"

namespace test {

const unsigned RANDOM_SEED = 42;

std::vector<std::shared_ptr<sim::IDevice>> createTestDevices(size_t count);

class TestDevice : public virtual sim::IDevice, public sim::RoutingModule {
public:
    TestDevice(Id a_id = "") : sim::RoutingModule(a_id) {};
    ~TestDevice() = default;

    bool notify_about_arrival(TimeNs arrival_time) final;

    sim::DeviceType get_type() const final;
    // Process a packet by moving it from ingress to egress
    // and schedule next process event after a delay.
    // Packets are taken from ingress buffers on a round-robin basis.
    // The iterator over ingress buffers is stored in m_next_link.
    TimeNs process() final;
};

class TestLink : public sim::ILink {
public:
    TestLink(std::shared_ptr<sim::IDevice> a_src,
             std::shared_ptr<sim::IDevice> a_dest,
             sim::Packet packet_to_return = sim::Packet());
    ~TestLink() = default;

    void schedule_arrival(sim::Packet packet) final;
    std::optional<sim::Packet> get_packet() final;
    std::shared_ptr<sim::IDevice> get_from() const final;
    std::shared_ptr<sim::IDevice> get_to() const final;

    SizeByte get_from_egress_queue_size() const final;
    SizeByte get_max_from_egress_buffer_size() const final;

    SizeByte get_to_ingress_queue_size() const final;
    SizeByte get_max_to_ingress_queue_size() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IDevice> src;
    std::weak_ptr<sim::IDevice> dst;
    sim::Packet packet;
};

}  // namespace test
