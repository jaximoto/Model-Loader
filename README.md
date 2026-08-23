# OpenGL Model Loader

A lightweight 3D model loader using OpenGL and ImGUI. Assimp is used to parse the models, Stbi loads the textures, and GLFW3 is used to handle the window and event callbacks. Users can use the GUI to load any combination of 3D model and vertex/frag shaders. I built this project to learn more about OpenGL and ImGUi.

# 📦Technologies
  - C++
  - OpenGL
  - ImGui
  - Assimp
  - Stbi
  - GLFW3
  - CMake
  
# ✨Features
- **Load models:** Any model that is compatible with the [Assimp Library](http://assimp.org/) can be loaded.
- **Pick Vertex and Fragment Shaders**: You can mix and match shaders with different models.
- **Camera Controls:** You can move around the scene using the arrow keys, look around with the mouse, and scroll the mouse wheel to zoom in.

# 🧑‍🍳Process
I started by rendering a window first with GLFW. I started this project on a Linux laptop but then had to switch to windows after my laptop died. Then I 
