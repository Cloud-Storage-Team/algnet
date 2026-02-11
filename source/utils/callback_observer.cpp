#include "callback_observer.hpp"

namespace utils {
CallbackObserver::CallbackObserver(std::size_t a_count,
                                   OnDeliveryCallback a_callback)
    : m_count(a_count), m_callbacks_triggered(0), m_callback(a_callback) {}

void CallbackObserver::on_single_callback() {
    if (++m_callbacks_triggered == m_count) {
        m_callback();
    }
}

}  // namespace utils
