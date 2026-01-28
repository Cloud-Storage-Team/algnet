#pragma once

#include <memory>

#include "data.hpp"
#include "mplb/i_new_mplb.hpp"
#include "types.hpp"

namespace sim {

struct ConnectionContext {
    SizeByte total_data_added;
    SizeByte total_data_confirmed;
    std::shared_ptr<INewMPLB> mplb;
};

// Logical path in network. Corresponds to application layer in TCP\IP stack
class INewConnection : public virtual Identifiable {
public:
    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) = 0;

    // Confirm data_size bytes delivery for data with given id
    virtual ConnectionContext get_context() const = 0;
};

}  // namespace sim
