#pragma once
#include "filesystem.hpp"

namespace matplot {

// Saves matplot figure to the given file
// If file does not exist, creates it and all directoriees on the path to it
void inline safe_save(matplot::figure_handle fix, std::filesystem::path path) {
    utils::create_all_directories(path);
    save(fix, path.string());
}
};  // namespace matplot
