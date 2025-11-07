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
};

// Multipath module - transport layer interface of reliable data delivery along
// multiple physical pathes.
// Owns multiple flows; ballances the load between them
class INewMPLB {
public:
    // Callback that triggers when ack receives to sender of given flow
    virtual void on_ack(std::shared_ptr<const INewFlow> flow, const Packet& ack,
                        std::vector<PacketNum> confinfed_packet_nums) = 0;

    // Forward to as much of data to transport layer
    // Returns size of transfered data
    virtual SizeByte send_data(Data data) = 0;
};

}  // namespace sim