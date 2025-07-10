#pragma once

#include "device/interfaces/i_host.hpp"
#include "flow_common.hpp"
#include "packet/packet.hpp"
#include "utils/flag_manager.hpp"

namespace sim {

class TcpFlow : public IFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    TcpFlow(FlowCommon a_flow_common, std::uint32_t a_ssthresh = 8,
            bool a_ecn_capable = true);
    virtual ~TcpFlow() = default;

    void start() final;
    Time create_new_data_packet() final;
    void update(Packet packet, DeviceType type) final;

    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const final;
    Id get_id() const final;

private:
    enum PacketType { ACK, DATA, ENUM_SIZE };

    static void initialize_flag_manager();

    static bool m_is_flag_manager_initialized;
    static FlagManager<std::string, PacketFlagsBase> m_flag_manager;
    static std::string packet_type_label;

    Packet generate_packet();
    bool try_to_put_data_to_device();

    FlowCommon m_flow_common;

    double m_ssthresh;  // Slow start threshold
    double m_cwnd;      // Congestion window

    bool m_ecn_capable;
};
}  // namespace sim
