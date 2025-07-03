#include <ranges>

#include "multi_id_metrics_storage.hpp"

namespace sim {
MultiIdMetricsStorage::MultiIdMetricsStorage(std::string a_metric_name)
    : metric_name(std::move(a_metric_name)) {
    m_needs_write.store(true);
    m_writer = std::thread(&MultiIdMetricsStorage::writer_cycle, this);
}

MultiIdMetricsStorage::~MultiIdMetricsStorage() {
    m_needs_write.store(false);
    {
        std::lock_guard lock(m_record_queue_mutex);
        m_condvar.notify_all();
    }
    if (m_writer.joinable()) {
        m_writer.join();
    }
}

void MultiIdMetricsStorage::add_record(Id id, Time time, double value) {
    std::lock_guard lock(m_record_queue_mutex);
    m_record_queue.emplace(std::move(id), std::move(time), std::move(value));
    m_condvar.notify_all();
}

void MultiIdMetricsStorage::export_to_files(
    std::filesystem::path output_dir_path) const {
    std::lock_guard lock(m_storage_mutex);
    for (auto& [id, values] : m_storage) {
        if (values) {
            values->export_to_file(output_dir_path / get_metrics_filename(id));
        }
    }
}

std::unordered_map<Id, MetricsStorage> MultiIdMetricsStorage::data() const {
    std::unordered_map<Id, MetricsStorage> result;
    std::lock_guard lock(m_storage_mutex);
    for (auto [id, maybe_storage] : m_storage) {
        if (maybe_storage) {
            result[id] = maybe_storage.value();
        }
    }
    return result;
}

void MultiIdMetricsStorage::set_filter(std::string filter) {
    m_filter = std::regex(filter);
}

std::string MultiIdMetricsStorage::get_metrics_filename(Id id) const {
    return fmt::format("{}/{}.txt", metric_name, id);
}

void MultiIdMetricsStorage::writer_cycle() {
    Id id;
    Time time;
    double value;
    std::unordered_map<Id, std::optional<MetricsStorage> >::iterator it;
    while (m_needs_write) {
        {
            std::unique_lock lock(m_record_queue_mutex);
            m_condvar.wait(lock, [this]() {
                return !m_record_queue.empty() || !m_needs_write;
            });

            if (!m_needs_write) {
                break;
            }

            std::tie(id, time, value) = m_record_queue.front();
            m_record_queue.pop();
        }
        {
            std::lock_guard lock(m_storage_mutex);
            it = m_storage.find(id);
            if (it == m_storage.end()) {
                if (!std::regex_match(get_metrics_filename(id), m_filter)) {
                    m_storage[id] = std::nullopt;
                } else {
                    MetricsStorage new_storage;
                    new_storage.add_record(time, value);
                    m_storage.emplace(id, std::move(new_storage));
                }
            } else if (it->second.has_value()) {
                it->second->add_record(time, value);
            }
        }
    }
}

}  // namespace sim
