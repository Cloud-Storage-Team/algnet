#pragma once
#include "../../flow/tcp/i_tcp_cc.hpp"
#include "../i_new_mplb.hpp"
#include "../path_chooser/i_path_chooser.hpp"

namespace sim {

class SingleCCMplb : public INewMPLB {
public:
    SingleCCMplb(std::unique_ptr<ITcpCC> a_cc,
                 std::unique_ptr<IPathChooser> a_path_chooser,
                 SizeByte a_packet_size = SizeByte(1500));

    [[nodiscard]] virtual utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    virtual MPLBContext get_context() const final;

private:
    SizeByte get_quota() const;

    std::unique_ptr<ITcpCC> m_cc;

    SizeByte m_sent_data_size;
    SizeByte m_delivered_data_size;
    std::size_t m_packets_in_flight;

    std::unique_ptr<IPathChooser> m_path_chooser;
    SizeByte m_packet_size;
};

}  // namespace sim
