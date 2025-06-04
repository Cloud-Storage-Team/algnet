#pragma once
#include <cstdint>
#include <iostream>

#include "flow/flow.hpp"
#include "packet.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class IReceiver;
class ISender;

enum SenderStatus {
    CREDIT_STOP_S,
    CREQ_SENT,
    CREDIT_RECEIVING,
    CSTOP_SENT
};

enum ReceiverStatus {
    CREDIT_STOP_R,
    CREDIT_SENDING
};

class EPFlow : public IFlow, public std::enable_shared_from_this<EPFlow> {
public:
    EPFlow(Id a_id, std::shared_ptr<ISender> a_src, std::shared_ptr<IReceiver> a_dest,
         Size a_packet_size, Time a_delay_between_packets,
         std::uint32_t a_packets_to_send);
    virtual ~EPFlow() {
        std::cout << "Max incr: " << std::max(m_max_meg_incr_in_a_row, m_incr_cnt) << std::endl;
    };

    // Start at time
    void start() final;

    Time create_new_data_packet() final;
    Time put_data_to_device();

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update(Packet packet, DeviceType type) final;

    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;

    Time send_credit();
    Time feedback_control_loop();

    Id get_id() const final;

private:
    void schedule_packet_generation(Time time);
    Packet generate_packet(PacketType type, bool from_receiver = false, std::uint32_t packet_num = 0, Time RTT = 0);
    void sender_send_stop();
    std::uint32_t get_next_packet_num();
    void process_getting_data_packet(Packet data_packet);

    Id m_id;
    std::weak_ptr<ISender> m_src;
    std::weak_ptr<IReceiver> m_dest;
    SenderStatus m_sender_status;
    ReceiverStatus m_receiver_status;

    bool m_stopped = false;
    std::uint32_t m_current_packet_num = 0;

    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_packets_to_send;
    Size m_system_packet_size = 84;


    double m_max_rate_coeff = 1.0;
    double m_current_rate_coeff = 1.0 / 4.0;
    double m_max_rate;
    double m_w = 1.0 / 8.0;
    double m_w_max = 0.5;
    double m_w_min = 0.01;
    double m_target_loss = 0.05;
    bool m_was_increasing = false;
    int m_incr_cnt = 0;
    int m_max_meg_incr_in_a_row = 0;
    Time m_last_credit_rate_update = 0;

    Time m_RTT = 9000;
    std::uint32_t m_last_obtained_packet_num = 0;
    std::uint32_t m_last_received_credit = 0;
    std::uint32_t m_sent_credits = 0;
    std::uint32_t m_lost_credits = 0;
    std::uint32_t m_last_feedback_run = 0;

    std::queue<Packet> m_sending_buffer;
    Time m_accum;
};

}  // namespace sim