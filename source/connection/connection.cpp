#include "connection/connection.hpp"
#include "scheduler.hpp"
#include "event/connection_send.hpp"

namespace sim {

Connection::Connection(Id a_id, Id a_sender_id, Id a_receiver_id, SizeByte a_packet_size ,std::shared_ptr<MPLBPolicy> a_policy, std::uint64_t a_packet_count)
    : m_id(a_id)
    , m_sender_id(a_sender_id)
    , m_receiver_id(a_receiver_id)
    , m_packet_size(a_packet_size)
    , m_policy(std::move(a_policy))
    , m_packets_left(a_packet_count)
{}


void Connection::configure(std::uint64_t packet_count)
{
    m_packets_left = packet_count;
    // m_packet_size = packet_size;
    // for (const auto& flow : m_flows) {
    //     flow->set_packet_size(packet_size);
    // }
}
void Connection::add_flow(std::shared_ptr<IFlow> flow)
{
    m_flows.push_back(flow);
    flow->set_packet_size(m_packet_size);
    //flow->set_owner(shared_from_this());
}

void Connection::kick()
{
    Scheduler::get_instance().add<ConnectionSend>(
        Scheduler::get_instance().get_current_time(), shared_from_this());
}

bool Connection::send_via_flow(const std::shared_ptr<IFlow>& flow)
{
    if (m_packets_left == 0) return false;
    if (!flow->try_send_one()) return false;
    --m_packets_left;
    return true;
}

void Connection::try_send_once()
{
    if (m_packets_left == 0 || m_flows.empty()) return;
    auto flow = m_policy->select_flow(m_flows);
    if (!flow) return;
    if (!send_via_flow(flow)) return;
    if (m_packets_left > 0) {
        TimeNs next = Scheduler::get_instance().get_current_time()
                    + m_policy->next_pacing_delay(flow);
        Scheduler::get_instance().add<ConnectionSend>(next, shared_from_this());
    }
}

void Connection::on_ack(const Packet& ack)
{
    for (auto& f : m_flows) {
        if (f.get() == ack.flow) {
            m_policy->on_ack(f, ack);
            break;
        }
    }
    Scheduler::get_instance().add<ConnectionSend>(
        Scheduler::get_instance().get_current_time(), shared_from_this());
}

Id Connection::get_id() const { return m_id; }

} // namespace sim