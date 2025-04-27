# Testing with Google Test and Google Mock

This directory contains tests for the Distiller project using Google Test (gtest) and Google Mock (gmock).

## Important Note on Testing Approach

The tests in this directory are designed to run on your development machine (desktop/laptop), not on the Arduino MKR WiFi 1010 hardware itself. This is a standard approach for embedded systems testing, as it allows for faster test execution and easier debugging.

### Desktop vs. Hardware Testing

- **Desktop Testing**: Tests run on your development machine using the native platform. This is what we've implemented here.
- **Hardware Testing**: Running tests directly on the Arduino hardware would be impractical due to memory constraints and lack of test output facilities.

## Test Setup

The project is configured to use Google Test and Google Mock for unit testing. The configuration is defined in the `platformio.ini` file:

```ini
[env:test]
platform = native
build_flags = 
	-std=c++23
	-DUNIT_TEST
build_unflags = -std=gnu++11
lib_deps =
	google/googletest@^1.12.1
test_framework = googletest
test_build_src = true
```

## Running Tests

To run the tests, you can use the following command with PlatformIO directly:

```bash
pio test -e test
```

Alternatively, and recommended for a consistent environment, you can use the provided Docker image:

```bash
docker run -v $(pwd):/project distiller-tools test
```

This will compile and run all tests in the `test` directory on your development machine, not on the Arduino hardware.

## Writing Tests

### Test Structure

Tests are organized using the Google Test framework. Each test file should:

1. Include the necessary headers:
   ```cpp
   #include <gtest/gtest.h>
   #include <gmock/gmock.h>
   ```

2. Define test fixtures (if needed) by inheriting from `::testing::Test`:
   ```cpp
   class MyTestFixture : public ::testing::Test {
   protected:
     void SetUp() override {
       // Setup code
     }
     
     void TearDown() override {
       // Cleanup code
     }
   };
   ```

3. Write test cases using the `TEST` or `TEST_F` macros:
   ```cpp
   TEST(TestSuiteName, TestName) {
     // Test code
   }
   
   TEST_F(MyTestFixture, TestName) {
     // Test code using the fixture
   }
   ```

4. Include a main function (unless using a shared main):
   ```cpp
   int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
   }
   ```

### Using Google Mock

Google Mock is used to create mock objects for dependencies. For example:

```cpp
class MockClass {
public:
  MOCK_METHOD(ReturnType, MethodName, (ParamType1, ParamType2), (Qualifiers));
};
```

Then in your tests:

```cpp
MockClass mock;
EXPECT_CALL(mock, MethodName(_, _))
  .WillOnce(::testing::Return(value));
```

## Testing Hardware-Dependent Code

Since the Distiller project interacts with hardware, we need to use mocks to test hardware-dependent code. This is particularly important because:

1. Google Mock can only mock virtual functions by default, while many Arduino library functions are non-virtual
2. Hardware-specific code cannot run in the native environment without proper mocking

Our approach is:

1. Create mock classes for hardware dependencies
2. Create testable versions of classes that accept mock dependencies
3. Test the behavior using the mock dependencies
4. Use conditional compilation to separate test code from production code

### Example of Conditional Compilation

```cpp
#ifdef UNIT_TEST
  // Include mock versions of hardware dependencies
  #include "mock_dependencies.h"
#else
  // Include real hardware dependencies
  #include <Arduino.h>
  #include <Wire.h>
#endif
```

### Example Files

We've provided example files to demonstrate how to adapt production classes for testing:

- `example_testable_class.h`: Shows how to modify a production class to support both testing and production environments using conditional compilation.
- `example_test.cpp`: Shows how to test a class that has been modified to support both testing and production environments.

These examples demonstrate:
1. How to use conditional compilation to switch between test and production code
2. How to inject mock dependencies in the test environment
3. How to write tests that verify behavior and interactions

### Adapting Production Classes for Testing

To adapt a production class for testing:

1. Add conditional compilation directives (`#ifdef UNIT_TEST`) to the class header
2. Create separate constructors for test and production environments
3. Use pointer or reference members for dependencies in the test environment
4. Use conditional compilation for method implementations that interact with hardware

Example:

```cpp
class MyClass {
private:
#ifdef UNIT_TEST
  // Test environment - use mock dependencies
  std::shared_ptr<MockDependency> dependency;
#else
  // Production environment - use real dependencies
  RealDependency dependency;
#endif

public:
#ifdef UNIT_TEST
  // Constructor for test environment
  MyClass(std::shared_ptr<MockDependency> dep) : dependency(dep) {}
#else
  // Constructor for production environment
  MyClass(int pin) : dependency(pin) {}
#endif

  void doSomething() {
#ifdef UNIT_TEST
    // Test environment implementation
    dependency->method();
#else
    // Production environment implementation
    dependency.method();
#endif
  }
};
```

See `test_thermometer.cpp`, `test_relay.cpp`, `test_heater_controller.cpp`, and `test_flow_controller.cpp` for examples of testing hardware-dependent classes.

### Limitations

Be aware of these limitations when testing hardware-dependent code:

1. **Non-Virtual Functions**: Google Mock can only mock virtual functions by default. For non-virtual functions, you may need to use wrapper classes or other techniques.
2. **Hardware-Specific Behavior**: Some hardware behavior is difficult to mock accurately. Focus on testing logic rather than hardware interactions.
3. **Timing-Dependent Code**: Code that relies on precise timing may behave differently in the native environment.

## C++23 Standard

The project is configured to use the C++23 standard for both production and test code. This allows using modern C++ features in tests.
