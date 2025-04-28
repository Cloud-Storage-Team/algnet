#include "event.hpp"
#include "express_pass/express_pass_flow.hpp"

namespace sim {

class SendCredit : public Event {
public:
    SendCredit(Time a_time, std::weak_ptr<ExpressPassFlow> a_flow, Size a_packet_size);
    virtual ~SendCredit() = default;
    void operator()() final;

private:
    std::weak_ptr<ExpressPassFlow> m_flow;
    Size m_packet_size;
};
 
class RunFeedbackControlLoop : public Event {
public:
    RunFeedbackControlLoop(Time a_time, std::weak_ptr<ExpressPassFlow> a_flow);
    virtual ~RunFeedbackControlLoop() = default;
    void operator()() final;

private:
    std::weak_ptr<IFlow> m_flow;
};

} // namespace sim
