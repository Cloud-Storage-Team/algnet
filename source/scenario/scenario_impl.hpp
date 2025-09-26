#pragma once

#include <memory>
#include <vector>

#include "event/add_data_to_connection.hpp"
#include "scenario/i_scenario.hpp"
#include "types.hpp"

namespace sim {

class IConnection;

class ScenarioImpl final : public IScenario {
public:
    struct SendDataConnBatch {
        TimeNs when = TimeNs(0);      // starting time
        SizeByte size = SizeByte(0);  // amount of data to send
        uint32_t num = 1;             // number of repetitions
        TimeNs gap = TimeNs(0);       // the interval between repetitions
        std::vector<std::weak_ptr<IConnection>> conns;  // target connections
    };

    ScenarioImpl() = default;
    ~ScenarioImpl() override = default;

    void add_send_data_conn_batch(SendDataConnBatch batch);

    // Moves actions to the Scheduler
    void start() override;

private:
    std::vector<SendDataConnBatch> m_send_batches;
};

}  // namespace sim
