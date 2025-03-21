#include "../../source/scheduler.hpp"
#include "../../source/tcp_flow.hpp"

namespace test {
// Mock structures for ISender and IReceiver for testing purposes
class TestSender : public sim::ISender {
public:
    void enqueue_packet(sim::Packet packet) override { m_packets.push(packet); }

    void add_inlink(std::shared_ptr<sim::Link> link) override {}
    void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<sim::Link> link) override {}
    std::shared_ptr<sim::Link> next_inlink() override { return nullptr; }
    sim::DeviceType get_type() const override { return sim::SENDER; }
    void process() override {}

    bool has_packets() const { return !m_packets.empty(); }

    sim::Packet dequeue_packet() {
        sim::Packet packet = m_packets.front();
        m_packets.pop();
        return packet;
    }

private:
    std::queue<sim::Packet> m_packets;
};

class TestReceiver : public sim::IReceiver {
public:
    void add_inlink(std::shared_ptr<sim::Link> link) override {}
    void update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                              std::shared_ptr<sim::Link> link) override {}
    std::shared_ptr<sim::Link> next_inlink() override { return nullptr; }
    sim::DeviceType get_type() const override { return sim::RECEIVER; }
    void process() override {}
};
}  // namespace test
