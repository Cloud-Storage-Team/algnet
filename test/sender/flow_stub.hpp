#pragma once
#include "flow.hpp"

namespace test {

class FlowStub : public sim::IFlow {
public:
    ~FlowStub() = default;
    FlowStub(std::shared_ptr<sim::ISender> a_sender, std::shared_ptr<sim::IReceiver> a_receiver);

    void start(std::uint32_t time) final;
    bool try_to_generate(std::uint32_t packet_size) final;

    void update() final;
    int get_cnt();
    std::shared_ptr<sim::ISender> get_source() const;
    std::shared_ptr<sim::IReceiver> get_destination() const final;

private:
    int cnt;
    std::shared_ptr<sim::IReceiver> m_receiver;
    std::shared_ptr<sim::ISender> m_sender;
};

}  // namespace test
