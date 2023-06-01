enable_testing()
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(Tests
  OperatorTests.cpp)

target_link_libraries(Tests
  PRIVATE
    UnitsLib
    GTest::gtest
    GTest::gtest_main)