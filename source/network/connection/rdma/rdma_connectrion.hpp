#pragma once
#include <deque>

#include "../i_connection.hpp"
#include "dcqcn/dcqcn.hpp"
#include "network/connection/flow/flow_four_tuple.hpp"
#include "network/connection/flow/packet.hpp"
#include "topology/device/host.hpp"

namespace sim {

class RdmaConnection;

using RdmaConnectionPtr = std::shared_ptr<RdmaConnection>;

class RdmaConnection : public IConnection,
                       public std::enable_shared_from_this<RdmaConnection> {
public:
    static RdmaConnectionPtr create_shared(const Id& a_id, const DCQCN& a_dcqcn,
                                           const FlowFourTuple& a_ft);

    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    ConnectionContext get_context() const final;

    virtual MetricsTable get_metrics_table() const final;

    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

    virtual const Id& get_id() const final;

private:
    RdmaConnection(const Id& a_id, const DCQCN& a_dcqcn,
                   const FlowFourTuple& a_endpoints);

    void start_data_sending();

    void schedule_data_send();

    void send_next_data_packet();

    Packet create_data_packet(const Data& data);

    void process_data_packet(const Packet& data);

    void send_ack();

    void process_ack(const Packet& ack);

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
    // TODO: make custom
    SizeByte m_packet_size = SizeByte(1500ul);

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
    // TODO: make custom
    uint32_t m_ack_threshold = 32;
    PacketNum m_next_expected_packet_num = 1;
    static constexpr SizeByte M_ACK_SIZE = SizeByte(1ul);
};

}  // namespace sim