#include "loop_iterator.hpp"

#include "../logger.hpp"

namespace sim {

void LOG::CRITICAL(std::string_view msg) { LOG_CRITICAL(msg); }

void LOG::ERROR(std::string_view msg) { LOG_ERROR(msg); }

}  // namespace sim
