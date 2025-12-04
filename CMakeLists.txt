cmake_minimum_required(VERSION 3.18)
project(highperf_parquet LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimization flags
if(MSVC)
    add_compile_options(/O2 /arch:AVX2)
else()
    add_compile_options(-O3 -march=native -Wall -Wextra)
endif()

# CUDA support
include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_ARCHITECTURES "native")
    add_compile_definitions(HPQ_USE_CUDA)
    message(STATUS "CUDA found. GPU acceleration enabled.")
else()
    message(STATUS "CUDA not found. GPU acceleration disabled.")
endif()

# Python bindings
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# pybind11 - assuming it's installed or we fetch it. For now, let's try find_package
find_package(pybind11 CONFIG)
if(NOT pybind11_FOUND)
    # Fallback to FetchContent if not found
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.12.0
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Include directories
include_directories(include)

# Main library
add_library(hpq_core SHARED
    src/writer/writer.cc
    src/schema/schema.cc
    src/encodings/encoding_base.cc
    src/encodings/rle_simd.cc
    src/encodings/bitpack_simd.cc
    src/encodings/delta_simd.cc
    src/encodings/dict_encoding.cc
    src/encodings/adaptive.cc
    src/io/file_writer.cc
    src/io/buffer.cc
    src/format/parquet_metadata.cc
    src/format/parquet_layout.cc
    src/format/bloom_filter.cc
)

if(CMAKE_CUDA_COMPILER)
    target_sources(hpq_core PRIVATE
        src/gpu/gpu_compress.cu
        src/gpu/gpu_utils.cc
    )
    set_target_properties(hpq_core PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
else()
    target_sources(hpq_core PRIVATE
        src/gpu/gpu_compress_fallback.cc
    )
endif()

# Python extension
pybind11_add_module(hpq_py python/hpq/bindings.cpp)
target_link_libraries(hpq_py PRIVATE hpq_core)

# Benchmarks
add_executable(benchmark_writer benchmarks/benchmark_writer.cc)
target_link_libraries(benchmark_writer PRIVATE hpq_core)

# Tests
enable_testing()
add_subdirectory(tests/cpp)
