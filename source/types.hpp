#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "units/time.hpp"
#include "units/units.hpp"
#include "data.hpp"

using TimeNs = Time<Nanosecond>;
using SizeByte = Size<Byte>;
using SpeedGbps = Speed<GBit, Second>;
using Id = std::string;

using PacketNum = std::uint32_t;
using TTL = std::uint32_t;

// Describes a type used by packet's bitset to store flags
using PacketFlagsBase = std::uint64_t;

template <typename K, typename V>
using MapWeakPtr =
    std::map<std::weak_ptr<K>, V, std::owner_less<std::weak_ptr<K>>>;
