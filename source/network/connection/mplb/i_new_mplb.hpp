#pragma once
#include <memory>
#include <optional>
#include <utility>

#include "metrics/metrics_table/i_metricable.hpp"
#include "network/connection/data.hpp"
#include "utils/str_expected.hpp"

namespace sim {

struct MPLBContext {
    SizeByte sent_data_size;
    SizeByte delivered_data_size;
    SizeByte sending_quota;  // maximal size of data that might be sent now
};

// Multipath module - transport layer interface of reliable data delivery along
// multiple physical paths.
// Owns multiple flows; balances the load between them
class INewMPLB : public virtual IMetricable {
public:
    // Forward data to transport layer
    // Returns error given data might not be sent (e.g. its size greater than
    // quota)
    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) = 0;

    virtual MPLBContext get_context() const = 0;
};

}  // namespace sim
