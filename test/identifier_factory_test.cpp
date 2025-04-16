#include "utils/identifier_factory.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <memory>

#include "device/device.hpp"

namespace test {

class TestIdentifierFactory : public testing::Test {
public:
    void TearDown() override {}
    void SetUp() override {}
};

class Entity final : public sim::Identifiable {
public:
    Entity() : m_id(sim::IdentifierFactory::get_instance().generate_id()) {}
    Id get_id() const final { return m_id; }

private:
    Id m_id;
};

TEST_F(TestIdentifierFactory, TestUniqueness) {
    constexpr std::uint32_t ENTITIES_NUMBER = 10;
    std::vector<Entity> entities(ENTITIES_NUMBER);

    std::generate(entities.begin(), entities.end(),
                  [] { return Entity(); });

    std::set<Id> ids;
    std::transform(entities.begin(), entities.end(),
                   std::inserter(ids, ids.begin()),
                   [](auto& e) { return e.get_id(); });

    ASSERT_EQ(ids.size(), ENTITIES_NUMBER);

    Entity e{};
    ASSERT_TRUE(!ids.contains(e.get_id()));
}

}  // namespace test
