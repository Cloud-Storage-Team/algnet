#pragma once
#include <filesystem>
#include <unordered_set>

#include "connection/flow/i_flow.hpp"
#include "connection/i_connection.hpp"
#include "types.hpp"

namespace sim {

using ErrorMessage = std::string;
using WarningMessage = std::string;

struct FlowSummary {
    SizeByte sent{0};
    SizeByte delivered{0};
    SizeByte packet_size{0};
    double overhead{0.0};
    SizeByte retransmit_size{0};
    uint32_t retransmit_count{0};
    SpeedGbps sending_rate{0};
    SpeedGbps throughput{0};
    TimeNs fct{0};
};

// Summaryt of simulation
// Maps flow Ids to count of delivered bytes
class Summary {
public:
    Summary(std::map<Id, std::map<Id, FlowSummary>> values);
    Summary(
        const std::unordered_set<std::shared_ptr<IConnection>>& connections);

    void write_to_csv(std::filesystem::path& output_path) const;
    void check() const;

private:
    std::map<Id, std::map<Id, FlowSummary>> m_values;
    std::vector<ErrorMessage> m_errors;
    std::vector<WarningMessage> m_warnings;
};

}  // namespace sim
