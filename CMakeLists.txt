cmake_minimum_required(VERSION 3.10)
project(SmartCarProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Library SmartCar
file(GLOB SRC "src/*.cpp")
add_library(libsmartcar ${SRC})
include_directories(src)
include_directories(assets)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(libsmartcar PUBLIC ${OPENGL_LIBRARIES})

# OOGL
set(OOGL_DIR "include/OOGL")
set(OOGL_INCLUDE_DIR ${OOGL_DIR}/include)
set(OOGL_LIBRARY ${CMAKE_SOURCE_DIR}/${OOGL_DIR}/lib/OOGL.a)
include_directories(${OOGL_INCLUDE_DIR})

# Assimp
set(ASSIMP_DIR include/assimp)
set(ASSIMP_INCLUDE_DIRS "${ASSIMP_DIR}/include" "build/${ASSIMP_DIR}/include")
set(ASSIMP_LIBRARIES "${CMAKE_SOURCE_DIR}/build/${ASSIMP_DIR}/bin/libassimp-5d.dll")
add_subdirectory(${ASSIMP_DIR})
include_directories(${ASSIMP_INCLUDE_DIRS})

message("msg" ${ASSIMP_INCLUDE_DIRS})
message("msg" ${ASSIMP_LIBRARIES})


# ImGui
set(IMGUI_DIR "include/imgui")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")
file(GLOB SRC_LIB "${IMGUI_DIR}/*.cpp" "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp" "${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp")
include_directories(${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})
target_link_libraries(libsmartcar PUBLIC dwmapi)

# Main
file(GLOB SRC_MAIN "main.cpp")
add_executable(SmartCarMain ${SRC_MAIN} ${SRC_LIB})
target_link_libraries(SmartCarMain ${OOGL_LIBRARY} ${ASSIMP_LIBRARIES} libsmartcar)

# Check include directories
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "include dir = '${dir}'")
endforeach()