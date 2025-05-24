#pragma once
#include <memory>

#include "device/device.hpp"
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

    Time process() final;
    sim::DeviceType get_type() const final;

    Id get_id() const final;
};

}  // namespace test
