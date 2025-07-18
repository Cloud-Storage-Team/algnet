#include <cstdint>
#include <map>
#include <memory>
#include <string>

// nanoseconds
using Time = std::uint32_t;
// bytes
using Size = std::uint32_t;
using Id = std::string;

// Describes a type used by packet's bitset to store flags
using PacketFlagsBase = std::uint32_t;

template <typename K, typename V>
using MapWeakPtr =
    std::map<std::weak_ptr<K>, V, std::owner_less<std::weak_ptr<K>>>;