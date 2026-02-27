#pragma once

#include <memory>

#include "data.hpp"
#include "metrics/metrics_table/i_metricable.hpp"
#include "types.hpp"
#include "utils/identifier_factory.hpp"
#include "utils/str_expected.hpp"

namespace sim {

struct ConnectionContext {
    SizeByte total_data_added;
    SizeByte total_data_confirmed;
};

// Logical path in network. Corresponds to application layer in TCP\IP stack
class INewConnection : public virtual Identifiable, public virtual IMetricable {
public:
    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) = 0;

    // Confirm data_size bytes delivery for data with given id
    virtual ConnectionContext get_context() const = 0;
};

}  // namespace sim
