#pragma once
#include <memory>

#include "device/receiver.hpp"
#include "link.hpp"

namespace test {

class ReceiverMock : public sim::IReceiver {
public:
    ReceiverMock() = default;
    ~ReceiverMock() = default;

    bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    bool add_outlink(std::shared_ptr<sim::ILink> link) final;
    bool update_routing_table(Id dest_id,
                              std::shared_ptr<sim::ILink> link, size_t paths_count) final;
    std::shared_ptr<sim::ILink> next_inlink() final;
    std::shared_ptr<sim::ILink> get_link_to_destination(sim::Packet packet) const final;
    std::set<std::shared_ptr<sim::ILink>> get_outlinks() final;
    bool notify_about_arrival(Time arrival_time) final;

    Time process() final;
    sim::DeviceType get_type() const final;
    Time send_system_packet(sim::Packet packet) final;

    Id get_id() const final;
};

}  // namespace test
