#pragma once
#include <gmock/gmock.h>

#include "network/connection/flow/packet.hpp"
#include "topology/device/interfaces/i_host.hpp"
#include "topology/link/i_link.hpp"

namespace test {
class HostGMock : public sim::IHost {
public:
    MOCK_METHOD(void, enqueue_packet, (sim::Packet packet), (final));
    MOCK_METHOD(TimeNs, send_packet, (), (final));
    MOCK_METHOD(bool, notify_about_arrival, (TimeNs arrival_time), (final));
    MOCK_METHOD(bool, add_inlink, (std::shared_ptr<sim::ILink> link), (final));
    MOCK_METHOD(bool, add_outlink, (std::shared_ptr<sim::ILink> link), (final));
    MOCK_METHOD(bool, update_routing_table,
                (Id dest_id, std::shared_ptr<sim::ILink> link,
                 size_t paths_count),
                (final));
    MOCK_METHOD(std::shared_ptr<sim::ILink>, get_link_to_destination,
                (sim::Packet packet), (final, const));
    MOCK_METHOD(std::shared_ptr<sim::ILink>, next_inlink, (), (final));
    MOCK_METHOD(std::set<std::shared_ptr<sim::ILink>>, get_outlinks, (),
                (final));
    MOCK_METHOD(Id, get_id, (), (final, const));
    MOCK_METHOD(TimeNs, process, (), (final));
};
}  // namespace test
