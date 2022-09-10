cmake_minimum_required(VERSION 3.6)

project(controller CXX)

set(CPP_FILES
  src/controller.cpp
  src/library.cpp
  src/graph.cpp
  src/dfs.cpp
)

set(HPP_FILES
  header/controller.hpp
  header/iview_adapter.hpp
  header/library.hpp
  header/graph.hpp
  header/dfs.hpp
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


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "/std:c++17 /O2 /Ob2 /EHsc")
	else()
		set(CMAKE_CXX_FLAGS "-std=c++17 -g3 -O0")
	endif()
else()
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "/std:c++17 /O2 /Ob2 /EHsc")
	else()
		set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
	endif()
endif()

set_target_properties(model
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
