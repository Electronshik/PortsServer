cmake_minimum_required(VERSION 3.16.3)

project(CppServer)
set(PROJECT_BINARY_DIR build)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# add_compile_options(-O0)
# -std=c++20 -fexperimental-library -lc++experimental)
# add_link_options(-O0)
# -std=c++20 -fexperimental-library -lc++experimental)
# add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++2a)

file(GLOB SOURCES
	"${PROJECT_SOURCE_DIR}/*.cpp",
	"${PROJECT_SOURCE_DIR}/libs/*.c"
)

include_directories(
	${CMAKE_SOURCE_DIR},
	${CMAKE_SOURCE_DIR}/libs
)

add_executable(CppServer ${SOURCES})
if(EXISTS "${PROJECT_SOURCE_DIR}/build/Debug/CppServer.exe")
	configure_file(${PROJECT_SOURCE_DIR}/build/Debug/CppServer.exe ${PROJECT_SOURCE_DIR}/build/CppServer.exe COPYONLY)
endif()
