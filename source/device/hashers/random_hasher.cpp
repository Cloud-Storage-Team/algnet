#include "random_hasher.hpp"

namespace sim {

std::uint32_t RandomHasher::get_hash(const Packet&) {
    return static_cast<std::uint32_t>(std::rand());
}

}  // namespace sim