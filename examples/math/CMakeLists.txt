cmake_minimum_required(VERSION 3.6)

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
	../../js_view_adapter/header
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

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

if (BUILD_VIEW)

	add_definitions(-DBUILD_VIEW)
	target_link_libraries(math_example 
		js_view_adapter
		nlohmann_json::nlohmann_json
	) 
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
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

set_target_properties(math_example
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
