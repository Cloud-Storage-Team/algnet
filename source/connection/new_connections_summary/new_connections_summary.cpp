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
    out << ", Delivery rate (Gbps)";
    out << '\n';
    for (const auto& [id, ctx] : m_contexts_table) {
        SpeedGbps delivery_rate =
            ctx.delivery_rate_statistics.get_mean().value_or(SpeedGbps(0));
        out << id;
        out << ", " << ctx.total_data_added;
        out << ", " << ctx.total_data_delivered;
        out << ", " << delivery_rate;
        out << '\n';
    }
}

}  // namespace sim
