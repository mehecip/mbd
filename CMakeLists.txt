cmake_minimum_required(VERSION 3.6)

project(mbd CXX)

add_subdirectory(lib/model)

# controller
option(BUILD_CONTROLLER "Build My Controller or make your own" ON)
if(BUILD_CONTROLLER)
  add_subdirectory(lib/controller)	
endif()

# examples
option(BUILD_EXAMPLES "Build Examples" ON)
if(BUILD_EXAMPLES)
	add_subdirectory(examples/math)	
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

