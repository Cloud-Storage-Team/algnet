#include <gtest/gtest.h>

#include <vector>

#include "../source/device.hpp"
#include "../source/link.hpp"

namespace {

class DeviceMock : public sim::Device {
public:
    DeviceMock(sim::DeviceType a_type);
    ~DeviceMock() = default;

    void process() override;
    sim::Link* public_next_inlink();
};

DeviceMock::DeviceMock(sim::DeviceType a_type) : Device(a_type) {}

void DeviceMock::process() {}

sim::Link* DeviceMock::public_next_inlink() { return next_inlink(); }

class TestDevice : public testing::Test {
public:
    TestDevice() = default;
    ~TestDevice() = default;
};

TEST_F(TestDevice, test_next_inlink) {
    DeviceMock sender(sim::DeviceType::SENDER);
    DeviceMock reciever(sim::DeviceType::RECEIVER);

    ASSERT_EQ(sender.public_next_inlink(), nullptr);

    const size_t LINKS_COUNT = 6;
    std::vector<sim::Link> links(LINKS_COUNT,
                                 sim::Link(&sender, &reciever, 0, 0));

    for (auto& link : links) {
        reciever.add_inlink(&link);
    }

    const size_t REPEAT_COUNT = 2;
    for (size_t i = 0; i < LINKS_COUNT * REPEAT_COUNT; i++) {
        ASSERT_EQ(reciever.public_next_inlink(), &links[i % LINKS_COUNT]);
    }
}

}  // namespace
