### Entrypoint ###
cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(LibSmartCar VERSION 1.0.0)

### Definitions ###
set(CMAKE_CXX_STANDARD 17)
set(LIB_SMART_CAR_ASSETS_DIR "../assets/")
set(LIB_SMART_CAR_NN_MODELS_DIR "../models/")
set(LIB_SMART_CAR_SHADER_DIR "../shader/")
set(LIB_SMART_CAR_SERIALIZED_FILES_DIR "serialized/")
# Provide includes (not using target_include_directories, because there are many libraries there, 
# so we just include these directories for all of them)
# CMAKE_CURRENT_SOURCE_DIR <- for LibSmartCar includes
# CMAKE_CURRENT_BINARY_DIR <- for config_out.hpp (to get variables from CMake into CPP code)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
# Set LIB_SMART_CAR_INCLUDE_DIRS for main project's CMakeLists.txt (not available in currnt CMakeLists.txt)
set(LIB_SMART_CAR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

### Config ###
# Provide directories for CPP sources
configure_file(
    config_in.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/config_out.hpp
)
# Create directory for serialized files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_SMART_CAR_SERIALIZED_FILES_DIR})

### Torch ###
# For Torch naming conventions
cmake_policy(SET CMP0054 NEW)
# Find Torch
find_package(Torch REQUIRED)

### OOGL ###
# Provide directories
set(OOGL_DIR "../include/OOGL")
set(OOGL_INCLUDE_DIR "${OOGL_DIR}/include")
set(OOGL_BINARY_DIR "${CMAKE_BINARY_DIR}/lib/OOGL")
# Add package
add_subdirectory(${OOGL_DIR} ${OOGL_BINARY_DIR})
# Provide includes (for main target)
include_directories(${OOGL_INCLUDE_DIR})

### Assimp ###
# No unit tests
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "ASSIMP_BUILD_TESTS" FORCE)
# Choose only GLTF and OBJ importers
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
# Provide directories
set(ASSIMP_DIR ../include/assimp)
set(ASSIMP_BINARY_DIR "${CMAKE_BINARY_DIR}/lib/assimp")
set(ASSIMP_INCLUDE_DIRS "${ASSIMP_DIR}/include" "${ASSIMP_BINARY_DIR}/include")
# Add package
add_subdirectory(${ASSIMP_DIR} ${ASSIMP_BINARY_DIR})
# Provide includes (for main target)
include_directories(${ASSIMP_INCLUDE_DIRS})

### ImGui ###
# Provide directories
set(IMGUI_DIR "../include/imgui")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")
# Build new library
file(GLOB SRC_IMGUI "${IMGUI_DIR}/*.cpp" "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp" "${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp")
add_library(MyImGuiSubset STATIC ${SRC_IMGUI})
# Provide includes
target_include_directories(MyImGuiSubset PUBLIC ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})
# Link the library
target_link_libraries(MyImGuiSubset PUBLIC dwmapi)
# Provide includes (for main target)
include_directories(${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})

### JSON ###
# Provide directories
set(JSON_DIR "../include/json")
set(JSON_INCLUDE_DIRS "${JSON_DIR}/include")
set(JSON_BINARY_DIR "${CMAKE_BINARY_DIR}/lib/json")
# Add package
add_subdirectory(${JSON_DIR} ${JSON_BINARY_DIR})
# Provide includes (for main target)
include_directories(${JSON_INCLUDE_DIRS})

### LibSmartCar ###
# Accelerator
add_library(Accelerator OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/accelerator/accelerator.cpp)
# BBox
add_library(BBox OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/bbox/bbox.cpp)
# Camera
add_library(Camera OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/camera/camera.cpp)
# Car model
add_library(CarModel OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/car_model/car_model.cpp)
# Config
add_library(Config OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/config/config_handler.cpp)
# Constants
add_library(Constants OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/constants/constants.cpp)
# Gui
add_library(Gui OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/gui/gui.cpp)
# Helpers
add_library(Helpers OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/helpers/helpers.cpp)
# Instanced model
add_library(InstancedModel OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/instanced_model/instanced_model.cpp)
# Intersector
add_library(Intersector OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/intersector/intersector.cpp)
# Loader
add_library(Loader OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/loader/loader.cpp)
# Material
add_library(Material OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/material/material.cpp)
# Mesh
add_library(Mesh OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/mesh/mesh.cpp)
# Model
add_library(Model OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/model/model.cpp)
# Skybox
add_library(Skybox OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/skybox/skybox.cpp)
# Texture
add_library(Texture OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/texture/texture.cpp)
# Timer
add_library(Timer OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/timer/timer.cpp)
# Transform
add_library(Transform OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/transform/transform.cpp)
# Window
add_library(Window OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/window/window.cpp)
# Add a new library (main target)
add_library(${PROJECT_NAME} STATIC 
    $<TARGET_OBJECTS:Accelerator> $<TARGET_OBJECTS:BBox> $<TARGET_OBJECTS:Camera> $<TARGET_OBJECTS:CarModel>
    $<TARGET_OBJECTS:Config> $<TARGET_OBJECTS:Constants> $<TARGET_OBJECTS:Gui> $<TARGET_OBJECTS:Helpers>
    $<TARGET_OBJECTS:InstancedModel> $<TARGET_OBJECTS:Intersector> $<TARGET_OBJECTS:Loader> 
    $<TARGET_OBJECTS:Material> $<TARGET_OBJECTS:Mesh> $<TARGET_OBJECTS:Model> $<TARGET_OBJECTS:Skybox> 
    $<TARGET_OBJECTS:Texture> $<TARGET_OBJECTS:Timer> $<TARGET_OBJECTS:Transform> $<TARGET_OBJECTS:Window>
)
# Link the library
target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES} OOGL 
    MyImGuiSubset assimp::assimp nlohmann_json::nlohmann_json ${TORCH_LIBRARIES}
)

# # TODO:
# ### Maybe fix copying later...
# file(GENERATE 
#     OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/CmakeCopyDLLs$<CONFIG>.cmake"
#     CONTENT "message(\"Copying DLLs for target $<CONFIG>...\")
# file(
#    COPY \"${TORCH_INSTALL_PREFIX}/lib/\"
#    DESTINATION \"$<TARGET_FILE_DIR:${PROJECT_NAME}>\"
#    PATTERN \"*.dll\"
#    PATTERN \"*.lib\" EXCLUDE
#    PATTERN \"*.pdb\" EXCLUDE
# )")

### Check include directories ###
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
	message(STATUS "src: include dir = '${dir}'")
endforeach()