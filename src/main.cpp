#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <model.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include <cmath>
#include <iostream>
#include <filesystem>
#include "paths.hpp"

const char* title = "3D Model Viewer";

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// Start with mouse locked
bool mouseLocked = true;

// Delta TIme
float lastFrame = 0.0f; // Time of last frame
float deltaTime = 0.0f; // Time between current frame and last frame

// Previous callbacks to chain imGui callbacks
GLFWcursorposfun prevCursorPosCallback = nullptr;
GLFWscrollfun prevScrollCallback = nullptr;

// lighting struct

int main()
{
    // Make Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Query primary monitor and get its video mode
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Size window as percentage of screen res
    unsigned int windowWidth = static_cast<unsigned int>(mode->width * 0.8f);
    unsigned int windowHeight = static_cast<unsigned int>(mode->height * 0.8f);
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Textures", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }

    // Specify viewport for openGL
    glViewport(0, 0, windowWidth, windowHeight);

    // Tell GLFW to call my func on resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    GLFWimage icon;
    icon.pixels = stbi_load((paths::resources() / "elite.jpg").string().c_str(), &icon.width, &icon.height, 0, 4); // force 4 channels (RGBA)

    if (icon.pixels)
    {
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(icon.pixels);
    }
    else
    {
        std::cout << "Failed to load window icon" << std::endl;
    }
    // Setup fonts
    struct Character
    {
        unsigned int TextureID;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        unsigned int Advance;
    };

    /*
    //map
    //FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, "../resources/fonts/yoster.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
        */
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Depth Testing
    glEnable(GL_DEPTH_TEST);

    Shader myShader(paths::shaders() / "shader.vs", paths::shaders() / "shader.fs");

    Model myModel = Model(paths::resources() / "n64/n64.obj");
	
    // LETS GOOO Render Loop!
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        
        glfwPollEvents();
        processInput(window);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Model Jam");
        if (ImGui::Button("Quit")){
            glfwSetWindowShouldClose(window, true);
		}
        ImGui::End();
        
		ImGuiIO& io = ImGui::GetIO();
        // Check if there is a mouse click and if the mouse isn't locked and not on ui
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !io.WantCaptureMouse && mouseLocked == false)
        {
			//std::cout << "Mouse Clicked, locking mouse" << std::endl;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseLocked = true;
		}
        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();

        // transformation
		//std::cout << "Camera Position: " << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
        
       
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        // Retrieve the matrix uniform locations
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);
        
       
		//std::cout << "View Matrix: " << to_string(view) << std::endl;
        
        

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, -7.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(.1f, 0.1f, 0.1f));     // it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        myShader.setMat4("model", model);
        myModel.Draw(myShader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // check and call events and swap buffers
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && mouseLocked)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		mouseLocked = false;
    }
        
       
        

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        //std::cout << "W pressed, deltaTime=" << deltaTime << std::endl;
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    // Call previous callback for ImGui
    if (prevCursorPosCallback)
    {
        prevCursorPosCallback(window, xposIn, yposIn);
	}
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

	if (mouseLocked)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // Call previous callback for ImGui
    if (prevScrollCallback)
    {
		prevScrollCallback(window, xoffset, yoffset);
    }
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
