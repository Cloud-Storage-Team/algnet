#pragma once
#include <random>

#include "i_new_action.hpp"
#include "network/connection/i_new_connection.hpp"
#include "scheduler/scheduler.hpp"
#include "types.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

class NewSendDataAction : public INewAction {
public:
    NewSendDataAction(TimeNs a_when, SizeByte a_size, RawDataId a_raw_data_id,
                      std::vector<std::shared_ptr<INewConnection>> a_conns,
                      size_t a_repeat_count, TimeNs a_repeat_interval,
                      TimeNs a_jitter);

    void schedule() final;

    const SendDataActionsSummary& get_summary() const;

private:
    TimeNs m_when;
    SizeByte m_size;
    RawDataId m_raw_data_id;
    std::vector<std::shared_ptr<INewConnection>> m_conns;
    size_t m_repeat_count;
    TimeNs m_repeat_interval;
    TimeNs m_jitter;
    std::shared_ptr<SendDataActionsSummary> m_summary;
};

}  // namespace sim
