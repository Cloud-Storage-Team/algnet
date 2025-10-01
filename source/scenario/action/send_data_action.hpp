#pragma once
#include "connection/i_connection.hpp"
#include "i_action.hpp"
#include "scheduler.hpp"
#include "types.hpp"

namespace sim {

class SendDataAction : public IAction {
public:
    SendDataAction(TimeNs a_when, SizeByte a_size,
                   std::vector<std::weak_ptr<IConnection>> a_conns,
                   int a_repeat_count, TimeNs a_repeat_interval);

    void schedule() final;

private:
    TimeNs m_when;
    SizeByte m_size;
    std::vector<std::weak_ptr<IConnection>> m_conns;
    size_t m_repeat_count;
    TimeNs m_repeat_interval;
};

}  // namespace sim
