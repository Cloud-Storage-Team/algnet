#include "random_hasher.hpp"

namespace sim {

std::uint32_t RandomHasher::get_hash(PacketPtr) {
    return static_cast<std::uint32_t>(std::rand());
}

}  // namespace sim