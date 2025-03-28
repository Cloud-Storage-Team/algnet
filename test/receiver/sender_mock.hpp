#pragma once

#include "packet.hpp"
#include "sender.hpp"

namespace test_receiver {

class MockSender : public sim::ISender {
public:
    MockSender() = default;
    ~MockSender() = default;

    void add_inlink(std::shared_ptr<sim::ILink> link) final {
        m_inlinks.push_back(link);
    }

    void update_routing_table(std::shared_ptr<sim::IRoutingDevice> dest,
                              std::shared_ptr<sim::ILink> link) final {
        m_routing_table[dest] = link;
    }

    std::vector<std::shared_ptr<IRoutingDevice>> get_neighbours() const final {
        return std::vector<std::shared_ptr<IRoutingDevice>>();
    };

    std::shared_ptr<sim::ILink> next_inlink() final { return nullptr; };

    std::shared_ptr<sim::ILink> get_link_to_destination(
        std::shared_ptr<IRoutingDevice> dest) const final {
        return nullptr;
    };

    sim::DeviceType get_type() const final { return sim::DeviceType::SENDER; }

    void process() final {}

    void enqueue_packet(sim::Packet packet) final {
        m_flow_buffer.push(packet);
    }

private:
    std::vector<std::shared_ptr<sim::ILink>> m_inlinks;
    std::unordered_map<std::shared_ptr<IRoutingDevice>,
                       std::shared_ptr<sim::ILink>>
        m_routing_table;
    std::queue<sim::Packet> m_flow_buffer;
};

}  // namespace test_receiver