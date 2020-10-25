cmake_minimum_required(VERSION 3.17)

project(math_example CXX)

set(CPP_FILES
  src/main.cpp
)

set(HPP_FILES
  header/gain.hpp
  header/mathop.hpp
  header/sink.hpp
  header/source.hpp
  header/type_convertor.hpp
  header/unit_delay.hpp
)


add_executable( math_example 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories ( 
	header
	../../lib/model/header
)

target_link_libraries(math_example 
	model
)

if(BUILD_CONTROLLER)
	add_definitions(-DBUILD_CONTROLLER_EXAMPLES)
	
	include_directories ( 
		../../lib/controller/header
	)
	
	target_link_libraries(math_example 
		controller
	)
endif()



set(CMAKE_CXX_STANDARD 14)

set_target_properties(math_example
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
