#pragma once

#include <vector>
#include <utility>
#include <cstdint>
#include <string>
#include <fstream>

enum class Metric : std::uint8_t {
    RTT,
    QUEUE_SIZE
};

class Logger {
public:
    Logger() = default;
    ~Logger();

    void Setup(const std::string& rtt_filename, const std::string& queue_size_filename);
    void Log(Metric metric, std::uint64_t simulation_time_ns, std::uint64_t metric_value);
private:
    std::ofstream rtt_stream;
    std::ofstream queue_size_stream;

    std::string rtt_file = "";
    std::string queue_size_file = "";
};