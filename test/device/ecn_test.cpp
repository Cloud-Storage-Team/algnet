#include "device/ecn.hpp"

#include <gtest/gtest.h>

namespace test {

class EcnTest : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(EcnTest, TestWrongParams) {
    bool invalid_argument_catched = false;
    try {
        volatile sim::ECN ecn(1.0, 0.0, 0.0);
    } catch (const std::invalid_argument& e) {
        invalid_argument_catched = true;
    }
    EXPECT_TRUE(invalid_argument_catched);
}

TEST_F(EcnTest, TestLowerMinThreshold) {
    const float MIN_THRESHOLD = 0.3;
    const float MAX_THRESHOLD = 0.5;
    sim::ECN ecn(MIN_THRESHOLD, MAX_THRESHOLD, 1.0);
    const size_t REPEAT_COUNT = 500;
    for (size_t i = 0; i < REPEAT_COUNT; i++) {
        EXPECT_FALSE(ecn.get_congestion_mark(MIN_THRESHOLD / 2));
    }
}

TEST_F(EcnTest, TestAtMinThreshold) {
    const float MIN_THRESHOLD = 0.3;
    const float MAX_THRESHOLD = 0.5;
    sim::ECN ecn(MIN_THRESHOLD, MAX_THRESHOLD, 1.0);
    const size_t REPEAT_COUNT = 500;
    for (size_t i = 0; i < REPEAT_COUNT; i++) {
        EXPECT_FALSE(ecn.get_congestion_mark(MIN_THRESHOLD));
    }
}

TEST_F(EcnTest, TestUpperMaxThreshold) {
    const float MIN_THRESHOLD = 0.3;
    const float MAX_THRESHOLD = 0.5;
    sim::ECN ecn(MIN_THRESHOLD, MAX_THRESHOLD, 1.0);
    const size_t REPEAT_COUNT = 500;
    for (size_t i = 0; i < REPEAT_COUNT; i++) {
        EXPECT_TRUE(
            ecn.get_congestion_mark(MAX_THRESHOLD + (1.0 - MAX_THRESHOLD) / 2));
    }
}

TEST_F(EcnTest, TestAtMaxThreshold) {
    const float MIN_THRESHOLD = 0.3;
    const float MAX_THRESHOLD = 0.5;
    sim::ECN ecn(MIN_THRESHOLD, MAX_THRESHOLD, 1.0);
    const size_t REPEAT_COUNT = 500;
    for (size_t i = 0; i < REPEAT_COUNT; i++) {
        EXPECT_TRUE(ecn.get_congestion_mark(MAX_THRESHOLD));
    }
}

TEST_F(EcnTest, TestBetweenMinAndMax) {
    const float MIN_THESHOLD = 0.23;
    const float MAX_THESHOLD = 0.57;
    const float PROBABILITY = 0.82;
    sim::ECN ecn(MIN_THESHOLD, MAX_THESHOLD, PROBABILITY);

    size_t mark_count = 0;
    const size_t REPEAT_COUNT = 500;
    const float QUERY_FILLING = (MIN_THESHOLD + MAX_THESHOLD) / 2.0;
    for (size_t i = 0; i < REPEAT_COUNT; i++) {
        mark_count += ecn.get_congestion_mark(QUERY_FILLING);
    }
    const float EXPECTED_MARK_PROBABILITY = PROBABILITY *
                                            (QUERY_FILLING - MIN_THESHOLD) /
                                            (MAX_THESHOLD - MIN_THESHOLD);
    float observed_mark_probability = mark_count / (float)REPEAT_COUNT;
    const float VARIANCE = 1 / 12.f; 
    ASSERT_NEAR(observed_mark_probability, EXPECTED_MARK_PROBABILITY,
                VARIANCE * 3);
}

}  // namespace test