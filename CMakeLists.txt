cmake_minimum_required(VERSION 3.17)

project(mbd CXX)

add_subdirectory(lib/model)

# controller
option(BUILD_CONTROLLER "Build CONTROLLER" OFF)
if(BUILD_CONTROLLER)
  message("Controller not implemented yet")
endif()

# examples
option(BUILD_EXAMPLES "Build Examples" ON)
if(BUILD_EXAMPLES)

	add_subdirectory(examples/math)	
	
	if(BUILD_CONTROLLER)
		message("No Examples for controller yet")
	endif()
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

