cmake_minimum_required(VERSION 3.10)

# Initialize toolchain file first, before the project command
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mingw-toolchain.cmake" CACHE PATH "Path to toolchain file")

project(RoR2Mod LANGUAGES CXX)

# Language standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Always build with debug info
set(CMAKE_BUILD_TYPE Debug)

# Consolidate all debug and compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -fpermissive")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -gdwarf-4 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--export-all-symbols")

# Debug definitions
add_definitions(-D_DEBUG -DDEBUG -D_CONSOLE)

# Add imgui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)
set(IMGUI_BACKEND_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

# Add MinHook
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/minhook)

# Define source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# C# Helper DLL Configuration
set(HELPER_CS_PROJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/helper/RoR2ModHelper.csproj)
set(HELPER_CS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/helper/RoR2ModHelper.cs)
set(HELPER_DLL_OUTPUT ${CMAKE_BINARY_DIR}/RoR2ModHelper.dll)
set(HELPER_HEADER_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/RoR2ModHelper_dll.h)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(DOTNET_EXECUTABLE dotnet REQUIRED)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/generated)
add_custom_command(
    OUTPUT ${HELPER_DLL_OUTPUT}
    COMMAND ${DOTNET_EXECUTABLE} restore ${HELPER_CS_PROJECT}
    COMMAND ${DOTNET_EXECUTABLE} build ${HELPER_CS_PROJECT} --no-restore -c Release
    DEPENDS ${HELPER_CS_SOURCE} ${HELPER_CS_PROJECT}
    COMMENT "Building C# helper DLL with dotnet..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/helper
    VERBATIM
)
add_custom_command(
    OUTPUT ${HELPER_HEADER_OUTPUT}
    COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/dll_to_header.py
        ${HELPER_DLL_OUTPUT}
        ${HELPER_HEADER_OUTPUT}
    DEPENDS ${HELPER_DLL_OUTPUT}
    COMMENT "Converting helper DLL to C++ header..."
    VERBATIM
)
add_custom_target(generate_helper ALL DEPENDS ${HELPER_HEADER_OUTPUT})

# Include directories
include_directories(
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/minhook/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Find all source files
file(GLOB_RECURSE CORE_SOURCES ${SRC_DIR}/core/*.cpp)
file(GLOB_RECURSE GAME_SOURCES ${SRC_DIR}/game/*.cpp)
file(GLOB_RECURSE GLOBAL_SOURCES ${SRC_DIR}/globals/*.cpp)
file(GLOB_RECURSE HOOK_SOURCES ${SRC_DIR}/hooks/*.cpp)
file(GLOB_RECURSE UTILS_SOURCES ${SRC_DIR}/utils/*.cpp)
file(GLOB_RECURSE MENU_SOURCES ${SRC_DIR}/menu/*.cpp)
file(GLOB_RECURSE FONT_SOURCES ${SRC_DIR}/fonts/*.cpp)
file(GLOB_RECURSE MODULE_SOURCES ${SRC_DIR}/modules/*.cpp)
file(GLOB_RECURSE CONFIG_SOURCES ${SRC_DIR}/config/*.cpp)
file(GLOB_RECURSE HELPER_SOURCES ${SRC_DIR}/helper/*.cpp)

# DLL
add_library(RoR2ModDLL SHARED
    ${SRC_DIR}/dllmain.cpp
    ${CORE_SOURCES}
    ${GAME_SOURCES}
    ${GLOBAL_SOURCES}
    ${HOOK_SOURCES}
    ${UTILS_SOURCES}
    ${MENU_SOURCES}
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${FONT_SOURCES}
    ${MODULE_SOURCES}
    ${CONFIG_SOURCES}
    ${HELPER_SOURCES}
)

add_dependencies(RoR2ModDLL generate_helper)

target_link_libraries(RoR2ModDLL PRIVATE
    minhook
    d3d11
    dxgi
    d3dcompiler
    dwmapi
    dbghelp
)

set_target_properties(RoR2ModDLL PROPERTIES
    OUTPUT_NAME "RoR2Mod"
    PREFIX ""
    SUFFIX ".dll"
)

# Keep Windows subsystem but add debug symbols and definitions
set_target_properties(RoR2ModDLL PROPERTIES
    LINK_FLAGS "-Wl,--subsystem,windows -Wl,--enable-runtime-pseudo-reloc -Wl,--export-all-symbols"
)

# Injector
add_executable(injector
    injector.cpp
)

set_target_properties(injector PROPERTIES
    LINK_FLAGS "-Wl,--subsystem,windows"
)