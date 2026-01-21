#pragma once
#include "../../flow/tcp/i_tcp_cc.hpp"
#include "../i_new_mplb.hpp"

namespace sim {

class SingleCCMplb : public INewMPLB,
                     public std::enable_shared_from_this<SingleCCMplb> {
public:
    std::shared_ptr<SingleCCMplb> create(
        std::unique_ptr<ITcpCC> a_cc,
        std::set<std::shared_ptr<INewFlow> > a_flows);

    virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    virtual const MPLBContext& get_context() const final;

private:
    SingleCCMplb(std::unique_ptr<ITcpCC> a_cc,
                 std::set<std::shared_ptr<INewFlow> > a_flows);

    std::unique_ptr<ITcpCC> m_cc;
    MPLBContext m_context;
};

}  // namespace sim
