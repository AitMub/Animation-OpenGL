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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

RenderScene* p_render_scene;
RenderParameter* p_render_parameter;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

GLFWwindow* Init();
bool SetGLState();
void ShowFPS(GLFWwindow* window);
void RenderUI(RenderParameter&);
void Render(Shader&, const RenderScene&, const RenderParameter&);

int main()
{
    GLFWwindow* window = Init();
    if (window == nullptr) return -1;
    
    if (SetGLState() == false) return -1;

    Shader shader("lighting.vs", "lighting.fs");

    p_render_scene = new RenderScene(Model("resource/T-Rex.glb"), Camera(glm::vec3(0.0f, 10.0f, 20.0f)));
    p_render_parameter = new RenderParameter();
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        ShowFPS(window);
        ProcessInput(window);
  
        RenderUI(*p_render_parameter);

        Render(shader, *p_render_scene, *p_render_parameter);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放ImGui资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete p_render_scene;
    delete p_render_parameter;
    glfwTerminate();
    return 0;
}

void Render(Shader& shader, const RenderScene& render_scene, const RenderParameter & render_parameter) {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable shader before setting uniforms
    shader.use();

    // view/projection transformations
    mat4 projection = glm::perspective(glm::radians(p_render_scene->camera_.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    mat4 view = p_render_scene->camera_.GetViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    // model transformations
    mat4 model_m = mat4(1.0f);
    model_m = glm::scale(model_m, glm::vec3(1.0f, 1.0f, 1.0f));	// scale it down
    shader.setMat4("model", model_m);
    // light
    shader.setVec3("lightColor", p_render_scene->light_.color_);
    shader.setVec3("lightPos", p_render_scene->light_.pos_);
    shader.setVec3("viewPos", p_render_scene->camera_.Position);

    // calculate bone transform matrix
    p_render_scene->model_.CalcBoneTransform(static_cast<float>(glfwGetTime()), render_parameter.anim_blend_weight);
    // render
    p_render_scene->model_.Draw(shader);
}

void RenderUI(RenderParameter& render_parameter) {
    // 启动ImGui框架
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        //开始配置ImGui窗口
        //设置窗口属性
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        ImGui::Begin("PARAMETER SETTINGS", 0, window_flags);

        ImGui::Text("Animation Blend Weight");
        ImGui::SliderFloat(" ", &render_parameter.anim_blend_weight, 0.0f, 1.0f);

        ImGui::End();
    }
    //ImGui::ShowDemoWindow();

    ImGui::Render();
}

GLFWwindow*  Init() {
    // glfw initialize and configure
    if (glfwInit() == false)
    {
        std::cout << "Failed to init GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // set callback function
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // imgui initialize and configure
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    return window;
}

bool SetGLState() {
    glEnable(GL_DEPTH_TEST);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    //设置颜色风格
    ImGui::StyleColorsDark();

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

