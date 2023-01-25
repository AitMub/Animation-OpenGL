#ifndef ANIM_UI_WINDOW_H
#define ANIM_UI_WINDOW_H

#include"render_parameter.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include "ui_window.h";

class AnimUIWindow : public UIWindow
{
public:
	void Render(RenderParameter& render_parameter) {
        if (play_anim_changed_ == true)
        {
            UpdateAnimDuration(render_parameter);
        }

        ImGui::Begin("ANIM PARAMETER SETTINGS", 0, window_flags_);

        ImGui::Text("Animation List");
        if (ImGui::BeginListBox(" "))
        {
            for (int i = 0; i < render_parameter.anim_names.size(); i++)
            {
                ImGui::Selectable(render_parameter.anim_names[i].c_str(), false);
            }
            ImGui::EndListBox();
        }
        ImGui::NewLine();

        ImGui::Text("Animation Mode");
        int anim_play_mode = static_cast<int>(render_parameter.eanim_play_mode);
        if (ImGui::RadioButton("Play Single Animtion", &anim_play_mode, static_cast<int>(EAnimtionPlayMode::eSingle)))
        {
            render_parameter.play_single_anim_para = PlaySingleAnimParameter();
            play_anim_changed_ = true;
        }
        if (ImGui::RadioButton("Blend Animtions", &anim_play_mode, static_cast<int>(EAnimtionPlayMode::eBlend)))
        {
            render_parameter.blend_anim_para = BlendAnimParameter();
            play_anim_changed_ = true;
        }
        if (ImGui::RadioButton("Transition Animtions", &anim_play_mode, static_cast<int>(EAnimtionPlayMode::eTransition)))
        {
            render_parameter.transition_anim_para = TransitionAnimParameter();
            play_anim_changed_ = true;
        }
        render_parameter.eanim_play_mode = static_cast<EAnimtionPlayMode>(anim_play_mode);
        ImGui::NewLine();

        int anim_cnt = render_parameter.anim_names.size();
        switch (render_parameter.eanim_play_mode)
        {
        case EAnimtionPlayMode::eSingle:
            play_anim_changed_ = ImGui::SliderInt("Index", &render_parameter.play_single_anim_para.anim_index, 0, anim_cnt - 1);
            GuiAnimationTimeLine(&render_parameter.play_single_anim_para.normalized_time, 1.0f, true);
            break;

        case EAnimtionPlayMode::eBlend:
            play_anim_changed_ = ImGui::SliderInt("Index1", &render_parameter.blend_anim_para.anim_index1, 0, anim_cnt - 1);
            ImGui::SliderInt("Index2", &render_parameter.blend_anim_para.anim_index2, 0, anim_cnt - 1);
            ImGui::SliderFloat("Weight", &render_parameter.blend_anim_para.anim_blend_weight, 0.0f, 1.0f);
            GuiAnimationTimeLine(&render_parameter.blend_anim_para.normalized_time, 1.0f, true);
            break;

        case EAnimtionPlayMode::eTransition:
            play_anim_changed_ = ImGui::SliderInt("Index1", &render_parameter.transition_anim_para.anim_index1, 0, anim_cnt - 1);
            ImGui::SliderInt("Index2", &render_parameter.transition_anim_para.anim_index2, 0, anim_cnt - 1);
            ImGui::SliderFloat("Transition Begin Time", &render_parameter.transition_anim_para.begin_trans_time_in_sec, 0.0f, anim_duration_);
            GuiAnimationTimeLine(&render_parameter.transition_anim_para.time_in_sec, 
                render_parameter.transition_anim_para.begin_trans_time_in_sec + render_parameter.anim_durations[render_parameter.transition_anim_para.anim_index2], false);
            break;
        }

        ImGui::End();
	}

private:
    void GuiAnimationTimeLine(float* p_time, float time_end, bool use_normalized_time) {
        ImGui::Checkbox("Auto Play", &auto_play_);

        UpdateTime(time_end);

        if (use_normalized_time == true)
        {
            ImGui::SliderFloat(" ", &anim_normalized_time_, 0.0f, time_end);
            *p_time = anim_normalized_time_;
        }
        else
        {
            ImGui::SliderFloat(" ", &anim_time_in_sec_, 0.0f, time_end);
            *p_time = anim_time_in_sec_;
        }
    }

    void UpdateTime(float time_end) {
        float time_test = glfwGetTime();
        float sec_elapsed = glfwGetTime() - local_clock_;
        local_clock_ += sec_elapsed;

        if (auto_play_ == true)
        {
            anim_time_in_sec_ += sec_elapsed;
            anim_normalized_time_ += sec_elapsed * (1.0f / anim_duration_);
            ClampTime(time_end);
        }
    }

    void ClampTime(float time_end) {
        if (anim_time_in_sec_ > time_end)
        {
            anim_time_in_sec_ = 0.0f;
        }

        if (anim_normalized_time_ > 1.0f)
        {
            anim_normalized_time_ = 0.0f;
        }
    }

    void UpdateAnimDuration(const RenderParameter& render_parameter) {
        switch (render_parameter.eanim_play_mode)
        {
        case EAnimtionPlayMode::eSingle:
            anim_duration_ = render_parameter.anim_durations[render_parameter.play_single_anim_para.anim_index];
            break;

        case EAnimtionPlayMode::eBlend:
            anim_duration_ = render_parameter.anim_durations[render_parameter.blend_anim_para.anim_index1];
            break;

        case EAnimtionPlayMode::eTransition:
            anim_duration_ = render_parameter.anim_durations[render_parameter.transition_anim_para.anim_index1];
            break;
        }

        play_anim_changed_ = false;
    }

    bool play_anim_changed_ = true;

    bool auto_play_ = true;

    float anim_duration_ = 0.0f;
    float anim_normalized_time_ = 0.0f;
    float anim_time_in_sec_ = 0.0f;

    float local_clock_ = 0.0f;
};

#endif