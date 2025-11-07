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

// Transport layer interface for reliable data delivery along single physical
// path
class INewFlow : public virtual Identifiable {
public:
    // Callback that should be triggered when data packet arrives to receiver
    virtual void on_recv(const Packet& data) = 0;

    // Callback that should be triggered when ack packet arrives to sender
    virtual void on_ack(const Packet& ack) = 0;

    virtual void send(std::vector<Packet> packets) = 0;

    virtual const FlowContext& get_context() = 0;
};

}  // namespace sim
