cmake_minimum_required(VERSION 3.8)

# https://github.com/libigl/libigl/issues/751
# http://lists.llvm.org/pipermail/llvm-commits/Week-of-Mon-20160425/351643.html
if(APPLE)
  if(NOT CMAKE_LIBTOOL)
    find_program(CMAKE_LIBTOOL NAMES libtool)
  endif()
  if(CMAKE_LIBTOOL)
    set(CMAKE_LIBTOOL ${CMAKE_LIBTOOL} CACHE PATH "libtool executable")
    message(STATUS "Found libtool - ${CMAKE_LIBTOOL}")
    get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
    foreach(lang ${languages})
      # Added -c
      set(CMAKE_${lang}_CREATE_STATIC_LIBRARY
        "${CMAKE_LIBTOOL} -c -static -o <TARGET> <LINK_FLAGS> <OBJECTS> ")
    endforeach()
  endif()
endif()

### Available options ###
option(LIBIGL_USE_STATIC_LIBRARY     "Use libigl as static library" OFF)
option(LIBIGL_WITH_CGAL              "Use CGAL"                     OFF)
option(LIBIGL_WITH_COMISO            "Use CoMiso"                   OFF)
option(LIBIGL_WITH_CORK              "Use Cork"                     OFF)
option(LIBIGL_WITH_EMBREE            "Use Embree"                   OFF)
option(LIBIGL_WITH_MATLAB            "Use Matlab"                   OFF)
option(LIBIGL_WITH_MOSEK             "Use MOSEK"                    OFF)
option(LIBIGL_WITH_OPENGL            "Use OpenGL"                   OFF)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"                     OFF)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"                    OFF)
option(LIBIGL_WITH_PNG               "Use PNG"                      OFF)
option(LIBIGL_WITH_TETGEN            "Use Tetgen"                   OFF)
option(LIBIGL_WITH_TRIANGLE          "Use Triangle"                 OFF)
option(LIBIGL_WITH_PREDICATES        "Use exact predicates"         OFF)
option(LIBIGL_WITH_XML               "Use XML"                      OFF)
option(LIBIGL_WITHOUT_COPYLEFT       "Disable Copyleft libraries"   OFF)
option(LIBIGL_EXPORT_TARGETS         "Export libigl CMake targets"  OFF)

if(LIBIGL_BUILD_PYTHON)
  message(FATAL_ERROR "Python bindings have been removed in this version. Please use an older version of libigl, or wait for the new bindings to be released.")
endif()

################################################################################

### Configuration
set(LIBIGL_ROOT ${CMAKE_SOURCE_DIR})
set(LIBIGL_EXTERNAL ${LIBIGL_ROOT}/external)
set(LIBIGL_SOURCE_DIR ${LIBIGL_EXTERNAL}/igl)

# Dependencies are linked as INTERFACE targets unless libigl is compiled as a static library
if(LIBIGL_USE_STATIC_LIBRARY)
  set(IGL_SCOPE PUBLIC)
else()
  set(IGL_SCOPE INTERFACE)
endif()

# Download and update 3rdparty libraries
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(LibiglDownloadExternal)

# Provides igl_set_folders() to set folders for Visual Studio/Xcode
include(LibiglFolders)

################################################################################
### IGL Common
################################################################################

