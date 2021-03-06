cmake_minimum_required(VERSION 3.6)

project(model CXX)

set(CPP_FILES
  src/msg_dispatcher.cpp
  src/model.cpp
  src/node.cpp
  src/port.cpp
)

set(HPP_FILES
  header/msg_dispatcher.hpp
  header/data_type.hpp
  header/model.hpp
  header/node.hpp
  header/port.hpp
  header/log_level.hpp
  header/connection_state.hpp
  header/enum_hash.hpp
)

add_library( model 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories(header)

target_link_libraries(model)


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
