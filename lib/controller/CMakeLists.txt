cmake_minimum_required(VERSION 3.17)

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

target_link_libraries(controller 
	model
)


set(CMAKE_CXX_STANDARD 14)

set_target_properties(model
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
