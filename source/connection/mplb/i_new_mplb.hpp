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
    // Callback that is triggered when ack is received by sender of a given flow
    virtual void on_ack(std::shared_ptr<const INewFlow> flow, const Packet& ack,
                        std::vector<PacketNum> confinfed_packet_nums) = 0;

    // Forward data to transport layer as much as possible
    // Returns size of transfered data
    virtual SizeByte send_data(Data data) = 0;
};

}  // namespace sim
