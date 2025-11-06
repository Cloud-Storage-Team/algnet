#pragma once

#include <memory>

#include "mplb/i_new_mplb.hpp"
#include "types.hpp"

namespace sim {

struct ConnectionContext {
    SizeByte total_data_added = SizeByte(0);
    SizeByte total_data_confirmed = SizeByte(0);
    std::shared_ptr<INewMPLB> mplb;
};

// Logical path in network. Corresponds to application layer in TCP\IP stack
class INewConnection : public virtual Identifiable {
public:
    // Adds data_size bytes to send
    virtual void add_data_to_send(SizeByte data_size) = 0;

    // Confirm data_size delivery
    virtual void confirm(SizeByte data_size) = 0;
};

}  // namespace sim
