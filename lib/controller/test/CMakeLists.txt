cmake_minimum_required(VERSION 3.10)
project(controller_unit_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "/std:c++17 /O2 /Ob2 /EHsc")
	else()
		set(CMAKE_CXX_FLAGS "-std=c++17 -g3 -O0 -Wall -Wextra")
	endif()
else()
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "/std:c++17 /O2 /Ob2 /EHsc")
	else()
		set(CMAKE_CXX_FLAGS "-std=c++17 -O3 -Wall -Wextra")
	endif()
endif()

set(BENCHMARK_ENABLE_TESTING off)
include(FetchContent)

FetchContent_Declare(
  googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.17.0
)

FetchContent_Declare(
  googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
)

FetchContent_MakeAvailable(
  googletest
  googlebenchmark
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

include_directories(../header)

add_executable(
    controller_unit_tests
    t_library.cpp
    t_controller.cpp
    controller_helper.cpp
    controller_helper_gtest.cpp
)

target_link_libraries(
    controller_unit_tests
    controller
    model
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(controller_unit_tests)

# beenchmark executable
add_executable(
    controller_benchmark
    t_performance.cpp
    controller_helper.cpp
)

target_link_libraries(
  controller_benchmark
  model
  controller
    benchmark::benchmark
    benchmark::benchmark_main
)
