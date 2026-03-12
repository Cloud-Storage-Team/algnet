#include "event.hpp"

namespace sim {

class CallAtTime : public Event {
public:
    CallAtTime(TimeNs when, std::function<void()> to_call);
    virtual void operator()() final;

private:
    std::function<void()> m_to_call;
};

}  // namespace sim
