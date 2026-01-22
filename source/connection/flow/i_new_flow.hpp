#pragma once

#include "device/interfaces/i_host.hpp"
#include "packet.hpp"
#include "utils/statistics.hpp"

namespace sim {

struct FlowContext {
    SizeByte sent_size;
    SizeByte delivered_size;
    SizeByte retransmit_size;

    std::optional<TimeNs> start_time;
    std::optional<TimeNs> last_ack_receive_time;
    utils::Statistics<TimeNs> rtt_statistics;

    std::weak_ptr<IHost> sender;
    std::weak_ptr<IHost> receiver;
};

struct PacketAckInfo {
    TimeNs rtt;
    TimeNs avg_rtt;
    bool ecn_flag;
};

using PacketCallback = std::function<void(PacketAckInfo)>;

struct PacketInfo {
    DataId id;
    SizeByte packet_size;
    PacketCallback callback;
    TimeNs generated_time;
};

// Transport layer interface for reliable data delivery along single physical
// path
class INewFlow : public virtual Identifiable {
public:
    virtual void send(std::vector<PacketInfo> packets) = 0;

    virtual const FlowContext& get_context() = 0;
};

}  // namespace sim
