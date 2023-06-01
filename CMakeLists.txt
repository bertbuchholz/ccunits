cmake_minimum_required(VERSION 3.1...3.26)

project(
  CCUnits
  VERSION 1.0
  LANGUAGES CXX)

add_executable(Test main.cpp)

target_compile_features(Test PUBLIC cxx_std_17)