#include <cstdint>
#include <map>
#include <memory>
#include <string>

#include "units/time.hpp"
#include "units/units.hpp"

using TimeNs = Time<Nanosecond>;
using SizeByte = Size<Byte>;
using SpeedGbps = Speed<GBit, Second>;
using Id = std::string;

// Describes a type used by packet's bitset to store flags
using PacketFlagsBase = std::uint32_t;

template <typename K, typename V>
using MapWeakPtr =
    std::map<std::weak_ptr<K>, V, std::owner_less<std::weak_ptr<K>>>;