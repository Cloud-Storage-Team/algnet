#include "tcp_flow.hpp"

#include "event.hpp"
#include "scheduler.hpp"

namespace sim {

TcpFlow::TcpFlow(std::shared_ptr<ISender> a_src,
                 std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
                 Time a_delay_between_packets, std::uint32_t a_packets_to_send,
                 Time a_delay_threshold)
    : m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_packets_to_send(a_packets_to_send),
      m_delay_threshold(a_delay_threshold),
      m_last_send_time(0),
      m_cwnd(1),
      m_ssthresh(128),
      m_linear_coeficient(2),
      m_packets_in_flight(0),
      m_packets_acked(0) {}

void TcpFlow::start() {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

Time TcpFlow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet data = generate_packet();
    m_sending_buffer.push(data);
    return put_data_to_device();
}

Time TcpFlow::put_data_to_device() {
    if (m_src.expired()) {
        LOG_ERROR("Flow source was deleted; can not put data to it");
        return 0;
    }

    if (m_packets_in_flight < m_cwnd) {
        m_packets_in_flight++;
        m_src.lock()->enqueue_packet(m_sending_buffer.front());
        m_sending_buffer.pop();
        return 1;  // TODO: fix it
    }

    LOG_ERROR(
        "Triggered TcpFlow::put_data_to_device when congestion window is full");

    return 0;
}

void TcpFlow::update(Packet packet, DeviceType type) {
    if (type != DeviceType::SENDER || packet.type != PacketType::ACK) {
        return;
    }

    Time current_time = Scheduler::get_instance().get_current_time();
    if (current_time < packet.)

        if (packet.< m_delay_threshold) {  // ack
            m_packets_in_flight--;
            if (m_cwnd < m_ssthresh) {
                m_cwnd++;
            } else {
                m_packets_acked++;
                if (m_packets_acked >= m_cwnd / m_linear_coeficient) {
                    m_cwnd++;
                    m_packets_acked = 0;
                }
            }
        } else {  // trigger_congestion
            m_ssthresh = m_cwnd / 2;
            m_cwnd = 1;
            m_packets_in_flight = 0;
        }
}

Packet TcpFlow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size_byte = m_packet_size;
    packet.flow = this;
    return packet;
}

bool TcpFlow::try_to_put_data_to_device() {
    if (m_packets_in_flight < m_cwnd) {
        return put_data_to_device() != 0;
    }
    return false;
}
}  // namespace sim
