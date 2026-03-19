#pragma once
#include <stdexcept>
#include <type_traits>

#include "identifiable.hpp"

namespace utils {

template <typename T>
concept IdentifiableT = std::derived_from<T, sim::Identifiable>;

template <IdentifiableT T>
class IdTable : public std::unordered_map<Id, std::shared_ptr<T> > {
private:
    using TPtr = std::shared_ptr<T>;

public:
    using std::unordered_map<Id, std::shared_ptr<T> >::unordered_map;

    std::optional<TPtr> get(const Id& id) const noexcept {
        auto it = this->find(id);
        if (it == this->end()) {
            return std::nullopt;
        }
        return std::optional<TPtr>(it->second);
    }

    template <typename E>
    TPtr get_or_throw(const Id& id, E&& err) const
    requires std::derived_from<std::remove_cvref_t<E>, std::exception> &&
             std::constructible_from<std::remove_cvref_t<E>, E>
    {
        auto it = this->find(id);
        if (it == this->end()) {
            throw std::forward<E>(err);
        }
        return it->second;
    }
};

}  // namespace utils
