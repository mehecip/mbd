cmake_minimum_required(VERSION 3.10)

project(mbd CXX)

add_subdirectory(lib/model)

# controller
option(BUILD_CONTROLLER "Build My Controller or make your own" ON)
if(BUILD_CONTROLLER)
  add_subdirectory(lib/controller)	
endif()

# view
option(BUILD_VIEW "Build View adapter" ON)
if(BUILD_VIEW)
   add_subdirectory(js_view_adapter)	
endif()

# examples
option(BUILD_EXAMPLES "Build Examples" ON)
if(BUILD_EXAMPLES)
	add_subdirectory(examples/math)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

