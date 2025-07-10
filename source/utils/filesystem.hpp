#pragma once
#include <matplot/matplot.h>
#include <spdlog/fmt/fmt.h>

#include <filesystem>

namespace utils {

// Creates all directories on the path to given file
void inline create_all_directories(std::filesystem::path file) {
    std::filesystem::path dir_path = file.parent_path();
    if (std::filesystem::exists(dir_path)) {
        return;
    }
    if (!std::filesystem::create_directories(dir_path) ||
        !std::filesystem::exists(dir_path)) {
        throw std::runtime_error(
            fmt::format("Can not create {} directory", dir_path.string()));
    }
}

}
