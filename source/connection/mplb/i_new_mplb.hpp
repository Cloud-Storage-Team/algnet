#pragma once
#include <memory>
#include <optional>
#include <utility>

#include "connection/flow/i_new_flow.hpp"

namespace sim {

struct MPLBContext {
    std::set<std::shared_ptr<INewFlow> > flows;
    SizeByte sent_data_size;
    SizeByte delivered_data_size;
};

// Multipath module - transport layer interface of reliable data delivery along
// multiple physical pathes.
// Owns multiple flows; ballances the load between them
class INewMPLB {
public:
    // Callback that triggers when ack receives to sender of given flow
    virtual void on_ack(const std::shared_ptr<INewFlow>& flow,
                        const Packet& ack) = 0;

    // By given maximal_size returns real size of data that was forwarded to
    // transport layer now
    virtual SizeByte send_data(SizeByte maximal_size) = 0;
};

}  // namespace sim