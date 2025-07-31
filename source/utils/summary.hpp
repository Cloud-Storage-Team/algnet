#pragma once
#include <filesystem>
#include <unordered_set>

#include "flow/i_flow.hpp"
#include "types.hpp"

namespace sim {

// Summaryt of simulation
// Maps flow Ids to count of delivered bytes
class Summary {
public:
    Summary(std::map<Id, SizeByte> a_values);
    Summary(std::unordered_set<std::shared_ptr<IFlow> > flows);

    void write_to_csv(std::filesystem::path output_path) const;

private:
    std::map<Id, SizeByte> m_values;
};

}  // namespace sim