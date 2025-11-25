#include "write_to_csv.hpp"

#include <cmath>
#include <limits>

namespace sim {

void write_to_csv(
    const std::vector<std::pair<MetricsStorage, std::string>>& storages,
    std::filesystem::path output_path)
{
    const size_t count_storages = storages.size();

    std::map<TimeNs, std::vector<double>> values;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const std::vector<double> default_values(count_storages, nan);

    for (size_t i = 0; i < count_storages; ++i) {
        for (const auto& [time, value] : storages[i].first.get_records()) {
            auto it = values.find(time);
            if (it == values.end()) {
                it = values.emplace(time, default_values).first;
            }
            it->second[i] = value;
        }
    }

    std::vector<double> previous_time_row = default_values;
    for (auto& [time, time_values] : values) {
        for (size_t i = 0; i < count_storages; ++i) {
            if (std::isnan(time_values[i])) {
                time_values[i] = previous_time_row[i];
            } else {
                previous_time_row[i] = time_values[i];
            }
        }
    }

    utils::create_all_directories(output_path);
    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open output file: " +
                                 output_path.string());
    }

    fmt::memory_buffer buffer;
    auto it = std::back_inserter(buffer);

    fmt::format_to(it, "Time");
    for (const auto& p : storages) {
        fmt::format_to(it, ",{}", p.second);
    }
    fmt::format_to(it, "\n");

    for (const auto& [time, time_values] : values) {
        fmt::format_to(it, "{}", time.value()); 
        for (double v : time_values) {
            fmt::format_to(it, ",{}", v);
        }
        fmt::format_to(it, "\n");
    }

    out.write(buffer.data(),
              static_cast<std::streamsize>(buffer.size()));
}

}  // namespace sim