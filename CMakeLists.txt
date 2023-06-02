### Entrypoint ###
cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(SmartCarProject)

### Definitions ###
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(APP_CONFIG_DIR "../configs/")
# Provide includes
# CMAKE_CURRENT_BINARY_DIR <- for config_application_out.hpp (to get variables from CMake into CPP code)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

### Torch ###
# For Torch naming conventions
cmake_policy(SET CMP0054 NEW)
# Find Torch
find_package(Torch REQUIRED)

### Config ###
# Provide directories for CPP sources
configure_file(
    config_application_in.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/config_application_out.hpp
)

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
target_link_libraries(SmartCarMain PUBLIC LibSmartCar ${TORCH_LIBRARIES})

### Move DLLs for SmartCar ###
# TODO: should be changed with cmake -P <configure_script.cmake> (like in torch test)
# add_custom_command(TARGET SmartCarMain POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:SmartCarMain> $<TARGET_FILE_DIR:SmartCarMain>
# 	COMMENT "Copying DLLs for SmartCar..."
#   	COMMAND_EXPAND_LISTS
# )

# TODO: FIX THIS PART! do not forget to turn on when recompiling from start
# ### Move DLLs for PyTorch ###
# file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
# add_custom_command(TARGET SmartCarMain POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCH_DLLS} $<TARGET_FILE_DIR:SmartCarMain>
#     COMMENT "Copying DLLs for PyTorch..."
#     COMMAND_EXPAND_LISTS
# )

### Check include directories ###
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "include dir = '${dir}'")
endforeach()