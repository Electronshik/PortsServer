cmake_minimum_required(VERSION 3.16.3)

project(CppServer)

include_directories(
	${CMAKE_SOURCE_DIR}/
)

add_executable(CppServer main.cpp sqlite3.c)
