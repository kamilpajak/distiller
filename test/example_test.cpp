#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

// Define UNIT_TEST for conditional compilation
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Mock classes for dependencies
class MockDependency1 {
public:
  MOCK_METHOD(void, method1, (), ());
};

class MockDependency2 {
public:
  MOCK_METHOD(int, method2, (), ());
};

// Include the class to be tested
// This will use the UNIT_TEST path in the conditional compilation
#include "example_testable_class.h"

class ExampleClassTest : public ::testing::Test {
protected:
  std::shared_ptr<MockDependency1> dependency1;
  std::shared_ptr<MockDependency2> dependency2;
  std::unique_ptr<ExampleClass> exampleClass;

  void SetUp() override {
    dependency1 = std::make_shared<MockDependency1>();
    dependency2 = std::make_shared<MockDependency2>();
    exampleClass = std::make_unique<ExampleClass>(dependency1, dependency2);
  }
};

/**
 * @brief Test case for InitialStateIsZero.
 * 
 * Given a new ExampleClass object is created.
 * When the state is checked.
 * Then it should be zero.
 */
TEST_F(ExampleClassTest, InitialStateIsZero) {
  // Assert
  EXPECT_EQ(0, exampleClass->getState());
}

/**
 * @brief Test case for DoSomethingUpdatesState.
 * 
 * Given an ExampleClass object.
 * When doSomething is called.
 * Then the state should be updated based on the dependency's return value.
 */
TEST_F(ExampleClassTest, DoSomethingUpdatesState) {
  // Arrange
  EXPECT_CALL(*dependency1, method1())
    .Times(1);
  
  EXPECT_CALL(*dependency2, method2())
    .WillOnce(::testing::Return(42));
  
  // Act
  exampleClass->doSomething();
  
  // Assert
  EXPECT_EQ(42, exampleClass->getState());
}

/**
 * @brief Test case for DoSomethingCallsDependencies.
 * 
 * Given an ExampleClass object.
 * When doSomething is called.
 * Then the dependencies' methods should be called the expected number of times.
 */
TEST_F(ExampleClassTest, DoSomethingCallsDependencies) {
  // Arrange
  EXPECT_CALL(*dependency1, method1())
    .Times(1);
  
  EXPECT_CALL(*dependency2, method2())
    .Times(1)
    .WillOnce(::testing::Return(0));
  
  // Act
  exampleClass->doSomething();
}
