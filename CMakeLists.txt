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
	"${PROJECT_SOURCE_DIR}/*.ixx"
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/libs/*.c"
	"${PROJECT_SOURCE_DIR}/Modules/*.ixx"
)

include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/libs
	${PROJECT_SOURCE_DIR}/Modules
)

add_executable(CppServer ${SOURCES})
# add_custom_command(TARGET CppServer POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CppServer> ${PROJECT_SOURCE_DIR}/build/CppServer.exe
#     COMMENT "Created ${PROJECT_SOURCE_DIR}/build/CppServer.exe"
# )

# set_target_properties(CppServer PROPERTIES
# 	RUNTIME_OUTPUT_DIRECTORY <debug ${PROJECT_SOURCE_DIR}/build/>
# 	RUNTIME_OUTPUT_DIRECTORY_RELEASE <release ${PROJECT_SOURCE_DIR}/build/>
# )
# configure_file(${PROJECT_SOURCE_DIR}/build/Debug/CppServer.exe ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/CppServer.exe COPYONLY)

if(EXISTS "${PROJECT_SOURCE_DIR}/build/Debug/CppServer.exe")
	file(COPY_FILE ${PROJECT_SOURCE_DIR}/build/Debug/CppServer.exe ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/CppServer.exe)
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/build/Debug/Configurations.sqlite")
	file(COPY_FILE ${PROJECT_SOURCE_DIR}/build/Debug/Configurations.sqlite ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/Configurations.sqlite)
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/html/")
	file(COPY ${PROJECT_SOURCE_DIR}/html/ DESTINATION ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/html/)
endif()
