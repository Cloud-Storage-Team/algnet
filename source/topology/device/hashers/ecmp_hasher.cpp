#include "ecmp_hasher.hpp"

namespace sim {

static inline std::size_t mix(std::size_t h) {
    // хороший недорогой финализатор
    h ^= h >> 30;
    h *= 0xbf58476d1ce4e5b9ULL;
    h ^= h >> 27;
    h *= 0x94d049bb133111ebULL;
    h ^= h >> 31;
    return h;
}

static inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= mix(value) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
}

std::uint32_t ECMPHasher::get_hash(const Packet& packet) {
    std::size_t hash = packet.sender_id.hash;
    hash_combine(hash, packet.receiver_id.hash);
    hash_combine(hash, packet.sender_port);
    hash_combine(hash, packet.receiver_port);
    return hash;
}

}  // namespace sim
