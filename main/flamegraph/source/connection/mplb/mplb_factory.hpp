#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "i_mplb.hpp"
#include "round_robin_mplb.hpp"

namespace sim {

inline std::shared_ptr<IMPLB> make_mplb(const std::string& name) {
    if (name == "round_robin") {
        return std::make_shared<RoundRobinMPLB>();
    }
    // Add other MPLB algorithms here as needed
    throw std::runtime_error("Unknown MPLB algorithm: " + name);
}

}  // namespace sim
