cmake_minimum_required(VERSION 3.6)
project(controller_unit_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(../header)

add_executable(
    controller_unit_tests
    t_library.cpp
)
target_link_libraries(
    controller_unit_tests
    controller
    model
    GTest::gtest_main
)



include(GoogleTest)
gtest_discover_tests(controller_unit_tests)