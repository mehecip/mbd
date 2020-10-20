cmake_minimum_required(VERSION 3.17)

project(model CXX)

set(CPP_FILES
  src/component.cpp
  src/node.cpp
  src/port.cpp
)

set(HPP_FILES
  header/data_type.hpp
  header/component.hpp
  header/node.hpp
  header/port.hpp
  header/log_level.hpp
  header/connection_state.hpp
)

add_library( model 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories ( header )

target_link_libraries(model)


set(CMAKE_CXX_STANDARD 14)

set_target_properties(model
  PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
