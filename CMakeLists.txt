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

# ---------------------------------------------------------------------------
# Warning flags, carried to every target via an interface library.
# INTERFACE targets have no sources — they only propagate properties.
# ---------------------------------------------------------------------------
add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic;-Wshadow;-Wconversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ---------------------------------------------------------------------------
# Optional sanitizers. Also propagated through an interface library so they
# reach every target, including the library itself.
#   -DENABLE_TSAN=ON   ThreadSanitizer (Phase 7)
#   -DENABLE_ASAN=ON   Address/UndefinedBehavior sanitizer (useful now)
# TSan and ASan are mutually incompatible. Both are slow — never measure
# performance with a sanitizer build.
# ---------------------------------------------------------------------------
option(ENABLE_TSAN "Build with ThreadSanitizer" OFF)
option(ENABLE_ASAN "Build with Address and UB sanitizers" OFF)

add_library(project_sanitizers INTERFACE)

if(ENABLE_TSAN AND ENABLE_ASAN)
    message(FATAL_ERROR "ENABLE_TSAN and ENABLE_ASAN cannot be used together")
endif()

if(ENABLE_TSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(project_sanitizers INTERFACE -fsanitize=thread -g -O1)
        target_link_options(project_sanitizers INTERFACE -fsanitize=thread)
    else()
        message(WARNING "ThreadSanitizer is not supported on this compiler")
    endif()
endif()

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(project_sanitizers INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer -g -O1)
        target_link_options(project_sanitizers INTERFACE -fsanitize=address,undefined)
    else()
        message(WARNING "Address sanitizer is not supported on this compiler")
    endif()
endif()

# ---------------------------------------------------------------------------
# Shared code: Pixel, Image, PPM writer
# ---------------------------------------------------------------------------
add_library(imagelib STATIC
    src/image.cpp
    src/ppm.cpp
)

target_include_directories(imagelib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(imagelib
    PRIVATE project_warnings
    PUBLIC  project_sanitizers
)

# ---------------------------------------------------------------------------
# Executables
# ---------------------------------------------------------------------------
add_executable(mandelbrot src/main.cpp)
target_link_libraries(mandelbrot PRIVATE imagelib project_warnings Threads::Threads)

add_executable(checkerboard tests/checkerboard.cpp)
target_link_libraries(checkerboard PRIVATE imagelib project_warnings)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
enable_testing()

add_test(NAME checkerboard COMMAND checkerboard)