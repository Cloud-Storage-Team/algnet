#pragma once
#include <filesystem>
#include <fstream>
#include <limits>
#include <utility>
#include <vector>

#include "metrics_storage.hpp"
#include "utils/filesystem.hpp"
namespace sim {

// by list of pairs (metrics storage, metric name) generates csv table and
// writes it to output_path
void write_to_csv(
    const std::vector<std::pair<MetricsStorage, std::string> >& storages,
    std::filesystem::path output_path);

}  // namespace sim