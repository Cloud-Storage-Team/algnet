#pragma once
#include <memory>
#include <optional>
#include <utility>

#include "connection/flow/i_new_flow.hpp"
#include "data.hpp"

namespace sim {

struct MPLBContext {
    std::set<std::shared_ptr<INewFlow> > flows;
    SizeByte sent_data_size;
    SizeByte delivered_data_size;
    SizeByte sending_quota;  // maximal size of data that might be sent now
};

// Multipath module - transport layer interface of reliable data delivery along
// multiple physical paths.
// Owns multiple flows; balances the load between them
class INewMPLB {
public:
    // Forward data to transport layer
    // Returns error given data might not be sent (e.g. its size greater than
    // quota)
    virtual utils::StrExpected<void> send_data(Data data,
                                               OnDeliveryCallback callback) = 0;

    virtual const MPLBContext& get_context() const = 0;
};

}  // namespace sim
