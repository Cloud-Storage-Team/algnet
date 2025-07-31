#include "summary.hpp"

#include "filesystem.hpp"

namespace sim {

Summary::Summary(std::map<Id, SizeByte> a_values)
    : m_values(std::move(a_values)) {}

Summary::Summary(std::unordered_set<std::shared_ptr<IFlow> > flows) {
    m_values = {};
    for (auto flow : flows) {
        m_values.emplace(flow->get_id(), flow->get_delivered_data_size());
    }
}

void Summary::write_to_csv(std::filesystem::path output_path) const {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    out << "Flow id, Delivered data (bytes)\n";
    for (const auto& [flow_id, value] : m_values) {
        out << flow_id << ", " << value << "\n";
    }
    out.close();
}

}  // namespace sim