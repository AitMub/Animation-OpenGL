#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include <vector>

#include "ui_window.h"
#include "render_parameter.h"

class UIManager
{
public:
    UIManager(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
    }

    ~UIManager() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }


    void RenderWindows(RenderParameter& render_parameter) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (UIWindow* window : p_windows_)
        {
            window->Render(render_parameter);
        }

        ImGui::Render();
    }

    void AddUIWindow(UIWindow* ui_window) {
        p_windows_.push_back(ui_window);
    }

private:
    std::vector<UIWindow*> p_windows_;
};

#endif