#pragma once
#include <fstream>

#include "data.hpp"
#include "str_expected.hpp"

namespace sim {

struct ActionsSummaryRow {
    DataId data_id;
    SizeByte data_size;
    TimeNs start_time;
    TimeNs finish_time;
};

using ActionsSummary = std::vector<ActionsSummaryRow>;

void write_to_csv(std::ofstream& out, const ActionsSummary& summary);

}  // namespace sim