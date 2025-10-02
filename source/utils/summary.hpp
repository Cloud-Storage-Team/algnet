#pragma once
#include <filesystem>
#include <unordered_set>

#include "connection/flow/i_flow.hpp"
#include "connection/i_connection.hpp"
#include "types.hpp"

namespace sim {

// Summaryt of simulation
// Maps flow Ids to count of delivered bytes
class Summary {
public:
    Summary(std::map<Id, std::map<Id, SpeedGbps>> values);
    Summary(
        const std::unordered_set<std::shared_ptr<IConnection>>& connections);

    void write_to_csv(std::filesystem::path output_path) const;

private:
    std::map<Id, std::map<Id, SpeedGbps>> m_values;
};

}  // namespace sim