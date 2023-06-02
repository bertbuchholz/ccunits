add_library(UnitsLib INTERFACE)

target_include_directories(UnitsLib INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib>
  $<INSTALL_INTERFACE:include/units>)

target_compile_features(UnitsLib INTERFACE cxx_std_17)