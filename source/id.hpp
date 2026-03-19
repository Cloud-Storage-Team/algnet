#pragma once
#include <string>

using Id = std::string;

struct IdWithHash {
    Id id;
    std::size_t hash;

    IdWithHash(const Id& a_id) : id(a_id), hash(std::hash<Id>{}(id)) {}

    auto operator<=>(const IdWithHash& id) const = default;
};

template <>
struct std::hash<IdWithHash> {
    std::size_t operator()(const IdWithHash& id) const noexcept {
        return id.hash;
    }
};
