get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Creating target: ${PROJECT_NAME}")

project(${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}) # set the binary directory to build folder
file(GLOB ${PROJECT_NAME}_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)
add_executable(${PROJECT_NAME}_bin ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME}_bin engine igl::core igl::opengl igl::opengl_glfw igl::opengl_glfw_imgui igl::png)
set_target_properties(${PROJECT_NAME}_bin PROPERTIES CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME}_bin PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$(SolutionDir)")
