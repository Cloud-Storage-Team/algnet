#include <cstdint>
#include <map>
#include <memory>
#include <string>

// nanoseconds
typedef std::uint32_t Time;
// bytes
typedef std::uint32_t Size;
typedef std::string Id;

template <typename K, typename V>
using MapWeakPtr =
    std::map<std::weak_ptr<K>, V, std::owner_less<std::weak_ptr<K>>>;