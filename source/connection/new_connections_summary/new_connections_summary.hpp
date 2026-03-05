#pragma once
#include <filesystem>

#include "../i_new_connection.hpp"
#include "utils/id_table.hpp"

namespace sim {

class NewConnectionsSummary {
public:
    explicit NewConnectionsSummary(
        const utils::IdTable<INewConnection>& connections_table);

    void write_to_csv(std::filesystem::path output_path) const;

private:
    std::map<Id, ConnectionContext> m_contexts_table;
};

}  // namespace sim
