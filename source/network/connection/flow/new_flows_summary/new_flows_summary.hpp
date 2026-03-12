#pragma once
#include "../i_new_flow.hpp"
#include "utils/id_table.hpp"
#include <filesystem>

namespace sim {

class NewFlowsSummary {
public:
    explicit NewFlowsSummary(const utils::IdTable<INewFlow>& flows_table);

    void write_to_csv(std::filesystem::path output_path) const;

private:
    std::unordered_map<Id, FlowContext> m_flows_contexts;
};

}  // namespace sim
