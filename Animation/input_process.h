#ifndef INPUT_PROCESS_H
#define INPUT_PROCESS_H

#include <GLFW/glfw3.h>

#include <iostream>

#include "render_scene.h"

extern RenderScene* p_render_scene;

bool left_pressing = false;
bool right_pressing = false;
bool begin = true;

extern float delta_time;
extern float last_frame;

void ProcessInput(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		p_render_scene->camera_.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        p_render_scene->camera_.ProcessKeyboard(DOWN, delta_time);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_button_callback(GLFWwindow* window, int buttons, int action, int mode) {
	if (action == GLFW_PRESS && buttons == GLFW_MOUSE_BUTTON_LEFT)
	{
		begin = true;//交互开始时begin可能不为1,会导致旋转时出现跳动,因此按下时设其为1
		left_pressing = true;
	}
	if (action == GLFW_RELEASE && buttons == GLFW_MOUSE_BUTTON_LEFT)
	{
		left_pressing = false;
	}

	if (action == GLFW_PRESS && buttons == GLFW_MOUSE_BUTTON_RIGHT)
	{
		begin = true;
		right_pressing = true;
	}
	if (action == GLFW_RELEASE && buttons == GLFW_MOUSE_BUTTON_RIGHT)
	{
		right_pressing = false;
	}
}


void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	static float xbegin, ybegin, xend, yend;

	if (begin == true)
	{
		xbegin = x;
		ybegin = y;
		begin = false;
	}
	else
	{
		xend = x;
		yend = y;
		begin = true;

		if (right_pressing == true)
		{
			p_render_scene->camera_.Rotate(xbegin - xend, ybegin - yend);
		}
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    p_render_scene->camera_.ProcessMouseScroll(static_cast<float>(yoffset));
}
#endif