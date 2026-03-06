#include "new_flows_summary.hpp"

#include "utils/filesystem.hpp"

namespace sim {
NewFlowsSummary::NewFlowsSummary(const utils::IdTable<INewFlow>& flows_table) {
    for (const auto& [id, flow] : flows_table) {
        m_flows_contexts.emplace(id, flow->get_context());
    }
}

void NewFlowsSummary::write_to_csv(std::filesystem::path output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    out << "Flow id";
    out << ", Sent size (bytes)";
    out << ", Delivered size (bytes)";
    out << ", Overhead (%)";
    out << ", Retransmit size (bytes)";
    out << ", Delivery rate (Gbps)";
    out << ", FCT (ns)";
    out << "\n";
    for (const auto& [flow_id, ctx] : m_flows_contexts) {
        TimeNs fct =
            (ctx.last_ack_receive_time.has_value() && ctx.start_time.has_value()
                 ? ctx.last_ack_receive_time.value() - ctx.start_time.value()
                 : TimeNs(0));

        double overhead = ctx.delivered_size != SizeByte(0)
                              ? (ctx.sent_size / ctx.delivered_size - 1) * 100
                              : std::nan("");

        SpeedGbps delivery_rate =
            ctx.delivery_rate_statistics.get_mean().value_or(SpeedGbps(0.0));

        out << flow_id;
        out << ", " << ctx.sent_size;
        out << ", " << ctx.delivered_size;
        out << ", " << overhead;
        out << ", " << ctx.retransmit_size;
        out << ", " << delivery_rate;
        out << ", " << fct;
        out << "\n";
    }
}

}  // namespace sim
