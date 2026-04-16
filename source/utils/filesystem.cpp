#include "filesystem.hpp"

namespace utils {

bool wipe_folder(std::filesystem::path dir_path) noexcept {
    try {
        std::filesystem::remove_all(dir_path);
    } catch (const std::runtime_error& err) {
        LOG_ERROR(fmt::format("Could not wipe folder '{}' due to error: '{}'",
                              dir_path.string(), err.what()));
        return false;
    }
    return true;
}

}  // namespace utils
