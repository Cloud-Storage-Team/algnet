#pragma once
#include <gtest/gtest.h>

#include "device/routing_module.hpp"
#include "link/interfaces/i_link.hpp"
#include "packet.hpp"

namespace test {

const unsigned RANDOM_SEED = 42;

std::vector<std::shared_ptr<sim::IRoutingDevice>> createRoutingModules(
    size_t count);

class TestLink : public sim::ILink {
public:
    TestLink(std::shared_ptr<sim::IRoutingDevice> a_src,
             std::shared_ptr<sim::IRoutingDevice> a_dest,
             sim::Packet packet_to_return = sim::Packet());
    ~TestLink() = default;

    void schedule_arrival(sim::Packet packet) final;
    void process_arrival(sim::Packet packet) final;
    std::optional<sim::Packet> get_packet() final;
    std::shared_ptr<sim::IRoutingDevice> get_from() const final;
    std::shared_ptr<sim::IRoutingDevice> get_to() const final;
    Size get_current_from_egress_buffer_size() const final;
    Size get_max_from_egress_buffer_size() const final;

    Id get_id() const final;

private:
    std::weak_ptr<sim::IRoutingDevice> src;
    std::weak_ptr<sim::IRoutingDevice> dst;
    sim::Packet packet;
};

}  // namespace test
