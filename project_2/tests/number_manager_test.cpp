#include "number_manager.hpp"
#include <gtest/gtest.h>

class NumberManagerTest : public ::testing::Test {
protected:
    NumberManager* numberManager;

    void SetUp() override {
        numberManager = new NumberManager(5); 
    }

    void TearDown() override {
        delete numberManager;
    }
};

TEST_F(NumberManagerTest, GiveNumber) {
    EXPECT_EQ(numberManager->giveNumber(), 0);
    EXPECT_EQ(numberManager->giveNumber(), 1); 
    EXPECT_EQ(numberManager->giveNumber(), 2);
}

TEST_F(NumberManagerTest, UsageCountIncrements) {
    numberManager->giveNumber(); 

    numberManager->returnNumber(0);
    
    EXPECT_EQ(numberManager->giveNumber(), 0);
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}