#include "host.hpp"

#include <spdlog/fmt/fmt.h>

#include "logger/logger.hpp"
#include "scheduler/scheduler.hpp"
#include "utils/defer.hpp"
#include "utils/validation.hpp"

namespace sim {

Host::Host(Id a_id, ECN a_ecn) : RoutingModule(a_id), m_ecn(a_ecn) {}

bool Host::notify_about_arrival() {
    if (++m_packets_on_inlinks == 1) {
        process();
        return true;
    }
    return false;
};

void Host::enqueue_packet(const Packet& packet) {
    LOG_INFO(fmt::format("Packet {} arrived to host", packet.to_string()));
    m_nic_buffer.push(packet);
    if (m_nic_buffer.size() == 1) {
        // first packet in queue => start sending it
        send_packet();
        return;
    }
    // packets sending already started;
}

void Host::process() {
    static constexpr TimeNs PACKET_PROCESSING_TIME = TimeNs(1);

    std::shared_ptr<ILink> current_inlink = next_inlink();

    if (current_inlink == nullptr) {
        LOG_WARN("No available inlinks for device");
        return;
    }

    auto process_lambda = [host = shared_from_this()]() { host->process(); };

    while (!current_inlink->has_packet()) {
        current_inlink = next_inlink();
    }

    Packet& packet = current_inlink->get_packet();
    utils::Defer defer{[current_inlink]() { current_inlink->pop_packet(); }};

    utils::Defer defer_reschedule{[this]() {
        if (--m_packets_on_inlinks != 0) {
            Scheduler& sched = Scheduler::get_instance();
            sched.add(sched.get_current_time() + PACKET_PROCESSING_TIME,
                      [host = shared_from_this()]() { host->process(); });
        }
    }};

    LOG_INFO(fmt::format("Host {}: processing packet {}", get_id(),
                         packet.to_string()));

    if (packet.receiver_id == get_id()) {
        packet.callback(packet);
    } else {
        LOG_WARN(
            "Host {}: arrived packet {} which destination differs from this "
            "host; use routing "
            "table to send it further");
        std::shared_ptr<ILink> next_link = get_link_to_destination(packet);

        if (next_link == nullptr) {
            LOG_WARN("No link corresponds to destination device");
            return;
        }

        if (packet.ttl == 0) {
            LOG_ERROR(
                fmt::format("Packet ttl expired on device {}; packet {} lost",
                            get_id(), packet.to_string()));
            return;
        }
        packet.ttl--;

        next_link->schedule_arrival(packet);
    }

    return;
}

void Host::send_packet() {
    static constexpr TimeNs PACKET_PROCESSING_TIME = TimeNs(1);
    if (m_nic_buffer.empty()) {
        LOG_ERROR(fmt::format("Host {}: call send_packet with empty nic buffer",
                              get_id()));
        return;
    }
    {
        Packet& data_packet = m_nic_buffer.front();
        utils::Defer defer{[this] { m_nic_buffer.pop(); }};

        LOG_INFO(fmt::format("Taken new data packet on host {}. Packet: {}",
                             get_id(), data_packet.to_string()));

        auto next_link = get_link_to_destination(data_packet);
        if (next_link == nullptr) {
            LOG_WARN("Link to send data packet does not exist");
            return;
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
    }

    if (!m_nic_buffer.empty()) {
        Scheduler& sched = Scheduler::get_instance();
        sched.add(sched.get_current_time() + PACKET_PROCESSING_TIME,
                  [host = shared_from_this()]() { host->send_packet(); });
    }
}

}  // namespace sim
