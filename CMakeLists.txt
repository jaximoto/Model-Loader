cmake_minimum_required(VERSION 3.10.0)
project(model-loader VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies resolved via vcpkg (see vcpkg.json)
find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(model-loader 
    src/main.cpp
    src/glad.c
    src/imgui.cpp
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
    src/imgui_demo.cpp
 )

target_include_directories(model-loader PRIVATE 
    include
    src
)

target_link_libraries(model-loader PRIVATE 
    assimp::assimp
    glfw
    Freetype::Freetype
    OpenGL::GL
)