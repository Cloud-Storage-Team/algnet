#pragma once

#include <map>

#include "i_new_connection.hpp"

namespace sim {

class NewConnection : public INewConnection,
                      std::enable_shared_from_this<NewConnection> {
public:
    static std::shared_ptr<NewConnection> create(
        Id a_id, std::shared_ptr<INewMPLB> mplb);

    [[nodiscard]] utils::StrExpected<void> send_data(
        Data data, OnDeliveryCallback callback) final;

    const ConnectionContext& get_context() const final;

    Id get_id() const final;

private:
    // to avoid creating on stack
    NewConnection(Id a_id, std::shared_ptr<INewMPLB> mplb);

    void send_new_portion(DataId id);

    struct DataContext {
        SizeByte total_size;
        SizeByte sent;
        SizeByte delivered;
        OnDeliveryCallback callback;
    };

    Id m_id;
    ConnectionContext m_context;
    std::map<DataId, DataContext> m_data_context_table;
};
}  // namespace sim