#pragma once
#include "types.hpp"

namespace utils {
class
CallbackObserver {
public:
    CallbackObserver(std::size_t a_count, OnDeliveryCallback a_callback);

    void on_single_callback();

private:
    std::size_t m_count;
    std::size_t m_callbacks_triggered;
    OnDeliveryCallback m_callback;
};
}  // namespace utils
