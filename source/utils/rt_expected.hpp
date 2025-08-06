#pragma once
#include <expected>
#include <stdexcept>

template <typename T>
class rt_expected : public std::expected<T, std::runtime_error> {};