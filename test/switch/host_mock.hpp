#pragma once
#include <memory>

#include "network/connection/flow/packet.hpp"
#include "topology/device/interfaces/i_host.hpp"
#include "topology/link/i_link.hpp"

namespace test {

class HostMock : public sim::IHost {
public:
    HostMock() = default;
    ~HostMock() = default;

    bool add_inlink(std::shared_ptr<sim::ILink> link) final;
    bool add_outlink(std::shared_ptr<sim::ILink> link) final;
    bool update_routing_table(Id dest_id, std::shared_ptr<sim::ILink> link,
                              size_t paths_count) final;
    std::shared_ptr<sim::ILink> next_inlink() final;
    std::shared_ptr<sim::ILink> get_link_to_destination(
        const sim::Packet& packet) const final;
    std::set<std::shared_ptr<sim::ILink>> get_outlinks() final;
    bool notify_about_arrival() final;

    TimeNs process() final;

    const Id& get_id() const final;

    void enqueue_packet(const sim::Packet& packet) final;

private:
    Id id = "";
};

}  // namespace test
