#pragma once
#include "connection/flow/i_new_flow.hpp"
#include "connection/mplb/i_new_mplb.hpp"
#include "packet.hpp"

namespace test {

class NewFlowMock : public sim::INewFlow {
public:
    ~NewFlowMock() = default;
    NewFlowMock(Id a_id, std::shared_ptr<sim::IHost> a_sender,
                std::shared_ptr<sim::IHost> a_receiver,
                sim::INewMPLB& a_mplb);
    Id get_id() const final;
    void on_packet(const sim::Packet& packet) final;
    void send(std::vector<sim::Packet> packets) final;
    const sim::FlowContext& get_context() final;

private:
    Id m_id;
    sim::FlowContext m_context;
    sim::INewMPLB& m_mplb;
};

}  // namespace test
