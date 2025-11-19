#pragma once

#include <memory>

#include "utils/identifier_factory.hpp"

#include "data.hpp"
#include "types.hpp"
#include <functional>

namespace sim {

class INewMPLB;

struct ConnectionContext {
    SizeByte total_data_added;
    SizeByte total_data_confirmed;
    std::shared_ptr<INewMPLB> mplb;
};

// Logical path in network. Corresponds to application layer in TCP\IP stack
class INewConnection : public virtual Identifiable {
public:
    // Callback type for data confirmation
    using ConnConfirmDataCallback = std::function<void(sim::DataId, SizeByte)>;

    // Add data to connection to be sent
    virtual void add_data_to_send(Data data) = 0;

    // Confirm data_size bytes delivery for data with given id
    virtual void confirm(DataId id, SizeByte data_size) = 0;
};

}  // namespace sim
