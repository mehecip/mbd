cmake_minimum_required(VERSION 3.10)

project(js_view_adapter CXX)

set(CPP_FILES
  src/js_view_adapter.cpp
  src/json_serializer.cpp
  src/main.cpp
)

set(HPP_FILES
  header/js_view_adapter.hpp
  header/json_serializer.hpp
)


add_library(js_view_adapter 
	${HPP_FILES}
	${CPP_FILES}
)

add_executable( js_view_adapter_exe 
	${HPP_FILES}
	${CPP_FILES}
)

include_directories(
header
../lib/model/header
../lib/controller/header
)
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Threads)
# find_package(cppzmq)
# find_package(nlohmann_json 3.11.2 REQUIRED)

target_link_libraries(js_view_adapter 
	PRIVATE model
	PRIVATE controller
	# PRIVATE cppzmq
	PRIVATE nlohmann_json::nlohmann_json
	${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries(js_view_adapter_exe 
	PRIVATE model
	PRIVATE controller
	# PRIVATE cppzmq
	PRIVATE nlohmann_json::nlohmann_json
	${CMAKE_THREAD_LIBS_INIT}
)



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

set_target_properties(js_view_adapter_exe
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)