#pragma once
#include "../i_new_flow.hpp"
#include "rto.hpp"
#include "utils/packet_num_monitor.hpp"

namespace sim {

class NewTcpFlow : public INewFlow,
                   public std::enable_shared_from_this<NewTcpFlow> {
public:
    std::shared_ptr<NewTcpFlow> create_shared(Id a_id,
                                              std::shared_ptr<IHost> a_sender,
                                              std::shared_ptr<IHost> a_receiver,
                                              bool a_ecn_capable = true,
                                              RTO a_rto = DEFAULT_START_RTO);

    virtual void send(std::vector<PacketInfo> packets_info) final;

    virtual const FlowContext& get_context() const final;

    virtual Id get_id() const final;

private:
    NewTcpFlow(Id a_id, std::shared_ptr<IHost> a_sender,
               std::shared_ptr<IHost> a_receiver, bool a_ecn_capable,
               RTO a_rto);

    struct Endpoints {
        std::shared_ptr<IHost> sender;
        std::shared_ptr<IHost> receiver;
    };

    utils::StrExpected<Endpoints> get_endpoints() const;

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
    void reatransmit_packet(Packet data);

private:
    // timeout event
    class Timeout;

    // flag labels
    enum PacketType { ACK, DATA, ENUM_SIZE };
    static std::string m_packet_type_label;
    static std::string m_ack_ttl_label;
    const static inline TTL M_MAX_TTL = 31;

    constexpr static inline RTO DEFAULT_START_RTO =
        RTO(TimeNs(2000), Time<Second>(1));

    // flag manager that used for every packet
    BaseFlagManager m_flag_manager;

    Id m_id;
    FlowContext m_context;
    bool m_ecn_capable;

    size_t m_next_packet_num = 0;
    PacketNumMonitor m_ack_monitor;

    // RTO parameters
    RTO m_rto;
};

}  // namespace sim
