#include "host.hpp"

#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"
#include "utils/validation.hpp"

namespace sim {

Host::Host(Id a_id, ECN a_ecn) : RoutingModule(a_id), m_ecn(a_ecn) {}

bool Host::notify_about_arrival(TimeNs arrival_time) {
    return m_process_scheduler.notify_about_arriving(arrival_time,
                                                     weak_from_this());
};

void Host::enqueue_packet(Packet packet) {
    m_nic_buffer.push(packet);
    m_send_data_scheduler.notify_about_arriving(
        Scheduler::get_instance().get_current_time(), weak_from_this());
    LOG_INFO(fmt::format("Packet {} arrived to host", packet.to_string()));
}

TimeNs Host::process() {
    std::shared_ptr<ILink> current_inlink = next_inlink();
    TimeNs total_processing_time = TimeNs(1);

    if (current_inlink == nullptr) {
        LOG_WARN("No available inlinks for device");
        return total_processing_time;
    }

    std::optional<Packet> opt_packet = current_inlink->get_packet();
    if (!opt_packet.has_value()) {
        LOG_WARN("No available packets from inlink for device");
        return total_processing_time;
    }

    Packet packet = opt_packet.value();
    if (packet.flow == nullptr) {
        LOG_ERROR("Packet flow does not exist");
        return total_processing_time;
    }

    // TODO: add some sender ID for easier packet path tracing
    LOG_INFO("Processing packet from link on host. Packet: " +
             packet.to_string());

    if (packet.dest_id == get_id()) {
        packet.flow->update(packet);
    } else {
        LOG_WARN(
            "Packet arrived to Host that is not its destination; use routing "
            "table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return total_processing_time;
        }

        if (packet.ttl == 0) {
            LOG_ERROR(
                fmt::format("Packet ttl expired on device {}; packet {} lost",
                            get_id(), packet.to_string()));
            return total_processing_time;
        }
        packet.ttl--;

        next_link->schedule_arrival(packet);
    }

    TimeNs current_time = Scheduler::get_instance().get_current_time();
    if (m_process_scheduler.notify_about_finish(current_time +
                                                total_processing_time)) {
        return TimeNs(0);
    }

    return total_processing_time;
}

TimeNs Host::send_packet() {
    TimeNs total_processing_time = TimeNs(1);

    if (m_nic_buffer.empty()) {
        LOG_WARN("No packets to send");
        return total_processing_time;
    }
    Packet data_packet = m_nic_buffer.front();
    m_nic_buffer.pop();

    LOG_INFO(fmt::format("Taken new data packet on host {}. Packet: {}",
                         get_id(), data_packet.to_string()));

    auto next_link = get_link_to_destination(data_packet);
    if (next_link == nullptr) {
        LOG_WARN("Link to send data packet does not exist");
        return total_processing_time;
    }

    LOG_INFO(fmt::format("Sent new packet from host. Packet: {}", get_id(),
                         data_packet.to_string()));

    if (data_packet.ecn_capable_transport) {
        float egress_queue_filling =
            (next_link->get_from_egress_queue_size() + data_packet.size) /
            next_link->get_max_from_egress_buffer_size();
        if (m_ecn.get_congestion_mark(egress_queue_filling)) {
            data_packet.congestion_experienced = true;
        }
    }

    next_link->schedule_arrival(data_packet);
    if (m_send_data_scheduler.notify_about_finish(
            Scheduler::get_instance().get_current_time() +
            total_processing_time)) {
        return TimeNs(0);
    }

    return total_processing_time;
}

}  // namespace sim
