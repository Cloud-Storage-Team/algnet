#include "Logger.hpp"

void Logger::Setup(const std::string& rtt_filename, const std::string& queue_size_filename) {
    m_rtt_stream.open(rtt_filename);
    m_queue_size_stream.open(queue_size_filename);

    if (!m_rtt_stream.is_open() || !m_queue_size_stream.is_open()) {
        throw std::runtime_error("Logger failed to create/open files.\n");
    }
}

Logger::~Logger() {
    m_rtt_stream.close();
    m_queue_size_stream.close();
}

void Logger::Log(Metric metric, std::uint64_t simulation_time_ns, std::uint64_t metric_value) {
    if (metric == Metric::RTT) {
        m_rtt_stream << simulation_time_ns << ' ' << metric_value << '\n';
    }
    else if (metric == Metric::QUEUE_SIZE) {
        m_queue_size_stream << simulation_time_ns << ' ' << metric_value << '\n';
    }
}
