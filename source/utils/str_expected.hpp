#pragma once
#include <expected>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "errors/base_error.hpp"

namespace utils {

template <typename T>
using str_expected = std::expected<T, std::string>;

template <typename T>
typename std::enable_if<std::is_copy_constructible<T>::value, T>::type
value_or_base_error(const str_expected<T>& opt) {
    if (!opt.has_value()) {
        throw BaseError(opt.error());
    }
    return opt.value();
}

template <typename T>
typename std::enable_if<!std::is_copy_constructible<T>::value, T>::type
value_or_base_error(str_expected<T>&& opt) {
    if (!opt.has_value()) {
        throw BaseError(opt.error());
    }
    return std::move(opt.value());
}

}  // namespace utils
