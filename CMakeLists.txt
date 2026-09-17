cmake_minimum_required(VERSION 3.20)

project(mandelbrot
    VERSION 0.1.0
    DESCRIPTION "Multithreaded Mandelbrot renderer"
    LANGUAGES CXX
)

# Default to Release so benchmarks aren't measuring debug-build overhead.
# Only applies to single-config generators (Make, Ninja); ignored by MSVC/Xcode.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for clangd / IDE tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_executable(mandelbrot
    src/main.cpp
)

target_link_libraries(mandelbrot PRIVATE Threads::Threads)

target_compile_options(mandelbrot PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Optional: -DENABLE_TSAN=ON to build with ThreadSanitizer (Phase 7).
# Incompatible with other sanitizers, and roughly 5-15x slower — never
# use a TSan build for timing measurements.
option(ENABLE_TSAN "Build with ThreadSanitizer" OFF)
if(ENABLE_TSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(mandelbrot PRIVATE -fsanitize=thread -g -O1)
        target_link_options(mandelbrot PRIVATE -fsanitize=thread)
    else()
        message(WARNING "ThreadSanitizer is not supported on this compiler")
    endif()
endif()