cmake_minimum_required(VERSION 3.16.3)

project(PortsServer)
set(PROJECT_BINARY_DIR build)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_SUPPRESS_REGENERATION true)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
	message("Cmake Debug mode")
	add_compile_definitions(DEBUG_BUILD)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

# add_compile_options(-O0)
# -std=c++20 -fexperimental-library -lc++experimental)
# add_link_options(-O0)
# -std=c++20 -fexperimental-library -lc++experimental)
# add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++2a)

file(GLOB SOURCES
	"${PROJECT_SOURCE_DIR}/*.cpp"
	"${PROJECT_SOURCE_DIR}/PortApi/*.cpp"
	"${PROJECT_SOURCE_DIR}/Modules/*.ixx"
	"${PROJECT_SOURCE_DIR}/Modules/SerialPort.cpp"
	"${PROJECT_SOURCE_DIR}/GitSubmodules/Sqlite3/sqlite3.c"
)

include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/PortApi
	${PROJECT_SOURCE_DIR}/Modules
	${PROJECT_SOURCE_DIR}/GitSubmodules/Sqlite3
	${PROJECT_SOURCE_DIR}/GitSubmodules/Json/include
	${PROJECT_SOURCE_DIR}/GitSubmodules/Json/include/nlohmann
	${PROJECT_SOURCE_DIR}/GitSubmodules/Inja/include/inja
	${PROJECT_SOURCE_DIR}/GitSubmodules/Httplib
)

add_executable(PortsServer ${SOURCES})
# add_custom_command(TARGET PortsServer POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:PortsServer> ${PROJECT_SOURCE_DIR}/build/PortsServer.exe
#     COMMENT "Created ${PROJECT_SOURCE_DIR}/build/PortsServer.exe"
# )

# set_target_properties(PortsServer PROPERTIES
# 	RUNTIME_OUTPUT_DIRECTORY <debug ${PROJECT_SOURCE_DIR}/build/>
# 	RUNTIME_OUTPUT_DIRECTORY_RELEASE <release ${PROJECT_SOURCE_DIR}/build/>
# )
# configure_file(${PROJECT_SOURCE_DIR}/build/Debug/PortsServer.exe ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/PortsServer.exe COPYONLY)

set(TARGET_NAME PortsServer.exe)
configure_file(${PROJECT_SOURCE_DIR}/NwJsApp/nwjs_index.js.in ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/index.js)

if(EXISTS "${PROJECT_SOURCE_DIR}/build/Debug/PortsServer.exe")
	file(COPY_FILE ${PROJECT_SOURCE_DIR}/build/Debug/PortsServer.exe ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/PortsServer.exe)
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/build/Debug/Configurations.sqlite")
	file(COPY_FILE ${PROJECT_SOURCE_DIR}/build/Debug/Configurations.sqlite ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/Configurations.sqlite)
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/GitSubmodules/Bootstrap/dist/")
	file(COPY ${PROJECT_SOURCE_DIR}/GitSubmodules/Bootstrap/dist/ DESTINATION ${PROJECT_SOURCE_DIR}/html/bootstrap-dist)
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/html/vue-js/vue.global.js")
	message(STATUS "Vue.js not found, downloading..")
	file(DOWNLOAD "https://cdn.jsdelivr.net/npm/vue@3.3.4/dist/vue.global.js" ${PROJECT_SOURCE_DIR}/html/vue-js/vue.global.js)
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/html/")
	file(COPY ${PROJECT_SOURCE_DIR}/html/ DESTINATION ${PROJECT_SOURCE_DIR}/NwJsApp/actual/package.nw/html/)
endif()

# git submodule add -f --name httplib https://github.com/yhirose/cpp-httplib.git  GitSubmodules/Httplib
