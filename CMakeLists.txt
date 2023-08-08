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

### LEGACY: old-style DLL copying for Graphics (is done every build) ###
# add_custom_command(TARGET SmartCarMain POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:SmartCarMain> $<TARGET_FILE_DIR:SmartCarMain>
# 	COMMENT "Copying DLLs for SmartCar..."
#   	COMMAND_EXPAND_LISTS
# )

### Copy DLLs for PyTorch ###
# Set necessary DLL`s names
set(PyTorchDDLNames
    "torch_cuda.dll"
    "torch_cpu.dll"
    "fbgemm.dll"
    "nvfuser_codegen.dll"
    "libiomp5md.dll"
    "c10.dll"
    "c10_cuda.dll"
    "asmjit.dll"
    "cudnn64_8.dll"
    "uv.dll"
    "nvToolsExt64_1.dll"

    "cusparse64_11.dll"
    "cufft64_10.dll"
    "cublas64_11.dll"
    "cublasLt64_11.dll"
)
# Check that all files exist, error if they don't
foreach(PyTorchDDLName ${PyTorchDDLNames})
    if(EXISTS "${TORCH_INSTALL_PREFIX}/lib/${PyTorchDDLName}")
        set(PyTorchDDLFile "${TORCH_INSTALL_PREFIX}/lib/${PyTorchDDLName}")
        list(APPEND PyTorchDDLFiles ${PyTorchDDLFile})
    else()
        message(FATAL_ERROR "${PyTorchDDLName} not found in ${TORCH_INSTALL_PREFIX}/lib; Please, move the file and try again")
    endif()
endforeach()
# Copy files to the folder with executable
message("Copying DLLs for PyTorch...")
file(
    COPY ${PyTorchDDLFiles}
    DESTINATION ${CMAKE_BINARY_DIR}
)
message("Copied DLLs for PyTorch successfully!")

### Check include directories ###
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "include dir = '${dir}'")
endforeach()