#include "write_to_csv.hpp"

#include <cmath>
#include <limits>
#include <span>

namespace sim {

void write_to_csv(
    const std::vector<std::pair<MetricsStorage, std::string>>& storages,
    const std::filesystem::path output_path) {
    static constexpr std::size_t FLUSH_THRESHOLD = (1 << 20);  // 1 MB
    const size_t count_storages = storages.size();

    using Sample = std::pair<TimeNs, double>;
    using SeriesSpan = std::span<const Sample>;

    // References to the underlying data vectors
    std::vector<SeriesSpan> series;
    series.reserve(count_storages);

    for (const auto& [storage, name] : storages) {
        const std::vector<Sample>& recs = storage.get_records();
        series.emplace_back(recs.data(), recs.size());
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

    auto get_next_time = [&]() -> std::optional<TimeNs> {
        std::optional<TimeNs> result;
        for (size_t i = 0; i < count_storages; ++i) {
            const auto& recs = series[i];
            if (idx[i] < recs.size()) {
                TimeNs t = recs[idx[i]].first;
                if (!result || t < result.value()) {
                    result = t;
                }
            }
        }
        return result;
    };

    auto flush_buffer = [&]() {
        out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        buffer.clear();
    };

    // Main time-merge loop (k-way merge)
    for (auto next_time_opt = get_next_time(); next_time_opt.has_value();
         next_time_opt = get_next_time()) {
        TimeNs next_time = next_time_opt.value();
        // 1) Update forward-filled values for all storages that have a record
        // at next_time
        for (size_t i = 0; i < count_storages; ++i) {
            const auto& recs = series[i];
            if (idx[i] < recs.size() && recs[idx[i]].first == next_time) {
                values[i] = recs[idx[i]].second;
                ++idx[i];
            }
        }
        // 2) Append the row to the buffer
        fmt::format_to(it, "{}", next_time.value());
        for (double v : values) {
            fmt::format_to(it, ",{}", v);
        }
        fmt::format_to(it, "\n");

        // 3) Optionally flush the buffer periodically to avoid memory growth
        // (e.g., every 1–8 MB)
        if (buffer.size() > FLUSH_THRESHOLD) {
            flush_buffer();
        }
    }

    // Final flush
    if (buffer.size()) {
        flush_buffer();
    }
}

}  // namespace sim
