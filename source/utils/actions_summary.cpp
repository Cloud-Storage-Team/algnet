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
    bool any_repeat_num_present = false;
    for (auto el : summary) {
        if (el.data.id.repeat_num.has_value()) {
            any_repeat_num_present = true;
        }
    }

    std::string joined_connection_ids;
    out << "Data Id";
    if (any_repeat_num_present) {
        out << ", " << "Repeant number";
    }
    out << ", " << "Connection Ids";
    out << ", " << "Data Size (bytes)";
    out << ", " << "Time Spent (ns)";
    out << ", " << "Throughput (Gbps)";
    out << ", " << "Start time (ns)";
    out << ", " << "Finish time (ns)";
    out << '\n';
    for (const SendDataActionsSummaryRow& row : summary) {
        TimeNs spent = row.finish_time - row.start_time;
        SpeedGbps throughput = row.data.size / spent;
        out << row.data.id.raw_id;
        if (any_repeat_num_present && row.data.id.repeat_num.has_value()) {
            out << ", " << row.data.id.repeat_num.value();
        }
        if (row.connection_ids.size() < 5) {
            out << ", " << join_ids(row.connection_ids);
        } else {
            out << ", " << row.connection_ids.size()
                << " different connections";
        }
        out << ", " << row.data.size;
        out << ", " << spent.value();
        out << ", " << throughput.value();
        out << ", " << row.start_time.value();
        out << ", " << row.finish_time.value();
        out << '\n';
    }
}
}  // namespace sim