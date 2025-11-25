#include "write_to_csv.hpp"

#include <cmath>
#include <limits>

namespace sim {

void write_to_csv(
    const std::vector<std::pair<MetricsStorage, std::string>>& storages,
    const std::filesystem::path output_path)
{
    const size_t count_storages = storages.size();
    if (count_storages == 0) {
        return;
    }

    // References to the underlying data vectors
    std::vector<const std::vector<std::pair<TimeNs, double>>*> series;
    series.reserve(count_storages);

    for (const auto& [storage, name] : storages) {
        const auto& recs = storage.get_records();
        // Under DEBUG, it may be validated that `recs` is sorted by timestamp
        series.push_back(&recs);
    }

    // Indices of the current element for each storage
    std::vector<size_t> idx(count_storages, 0);

    const double nan = std::numeric_limits<double>::quiet_NaN();
    // Most recent known values (forward-fill state)
    std::vector<double> values(count_storages, nan);

    utils::create_all_directories(output_path);
    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open output file: " +
                                 output_path.string());
    }

    fmt::memory_buffer buffer;
    auto it = std::back_inserter(buffer);

    // Header row
    fmt::format_to(it, "Time");
    for (const auto& p : storages) {
        fmt::format_to(it, ",{}", p.second);
    }
    fmt::format_to(it, "\n");

    // Main time-merge loop (k-way merge)
    while (true) {
        bool have_next_time = false;
        TimeNs next_time{};  // Smallest next timestamp among all storages

        // 1) Find the minimum unprocessed timestamp
        for (size_t i = 0; i < count_storages; ++i) {
            const auto& recs = *series[i];
            if (idx[i] >= recs.size()) {
                continue;
            }
            TimeNs t = recs[idx[i]].first;
            if (!have_next_time || t < next_time) {
                next_time = t;
                have_next_time = true;
            }
        }

        if (!have_next_time) {
            break; // All series are exhausted
        }

        // 2) Update forward-filled values for this timestamp
        for (size_t i = 0; i < count_storages; ++i) {
            const auto& recs = *series[i];
            if (idx[i] < recs.size() && recs[idx[i]].first == next_time) {
                values[i] = recs[idx[i]].second;
                ++idx[i];
            }
        }

        // 3) Append the row to the buffer
        fmt::format_to(it, "{}", next_time.value());
        for (size_t i = 0; i < count_storages; ++i) {
            fmt::format_to(it, ",{}", values[i]);
        }
        fmt::format_to(it, "\n");

        // 4) Optionally flush the buffer periodically to avoid memory growth
        //    (e.g., every 1–8 MB)
        if (buffer.size() > 1 << 20) { // 1 MB
            out.write(buffer.data(),
                      static_cast<std::streamsize>(buffer.size()));
            buffer.clear();
        }
    }

    // Final flush
    if (!buffer.size()) {
        out.write(buffer.data(),
                  static_cast<std::streamsize>(buffer.size()));
    }
}

}  // namespace sim