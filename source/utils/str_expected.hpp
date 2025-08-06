#pragma once
#include <expected>
#include <stdexcept>
#include <string>

template <typename T>
using str_expected = std::expected<T, std::string>;

template <typename T>
static T value_or_rt(str_expected<T> opt) {
    if (!opt.has_value()) {
        throw std::runtime_error(opt.error());
    }
    return opt.value();
}