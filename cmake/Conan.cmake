macro(run_conan)
# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
       "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(GENERATORS cmake_find_package_multi)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
  foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
    message("Conan config for ${TYPE}")
    conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})
    conan_cmake_install(
      PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}
      BUILD missing
      # OUTPUT_QUIET
      REMOTE conancenter
      SETTINGS ${settings})
  endforeach()
else()
  conan_cmake_autodetect(settings)
  conan_cmake_install(
    PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}
    BUILD missing
    # OUTPUT_QUIET
    REMOTE conancenter
    SETTINGS ${settings})
endif()

endmacro()
