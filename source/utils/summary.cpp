#include "summary.hpp"

#include "filesystem.hpp"

namespace sim {

Summary::Summary(std::map<Id, std::map<Id, FlowSummary>> a_values)
    : m_values(std::move(a_values)) {}

Summary::Summary(
    const std::unordered_set<std::shared_ptr<IConnection>>& connections) {
    for (const auto& conn : connections) {
        Id conn_id = conn->get_id();
        SizeByte expt_data_delivery = conn->get_total_data_added();
        SizeByte real_data_delivery(0);
        auto flows = conn->get_flows();

        for (const auto& flow : flows) {
            const SizeByte delivered = flow->get_delivered_data_size();
            real_data_delivery += delivered;

            const SizeByte sent = flow->get_sent_data_size();
            if (sent < delivered) {
                throw std::runtime_error(fmt::format(
                    "For flow {} of connection {} sent {} bytes but delivered "
                    "{} bytes",
                    flow->get_id(), conn_id, sent.value(), delivered.value()));
            }

            const TimeNs fct = flow->get_fct();
            SpeedGbps sending_rate = sent / fct;
            SpeedGbps throughput = delivered / fct;
            uint32_t retransmit_count = flow->retransmit_count();

            m_values[conn_id][flow->get_id()] =
                FlowSummary{sent,       delivered, sending_rate,
                            throughput, fct,       retransmit_count};
        }
        if (expt_data_delivery != real_data_delivery) {
            throw std::runtime_error(fmt::format(
                "For connection {} expected delivery {} but real is {}",
                conn_id, expt_data_delivery.value(),
                real_data_delivery.value()));
        }
    }
}

void Summary::write_to_csv(std::filesystem::path output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    out << "Conn id\n  Flow id, Sent (bytes), Delivered (bytes), Sending "
           "rate "
           "(Gbps), Throughput (Gbps), FCT (ns), Retransmit count\n\n";
    for (const auto& [conn_id, flows] : m_values) {
        for (const auto& [flow_id, fs] : flows) {
            out << conn_id << ":\n  " << flow_id << ", " << fs.sent.value()
                << ", " << fs.delivered.value() << ", "
                << fs.sending_rate.value() << ", " << fs.throughput << ", "
                << fs.fct.value() << ", " << fs.retransmit_count << "\n";
        }
    }
    out.close();
}

}  // namespace sim
