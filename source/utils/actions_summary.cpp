#include "actions_summary.hpp"

#include "utils/filesystem.hpp"

static std::string join_ids(const std::vector<Id>& ids) {
    std::string result;
    std::size_t ids_count = ids.size();
    for (std::size_t i = 0; i < ids_count; i++) {
        result += ids[i];
        if (i + 1 < ids_count) {
            result += "; ";
        }
    }
    return result;
}

namespace sim {
void write_to_csv(const std::string& output_path,
                  const SendDataActionsSummary& summary) {
    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    if (!out) {
        throw std::runtime_error("Failed to create file for summary");
    }
    std::string joined_connection_ids;
    out << "Data Id";
    out << ", " << "Connection Ids";
    out << ", " << "Data Size (bytes)";
    out << ", " << "Time Spent (ns)";
    out << ", " << "Throughput (Gbps)";
    out << ", " << "Start time (ns)";
    out << ", " << "Finish time (ns)";
    out << '\n';
    for (const SendDataActionsSummaryRow& row : summary) {
        TimeNs spent = row.finish_time - row.start_time;
        SpeedGbps throughput = row.data_size / spent;
        out << row.data_id;
        out << ", " << join_ids(row.connection_ids);
        out << ", " << row.data_size;
        out << ", " << spent.value();
        out << ", " << throughput.value();
        out << ", " << row.start_time.value();
        out << ", " << row.finish_time.value();
        out << '\n';
    }
}
}  // namespace sim