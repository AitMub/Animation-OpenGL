#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <iostream>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "render_scene.h"
#include "render_parameter.h"
#include "input_process.h"
#include "ui_manager.h"
#include "anim_ui_window.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float delta_time;
float last_frame;

GLFWwindow* Init();
bool SetGLState();
void MainLoop(GLFWwindow*, RenderScene&, UIManager&);
void ShowFPS(GLFWwindow*);
void Render(RenderScene&);

// used to be accessed by glfw callback functions
RenderScene* p_render_scene = nullptr;

int main()
{
    GLFWwindow* window = nullptr;
    try
    {
        window = Init();
        SetGLState();
    }
    catch (string error_message)
    {
        std::cout << "Init Failed: " << error_message << std::endl;
        return -1;
    }

    p_render_scene = new RenderScene(Model("resource/T-Rex.glb"), Shader("lighting.vs", "lighting.fs"), 
        RenderVolume(45.0f, SCR_WIDTH, SCR_HEIGHT, 0.1f, 1000.0f), Camera(glm::vec3(0.0f, 0.0f, 20.0f)));
    p_render_scene->SetTransform(vec3(0.0f, 0.0f, 0.0f), 45.0f, vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
    
    UIManager ui_manager(window);
    AnimUIWindow anim_ui_window;
    ui_manager.AddUIWindow(&anim_ui_window);

    MainLoop(window, *p_render_scene, ui_manager);

    delete p_render_scene;
    glfwTerminate();
    return 0;
}

void MainLoop(GLFWwindow* window, RenderScene& render_scene, UIManager& ui_manager) {
    while (!glfwWindowShouldClose(window))
    {
        ShowFPS(window);

        ProcessInput(window);

        ui_manager.RenderWindows(render_scene.render_parameter_);
        Render(render_scene);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// 暂时设置为非const, 因为Draw函数内部还会更新model的骨骼, 这部分可以单独分离出一个非const的函数
void Render(RenderScene& render_scene) {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_scene.Draw();
}

GLFWwindow*  Init() {
    // glfw initialize and configure
    if (glfwInit() == false)
    {
        throw string("glfw Init Failed");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw string("glfw Window Creation Failed");
    }

    // set callback function
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw string("glad Init Failed");
    }

    return window;
}

bool SetGLState() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cout << "gl error code: " << error << std::endl;
        return false;
    }
    else
    {
        return true;
    }
}

void ShowFPS(GLFWwindow* window) {
    float currentFrame = static_cast<float>(glfwGetTime());
    static float time_lapse = 0.0f;
    delta_time = currentFrame - last_frame;
    time_lapse += delta_time;
    last_frame = currentFrame;

    static char FPS_s[10];
    if (time_lapse > 1.0f)
    {
        sprintf_s(FPS_s, "%9.2f", 1.0f / delta_time);
        glfwSetWindowTitle(window, FPS_s);
        time_lapse = 0.0f;
    }
}