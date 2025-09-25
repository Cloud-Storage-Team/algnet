#pragma once

#include <cstdint>
#include <memory>
#include <set>

#include "connection/i_connection.hpp"
#include "flow/i_flow.hpp"
#include "packet.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class ConnectionImpl final
    : public IConnection,
      public std::enable_shared_from_this<ConnectionImpl> {
public:
    ConnectionImpl(Id a_id, std::shared_ptr<IHost> a_src,
                   std::shared_ptr<IHost> a_dest, std::shared_ptr<IMPLB> a_mplb,
                   SizeByte a_data_to_send = SizeByte(0));

    ~ConnectionImpl() override = default;

    Id get_id() const override;

    void start() override;

    void add_flow(std::shared_ptr<IFlow> flow) override;

    void delete_flow(std::shared_ptr<IFlow> flow) override;

    void add_data_to_send(SizeByte data) override;

    void update(const std::shared_ptr<IFlow>& flow) override;

    std::set<std::shared_ptr<IFlow>> get_flows() const override;

    void clear_flows() override;

    std::shared_ptr<IHost> get_sender() const override;

    std::shared_ptr<IHost> get_receiver() const override;

private:
    // Tries to send data using the MPLB-selected flow(s), as long as
    // allowed.
    void send_data();

    Id m_id;
    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;
    std::shared_ptr<IMPLB> m_mplb;
    SizeByte m_data_to_send;
    std::set<std::shared_ptr<IFlow>> m_flows;
};

}  // namespace sim
