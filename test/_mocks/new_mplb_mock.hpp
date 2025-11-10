#pragma once
#include "connection/mplb/i_new_mplb.hpp"

namespace test {

class NewMPLBMock : public sim::INewMPLB {
public:
    ~NewMPLBMock() = default;
    NewMPLBMock() = default;
    void on_ack(std::shared_ptr<const sim::INewFlow> flow,
                const sim::Packet& ack,
                std::vector<PacketNum> confirmed_packet_nums) final;
    SizeByte send_data(sim::Data data) final;
};

}  // namespace test
