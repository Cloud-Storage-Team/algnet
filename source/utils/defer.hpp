#pragma once
#include <type_traits>

namespace utils {

// Go-style defer
// Call given function when Defer leaves the scope
template <typename F>
requires std::is_invocable_v<F&>
class [[nodiscard]] Defer {
public:
    explicit inline Defer(F a_func) : m_func(std::move(a_func)) {}

    ~Defer() noexcept {
        if constexpr (std::is_nothrow_invocable_v<F&>) {
            std::invoke(m_func);
        } else {
            try {
                std::invoke(m_func);
            } catch (...) {
                std::terminate();
            }
        }
    }

private:
    F m_func;
};

}  // namespace utils