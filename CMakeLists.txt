cmake_minimum_required(VERSION 3.16)

project(smart_ptr
    VERSION 1.0
    DESCRIPTION "High-performance C++ smart pointer library"
    LANGUAGES CXX
)

# -------------------------------
# C++ Standard
# -------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------
# Options
# -------------------------------
option(ENABLE_TESTS "Build tests" ON)
option(ENABLE_BENCHMARKS "Build benchmarks" ON)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# -------------------------------
# Compiler Flags (Serious setup)
# -------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wold-style-cast
        -Wnull-dereference
        -Wdouble-promotion
    )
endif()

# -------------------------------
# Sanitizers
# -------------------------------
if (ENABLE_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

if (ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

# -------------------------------
# Library Target (Header-only for now)
# -------------------------------
add_library(smart_ptr INTERFACE)

target_include_directories(smart_ptr
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(smart_ptr INTERFACE cxx_std_20)

# -------------------------------
# Tests
# -------------------------------
if (ENABLE_TESTS)
    enable_testing()

    add_executable(test_unique_ptr
        tests/test_unique_ptr.cpp
    )

    add_executable(test_shared_ptr
        tests/test_shared_ptr.cpp
    )

    target_link_libraries(test_unique_ptr PRIVATE smart_ptr)

    add_test(NAME unique_ptr_tests COMMAND test_unique_ptr)
endif()

# -------------------------------
# Benchmarks
# -------------------------------
if (ENABLE_BENCHMARKS)
    add_executable(bench_unique_ptr
        benchmarks/bench_unique_ptr.cpp
    )

    target_link_libraries(bench_unique_ptr PRIVATE smart_ptr)

    add_executable(bench_shared_ptr
        benchmarks/bench_shared_ptr.cpp
    )

    target_link_libraries(bench_shared_ptr PRIVATE smart_ptr)
endif()