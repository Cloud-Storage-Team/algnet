#include "single_cc_mplb.hpp"

#include <cmath>

#include "event/call_at_time.hpp"
#include "event/send_data.hpp"
#include "scheduler.hpp"
#include "utils/callback_observer.hpp"

namespace sim {

std::shared_ptr<SingleCCMplb> SingleCCMplb::create_shared(
    std::unique_ptr<ITcpCC> a_cc, std::unique_ptr<IPathChooser> a_path_chooser,
    SizeByte a_packet_size) {
    return std::shared_ptr<SingleCCMplb>(new SingleCCMplb(
        std::move(a_cc), std::move(a_path_chooser), a_packet_size));
}

SingleCCMplb::SingleCCMplb(std::unique_ptr<ITcpCC> a_cc,
                           std::unique_ptr<IPathChooser> a_path_chooser,
                           SizeByte a_packet_size)
    : m_cc(std::move(a_cc)),
      m_sent_data_size(0),
      m_delivered_data_size(0),
      m_packets_in_flight(0),
      m_path_chooser(std::move(a_path_chooser)),
      m_packet_size(a_packet_size) {}

utils::StrExpected<void> SingleCCMplb::send_data(Data data,
                                                 OnDeliveryCallback callback) {
    SizeByte quota = get_quota();
    if (data.size > quota) {
        return std::unexpected(
            fmt::format("Data size {}b is bigger that quota size: {}b",
                        data.size.value(), quota.value()));
    }

    TimeNs pacing_delay = m_cc->get_pacing_delay();
    TimeNs shift(0);

    std::size_t packets_count = std::ceil(data.size / m_packet_size);

    std::shared_ptr<utils::CallbackObserver> observer =
        std::make_shared<utils::CallbackObserver>(packets_count, callback);
    TimeNs now = Scheduler::get_instance().get_current_time();

    for (size_t packet_num = 0; packet_num < packets_count; packet_num++) {
        // TODO: think about pasing delay & sending data to many flows (is it
        // really needed in such case?)
        std::weak_ptr<INewFlow> flow_weak = m_path_chooser->choose_flow();
        shift += pacing_delay;
        std::weak_ptr<SingleCCMplb> mplb_weak = shared_from_this();
        PacketCallback packet_callback = [observer, mplb_weak,
                                          flow_weak](PacketAckInfo info) {
            observer->on_single_callback();
            if (mplb_weak.expired()) {
                LOG_ERROR("MPLB pointer expired, could not call callback");
                return;
            }
            auto mplb = mplb_weak.lock();
            mplb->m_cc->on_ack(info.rtt, info.avg_rtt, info.ecn_flag);
            mplb->m_delivered_data_size += mplb->m_packet_size;
        };
        PacketInfo info{data.id, m_packet_size, packet_callback, now};
        Scheduler::get_instance().add<CallAtTime>(
            now + shift,
            [mplb_weak, flow_weak, packet_info = std::move(info)]() {
                if (flow_weak.expired()) {
                    LOG_ERROR("Flow pointer expired; could not send data");
                    return;
                }
                if (mplb_weak.expired()) {
                    LOG_ERROR(
                        "MPLB pointer expired; could not increase sent data "
                        "size");
                } else {
                    mplb_weak.lock()->m_sent_data_size +=
                        packet_info.packet_size;
                }
                flow_weak.lock()->send(
                    std::vector<PacketInfo>(1, std::move(packet_info)));
            });
        m_packets_in_flight++;
    }

    return {};
}

MPLBContext SingleCCMplb::get_context() const {
    return MPLBContext{m_path_chooser->get_flows(), m_sent_data_size,
                       m_delivered_data_size, get_quota()};
}

SizeByte SingleCCMplb::get_quota() const {
    const double cwnd = m_cc->get_cwnd();

    // Effective window: the whole part of cwnd; if cwnd < 1 and inflight == 0,
    // allow 1 packet
    std::uint32_t effective_cwnd = static_cast<std::uint32_t>(std::floor(cwnd));
    if (m_packets_in_flight == 0 && cwnd < 1.0) {
        effective_cwnd = 1;
    }
    const std::uint32_t quota_pkts =
        (effective_cwnd > m_packets_in_flight)
            ? (effective_cwnd - m_packets_in_flight)
            : 0;

    // Quota in bytes, multiple of the packet size
    return quota_pkts * m_packet_size;
}

}  // namespace sim
