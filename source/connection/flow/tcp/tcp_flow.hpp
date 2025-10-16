#pragma once

#include <type_traits>

#include "connection/flow/i_flow.hpp"
#include "device/interfaces/i_host.hpp"
#include "packet.hpp"
#include "tcp_common.hpp"
#include "tcp_receiver.hpp"
#include "tcp_sender.hpp"
#include "utils/str_expected.hpp"

namespace sim {

class TcpFlow : public IFlow, public std::enable_shared_from_this<TcpFlow> {
public:
    TcpFlow(Id a_id, std::shared_ptr<IConnection> a_conn,
            std::unique_ptr<ITcpCC> a_cc, SizeByte a_packet_size,
            bool a_ecn_capable = true);
    void update(Packet packet) final;
    void send_data(SizeByte data) final;

    SizeByte get_sending_quota() const final;
    std::optional<TimeNs> get_last_rtt() const final;
    SizeByte get_delivered_data_size() const final;
    const BaseFlagManager& get_flag_manager() const final;
    // Returns time elapced from flow start (firsrt call of send_packet)
    // to last ack arrive
    std::optional<TimeNs> get_fct() const final;

    std::shared_ptr<IHost> get_sender() const final;
    std::shared_ptr<IHost> get_receiver() const final;

    Id get_id() const final;
    SizeByte get_delivered_bytes() const;
    std::string to_string() const;

private:
    TcpCommonPtr m_common;
    TcpSenderPtr m_sender;
    TcpReceiver m_receiver;
};

}  // namespace sim
