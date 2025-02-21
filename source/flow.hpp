#pragma once

namespace sim {

class Packet;

class Flow {
public:
    Flow(Device* src, size_t packet_size, size_t delay_between_packets,
         size_t total_packets, size_t congestion_delay);

    void start(int time);

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    bool try_to_generate(size_t current_time);

    /*
    Slow Start:
        The algorithm increases the m_cwnd until it reaches a threshold
        (m_ssthresh). Initially, m_cwnd starts at 1 and increases by 1 for each
        ACK received.
    */

    /*Congestion Avoidance:
        Once m_cwnd reaches m_ssthresh, the
        algorithm enters a congestion avoidance phase where it increases m_cwnd
        by 1 for every m_cwnd / m_linear_coeficient packets acknowledged. */

    /*Congestion Detection and Reaction:
        The algorithm checks if the delay is less than a
        specified threshold (m_delay). If it is, an ACK is processed; otherwise,
        congestion is triggered. Upon detecting congestion, the algorithm halves
        the m_ssthresh and resets m_cwnd to 1
    */
    void update(size_t delay);

    Device* m_src;

private:
    void schedule_packet_generation(int time);

    size_t m_packet_size;
    size_t m_delay_between_packets;
    size_t m_total_packets;
    size_t m_packets_sent;
    size_t m_last_send_time;

    size_t m_cwnd;      // Congestion window
    size_t m_ssthresh;  // Slow start threshold
    size_t m_linear_coeficient;
    size_t m_packets_in_flight;
    size_t m_packets_acked;
    size_t m_delay;  // delay for update
};

}  // namespace sim
