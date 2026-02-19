#pragma once
#include "identifier_factory.hpp"

namespace utils {

template <typename T>
concept IdentifiableT = std::derived_from<T, sim::Identifiable>;

template <IdentifiableT T>
using IdTable = std::unordered_map<Id, std::shared_ptr<T> >;

}  // namespace utils
