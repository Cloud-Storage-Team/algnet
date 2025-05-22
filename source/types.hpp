#include <cstdint>

// nanoseconds
typedef std::uint32_t Time;
// bytes
typedef std::uint32_t Size;
typedef std::uint32_t Id;


template <typename K, typename V>
using MapWeakPtr =
    std::map<std::weak_ptr<K>, V, std::owner_less<std::weak_ptr<K>>>;