#include <gmock/gmock.h>

#include "network/connection/flow/i_flow.hpp"

namespace test {

struct FlowGMock : public sim::IFlow {
public:
    explicit FlowGMock(std::string name = "") : name(std::move(name)) {}

    MOCK_METHOD(void, send, (std::vector<sim::PacketInfo> packets), (override));
    MOCK_METHOD(const sim::FlowContext&, get_context, (), (const, override));
    MOCK_METHOD(Id, get_id, (), (const, override));
    MOCK_METHOD(sim::MetricsTable, get_metrics_table, (), (const, override));
    MOCK_METHOD(void, write_inner_metrics, (std::filesystem::path),
                (const, override));

    std::string name;
};

}  // namespace test
