#include "utils/identifier_factory.hpp"

#include <gtest/gtest.h>

#include <algorithm>
// #include <memory>

// #include "device/interfaces/i_routing_device.hpp"

namespace test {

class TestIdentifierFactory : public testing::Test {
public:
    void TearDown() override {}
    void SetUp() override {}
};

class Entity final : public sim::Identifiable {
public:
    Entity(Id a_id) : m_id(a_id) {}
    Id get_id() const final { return m_id; }

private:
    Id m_id;
};

TEST_F(TestIdentifierFactory, TestUniqueness) {
    constexpr std::uint32_t ENTITIES_NUMBER = 10;
    std::vector<Id> entitiy_ids(ENTITIES_NUMBER);
    {
        size_t index = 0;
        std::generate(entitiy_ids.begin(), entitiy_ids.end(),
                      [&index] { return "id_" + std::to_string(index++); });
    }

    for (Id id : entitiy_ids) {
        ASSERT_TRUE(
            sim::IdentifierFactory::get_instance().add_object<Entity>(id));
        ASSERT_EQ(id, sim::IdentifierFactory::get_instance()
                          .get_object<Entity>(id)
                          ->get_id());
    }
}

}  // namespace test
