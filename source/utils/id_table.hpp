#pragma once
#include "identifier_factory.hpp"

namespace utils {

template <typename T>
concept IdentifiableT = std::derived_from<T, sim::Identifiable>;

template <IdentifiableT T>
class IdTable : public std::unordered_map<Id, std::shared_ptr<T> > {
private:
    using TPtr = std::shared_ptr<T>;

public:
    std::optional<TPtr> get(const Id& id) const noexcept {
        auto it = this->find(id);
        if (it == this->end()) {
            return std::nullopt;
        }
        return std::optional<TPtr>(it->second);
    }

    TPtr get_or_throw(const Id& id, const std::exception& err) const {
        auto it = this->find(id);
        if (it == this->end()) {
            throw err;
        }
        return it->second;
    }
};

}  // namespace utils
