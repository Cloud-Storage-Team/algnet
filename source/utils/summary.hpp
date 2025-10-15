#pragma once
#include <filesystem>
#include <unordered_set>

#include "connection/flow/i_flow.hpp"
#include "connection/i_connection.hpp"
#include "types.hpp"

namespace sim {

struct FlowSummary {
    SizeByte sent{0};
    SizeByte delivered{0};
    SpeedGbps sending_rate{0};
    SpeedGbps throughput{0};
    TimeNs fct{0};
    uint32_t retransmit_count{0};
};

// Summaryt of simulation
// Maps flow Ids to count of delivered bytes
class Summary {
public:
    Summary(std::map<Id, std::map<Id, FlowSummary>> values);
    Summary(
        const std::unordered_set<std::shared_ptr<IConnection>>& connections);

    void write_to_csv(std::filesystem::path output_path) const;

private:
    std::map<Id, std::map<Id, FlowSummary>> m_values;
};

}  // namespace sim
