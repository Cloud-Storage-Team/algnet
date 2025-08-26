#include "summary.hpp"

#include "filesystem.hpp"

namespace sim {

Summary::Summary(std::map<Id, std::map<Id, SpeedGbps>> a_values)
    : m_values(std::move(a_values)) {}

Summary::Summary(
    const std::unordered_set<std::shared_ptr<IConnection>>& connections) {
    for (const auto& conn : connections) {
        Id conn_id = conn->get_id();
        auto flows = conn->get_flows();
        for (const auto& flow : flows) {
            SpeedGbps throughput =
                flow->get_delivered_data_size() / flow->get_fct();
            m_values[conn_id].insert(
                std::make_pair(flow->get_id(), throughput));
        }
    }
}

void Summary::write_to_csv(std::filesystem::path output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    out << "Conn id, Flow id, Throughput (Gbps)\n";
    for (const auto& [conn_id, flows] : m_values) {
        for (const auto& [flow_id, value] : flows) {
            out << conn_id << ", " << flow_id << ", " << value << "\n";
        }
    }
    out.close();
}

}  // namespace sim
