cmake_minimum_required(VERSION 3.10)
project(SmartCarProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/tools/cmake")
set(CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/src")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Library SmartCar
file(GLOB SRC "src/*.cpp")
add_library(libsmartcar ${SRC})
target_include_directories(libsmartcar PUBLIC src)

# Main mode
file(GLOB SRC_MAIN "main.cpp")
add_executable(SmartCarMain ${SRC_MAIN})
target_link_libraries(SmartCarMain libsmartcar)
