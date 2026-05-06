#pragma once
#include <deque>
#include <optional>

#include "../i_connection.hpp"
#include "network/connection/flow/flow_four_tuple.hpp"
#include "network/connection/flow/packet.hpp"
#include "rdma_params.hpp"
#include "topology/device/host.hpp"

namespace sim {

class RdmaConnection;

using RdmaConnectionPtr = std::shared_ptr<RdmaConnection>;

class RdmaConnection : public IConnection,
                       public std::enable_shared_from_this<RdmaConnection> {
public:
    static RdmaConnectionPtr create_shared(const RdmaParams& a_params);

    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    ConnectionContext get_context() const final;

    virtual MetricsTable get_metrics_table() const final;

    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

    virtual const Id& get_id() const final;

private:
    RdmaConnection(const RdmaParams& a_params);

    void schedule_ack_timer();

    void on_ack_timer();

    void on_retry_timout();

    void schedule_retry_timout();

    void retransmit_packets();

    void start_data_sending();

    void schedule_data_send();

    void send_next_data_packet();

    Packet create_data_packet(const Data& data);

    void process_data_packet(const Packet& data);

    void process_expected_data_packet();

    void send_nak();

    void process_nak(const Packet& nak);

    void send_ack();

    void process_ack(const Packet& ack);

    void send_cnp();

    void process_cnp(const Packet& cnp);

    void confirm_first_unconfirmed_packet();

    void send_ack_request();

    void process_ack_request();

    ConnectionContext m_ctx;
    Id m_id;
    DCQCN m_dcqcn;

    // --------------------sender part--------------------
    static constexpr SizeByte ACK_REQUEST_SIZE = SizeByte(1ul);
    std::shared_ptr<IHost> m_sender;
    Port m_sender_port;

    SizeByte m_packet_size;
    TimeNs m_retry_timout = TimeNs(50000);

    // Invariant: packet_num of first packet in this queue is equal to
    // m_last_acked_pcn
    std::deque<Packet> m_send_queue;

    uint32_t m_send_window;

    // number of first packet that was not send
    PacketNum m_pcn = 1;
    PacketNum m_last_acked_pcn = 0;
    PacketNum m_next_packet_num = 1;

    struct DataContext {
        SizeByte total_size;
        SizeByte sent;
        SizeByte delivered;
        OnDeliveryCallback callback;
    };
    std::map<DataId, DataContext> m_data_context_table;

    // --------------------receiver part--------------------
    std::shared_ptr<IHost> m_receiver;
    Port m_receiver_port;
    uint32_t m_data_packets_on_receiver = 0;

    uint32_t m_ack_threshold;
    PacketNum m_next_expected_packet_num = 1;
    static constexpr SizeByte M_ACK_SIZE = SizeByte(1ul);
    static constexpr SizeByte M_NAK_SIZE = M_ACK_SIZE;

    // Invariant: if packet with number i > m_next_packet_num received,
    // m_reorder_buffer[i - m_next_packet_num - 1] contains it
    std::deque<std::optional<Packet> > m_reorder_buffer;
    std::size_t m_max_reorder_buffer_size;
    TimeNs m_ack_receiver_timout = TimeNs(50000);
    TimeNs m_last_ack_send = TimeNs(0);
    bool m_receiver_started = false;
};

}  // namespace sim
