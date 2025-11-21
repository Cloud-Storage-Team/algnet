#pragma once
#include "connection/flow/i_flow_factory.hpp"
#include "connection/flow/tcp/i_tcp_cc_factory.hpp"
#include "connection/i_new_connection.hpp"
#include "connection/mplb/i_new_mplb.hpp"

namespace test {

class NewMPLBMock : public sim::INewMPLB {
public:
    ~NewMPLBMock() = default;
    NewMPLBMock(std::shared_ptr<sim::IHost> a_src,
                std::shared_ptr<sim::IHost> a_dest,
                std::unique_ptr<sim::INewFlowFactory> a_flow_factory,
                std::unique_ptr<sim::ITcpCCFactory> a_cc_factory, sim::MPLBType a_mplb_type);
    void on_ack(std::shared_ptr<const sim::INewFlow> flow,
                const sim::Packet& ack,
                std::vector<PacketNum> confirmed_packet_nums) final;
    SizeByte send_data(sim::Data data) final;
    void set_conn_confirm_data_cb(sim::ConnConfirmDataCallback cb) final;

private:
    sim::MPLBContext m_context;
    sim::ConnConfirmDataCallback m_confirm_data_cb;
    std::unique_ptr<sim::INewFlowFactory> m_flow_factory;
    std::unique_ptr<sim::ITcpCCFactory> m_cc_factory;
};

}  // namespace test
