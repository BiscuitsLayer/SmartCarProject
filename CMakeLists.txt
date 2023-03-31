### Entrypoint ###
cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(SmartCarProject)

### Definitions ###
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

### LibSmartCar ###
set(LIB_SMART_CAR_DIR "src")
# Add package
add_subdirectory(${LIB_SMART_CAR_DIR})
# Provide includes
include_directories(${LIB_SMART_CAR_INCLUDE_DIRS})

### Main ###
# Build main target
file(GLOB SRC_MAIN "main.cpp")
add_executable(SmartCarMain ${SRC_MAIN})
# Set binaries output path (for MSVC to ignore Debug/Release folders)
set_target_properties(SmartCarMain PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<0:>)
# Link the main target
target_link_libraries(SmartCarMain PUBLIC LibSmartCar)

### Move DLLs ###
add_custom_command(TARGET SmartCarMain POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:SmartCarMain> $<TARGET_FILE_DIR:SmartCarMain>
  	COMMAND_EXPAND_LISTS
)

### Check include directories ###
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "include dir = '${dir}'")
endforeach()