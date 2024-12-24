#include "Logger.hpp"

#include <iterator>

void Logger::Setup(const std::string& rtt_filename, const std::string& queue_size_filename) {
    rtt_stream.open(rtt_filename);
    queue_size_stream.open(queue_size_filename);

    if (!rtt_stream.is_open() || !queue_size_stream.is_open()) {
        throw std::runtime_error("Logger failed to create/open files.\n");
    }
}

Logger::~Logger() {
    rtt_stream.close();
    queue_size_stream.close();
}

void Logger::Log(Metric metric, std::uint64_t simulation_time_ns, std::uint64_t metric_value) {
    if (metric == Metric::RTT) {
        rtt_stream << simulation_time_ns << ' ' << metric_value << '\n';
    }
    else if (metric == Metric::QUEUE_SIZE) {
        queue_size_stream << simulation_time_ns << ' ' << metric_value << '\n';
    }
}