cmake_minimum_required(VERSION 3.18)

project(
  CCUnits
  VERSION 1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(Conan)
run_conan()

# Make sure cmake picks up the conan packages instead of locally installed ones.
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

include(CompilerOptions)

add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(examples)