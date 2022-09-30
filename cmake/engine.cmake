### cmake_minimum_required(VERSION 3.8)

message(STATUS "Creating target: engine")

project(engine)

file(GLOB ENGINE_SOURCES
	${CMAKE_SOURCE_DIR}/engine/*.cpp
	${CMAKE_SOURCE_DIR}/engine/*.h*)

add_library(engine STATIC ${ENGINE_SOURCES})

set_target_properties(engine PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		LINKER_LANGUAGE CXX
		FOLDER "lib")

target_link_libraries(engine PRIVATE igl igl_opengl igl_opengl_glfw igl_opengl_glfw_imgui igl_eigen)

target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/engine)
