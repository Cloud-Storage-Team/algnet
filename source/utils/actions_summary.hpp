#pragma once
#include <fstream>

#include "data.hpp"
#include "str_expected.hpp"

namespace sim {

struct SendDataActionsSummaryRow {
    DataId data_id;
    SizeByte data_size;
    TimeNs start_time;
    TimeNs finish_time;
};

using SendDataActionsSummary = std::vector<SendDataActionsSummaryRow>;

void write_to_csv(const std::string& output_path,
                  const SendDataActionsSummary& summary);

}  // namespace sim