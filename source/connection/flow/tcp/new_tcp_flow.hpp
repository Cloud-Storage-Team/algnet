#pragma once
#include "../i_new_flow.hpp"

namespace sim {

class NewTcpFlow : public INewFlow,
                   public std::enable_shared_from_this<NewTcpFlow> {
public:
    std::shared_ptr<NewTcpFlow> create_shared(Id a_id,
                                              std::shared_ptr<IHost> a_sender,
                                              std::shared_ptr<IHost> a_receiver,
                                              bool a_ecn_capable = true);

    virtual void send(std::vector<PacketInfo> packets_info) final;

    virtual const FlowContext& get_context() const final;

    virtual Id get_id() const final;

private:
    NewTcpFlow(Id a_id, std::shared_ptr<IHost> a_sender,
               std::shared_ptr<IHost> a_receiver, bool a_ecn_capable = true);

    struct Endpoints {
        std::shared_ptr<IHost> sender;
        std::shared_ptr<IHost> receiver;
    };

    utils::StrExpected<Endpoints> get_endpoints() const;

    Packet create_data_packet(PacketInfo info, std::shared_ptr<IHost> sender,
                              std::shared_ptr<IHost> receiver);

    void process_data_packet(const Packet& data_packet,
                             PacketCallback callback);
    void process_ack(const Packet& ack, SizeByte data_packet_size,
                     PacketCallback callback);

    Id m_id;
    bool m_ecn_capable;
    FlowContext m_context;
    size_t m_next_packet_num = 0;

    static std::string m_packet_type_label;
    enum PacketType { ACK, DATA, ENUM_SIZE };

    static std::string m_ack_ttl_label;

    // flag manager that used for every packet
    BaseFlagManager m_flag_manager;
    const static inline TTL M_MAX_TTL = 31;
};

}  // namespace sim
