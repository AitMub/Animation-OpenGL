#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "render_scene.h"
#include "input_process.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

RenderScene* p_render_scene;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

GLFWwindow* Init();
bool SetGLState();
void ShowFPS(GLFWwindow* window);
void Render(Shader&, const RenderScene&);

int main()
{
    GLFWwindow* window = Init();
    if (window == nullptr) return -1;
    
    if (SetGLState() == false) return -1;

    Shader shader("lighting.vs", "lighting.fs");

    p_render_scene = new RenderScene(Model("resource/nanosuit/nanosuit.obj"), Camera(glm::vec3(0.0f, 0.0f, 3.0f)));

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        ShowFPS(window);
        ProcessInput(window);

        Render(shader, *p_render_scene);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete p_render_scene;
    glfwTerminate();
    return 0;
}

void Render(Shader& shader, const RenderScene& render_scene) {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable shader before setting uniforms
    shader.use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(p_render_scene->camera_.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = p_render_scene->camera_.GetViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model_m = glm::mat4(1.0f);
    model_m = glm::translate(model_m, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model_m = glm::scale(model_m, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model_m);
    p_render_scene->model_.Draw(shader);
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}

bool SetGLState() {
    glEnable(GL_DEPTH_TEST);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

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

