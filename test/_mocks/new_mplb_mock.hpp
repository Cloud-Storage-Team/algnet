#pragma once
#include "connection/flow/i_flow_factory.hpp"
#include "connection/flow/tcp/i_tcp_cc_factory.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace test {

class NewMPLBMock : public sim::INewMPLB {
public:
    ~NewMPLBMock() = default;
    NewMPLBMock(std::shared_ptr<sim::IHost> a_src,
                std::shared_ptr<sim::IHost> a_dest,
                std::unique_ptr<sim::INewFlowFactory> flow_factory,
                std::unique_ptr<sim::ITcpCCFactory> cc_factory);
    void on_ack(std::shared_ptr<const sim::INewFlow> flow,
                const sim::Packet& ack,
                std::vector<PacketNum> confirmed_packet_nums) final;
    SizeByte send_data(sim::Data data) final;

private:
    sim::MPLBContext m_context;
    std::unique_ptr<sim::INewFlowFactory> m_flow_factory;
    std::unique_ptr<sim::ITcpCCFactory> m_cc_factory;
};

}  // namespace test
