### Entrypoint ###
cmake_minimum_required(VERSION 3.10)
project(TorchTest)

# set(CXX_STANDARD_REQUIRED 17)
# set(CMAKE_CUDA_STANDARD 17)
# add_compile_options(/std=c++17)

### Torch ###
# For Torch naming conventions
cmake_policy(SET CMP0054 NEW)
# Find Torch
find_package(Torch REQUIRED)

### Main ###
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} "${TORCH_LIBRARIES}")

### Move DLLs ###
file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCH_DLLS} $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)