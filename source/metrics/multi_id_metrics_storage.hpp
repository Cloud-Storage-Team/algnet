#pragma once
#include <spdlog/fmt/fmt.h>

#include <condition_variable>
#include <mutex>
#include <queue>
#include <regex>
#include <thread>

#include "metrics_storage.hpp"

namespace sim {
class MultiIdMetricsStorage {
public:
    MultiIdMetricsStorage(std::string a_metric_name);
    ~MultiIdMetricsStorage();

    void add_record(Id id, Time time, double value);
    void export_to_files(std::filesystem::path output_dir_path) const;

    std::unordered_map<Id, MetricsStorage> data() const;

    void set_filter(std::string filter);

private:
    std::string get_metrics_filename(Id id) const;
    void writer_cycle();

    std::string metric_name;

    std::mutex m_record_queue_mutex;    // Protecs m_record_queue
    std::condition_variable m_condvar;  // If m_record_queue is not empty

    std::vector<std::tuple<Id, Time, double> >
        m_record_queue;  // queue of metrics records

    mutable std::mutex m_storage_mutex;  // Protects m_storage

    // If m_storage does not contain some id, there was no check is metrics file
    // name for id correspond to m_filter
    // If m_storage[id] = std::nullopt, this check was failed
    // Otherwice, check was succseed
    std::unordered_map<Id, std::optional<MetricsStorage> > m_storage;
    std::regex m_filter;

    std::atomic<bool> m_needs_write;
    std::thread m_writer;
};

}  // namespace sim