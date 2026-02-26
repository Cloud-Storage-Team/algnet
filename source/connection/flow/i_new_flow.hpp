#pragma once

#include "device/interfaces/i_host.hpp"
#include "endpoints.hpp"
#include "metrics/metrics_storage.hpp"
#include "metrics/metrics_table/i_metricable.hpp"
#include "packet_ack_info.hpp"
#include "packet_info.hpp"
#include "utils/statistics.hpp"

namespace sim {

struct FlowContext : public Endpoints {
    SizeByte sent_size;
    SizeByte delivered_size;
    SizeByte retransmit_size;

    std::optional<TimeNs> start_time;
    std::optional<TimeNs> last_ack_receive_time;
    utils::Statistics<TimeNs> rtt_statistics;
};

// Transport layer interface for reliable data delivery along single physical
// path
class INewFlow : public virtual Identifiable, public virtual IMetricable {
public:
    virtual void send(std::vector<PacketInfo> packets) = 0;

    virtual const FlowContext& get_context() const = 0;
};

}  // namespace sim
