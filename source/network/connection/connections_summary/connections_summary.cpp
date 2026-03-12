#include "connections_summary.hpp"

#include "utils/filesystem.hpp"

namespace sim {
ConnectionsSummary::ConnectionsSummary(
    const utils::IdTable<IConnection>& connections_table) {
    for (const auto& [id, connection] : connections_table) {
        m_contexts_table.emplace(id, connection->get_context());
    }
}

void ConnectionsSummary::write_to_csv(std::filesystem::path output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error(fmt::format(
            "Failed to create file for summary: {}", output_path.string()));
    }
    out << "Connection id"
        << ", Sent size (bytes)"
        << ", Delivered size (bytes)"
        << ", Effective delivery rate (Gbps)"
        << ", Delivery rate (Gbps)"
        << ", CCT (ns)\n";
    for (const auto& [id, ctx] : m_contexts_table) {
        // Effective delivery rate is average delivery rate calculated on the
        // spot (when connection is active) so does not include incative time
        // segments
        SpeedGbps effective_delivery_rate =
            ctx.delivery_rate_statistics.get_mean().value_or(SpeedGbps(0));
        TimeNs cct = ctx.first_send_data_time.has_value() &&
                             ctx.last_data_delivery_time.has_value()
                         ? ctx.last_data_delivery_time.value() -
                               ctx.first_send_data_time.value()
                         : TimeNs(0);
        SpeedGbps delivery_rate =
            (cct != TimeNs(0) ? SpeedGbps(ctx.total_data_delivered / cct)
                              : SpeedGbps(0.0));
        out << id;
        out << ", " << ctx.total_data_added;
        out << ", " << ctx.total_data_delivered;
        out << ", " << effective_delivery_rate;
        out << ", " << delivery_rate;
        out << ", " << cct;
        out << '\n';
    }
}

}  // namespace sim
