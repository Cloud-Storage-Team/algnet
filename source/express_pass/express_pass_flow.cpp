#include "express_pass/express_pass_flow.hpp"

#include <memory>

#include "scheduler.hpp"

namespace sim {

ExpressPassFlow::ExpressPassFlow(std::shared_ptr<ExpressPassSender> a_src, std::shared_ptr<ExpressPassReceiver> a_dest,
           Size a_packet_size, Time a_delay_between_packets,
           std::uint32_t a_packets_to_send)
    : m_src(a_src),
      m_dest(a_dest),
      m_sender_status(SenderStatus::CREDIT_STOP),
      m_receiver_status(ReceiverStatus::CREDIT_STOP),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_packets_to_send(a_packets_to_send),
      m_id(IdentifierFactory::get_instance().generate_id()) {
        
      }

void ExpressPassFlow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

Packet ExpressPassFlow::generate_data_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size = m_packet_size;
    packet.flow = this;
    packet.source_id = m_src->get_id();
    packet.dest_id = m_dest->get_id();
    packet.RTT = 0;
    return packet;
}

std::uint32_t ExpressPassFlow::get_next_packet_num() {
    return m_current_packet_num++;
}

// TODO: fix, need to send CREDIT_REQUEST from sender to receiver
void ExpressPassFlow::start(Time time) { 
    schedule_packet_generation(time); 
    // send CREDIT_REQUEST
    m_sender_status = SenderStatus::CREQ_SENT;
}

// TODO: fix comments
Time ExpressPassFlow::send_credit(Time time) {
    sim::Packet packet;
    packet.type = sim::PacketType::CREDIT;
    packet.size = m_system_packet_size;
    packet.flow = this;
    packet.source_id = m_dest->get_id();
    packet.dest_id = m_src->get_id();
    packet.RTT = time;
    packet.packet_num = get_next_packet_num();

    // m_dest->send_packet(packet);

    return time + 5; // replace with current sending rate

}

// start sending credits simple:
// 1) Scheduled by event (event calls flow's method send_credit)
// 2) Creates credit and se

// stop sending credits:
// set some flag which is checked in send_credit

// create buffer for storing credits

// create method for generating system packets

// feedback control launch once per RTT (scheduled by event)

// on sender and receiver create methods for sending system packets

// TODO: fix
void ExpressPassFlow::update(Packet packet, DeviceType type) {
    if (packet.type == PacketType::CREDIT_REQUEST && type == DeviceType::RECEIVER && m_receiver_status == ReceiverStatus::CREDIT_STOP) {
        m_receiver_status = ReceiverStatus::CREDIT_SENDING;
        // start sending credits
    }
    else if (packet.type == PacketType::CREDIT_STOP && type == DeviceType::RECEIVER && m_receiver_status == ReceiverStatus::CREDIT_SENDING) {
        m_receiver_status = ReceiverStatus::CREDIT_STOP;
        // stop sending credits
    }
    else if (packet.type == PacketType::DATA && type == DeviceType::RECEIVER) {
        // calculate data statistics
        // check corresponding credit id and update state
    } else if (packet.type == PacketType::CREDIT && type == DeviceType::SENDER) {
        if (m_sender_status != SenderStatus::CREQ_SENT || m_sender_status != SenderStatus::CREDIT_RECEIVING) {
            // Log error state
            return;
        }
        if (m_sender_status == SenderStatus::CREQ_SENT) {
            m_sender_status = SenderStatus::CREDIT_RECEIVING;
        }
        // send data
    }
    else {
        // Log error state
    }
}    


Time ExpressPassFlow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet data = generate_data_packet();
    m_sending_buffer.push(data);
    return m_delay_between_packets;
}

// TODO: fix
Time ExpressPassFlow::put_data_to_device() {
    m_src->enqueue_packet(m_sending_buffer.front());
    m_sending_buffer.pop();
    return m_delay_between_packets;
}   

std::shared_ptr<ISender> ExpressPassFlow::get_sender() const { return m_src; }

std::shared_ptr<IReceiver> ExpressPassFlow::get_receiver() const { return m_dest; }

Id ExpressPassFlow::get_id() const { return m_id; }

}  // namespace sim
