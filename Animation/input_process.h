#ifndef INPUT_PROCESS_H
#define INPUT_PROCESS_H

#include <GLFW/glfw3.h>

#include <iostream>

#include "camera.h"
#include "model.h"
#include "render_scene.h"

extern RenderScene* p_render_scene;


float last_x = 0.0f;
float last_y = 0.0f;
bool first_mouse = true;

extern float delta_time;
extern float last_frame;

void ProcessInput(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(FORWARD, delta_time * 100);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(BACKWARD, delta_time * 100);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(LEFT, delta_time * 100);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(RIGHT, delta_time * 100);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

    last_x = xpos;
    last_y = ypos;

    p_render_scene->camera_.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    p_render_scene->camera_.ProcessMouseScroll(static_cast<float>(yoffset));
}
#endif