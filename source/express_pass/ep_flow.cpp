#include "express_pass/ep_flow.hpp"

#include <memory>
#include <cstdlib>
#include <iostream>

#include "scheduler.hpp"
#include "logger/logger.hpp"
#include "express_pass/ep_event.hpp"

namespace sim {

EPFlow::EPFlow(std::shared_ptr<EPSender> a_src, std::shared_ptr<EPReceiver> a_dest,
           Size a_packet_size, Time a_delay_between_packets,
           std::uint32_t a_packets_to_send)
    : m_src(a_src),
      m_dest(a_dest),
      m_sender_status(SenderStatus::CREDIT_STOP_S),
      m_receiver_status(ReceiverStatus::CREDIT_STOP_R),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_packets_to_send(a_packets_to_send),
      m_id(IdentifierFactory::get_instance().generate_id()) {
        auto link = m_dest.lock()->get_link_to_destination(generate_packet(PacketType::CREDIT, true));
        m_max_rate = (double)(m_packet_size + m_system_packet_size) / 10.0; // TODO: / link.get_speed(); 
      }

void EPFlow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

Packet EPFlow::generate_packet(PacketType type, bool from_receiver, std::uint32_t packet_num, Time RTT) {
    sim::Packet packet;
    packet.type = type;
    if (type == PacketType::DATA) {
        packet.size_byte = m_packet_size;
    } else {
        packet.size_byte = m_system_packet_size;
    }
    packet.flow = this;
    packet.source_id = (from_receiver ? m_dest.lock()->get_id() : m_src.lock()->get_id());
    packet.dest_id = (from_receiver ? m_src.lock()->get_id() : m_dest.lock()->get_id());
    packet.packet_num = packet_num;
    packet.RTT = RTT;
    return packet;
}

std::uint32_t EPFlow::get_next_packet_num() {
    return m_current_packet_num++;
}

void EPFlow::start(Time time) { 
    schedule_packet_generation(time); 

    sim::Packet packet = generate_packet(PacketType::CREDIT_REQUEST, false);
    m_src.lock()->send_system_packet(time, packet);
    m_sender_status = SenderStatus::CREQ_SENT;
}

Time EPFlow::send_credit(Time time) {
    if (m_stopped) {
        return 0;
    }

    auto packet = generate_packet(PacketType::CREDIT, true, get_next_packet_num(), time);
    m_dest.lock()->send_system_packet(time, packet);
    int rnd_part = (std::rand() % 209) - 104;
    // std::cout << get_id() << ": " << packet.packet_num << " jitter: " << rnd_part << std::endl;

    return m_max_rate + rnd_part; //m_current_rate * m_max_rate;

}

// feedback control launch once per RTT (scheduled by event)

void EPFlow::update(Packet packet, DeviceType type) {
    if (packet.type == PacketType::CREDIT_REQUEST && type == DeviceType::RECEIVER && m_receiver_status == ReceiverStatus::CREDIT_STOP_R) {
        m_receiver_status = ReceiverStatus::CREDIT_SENDING;
        auto send_credit_event_ptr =
            std::make_unique<SendCredit>(time, shared_from_this(), m_system_packet_size);
        Scheduler::get_instance().add(std::move(send_credit_event_ptr));
    }
    else if (packet.type == PacketType::CREDIT_STOP_P && type == DeviceType::RECEIVER && m_receiver_status == ReceiverStatus::CREDIT_SENDING) {
        m_receiver_status = ReceiverStatus::CREDIT_STOP_R;
        m_stopped = true;
    }
    else if (packet.type == PacketType::DATA && type == DeviceType::RECEIVER) {
        // calculate data statistics
        // check corresponding credit id and update state
            LOG_INFO("Got DATA " + std::to_string(get_id()));
    } else if (packet.type == PacketType::CREDIT && type == DeviceType::SENDER) {
        if (m_sender_status != SenderStatus::CREQ_SENT && m_sender_status != SenderStatus::CREDIT_RECEIVING) {
            LOG_ERROR("Unexpected sender status when got credit");
            return;
        }
        if (m_sender_status == SenderStatus::CREQ_SENT) {
            m_sender_status = SenderStatus::CREDIT_RECEIVING;
        }

        if (m_sending_buffer.empty()) {
            m_sender_status = SenderStatus::CSTOP_SENT;

            auto packet = generate_packet(PacketType::CREDIT_STOP_P, false);
            m_src.lock()->send_system_packet(packet);
        } else {
            m_sending_buffer.front().RTT = packet.RTT;
            m_sending_buffer.front().packet_num = packet.packet_num;
            put_data_to_device();
        }
    }
    else {
        LOG_ERROR("Unexpected update state");
    }
}    


Time EPFlow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet data = generate_packet(PacketType::DATA);
    LOG_INFO("Created packet: " + data.to_string());
    m_sending_buffer.push(data);
    return m_delay_between_packets;
}

Time EPFlow::put_data_to_device() {
    m_src.lock()->enqueue_packet(m_sending_buffer.front());
    LOG_INFO("Put data to device: " + m_sending_buffer.front().to_string());
    m_sending_buffer.pop();
    return m_delay_between_packets;
}   

std::shared_ptr<ISender> EPFlow::get_sender() const { return m_src.lock(); }

std::shared_ptr<IReceiver> EPFlow::get_receiver() const { return m_dest.lock(); }

Id EPFlow::get_id() const { return m_id; }

}  // namespace sim