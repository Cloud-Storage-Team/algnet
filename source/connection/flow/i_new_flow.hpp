#pragma once

#include "device/interfaces/i_host.hpp"
#include "flow_four_tuple.hpp"
#include "metrics/metrics_storage.hpp"
#include "metrics/metrics_table/i_metricable.hpp"
#include "packet_ack_info.hpp"
#include "packet_info.hpp"
#include "utils/statistics.hpp"

namespace sim {

struct FlowContext : FlowFourTuple {
    using FlowFourTuple::FlowFourTuple;
    SizeByte sent_size = SizeByte(0);
    SizeByte delivered_size = SizeByte(0);
    SizeByte retransmit_size = SizeByte(0);

    std::optional<TimeNs> start_time = std::nullopt;
    std::optional<TimeNs> last_ack_receive_time = std::nullopt;
    utils::Statistics<TimeNs> rtt_statistics;
    utils::Statistics<SpeedGbps> delivery_rate_statistics;
};

// Transport layer interface for reliable data delivery along single physical
// path
class INewFlow : public virtual Identifiable, public virtual IMetricable {
public:
    virtual void send(std::vector<PacketInfo> packets) = 0;

    virtual const FlowContext& get_context() const = 0;
};

}  // namespace sim
