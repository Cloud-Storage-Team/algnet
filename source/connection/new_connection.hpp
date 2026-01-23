#pragma once

#include <map>
#include <memory>
#include <queue>
#include <memory>

#include "i_new_connection.hpp"

namespace sim {

class NewConnection : public INewConnection,
                      public std::enable_shared_from_this<NewConnection> {
public:
    static std::shared_ptr<NewConnection> create(
        Id a_id, std::shared_ptr<INewMPLB> a_mplb);

    [[nodiscard]] utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    ConnectionContext get_context() const final;

    Id get_id() const final;

private:
    // to avoid creating on stack
    NewConnection(Id a_id, std::shared_ptr<INewMPLB> mplb);

    void send_new_portion();

    struct DataContext {
        SizeByte total_size;
        SizeByte sent;
        SizeByte delivered;
        OnDeliveryCallback callback;
    };

    Id m_id;
    ConnectionContext m_context;
    std::map<DataId, DataContext> m_data_context_table;
    std::queue<DataId> m_sending_queue;
};
}  // namespace sim
