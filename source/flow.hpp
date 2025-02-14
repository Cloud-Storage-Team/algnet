#pragma once

namespace sim {


class Packet;

class Flow {
public:
    Flow(Device* src, Device* dest, size_t packet_size, size_t delay_between_packets, size_t total_packets);

    void start(int time);

    // Try to generate a new packet if the internal state allows to do so.
    // by placing it into the flow buffer of the source node.
    // Schedule the next generation event.
    bool try_to_generate();

    // Update the internal state according to some congestion control algorithm
    // Call on sender-only if ACK packet was received 
    void ack();

    // Call on sender-only in case of congestions
    void trigger_congestion();

    Device *m_src;
    Device *m_dest;

private:
    void schedule_packet_generation(int time);
    
    size_t m_packet_size;
    size_t m_delay_between_packets;
    size_t m_total_packets;
    size_t m_packets_sent;
    size_t m_last_send_time;

    size_t m_cwnd;                      // Congestion window
    size_t m_ssthresh;                  // Slow start threshold
    size_t m_linear_coeficient;
    size_t m_packets_in_flight;
    size_t m_packets_acked;
};

}  // namespace sim
