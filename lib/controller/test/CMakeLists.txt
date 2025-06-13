cmake_minimum_required(VERSION 3.10)
project(controller_unit_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(../header)

add_executable(
    controller_unit_tests
    t_library.cpp
    t_controller.cpp
)
target_link_libraries(
    controller_unit_tests
    controller
    model
    GTest::gtest_main
)



include(GoogleTest)
gtest_discover_tests(controller_unit_tests)