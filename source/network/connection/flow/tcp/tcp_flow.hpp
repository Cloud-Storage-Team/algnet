#pragma once
#include "../i_flow.hpp"
#include "metrics/metrics_table/i_metricable.hpp"
#include "metrics/packet_reordering/simple_packet_reordering.hpp"
#include "rto.hpp"
#include "utils/packet_num_monitor.hpp"

namespace sim {

struct TcpFlowMetricsFilters {
    bool rtt = true;
    bool delivery_rate = true;
    bool reordering = true;
};

class TcpFlow : public IFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    constexpr static inline RTO DEFAULT_START_RTO =
        RTO(TimeNs(2000), Time<Second>(1));

    constexpr static inline bool DEFAULT_ECN_CAPABLE = true;
    constexpr static inline TcpFlowMetricsFilters DEFAULT_METRICS_FLAGS = {};

    static std::shared_ptr<TcpFlow> create_shared(
        Id a_id, FlowFourTuple a_four_tuple, bool a_ecn_capable = true,
        RTO a_rto = DEFAULT_START_RTO,
        TcpFlowMetricsFilters a_metrics_flags = DEFAULT_METRICS_FLAGS);

    virtual void send(std::vector<PacketInfo> packets_info) final;

    virtual const FlowContext& get_context() const final;

    virtual Id get_id() const final;

    virtual MetricsTable get_metrics_table() const final;

    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

private:
    TcpFlow(Id a_id, FlowFourTuple a_four_tuple, bool a_ecn_capable, RTO a_rto,
            TcpFlowMetricsFilters a_metrics_flags);

    Packet create_data_packet(PacketInfo info, std::shared_ptr<IHost> sender,
                              std::shared_ptr<IHost> receiver);

    void set_avg_rtt_if_present(Packet& packet);

    void send_data_packet(Packet data);

    void process_data_packet(const Packet& data_packet,
                             PacketCallback callback);

    void process_ack(const Packet& ack, SizeByte data_packet_size,
                     PacketCallback callback);

    void update_rto_on_ack();

    void on_timeout(Packet data);
    void update_rto_on_timeout();
    void retransmit_packet(Packet data);

private:
    // timeout event
    class Timeout;

    // flag labels
    enum PacketType { ACK, DATA, ENUM_SIZE };
    static std::string m_packet_type_label;
    const static inline TTL M_MAX_TTL = 31;

    // flag manager that used for every packet
    BaseFlagManager m_flag_manager;

    Id m_id;
    FlowContext m_context;
    SimplePacketReordering m_packet_reordering;
    bool m_ecn_capable;

    size_t m_next_packet_num = 0;
    PacketNumMonitor m_ack_monitor;

    // RTO parameters
    RTO m_rto;

    struct Metrics {
        std::shared_ptr<MetricsStorage> rtt;
        std::shared_ptr<MetricsStorage> delivery_rate;
        std::shared_ptr<MetricsStorage> packet_reordering;
    } m_metrics;

    TcpFlowMetricsFilters m_metrics_filters;
};

}  // namespace sim
