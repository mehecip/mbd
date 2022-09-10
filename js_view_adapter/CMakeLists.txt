cmake_minimum_required(VERSION 3.6)

project(js_view_adapter CXX)

set(CPP_FILES
  src/js_view_adapter.cpp
  src/main.cpp
)

set(HPP_FILES
  header/js_view_adapter.hpp
)

# add_library(js_view_adapter 
# 	${HPP_FILES}
# 	${CPP_FILES}
# )

add_executable( js_view_adapter 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories(
header
../lib/model/header
../lib/controller/header
)

find_package(Threads)
find_package(cppzmq)
target_link_libraries(js_view_adapter 
	model
	cppzmq
	${CMAKE_THREAD_LIBS_INIT}
)



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "/O2 /Ob2 /EHsc")
else()
	set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
endif()

set_target_properties(js_view_adapter
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)