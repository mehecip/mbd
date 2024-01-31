cmake_minimum_required(VERSION 3.6)
project(model_unit_tests)

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
    model_unit_tests
    t_node.cpp
    t_model.cpp
    t_connection.cpp
)
target_link_libraries(
    model_unit_tests
    model
    GTest::gtest_main
)



include(GoogleTest)
gtest_discover_tests(model_unit_tests)