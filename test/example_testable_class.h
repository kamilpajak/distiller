#ifndef EXAMPLE_TESTABLE_CLASS_H
#define EXAMPLE_TESTABLE_CLASS_H

// This is an example of how to modify a production class to support both
// testing and production environments using conditional compilation.

// Define UNIT_TEST if compiling for tests
#ifdef UNIT_TEST
// Test environment includes
#include <memory>
// Forward declarations for mock classes
class MockDependency1;
class MockDependency2;
#else
// Production environment includes
#include "real_dependency1.h"
#include "real_dependency2.h"

#include <Arduino.h>
#endif

/**
 * Example class that can be tested with Google Test and Google Mock.
 */
class ExampleClass {
private:
#ifdef UNIT_TEST
  // Test environment - use mock dependencies
  std::shared_ptr<MockDependency1> dependency1;
  std::shared_ptr<MockDependency2> dependency2;
#else
  // Production environment - use real dependencies
  RealDependency1 dependency1;
  RealDependency2 dependency2;
#endif
  int someState{0};

public:
#ifdef UNIT_TEST
  // Constructor for test environment
  ExampleClass(std::shared_ptr<MockDependency1> dep1, std::shared_ptr<MockDependency2> dep2)
    : dependency1(std::move(dep1)), dependency2(std::move(dep2)) {
    // Test-specific initialization
  }
#else
  // Constructor for production environment
  ExampleClass(int pin1, int pin2) : dependency1(pin1), dependency2(pin2) {
    // Production-specific initialization
    pinMode(pin1, OUTPUT);
    pinMode(pin2, INPUT);
  }
#endif

  // Methods that work in both environments
  void doSomething() {
#ifdef UNIT_TEST
    // Test environment implementation
    dependency1->method1();
    int result = dependency2->method2();
#else
    // Production environment implementation
    dependency1.method1();
    int result = dependency2.method2();
#endif
    someState = result;
  }

  [[nodiscard]] int getState() const { return someState; }
};

#endif // EXAMPLE_TESTABLE_CLASS_H
