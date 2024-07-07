find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(Tests
  ConversionTests.cpp
  MathTests.cpp
  OperatorTests.cpp
  RelationTests.cpp
  UserDefinitions.cpp)

target_link_libraries(Tests
  PRIVATE
    compiler_options
    UnitsLib
    GTest::gtest
    GTest::gtest_main)

gtest_discover_tests(Tests)