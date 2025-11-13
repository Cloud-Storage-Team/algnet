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
    template <typename U>
    StrExpected(std::unexpected<U> a_unexpected)
        : std::expected<T, std::string>(a_unexpected) {
        static_assert(std::is_constructible_v<std::expected<T, std::string>,
                                              std::unexpected<U> >);
    }

    StrExpected(T a_value)
        : std::expected<T, std::string>(std::move(a_value)) {}

    template <typename TErr = std::runtime_error>
    auto value_or_throw() const {
        static_assert(std::is_base_of_v<std::exception, TErr>,
                      "TErr must inherit std::exception");
        static_assert(std::is_constructible_v<TErr, const std::string&>,
                      "TErr should be constructable from rvalue std::string");
        if (!this->has_value()) {
            throw TErr(this->error());
        }
        return this->value();
    }

    auto apply_or(std::function<void(const T&)> apply_value,
                  std::function<void(const std::string&)> apply_error) {
        if (this->has_value()) {
            apply_value(this->value());
        } else {
            apply_error(this->error());
        }
    }

    template <typename Ret>
    Ret apply_or(std::function<Ret(const T&)> apply_value,
                 std::function<Ret(const std::string&)> apply_error) {
        if (this->has_value()) {
            return apply_value(this->value());
        } else {
            return apply_error(this->error());
        }
    }

    template <typename Ret>
    Ret apply_or_default(std::function<Ret(const T&)> apply_value,
                         Ret default_value) {
        if (this->has_value()) {
            return apply_value(this->value());
        } else {
            return default_value;
        }
    }

    bool apply_if_present(std::function<void(const T&)> apply_func) {
        bool result = this->has_value();
        if (result) {
            apply_func(this->value());
        }
        return result;
    }

    bool apply_if_not_present(std::function<void(const std::string&)> apply_func) {
        bool result = !this->has_value();
        if (result) {
            apply_func(this->error());
        }
        return result;
    }

    bool log_err_if_not_present(std::string msg = "error occurred") {
        bool result = !this->has_value();
        auto log_func = [](std::string err) { LOG_ERROR(std::move(err)); };

        if (result) {
            log_func("Message: " + msg + "; Error: " + this->error());
        }
        return result;
    }
};

template <>
class StrExpected<void> : public std::expected<void, std::string> {
public:
    // template for possible unexpected conversions
    template <typename U>
    StrExpected(std::unexpected<U> a_unexpected)
        : std::expected<void, std::string>(a_unexpected) {
        static_assert(std::is_constructible_v<std::expected<void, std::string>,
                                              std::unexpected<U>>);
    }

    StrExpected()
        : std::expected<void, std::string>(std::in_place) {}

    template <typename TErr = std::runtime_error>
    void value_or_throw() const {
        static_assert(std::is_base_of_v<std::exception, TErr>,
                      "TErr must inherit std::exception");
        static_assert(std::is_constructible_v<TErr, const std::string&>,
                      "TErr should be constructable from rvalue std::string");
        if (!this->has_value()) {
            throw TErr(this->error());
        }
        // No return for void
    }

    auto apply_or(std::function<void(std::monostate)> apply_value,
                  std::function<void(const std::string&)> apply_error) {
        if (this->has_value()) {
            apply_value(std::monostate{});
        } else {
            apply_error(this->error());
        }
    }

    template <typename Ret>
    Ret apply_or(std::function<Ret(std::monostate)> apply_value,
                 std::function<Ret(const std::string&)> apply_error) {
        if (this->has_value()) {
            return apply_value(std::monostate{});
        } else {
            return apply_error(this->error());
        }
    }

    template <typename Ret>
    Ret apply_or_default(std::function<Ret(std::monostate)> apply_value,
                         Ret default_value) {
        if (this->has_value()) {
            return apply_value(std::monostate{});
        } else {
            return default_value;
        }
    }

    bool apply_if_present(std::function<void(std::monostate)> apply_func) {
        bool result = this->has_value();
        if (result) {
            apply_func(std::monostate{});
        }
        return result;
    }

    bool apply_if_not_present(std::function<void(const std::string&)> apply_func) {
        bool result = !this->has_value();
        if (result) {
            apply_func(this->error());
        }
        return result;
    }

    bool log_err_if_not_present(std::string msg = "error occurred") {
        bool result = !this->has_value();
        auto log_func = [](std::string err) { LOG_ERROR(std::move(err)); };

        if (result) {
            log_func("Message: " + msg + "; Error: " + this->error());
        }
        return result;
    }
};

}  // namespace utils