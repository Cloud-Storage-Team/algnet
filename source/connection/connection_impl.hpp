#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <set>
#include <utility>

#include "connection/flow/i_flow.hpp"
#include "connection/i_connection.hpp"
#include "packet.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class ConnectionImpl final
    : public IConnection,
      public std::enable_shared_from_this<ConnectionImpl> {
public:
    ConnectionImpl(Id a_id, std::shared_ptr<IHost> a_src,
                   std::shared_ptr<IHost> a_dest,
                   std::shared_ptr<IMPLB> a_mplb);

    ~ConnectionImpl() final = default;

    Id get_id() const final;

    void add_flow(std::shared_ptr<IFlow> flow) final;

    void delete_flow(std::shared_ptr<IFlow> flow) final;

    utils::StrExpected<void> add_data_to_send(
        Data data, OnDeliveryCallback callback) final;

    SizeByte get_total_data_added() const final;

    void update(const std::shared_ptr<IFlow>& flow) final;

    std::set<std::shared_ptr<IFlow>> get_flows() const final;

    void clear_flows() final;

    std::shared_ptr<IHost> get_sender() const final;

    std::shared_ptr<IHost> get_receiver() const final;

private:
    // Tries to send data using the MPLB-selected flow(s), as long as
    // allowed.
    void send_data();

    Id m_id;
    std::weak_ptr<IHost> m_src;
    std::weak_ptr<IHost> m_dest;
    std::shared_ptr<IMPLB> m_mplb;

    struct DataContext {
        SizeByte total_size;
        SizeByte sent;
        SizeByte delivered;
        OnDeliveryCallback callback;
    };

    // stores delivered size & callback for every data id
    std::unordered_map<DataId, DataContext> m_data_context_table;
    std::queue<DataId> m_sending_queue;
    SizeByte m_total_data_added;
    std::set<std::shared_ptr<IFlow>> m_flows;
};

}  // namespace sim
