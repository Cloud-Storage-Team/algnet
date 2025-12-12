#pragma once
#include <random>

#include "connection/i_connection.hpp"
#include "i_action.hpp"
#include "scheduler.hpp"
#include "types.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

class SendDataAction : public IAction {
public:
    SendDataAction(TimeNs a_when, SizeByte a_size, RawDataId a_raw_data_id,
                   std::vector<std::weak_ptr<IConnection>> a_conns,
                   int a_repeat_count, TimeNs a_repeat_interval,
                   TimeNs a_jitter,
                   std::shared_ptr<SendDataActionsSummary> a_summary);

    void schedule() final;

private:
    class CallbackObserver {
    public:
        CallbackObserver(std::size_t a_count, OnDeliveryCallback a_callback);

        void on_single_callback();

    private:
        std::size_t m_count;
        std::size_t m_callbacks_triggered;
        OnDeliveryCallback m_callback;
    };

    TimeNs m_when;
    SizeByte m_size;
    RawDataId m_raw_data_id;
    std::vector<std::weak_ptr<IConnection>> m_conns;
    size_t m_repeat_count;
    TimeNs m_repeat_interval;
    TimeNs m_jitter;
    std::shared_ptr<SendDataActionsSummary> m_summary;
};

}  // namespace sim
