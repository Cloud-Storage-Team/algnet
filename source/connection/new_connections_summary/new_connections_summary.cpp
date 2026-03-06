#include "new_connections_summary.hpp"

#include "utils/filesystem.hpp"

namespace sim {
NewConnectionsSummary::NewConnectionsSummary(
    const utils::IdTable<INewConnection>& connections_table) {
    for (const auto& [id, connection] : connections_table) {
        m_contexts_table.emplace(id, connection->get_context());
    }
}

void NewConnectionsSummary::write_to_csv(
    std::filesystem::path output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    out << "Connection id";
    out << ", Sent size (bytes)";
    out << ", Delivered size (bytes)";
    out << ", Effective delivery rate (Gbps)";
    out << ", Delivery rate (Gbps)";
    out << ", CCT (ns)";
    out << '\n';
    for (const auto& [id, ctx] : m_contexts_table) {
        SpeedGbps effective_delivery_rate =
            ctx.delivery_rate_statistics.get_mean().value_or(SpeedGbps(0));
        std::optional<TimeNs> cct =
            (ctx.first_send_data_time.has_value() &&
                     ctx.last_data_delivery_time.has_value()
                 ? std::optional(ctx.last_data_delivery_time.value() -
                                 ctx.first_send_data_time.value())
                 : std::nullopt);
        SpeedGbps delivery_rate =
            (cct.has_value() ? SpeedGbps(ctx.total_data_delivered / cct.value())
                             : SpeedGbps(0.0));
        out << id;
        out << ", " << ctx.total_data_added;
        out << ", " << ctx.total_data_delivered;
        out << ", " << effective_delivery_rate;
        out << ", " << delivery_rate;
        out << ", " << cct.value_or(TimeNs(0.0));
        out << '\n';
    }
}

}  // namespace sim
