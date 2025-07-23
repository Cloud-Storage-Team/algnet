#include "write_to_csv.hpp"

#include <cmath>
#include <limits>

#include "utils/filesystem.hpp"
namespace sim {

// by list of pairs (metrics storage, metric name) generates csv table and
// writes it to output_path
void write_to_csv(
    const std::vector<std::pair<MetricsStorage, std::string> >& storages,
    std::filesystem::path output_path) {
    size_t count_storages = storages.size();
    // values[time][i] is a value of metric for i-th storage at time time;
    // If there were no measurement at time, values[time][i] =
    // std::numeric_limits<double>::quiet_NaN()
    std::map<TimeNs, std::vector<double> > values;
    double nan = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> default_values(count_storages, nan);
    for (size_t i = 0; i < count_storages; i++) {
        for (const auto& [time, value] : storages[i].first.get_records()) {
            if (values.find(time) == values.end()) {
                values[time] = default_values;
            }
            values[time][i] = value;
        }
    }

    std::vector<double> previous_time_row = default_values;
    // push values by time using increasing order of keys (time) in std::map
    for (auto& [time, time_values] : values) {
        for (size_t i = 0; i < count_storages; i++) {
            if (std::isnan(time_values[i])) {
                time_values[i] = previous_time_row[i];
            } else {
                previous_time_row[i] = time_values[i];
            }
        }
    }

    utils::create_all_directories(output_path);
    std::ofstream out(output_path);
    out << "Time";
    for (size_t i = 0; i < count_storages; i++) {
        out << ',' << storages[i].second;
    }
    out << '\n';
    for (const auto& [time, time_values] : values) {
        out << time;
        for (auto value : time_values) {
            out << ',' << value;
        }
        out << '\n';
    }
}

}  // namespace sim