#pragma once
#include <functional>

#include "types.hpp"

namespace sim {

using EventCall = std::move_only_function<void()>;

struct NewEvent {
    TimeNs time;
    EventCall call;

    template <typename Func>
    requires std::constructible_from<EventCall, Func&&>
    NewEvent(TimeNs a_time, Func&& a_call)
        : time(a_time), call(std::forward<Func>(a_call)) {}

    bool operator>(const NewEvent& event) const { return time > event.time; }
};

}  // namespace sim