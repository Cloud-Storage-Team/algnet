#pragma once
#include <cstdint>

#include "device/receiver.hpp"
#include "device/sender.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class IReceiver;
class ISender;

class IFlow : public Identifiable {
public:
    virtual void start(Time time) = 0;
    // Adds new packet to sending queue
    // Used in event Generate
    virtual Time create_new_data_packet() = 0;
    // Puts some pakets from sending buffer to sender's buffer according to the internal state
    virtual Time put_data_to_device() = 0;

    // Update the internal state according to some congestion control algorithm
    // Calls when data available for sending on corresponding device
    virtual void update(Packet packet, DeviceType type) = 0;
    virtual std::shared_ptr<ISender> get_sender() const = 0;
    virtual std::shared_ptr<IReceiver> get_receiver() const = 0;
};

class Flow : public IFlow, public std::enable_shared_from_this<Flow> {
public:
    Flow(std::shared_ptr<ISender> a_src, std::shared_ptr<IReceiver> a_dest,
         Size a_packet_size, Time a_delay_between_packets,
         std::uint32_t a_packets_to_send);
    virtual ~Flow() = default;

    // Start at time
    void start(Time time) final;

    Time create_new_data_packet() final;
    Time put_data_to_device() final;

    // Update the internal state according to some congestion control algorithm
    // Call try_to_generate upon the update
    void update(Packet packet, DeviceType type) final;
    std::uint32_t get_updates_number() const;

    std::shared_ptr<ISender> get_sender() const final;
    std::shared_ptr<IReceiver> get_receiver() const final;

    Id get_id() const final;

private:
    void schedule_packet_generation(Time time);
    Packet generate_packet();

    std::shared_ptr<ISender> m_src;
    std::shared_ptr<IReceiver> m_dest;
    Size m_packet_size;
    Time m_delay_between_packets;
    std::uint32_t m_updates_number;
    std::uint32_t m_packets_to_send;

    std::queue<Packet> m_sending_buffer;
    Id m_id;
};

}  // namespace sim
