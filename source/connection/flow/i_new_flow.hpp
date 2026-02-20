#pragma once

#include "device/interfaces/i_host.hpp"
#include "metrics/metrics_storage.hpp"
#include "packet.hpp"
#include "packet_ack_info.hpp"
#include "packet_info.hpp"
#include "utils/statistics.hpp"

namespace sim {

struct Endpoints {
    std::shared_ptr<IHost> sender;
    std::shared_ptr<IHost> receiver;
};

struct FlowContext : public Endpoints {
    SizeByte sent_size;
    SizeByte delivered_size;
    SizeByte retransmit_size;

    std::optional<TimeNs> start_time;
    std::optional<TimeNs> last_ack_receive_time;
    utils::Statistics<TimeNs> rtt_statistics;

    struct Metrics {
        MetricsStorage rtt;
        MetricsStorage delivery_rate;
        MetricsStorage packet_reordering;
        MetricsStorage packet_spacing;
    } metrics;
};

// Transport layer interface for reliable data delivery along single physical
// path
class INewFlow : public virtual Identifiable {
public:
    virtual void send(std::vector<PacketInfo> packets) = 0;

    virtual const FlowContext& get_context() const = 0;
};

}  // namespace sim
