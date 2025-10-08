#pragma once
#include <expected>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "errors/base_error.hpp"

namespace utils {

template <typename T>
using str_expected = std::expected<T, std::string>;

template <typename T, typename TErr = BaseError>
T value_or_base_error(str_expected<T> opt) {
    static_assert(std::is_base_of_v<std::exception, TErr>,
                  "TErr must inhetir std::exception");
    static_assert(std::is_constructible_v<TErr, std::string&&>,
                  "TErr should be constructable from rvalue std::string");
    if (!opt.has_value()) {
        throw TErr(opt.error());
    }
    return opt.value();
}

}  // namespace utils