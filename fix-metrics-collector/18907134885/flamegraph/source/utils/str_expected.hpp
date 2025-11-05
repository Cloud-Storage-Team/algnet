#pragma once
#include <expected>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace utils {

template <typename T>
class StrExpected : public std::expected<T, std::string> {
public:
    // template for possible unexpected conversions (like std::unexpected<const
    // char*> to std::unexpected<std::string>)
    template <typename U>
    StrExpected(std::unexpected<U> a_unexpected)
        : std::expected<T, std::string>(a_unexpected) {
        static_assert(std::is_constructible_v<std::expected<T, std::string>,
                                              std::unexpected<U> >);
    }

    StrExpected(T a_value)
        : std::expected<T, std::string>(std::move(a_value)) {}

    template <typename TErr = std::runtime_error>
    T value_or_throw() const {
        static_assert(std::is_base_of_v<std::exception, TErr>,
                      "TErr must inherit std::exception");
        static_assert(std::is_constructible_v<TErr, const std::string&>,
                      "TErr should be constructable from rvalue std::string");
        if (!this->has_value()) {
            throw TErr(this->error());
        }
        return this->value();
    }

    void apply_or(std::function<void(const T&)> apply_value,
                  std::function<void(const std::string&)> apply_error) {
        if (this->has_value()) {
            apply_value(this->value());
        } else {
            apply_error(this->error());
        }
    }

    template <typename U>
    U apply_or(std::function<U(const T&)> apply_value,
               std::function<U(const std::string&)> apply_error) {
        if (this->has_value()) {
            return apply_value(this->value());
        } else {
            return apply_error(this->error());
        }
    }

    template <typename U>
    U apply_or_default(std::function<U(const T&)> apply_value,
                       U default_value) {
        if (this->has_value()) {
            return apply_value(this->value());
        } else {
            return default_value;
        }
    }

    void apply_if_present(std::function<void(const T&)> apply_func) {
        if (this->has_value()) {
            apply_func(this->value());
        }
    }
};

}  // namespace utils