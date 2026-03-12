#pragma once

#include <map>
#include <memory>
#include <queue>

#include "i_new_connection.hpp"
#include "mplb/i_new_mplb.hpp"

namespace sim {

class Connection : public INewConnection,
                   public std::enable_shared_from_this<Connection> {
public:
    static std::shared_ptr<Connection> create_shared(
        Id a_id, std::shared_ptr<INewMPLB> a_mplb);

    [[nodiscard]] utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    ConnectionContext get_context() const final;

    virtual MetricsTable get_metrics_table() const final;

    virtual void write_inner_metrics(
        std::filesystem::path output_dir) const final;

    Id get_id() const final;

private:
    // to avoid creating on stack
    Connection(Id a_id, std::shared_ptr<INewMPLB> mplb);

    void send_new_portion();

    void process_data_delivery(DataId data_id, SizeByte delivery_size,
                               SizeByte total_delivered_on_send,
                               TimeNs send_time);

    struct DataContext {
        SizeByte total_size;
        SizeByte sent;
        SizeByte delivered;
        OnDeliveryCallback callback;
    };

    Id m_id;
    ConnectionContext m_context;
    std::shared_ptr<INewMPLB> m_mplb;
    std::map<DataId, DataContext> m_data_context_table;
    std::queue<DataId> m_sending_queue;
};
}  // namespace sim
