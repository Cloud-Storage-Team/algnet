#pragma once

#include <memory>
#include <string>
#include <stdexcept>

#include "connection/mplb_policy.hpp"
#include "connection/round_robin_policy.hpp"
// #include "connection/rtt_aware_policy.hpp"  // когда появится

namespace sim {

inline std::shared_ptr<MPLBPolicy> make_mplb_policy(const std::string& name) {
    if (name == "round_robin") {
        return std::make_shared<RoundRobinPolicy>();
    }
    // else if (name == "rtt_aware") {
    //     return std::make_shared<RttAwarePolicy>();
    // }
    throw std::runtime_error("Unknown MPLB algorithm: " + name);
}

}  // namespace sim
