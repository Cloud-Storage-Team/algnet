#pragma once
#include "connection/flow/i_flow_factory.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace test {

class NewMPLBMock : public sim::INewMPLB {
public:
    ~NewMPLBMock() = default;
    NewMPLBMock(std::shared_ptr<sim::INewFlowFactory> flow_factory);
    void on_ack(std::shared_ptr<const sim::INewFlow> flow,
                const sim::Packet& ack,
                std::vector<PacketNum> confirmed_packet_nums) final;
    SizeByte send_data(sim::Data data) final;

private:
    sim::MPLBContext m_context;
    std::shared_ptr<sim::INewFlowFactory> m_flow_factory;
};

}  // namespace test
