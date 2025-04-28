#pragma once
#include <cstdint>

#include "flow.hpp"
#include "express_pass/express_pass_receiver.hpp"
#include "express_pass/express_pass_sender.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class IReceiver;
class ISender;

enum SenderStatus {
    CREDIT_STOP,
    CREQ_SENT,
    CREDIT_RECEIVING,
    CSTOP_SENT
};

enum ReceiverStatus {
    CREDIT_STOP,
    CREDIT_SENDING
};

class ExpressPassFlow : public IFlow, public std::enable_shared_from_this<ExpressPassFlow> {
public:
    ExpressPassFlow(std::shared_ptr<ExpressPassSender> a_src, std::shared_ptr<ExpressPassReceiver> a_dest,
         Size a_packet_size, Time a_delay_between_packets,
         std::uint32_t a_packets_to_send);
    virtual ~ExpressPassFlow() = default;

    // Start at time
    void start(Time time) final;

    Time create_new_data_packet() final;
    Time put_data_to_device() final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update(Packet packet, DeviceType type) final;

    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;

    Time ExpressPassFlow::send_credit(Time time);

    Id get_id() const final;

private:
    void schedule_packet_generation(Time time);
    Packet generate_data_packet();
    void sender_send_stop();
    std::uint32_t get_next_packet_num();

    std::shared_ptr<ExpressPassSender> m_src;
    std::shared_ptr<IReceiver> m_dest;
    SenderStatus m_sender_status;
    ReceiverStatus m_receiver_status;

    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_packets_to_send;

    bool m_stopped = false;
    std::uint32_t m_current_packet_num = 0;
    bool m_system_packet_size = 64;
    double m_current_rate = 1.0 / 8.0;
    double w = 1.0 / 2.0;

    std::queue<Packet> m_sending_buffer;
    Id m_id;
};

}  // namespace sim