igl_download_core()
add_library(igl_common INTERFACE)
target_include_directories(igl_common SYSTEM INTERFACE
  $<BUILD_INTERFACE:${LIBIGL_EXTERNAL}>
  $<BUILD_INTERFACE:${LIBIGL_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
# Export igl_common as igl::common
set_property(TARGET igl_common PROPERTY EXPORT_NAME igl::common)
if(LIBIGL_USE_STATIC_LIBRARY)
  target_compile_definitions(igl_common INTERFACE -DIGL_STATIC_LIBRARY)
endif()

# Transitive C++11 flags
include(CXXFeatures)
target_compile_features(igl_common INTERFACE ${CXX11_FEATURES})

# Other compilation flags
if(MSVC)
  # Enable parallel compilation for Visual Studio
  target_compile_options(igl_common INTERFACE /MP /bigobj)
  target_compile_definitions(igl_common INTERFACE -DNOMINMAX)
endif()

# Controls whether to use the static MSVC runtime or not
include(LibiglWindows)

if(BUILD_SHARED_LIBS)
  # Generate position independent code
  set_target_properties(igl_common PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
endif()

if(UNIX AND NOT HUNTER_ENABLED)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(HUNTER_ENABLED)
  hunter_add_package(Eigen)
  find_package(Eigen3 CONFIG REQUIRED)
endif()

# Eigen
if(NOT TARGET Eigen3::Eigen)
  igl_download_eigen()
  add_library(igl_eigen INTERFACE)
  target_include_directories(igl_eigen SYSTEM INTERFACE
    $<BUILD_INTERFACE:${LIBIGL_EXTERNAL}/eigen>
    $<INSTALL_INTERFACE:include>
  )
  set_property(TARGET igl_eigen PROPERTY EXPORT_NAME Eigen3::Eigen)
  add_library(Eigen3::Eigen ALIAS igl_eigen)
endif()
target_link_libraries(igl_common INTERFACE Eigen3::Eigen)

# C++11 Thread library
find_package(Threads REQUIRED)
target_link_libraries(igl_common INTERFACE ${CMAKE_THREAD_LIBS_INIT})

################################################################################

function(compile_igl_module module_dir)
  string(REPLACE "/" "_" module_name "${module_dir}")
  if(module_name STREQUAL "core")
    set(module_libname "igl")
  else()
    set(module_libname "igl_${module_name}")
  endif()
  if(LIBIGL_USE_STATIC_LIBRARY)
    file(GLOB SOURCES_IGL_${module_name}
      "${LIBIGL_SOURCE_DIR}/${module_dir}/*.cpp"
      "${LIBIGL_SOURCE_DIR}/${module_dir}/*.h*"
    )
    if(NOT LIBIGL_WITHOUT_COPYLEFT)
      file(GLOB COPYLEFT_SOURCES_IGL_${module_name}
        "${LIBIGL_SOURCE_DIR}/copyleft/${module_dir}/*.cpp"
        "${LIBIGL_SOURCE_DIR}/copyleft/${module_dir}/*.h*"
      )
      list(APPEND SOURCES_IGL_${module_name} ${COPYLEFT_SOURCES_IGL_${module_name}})
    endif()
    add_library(${module_libname} STATIC ${SOURCES_IGL_${module_name}} ${ARGN})
    if(MSVC)
      # Silencing some compile warnings
      target_compile_options(${module_libname} PRIVATE
        # Type conversion warnings. These can be fixed with some effort and possibly more verbose code.
        /wd4267 # conversion from 'size_t' to 'type', possible loss of data
        /wd4244 # conversion from 'type1' to 'type2', possible loss of data
        /wd4018 # signed/unsigned mismatch
        /wd4305 # truncation from 'double' to 'float'
        # This one is from template instantiations generated by autoexplicit.sh:
        /wd4667 # no function template defined that matches forced instantiation ()
        # This one is easy to fix, just need to switch to safe version of C functions
        /wd4996 # this function or variable may be unsafe
        # This one is when using bools in adjacency matrices
        /wd4804 #'+=': unsafe use of type 'bool' in operation
      )
    endif()
  else()
    add_library(${module_libname} INTERFACE)
  endif()

  target_link_libraries(${module_libname} ${IGL_SCOPE} igl_common)
  if(NOT module_name STREQUAL "core")
    target_link_libraries(${module_libname} ${IGL_SCOPE} igl)
  endif()

  set_target_properties(${module_libname} PROPERTIES FOLDER "lib")

  # Alias target because it looks nicer
  message(STATUS "Creating target: igl::${module_name} (${module_libname})")
  add_library(igl::${module_name} ALIAS ${module_libname})
  # Export as igl::${module_name}
  set_property(TARGET ${module_libname} PROPERTY EXPORT_NAME igl::${module_name})
endfunction()

################################################################################
### IGL Core
################################################################################

if(LIBIGL_USE_STATIC_LIBRARY)
  file(GLOB SOURCES_IGL
    "${LIBIGL_SOURCE_DIR}/*.cpp"
    "${LIBIGL_SOURCE_DIR}/*.h*"
    "${LIBIGL_SOURCE_DIR}/copyleft/*.cpp"
    "${LIBIGL_SOURCE_DIR}/copyleft/*.h*"
  )
endif()
compile_igl_module("core" ${SOURCES_IGL})

# for workaround to get the libs into lib folder
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

function(get_all_targets _result _dir)
    get_property(_subdirs DIRECTORY "${_dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        get_all_targets(${_result} "${_subdir}")
    endforeach()
    get_property(_sub_targets DIRECTORY "${_dir}" PROPERTY BUILDSYSTEM_TARGETS)
    set(${_result} ${${_result}} ${_sub_targets} PARENT_SCOPE)
endfunction()

function(add_subdirectory_with_folder _subdirectory _folder_name)
    add_subdirectory(${_subdirectory} ${ARGN})
    get_all_targets(_targets "${_subdirectory}")
    foreach(_target IN LISTS _targets)
        set_target_properties(
            ${_target}
            PROPERTIES FOLDER "${_folder_name}"
        )
    endforeach()
endfunction()

################################################################################
### Compile the opengl part ###
if(LIBIGL_WITH_OPENGL)
  # OpenGL module
  compile_igl_module("opengl")

  # OpenGL library
  if (NOT CMAKE_VERSION VERSION_LESS "3.11")
    cmake_policy(SET CMP0072 NEW)
  endif()
  find_package(OpenGL REQUIRED)
  if(TARGET OpenGL::GL)
    target_link_libraries(igl_opengl ${IGL_SCOPE} OpenGL::GL)
    target_include_directories(igl_opengl ${IGL_SCOPE} ${LIBIGL_SOURCE_DIR}/opengl)
  else()
    target_link_libraries(igl_opengl ${IGL_SCOPE} ${OPENGL_GL_LIBRARY})
    target_include_directories(igl_opengl SYSTEM ${IGL_SCOPE} ${OPENGL_INCLUDE_DIR})
  endif()

  # glad module
  if(NOT TARGET glad)
	add_subdirectory_with_folder(${LIBIGL_ROOT}/external/glad "lib" glad)
    target_include_directories(igl_opengl ${IGL_SCOPE} ${LIBIGL_ROOT}/glad/include)
  endif()
  target_link_libraries(igl_opengl ${IGL_SCOPE} glad)
endif()

################################################################################
### Compile the GLFW part ###
if(LIBIGL_WITH_OPENGL_GLFW)
  if(TARGET igl::opengl)
    # GLFW module
    compile_igl_module("opengl/glfw")
    if(NOT TARGET glfw)
      igl_download_glfw()
      option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
      option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
      option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
      option(GLFW_INSTALL "Generate installation target" OFF)
      if(IGL_STATIC_RUNTIME)
        set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "Use MSVC runtime library DLL" FORCE)
      else()
        set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC runtime library DLL" FORCE)
      endif()
      add_subdirectory_with_folder(${LIBIGL_EXTERNAL}/glfw "lib" glfw)
	endif()
    target_link_libraries(igl_opengl_glfw ${IGL_SCOPE} igl_opengl glfw)
  endif()
endif()

################################################################################
### Compile the ImGui part ###
if(LIBIGL_WITH_OPENGL_GLFW_IMGUI)
  if(TARGET igl::opengl_glfw)
    # ImGui module
    compile_igl_module("opengl/glfw/imgui")
    if(NOT TARGET imgui)
      igl_download_imgui()
      add_subdirectory_with_folder(${LIBIGL_EXTERNAL}/libigl-imgui "lib" imgui)
    endif()
    if(NOT TARGET imguizmo)
      igl_download_imguizmo()
      add_library(imguizmo ${LIBIGL_EXTERNAL}/imguizmo/ImGuizmo.cpp ${LIBIGL_EXTERNAL}/imguizmo/ImGuizmo.h)
      target_compile_features(imguizmo PUBLIC cxx_std_11)
      target_link_libraries(imguizmo PUBLIC imgui igl_eigen)
	  target_include_directories(imguizmo PUBLIC ${LIBIGL_EXTERNAL}/imguizmo)
      set_target_properties(imguizmo PROPERTIES FOLDER "lib")
    endif()
    target_link_libraries(igl_opengl_glfw_imgui ${IGL_SCOPE} igl_opengl_glfw imguizmo imgui)
    target_include_directories(igl_opengl_glfw_imgui ${IGL_SCOPE} ${LIBIGL_SOURCE_DIR}/opengl/glfw/imgui)
  endif()
endif()

################################################################################
### Compile the png part ###
if(LIBIGL_WITH_PNG)
  # png/ module is anomalous because it also depends on opengl it really should
  # be moved into the opengl/ directory and namespace ...
  if(TARGET igl_opengl)
    if(NOT TARGET stb_image)
      igl_download_stb()
      add_subdirectory_with_folder(${LIBIGL_EXTERNAL}/stb "lib" stb_image)
    endif()
    compile_igl_module("png" "")
    target_link_libraries(igl_png ${IGL_SCOPE} igl_stb_image igl_opengl)
  endif()
endif()

################################################################################
### Install and export all modules

if(NOT LIBIGL_EXPORT_TARGETS)
  return()
endif()

function(install_dir_files dir_name)
  if (dir_name STREQUAL "core")
    set(subpath "")
  else()
    set(subpath "/${dir_name}")
  endif()

  file(GLOB public_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.hpp
  )

  set(files_to_install ${public_headers})

  if(NOT LIBIGL_USE_STATIC_LIBRARY)
    file(GLOB public_sources
      ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/include/igl${subpath}/*.c
    )
  endif()
  list(APPEND files_to_install ${public_sources})

  install(
    FILES ${files_to_install}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/igl${subpath}
  )
endfunction()

################################################################################

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(TARGET igl_eigen)
  set(IGL_EIGEN igl_eigen)
else()
  set(IGL_EIGEN)
  message(WARNING "Trying to export igl targets while using an imported target for Eigen.")
endif()

# Install and export core library
install(
  TARGETS
    igl
    igl_common
    ${IGL_EIGEN}
  EXPORT igl-export
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
export(
  TARGETS
    igl
    igl_common
    ${IGL_EIGEN}
  FILE libigl-export.cmake
)

# Install headers for core library
install_dir_files(core)
install_dir_files(copyleft)

# Write package configuration file
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/libigl-config.cmake.in
  ${CMAKE_BINARY_DIR}/libigl-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/libigl/cmake
)
install(
  FILES
    ${CMAKE_BINARY_DIR}/libigl-config.cmake
  DESTINATION
    ${CMAKE_INSTALL_DATADIR}/libigl/cmake
)

# Write export file
export(EXPORT igl-export
  FILE "${CMAKE_BINARY_DIR}/libigl-export.cmake"
)
install(EXPORT igl-export DESTINATION ${CMAKE_INSTALL_DATADIR}/libigl/cmake FILE libigl-export.cmake)


export(PACKAGE libigl)

