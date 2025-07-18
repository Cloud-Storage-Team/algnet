#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "units.hpp"

namespace sim {

// nanoseconds
using Time = std::uint32_t;
// bytes
using Size = TemplateSize<Byte>;
using Id = std::string;

// Describes a type used by packet's bitset to store flags
using PacketFlagsBase = std::uint32_t;

template <typename K, typename V>
using MapWeakPtr =
    std::map<std::weak_ptr<K>, V, std::owner_less<std::weak_ptr<K>>>;

}  // namespace sim