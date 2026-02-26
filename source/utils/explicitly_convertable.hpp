#pragma once
#include <concepts>

namespace utils {

template <typename T, typename U>
concept ExplicitlyConvertable =
    requires(T t) { static_cast<U>(t); } && std::constructible_from<T, U>;

}  // namespace utils