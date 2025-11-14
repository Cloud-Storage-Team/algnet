#include "new_conn_mock.hpp"

namespace test {

NewConnectionMock::NewConnectionMock(
    Id a_id, [[maybe_unused]] std::shared_ptr<sim::IHost> a_src,
    [[maybe_unused]] std::shared_ptr<sim::IHost> a_dest,
    std::shared_ptr<sim::INewMPLB> a_mplb)
    : m_id(std::move(a_id)) {
    m_context.mplb = std::move(a_mplb);
}

Id NewConnectionMock::get_id() const { return m_id; }

void NewConnectionMock::add_data_to_send([[maybe_unused]] sim::Data data) {
    // Mock implementation
}

void NewConnectionMock::confirm([[maybe_unused]] sim::DataId id,
                                [[maybe_unused]] SizeByte data_size) {
    // Mock implementation
}

}  // namespace test
