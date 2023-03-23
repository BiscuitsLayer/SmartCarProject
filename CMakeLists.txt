### Entrypoint ###
cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(SmartCarProject)

### Definitions ###
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/src")

### Library SmartCar (create part) ###
# Add a new library
file(GLOB SRC "src/*/*.cpp")
add_library(LibSmartCar STATIC ${SRC})
# Provide includes
target_include_directories(LibSmartCar PUBLIC src)
target_include_directories(LibSmartCar PUBLIC assets)

### OOGL ###
# Provide directories
set(OOGL_DIR "include/OOGL")
set(OOGL_INCLUDE_DIR "${OOGL_DIR}/include")
# Add package
add_subdirectory(${OOGL_DIR})
# Provide includes
target_include_directories(LibSmartCar PUBLIC ${OOGL_INCLUDE_DIR})

### Assimp ###
# No unit tests
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "ASSIMP_BUILD_TESTS" FORCE)
# Choose only GLTF and OBJ importers
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
# Provide directories
set(ASSIMP_DIR include/assimp)
set(ASSIMP_INCLUDE_DIRS "${ASSIMP_DIR}/include" "build/${ASSIMP_DIR}/include")
# Add package
add_subdirectory(${ASSIMP_DIR})
# Provide includes
target_include_directories(LibSmartCar PUBLIC ${ASSIMP_INCLUDE_DIRS})

### ImGui ###
# Provide directories
set(IMGUI_DIR "include/imgui")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")
# Build new library
file(GLOB SRC_IMGUI "${IMGUI_DIR}/*.cpp" "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp" "${IMGUI_BACKENDS_DIR}/imgui_impl_win32.cpp")
add_library(MyImGuiSubset STATIC ${SRC_IMGUI})
# Provide includes
target_include_directories(MyImGuiSubset PUBLIC ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})
# Link the library
target_link_libraries(MyImGuiSubset PUBLIC dwmapi)
# Provide includes
target_include_directories(LibSmartCar PUBLIC ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR})

### JSON ###
# Provide directories
set(JSON_DIR "include/json")
set(JSON_INCLUDE_DIRS "${JSON_DIR}/include")
# Add package
add_subdirectory(${JSON_DIR})
# Provide includes
target_include_directories(LibSmartCar PUBLIC ${JSON_INCLUDE_DIRS})

### Library SmartCar (build and link part) ###
target_link_libraries(LibSmartCar PUBLIC ${OPENGL_LIBRARIES} OOGL MyImGuiSubset assimp::assimp nlohmann_json::nlohmann_json)

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