add_library(compiler_options INTERFACE)

target_compile_options(compiler_options
    INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror -Wnarrowing>
        $<$<CXX_COMPILER_ID:CLANG>:-Wall -Wextra -Wpedantic -Werror -Wnarrowing>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        ${compiler_options})

target_link_options(compiler_options
    INTERFACE
        ${linker_options})

target_compile_features(compiler_options
    INTERFACE
        cxx_std_17)
