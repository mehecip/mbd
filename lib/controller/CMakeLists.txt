cmake_minimum_required(VERSION 3.6)

project(controller CXX)

set(CPP_FILES
  src/controller.cpp
)

set(HPP_FILES
  header/controller.hpp
)

add_library( controller 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories(
header
../../lib/model/header
)

find_package(Threads)
target_link_libraries(controller 
	model
	${CMAKE_THREAD_LIBS_INIT}
)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-std=c++14 -O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "/O2 /Ob2 /EHsc")
endif()

set_target_properties(model
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
