cmake_minimum_required(VERSION 3.10.0)
project(waves VERSION 0.1.0 LANGUAGES C CXX)

# Path to where libassimp.so and includes are located
set(ASSIMP_DIR "/home/jaxon/assimp")
set(FREETYPE_DIR "/home/jaxon/Downloads/freetype-2.10.0")

# Find the libassimp.so library
find_library(ASSIMP_LIB assimp HINTS "${ASSIMP_DIR}/bin" REQUIRED)
message(STATUS "Assimp found at: ${ASSIMP_LIB}")

# Create the executable
add_executable(waves 
    src/main.cpp
    src/glad.h
    src/glad.c
    src/imgui.cpp
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
    src/imgui_demo.cpp
)

# Include directories (headers)
target_include_directories(waves PRIVATE 
    ${ASSIMP_DIR}/include
    ${FREETYPE_DIR}/include
    include
    src
)

# Link the Assimp library and GLFW
target_link_libraries(waves PRIVATE ${ASSIMP_LIB} glfw freetype)
