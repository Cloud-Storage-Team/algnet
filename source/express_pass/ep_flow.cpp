#include "express_pass/ep_flow.hpp"

#include <memory>
#include <cstdlib>
#include <iostream>

#include "scheduler.hpp"
#include "logger/logger.hpp"
#include "express_pass/ep_event.hpp"

namespace sim {

EPFlow::EPFlow(Id a_id, std::shared_ptr<ISender> a_src, std::shared_ptr<IReceiver> a_dest,
           Size a_packet_size, Time a_delay_between_packets,
           std::uint32_t a_packets_to_send)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_sender_status(SenderStatus::CREDIT_STOP_S),
      m_receiver_status(ReceiverStatus::CREDIT_STOP_R),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_packets_to_send(a_packets_to_send) {

        auto link = m_dest.lock()->get_link_to_destination(generate_packet(PacketType::CREDIT, true));
        m_max_rate = (double)(m_packet_size * 8) / 10.0; // TODO: / link.get_speed(); 
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
        packet.size_byte = m_system_packet_size + (rand() % 20);
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

void EPFlow::start() { 
    schedule_packet_generation(Scheduler::get_instance().get_current_time()); 

    sim::Packet packet = generate_packet(PacketType::CREDIT_REQUEST, false);
    m_src.lock()->send_system_packet(packet);
    m_sender_status = SenderStatus::CREQ_SENT;
}

Time EPFlow::send_credit() {
    if (m_stopped) {
        return 0;
    }

    feedback_control_loop();

    auto packet = generate_packet(PacketType::CREDIT, true, get_next_packet_num(), Scheduler::get_instance().get_current_time());
    m_dest.lock()->send_system_packet(packet);
    m_sent_credits++;

    std::uint32_t rate = m_max_rate / m_current_rate_coeff;
    int rnd_part = (std::rand() % (int)(rate * 0.08)) - (int)(rate * 0.04);
    m_accum += rate + rnd_part;
    return rate + rnd_part;

}


void EPFlow::process_getting_data_packet(Packet data_packet) {
    double alpha = 0.2;
    m_RTT = (1 - alpha) * m_RTT + alpha * (Scheduler::get_instance().get_current_time() - data_packet.RTT);

    int distance = data_packet.packet_num - m_last_obtained_packet_num;

    if (distance < 0) {
      // TODO: log error credit packet reordering or credit sequence number overflow happend.
      throw "ERR";
    }
    m_sent_credits += (distance + 1);
    m_lost_credits += distance;

    m_last_obtained_packet_num = data_packet.packet_num + 1;

    // LOG_ERROR("Got DATA " + std::to_string(get_id()) + " Packet: " + data_packet.to_string() + " RTT: " + std::to_string(m_RTT) + " REAL RTT: " + std::to_string(Scheduler::get_instance().get_current_time() - data_packet.RTT));
}

// feedback control launch once per RTT (scheduled by event)
Time EPFlow::feedback_control_loop() {
    if (m_sent_credits == 0) {
        return m_RTT;
    }

    if (Scheduler::get_instance().get_current_time() - m_last_credit_rate_update < m_RTT) {
        return m_RTT;
    }

    // std::cout << "Lost: " << m_lost_credits << std::endl;
    // std::cout << "Sent: " << m_sent_credits << std::endl;

    auto old_coeff = m_current_rate_coeff;
    double credit_loss = (double)m_lost_credits / (double)m_sent_credits;
    
    if (credit_loss <= m_target_loss && m_incr_cnt <= 10) {
        if (m_was_increasing) {
            m_w = (m_w + m_w_max) / 2;
            m_incr_cnt++;
        }
        m_was_increasing = true;
        m_current_rate_coeff = (1 - m_w) * m_current_rate_coeff + m_w * m_max_rate_coeff * (1 + m_target_loss);
    } else {
        m_max_meg_incr_in_a_row = std::max(m_max_meg_incr_in_a_row, m_incr_cnt);
        m_was_increasing = false;
        if (credit_loss >= 1.0) {
            m_current_rate_coeff = m_max_rate / (double)m_RTT;
        } else {
            if (m_incr_cnt > 10) {
                m_current_rate_coeff /= (8.0 * (1 - credit_loss));
            } else {
                m_current_rate_coeff = m_current_rate_coeff * (1 - credit_loss) * (1 + m_target_loss);
            }


            // TEST PART
            // m_current_rate_coeff = m_max_rate*(m_sent_credits - m_lost_credits)
            //              / (double)(Scheduler::get_instance().get_current_time() - m_last_credit_rate_update)
            //              * (1.0 + m_target_loss);
            // m_current_rate_coeff = m_current_rate_coeff * (1 - credit_loss) * (1 + m_target_loss);
        }
        if (m_current_rate_coeff > old_coeff) {
            m_current_rate_coeff = old_coeff;
        }
        
        m_incr_cnt = 0;
        m_w = std::max(m_w / 2, m_w_min);
    }

    // std::cout << "Rate: " << m_current_rate_coeff << std::endl;

    m_lost_credits = 0;
    m_sent_credits = 0;
    m_last_credit_rate_update = Scheduler::get_instance().get_current_time();
    
    return m_RTT;
}

void EPFlow::update(Packet packet, DeviceType type) {
    if (packet.type == PacketType::CREDIT_REQUEST && type == DeviceType::RECEIVER && m_receiver_status == ReceiverStatus::CREDIT_STOP_R) {
        m_receiver_status = ReceiverStatus::CREDIT_SENDING;
        
        auto send_credit_event_ptr =
            std::make_unique<SendCredit>(Scheduler::get_instance().get_current_time(), shared_from_this(), m_system_packet_size);
        Scheduler::get_instance().add(std::move(send_credit_event_ptr));

        // auto feedback_control_event_ptr =
        //     std::make_unique<RunFeedbackControlLoop>(Scheduler::get_instance().get_current_time(), shared_from_this());
        // Scheduler::get_instance().add(std::move(feedback_control_event_ptr));
    }
    else if (packet.type == PacketType::CREDIT_STOP_P && type == DeviceType::RECEIVER && m_receiver_status == ReceiverStatus::CREDIT_SENDING) {
        m_receiver_status = ReceiverStatus::CREDIT_STOP_R;
        m_stopped = true;
    }
    else if (packet.type == PacketType::DATA && type == DeviceType::RECEIVER) {
        // calculate data statistics
        // check corresponding credit id and update state
        process_getting_data_packet(packet);
    } else if (packet.type == PacketType::CREDIT && type == DeviceType::SENDER) {
            // LOG_ERROR("Got CREDIT " + std::to_string(get_id()) + " TIME: " + std::to_string(Scheduler::get_instance().get_current_time()));
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