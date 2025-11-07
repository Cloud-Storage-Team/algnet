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
    virtual void add_data_to_send(Data data) = 0;

    // Confirm data_size bytes delivery for data with given id
    virtual void confirm(DataId id, SizeByte data_size) = 0;
};

}  // namespace sim
