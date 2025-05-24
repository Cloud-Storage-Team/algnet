#include <cstdint>

#include "packet.hpp"

namespace sim {

class IHasher {
public:
    virtual ~IHasher() = default;
    virtual std::uint32_t get_hash(Packet packet) const = 0;
};

class RandomHasher : public IHasher {
public:
    std::uint32_t get_hash(Packet) const override {
        return static_cast<std::uint32_t>(std::rand());
    }
};

class BaseHasher : public IHasher {
public:
    std::uint32_t get_hash(Packet packet) const override {
        std::string flow_id_str = ((packet.flow == nullptr) ? "0" : std::to_string(packet.flow->get_id()));

        std::hash<std::string> hasher;
        std::string header_str = flow_id_str + " " + std::to_string(packet.source_id) + " " + std::to_string(packet.dest_id);
        return static_cast<uint32_t>(hasher(header_str));
    }
};

class SymmetricHasher : public IHasher {
public:
    std::uint32_t get_hash(Packet packet) const override {
        std::string combined_id_str = std::to_string(packet.source_id ^ packet.dest_id);
        std::string flow_id_str = ((packet.flow == nullptr) ? "0" : std::to_string(packet.flow->get_id()));

        std::hash<std::string> hasher;
        std::string header_str = flow_id_str + " " + combined_id_str;
        return static_cast<uint32_t>(hasher(header_str));
    }
};

} // namespace sim
