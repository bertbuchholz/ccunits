conan install . --output-folder=../build-ccunits
cmake -S . -B ../build-ccunits -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake build ../build-ccunits