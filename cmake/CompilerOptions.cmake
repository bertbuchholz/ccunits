add_library(compiler_options INTERFACE)

set(is_gnu "$<CXX_COMPILER_ID:GNU>")
set(is_clang "$<CXX_COMPILER_ID:CLANG>")
set(is_msvc "$<CXX_COMPILER_ID:MSCV>")
set(is_debug_without_msvc "$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Debug>>")
set(is_debug_with_msvc "$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>")

target_compile_options(compiler_options
    INTERFACE
        $<${is_gnu}:-Wall -Wextra -Wpedantic -Werror -Wnarrowing>
        $<${is_clang}:-Wall -Wextra -Wpedantic -Werror -Wnarrowing>
        $<${is_msvc}:/W4 /WX>
        $<$<CONFIG:Debug>:-fsanitize=address>
        ${compiler_options})

target_compile_definitions(compiler_options
    INTERFACE
        $<${is_debug_with_msvc}:_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION>
)

target_link_options(compiler_options
    INTERFACE
        $<${is_debug_without_msvc}:-fsanitize=address>
        ${linker_options})

target_compile_features(compiler_options
    INTERFACE
        cxx_std_17)
