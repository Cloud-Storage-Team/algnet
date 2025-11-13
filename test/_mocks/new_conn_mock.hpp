#pragma once
#include "connection/i_new_connection.hpp"

namespace test {

class NewConnectionMock : public sim::INewConnection {
public:
    ~NewConnectionMock() = default;
    NewConnectionMock(Id a_id, std::shared_ptr<sim::IHost> a_src,
                      std::shared_ptr<sim::IHost> a_dest,
                      std::shared_ptr<sim::INewMPLB> a_mplb);
    void add_data_to_send(sim::Data data) final;
    void confirm(sim::DataId id, SizeByte data_size) final;

private:
    Id m_id;
    sim::ConnectionContext m_context;
};

}  // namespace test
