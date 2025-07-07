#pragma once

#include <queue>

#include "device/interfaces/i_host.hpp"
#include "flow_common.hpp"
#include "packet/packet.hpp"
#include "utils/flag_manager.hpp"

namespace sim {

// Basic flow version, no congestion control
class BasicFlow : public IFlow, public std::enable_shared_from_this<BasicFlow> {
public:
    BasicFlow(FlowCommon a_flow_common);
    virtual ~BasicFlow() = default;

    Id get_id() const final;

    void start() final;
    Time create_new_data_packet() final;
    void update(Packet packet, DeviceType type) final;

    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const final;

    std::uint32_t get_updates_number() const;

private:
    enum PacketType { ACK, DATA, ENUM_SIZE };

    static void initialize_flag_manager();

    static std::string packet_type_label;
    static bool m_is_flag_manager_initialized;
    static FlagManager<std::string, PacketFlagsBase> m_flag_manager;

    Time put_data_to_device();
    void schedule_packet_generation(Time time);
    Packet generate_packet();

    FlowCommon m_flow_common;
};

}  // namespace sim
