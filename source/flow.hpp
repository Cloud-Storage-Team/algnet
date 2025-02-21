#pragma once
#include <cstdint>

namespace sim {


class Device;
class Packet;

class Flow {
public:

    Flow(Device* a_src, Device* a_dest, std::uint32_t a_packet_size,
         std::uint32_t a_delay_between_packets, std::uint32_t a_total_packets,
         std::uint32_t a_delay_threshold);


    // Start at time
    void start(std::uint32_t time);

    // Try to generate a new packet if the internal state allows to do so.
    // Returns true if a generate_event was created and placed in scheduler
    bool try_to_generate(std::uint32_t current_time);

    /*
    Slow Start:
        The algorithm increases the m_cwnd until it reaches a threshold
        (m_ssthresh). Initially, m_cwnd starts at 1 and increases by 1 for each
        ACK received.

    Congestion Avoidance:
        Once m_cwnd reaches m_ssthresh, the
        algorithm enters a congestion avoidance phase where it increases m_cwnd
        by 1 for every m_cwnd / m_linear_coeficient packets acknowledged.

    Congestion Detection and Reaction:
        The algorithm checks if the delay is less than a
        specified threshold (m_delay). If it is, an ACK is processed; otherwise,
        congestion is triggered. Upon detecting congestion, the algorithm halves
        the m_ssthresh and resets m_cwnd to 1
    */
    void update(std::uint32_t delay);

    Device* m_src;
    Device* m_dest;

private:
    void schedule_packet_generation(std::uint32_t time);

    std::uint32_t m_packet_size;
    std::uint32_t m_delay_between_packets;
    std::uint32_t m_total_packets;
    std::uint32_t m_packets_sent;
    std::uint32_t m_last_send_time;

    std::uint32_t m_cwnd;      // Congestion window
    std::uint32_t m_ssthresh;  // Slow start threshold
    std::uint32_t m_linear_coeficient;
    std::uint32_t m_packets_in_flight;
    std::uint32_t m_packets_acked;
    std::uint32_t m_delay_threshold;  // delay threshold for update
};

}  // namespace sim
