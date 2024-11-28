#include "number_manager.hpp"
#include <gtest/gtest.h>
#include <set>

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
    std::set<int> numbers;

    for(int i = 0; i<100; ++i){
       ASSERT_EQ(numberManager->getNumber(),i%5);
    }
    
}

TEST_F(NumberManagerTest, SameGetNumberAfterReturn) {
   int number = numberManager->getNumber(); 
   numberManager->returnNumber(number);
   ASSERT_EQ(number, numberManager->getNumber());
}

//TODO more tests



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}