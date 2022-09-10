cmake_minimum_required(VERSION 3.6)

project(model CXX)

set(CPP_FILES
  src/msg_dispatcher.cpp
  src/model.cpp
  src/node.cpp
  src/port.cpp
  src/param.cpp
  src/data_holder.cpp
  src/uuid.cpp
  src/connection.cpp
)

set(HPP_FILES
  header/msg_dispatcher.hpp
  header/data_type.hpp
  header/model.hpp
  header/node.hpp
  header/port.hpp
  header/param.hpp
  header/data_holder.hpp
  header/log_level.hpp
  header/connection.hpp
  header/connection_state.hpp
  header/enum_hash.hpp
  header/uuid.hpp
)

add_library( model 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories(header)

target_link_libraries(model)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "/std:c++17 /O2 /Ob2 /EHsc")
	else()
		set(CMAKE_CXX_FLAGS "-std=c++17 -g3 -O0 -Wall -Wextra")
	endif()
else()
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "/std:c++17 /O2 /Ob2 /EHsc")
	else()
		set(CMAKE_CXX_FLAGS "-std=c++17 -O3 -Wall -Wextra")
	endif()
endif()

set_target_properties(model
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
