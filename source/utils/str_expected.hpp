#pragma once
#include <expected>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

#include "logger/logger.hpp"

namespace utils {

template <typename T>
class StrExpected : public std::expected<T, std::string> {
public:
    // TODO: think about this approach
    // static std::function<void(const T&)> log_func = [](const std::string& err) { LOG_ERROR(err); };

    // template for possible unexpected conversions (like std::unexpected<const
    // char*> to std::unexpected<std::string>)
    template <typename U>
    StrExpected(std::unexpected<U> a_unexpected)
        : std::expected<T, std::string>(a_unexpected) {
        static_assert(std::is_constructible_v<std::expected<T, std::string>,
                                              std::unexpected<U> >);
    }

    template <typename U = T, std::enable_if_t<!std::is_void_v<U>, int> = 0>
    StrExpected(U a_value)
        : std::expected<U, std::string>(std::move(a_value)) {}

    template <typename U = T, std::enable_if_t<std::is_void_v<U>, int> = 0>
    StrExpected(std::nullptr_t = nullptr)
        : std::expected<T, std::string>(std::in_place) {}

    template <typename TErr = std::runtime_error>
    auto value_or_throw() const {
        static_assert(std::is_base_of_v<std::exception, TErr>,
                      "TErr must inherit std::exception");
        static_assert(std::is_constructible_v<TErr, const std::string&>,
                      "TErr should be constructable from rvalue std::string");
        if (!this->has_value()) {
            throw TErr(this->error());
        }
        
        if constexpr (!std::is_void_v<T>) {
            return this->value();
        }
    }

    template <typename U = T>
    auto apply_or(std::function<void(std::conditional_t<std::is_void_v<U>, std::monostate, const U&>)> apply_value,
                  std::function<void(const std::string&)> apply_error) {
        if (this->has_value()) {
            if constexpr (std::is_void_v<U>) {
                apply_value(std::monostate{});
            } else {
                apply_value(this->value());
            }
        } else {
            apply_error(this->error());
        }
    }

    template <typename Ret, typename U = T>
    Ret apply_or(std::function<Ret(std::conditional_t<std::is_void_v<U>, std::monostate, const U&>)> apply_value,
                 std::function<Ret(const std::string&)> apply_error) {
        if (this->has_value()) {
            if constexpr (std::is_void_v<U>) {
                return apply_value(std::monostate{});
            } else {
                return apply_value(this->value());
            }
        } else {
            return apply_error(this->error());
        }
    }

    template <typename Ret, typename U = T>
    Ret apply_or_default(std::function<Ret(std::conditional_t<std::is_void_v<U>, std::monostate, const U&>)> apply_value,
                         Ret default_value) {
        if (this->has_value()) {
            if constexpr (std::is_void_v<U>) {
                return apply_value(std::monostate{});
            } else {
                return apply_value(this->value());
            }
        } else {
            return default_value;
        }
    }

    template <typename U = T>
    void apply_if_present(std::function<void(std::conditional_t<std::is_void_v<U>, std::monostate, const U&>)> apply_func) {
        if (this->has_value()) {
            if constexpr (std::is_void_v<U>) {
                apply_func(std::monostate{});
            } else {
                apply_func(this->value());
            }
        }
    }

    void apply_if_not_present(std::function<void(const std::string&)> apply_func) {
        if (!this->has_value()) {
            apply_func(this->error());
        }
    }

    void log_err_if_not_present(std::string msg = "error occurred") {
        auto log_func = [](std::string err) { LOG_ERROR(std::move(err)); };

        if (!this->has_value()) {
            log_func("Message: " + msg + "; Error: " + this->error());
        }
    }
};

}  // namespace utils