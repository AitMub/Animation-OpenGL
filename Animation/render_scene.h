#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include"camera.h"
#include"model.h"
#include"light.h"
#include"render_parameter.h"
#include"shader.h"
#include"render_volume.h"

using glm::mat4;

// Scene that contains one model and one point light
class RenderScene
{
public:
	RenderScene(Model model, Shader shader, RenderVolume render_volume, Camera camera = Camera(), Light light = Light())
		: model_(model), shader_(shader), camera_(camera), light_(light), 
		render_parameter_(model.HaveAnimation(), model.GetAnimationNameList(), model.GetAnimationDurationList()) 
	{
		shader_.use();

		projection_mat_ = glm::perspective(glm::radians(render_volume.fov_in_degree),
			(float)render_volume.screen_width / (float)render_volume.screen_height, 
			render_volume.near_z, render_volume.far_z);
	}

	void Draw() { 
		PassUniforms();

		if (render_parameter_.have_animtion == true)
		{
			switch (render_parameter_.eanim_play_mode)
			{
			case EAnimtionPlayMode::eSingle:
				model_.PlaySingleAnimation(render_parameter_.play_single_anim.anim_index, render_parameter_.play_single_anim.normalized_time);
				break;
			case EAnimtionPlayMode::eBlend:
				model_.BlendAnimation1D(render_parameter_.blend_anim.anim_index1, render_parameter_.blend_anim.anim_index2,
					render_parameter_.blend_anim.normalized_time, render_parameter_.blend_anim.anim_blend_weight);
				break;
			case EAnimtionPlayMode::eTransition:
				model_.PlayAnimationTransition(render_parameter_.transition_anim.anim_index1, render_parameter_.transition_anim.anim_index2,
					render_parameter_.transition_anim.time_in_sec, render_parameter_.transition_anim.begin_trans_time_in_sec);
				break;
			}
		}

		model_.Draw(shader_);
	}

	void SetTransform(vec3 position, float rotate_angle, vec3 rotate_axis, vec3 scale) {
		model_mat_ = mat4(1.0f);
		model_mat_ = glm::translate(model_mat_, position);
		model_mat_ = glm::rotate(model_mat_, glm::radians(rotate_angle), rotate_axis);
		model_mat_ = glm::scale(model_mat_, scale);
	}

	Model model_;
	Camera camera_;
	Light light_;
	RenderParameter render_parameter_;
	Shader shader_;

private:
	mat4 projection_mat_;

	mat4 model_mat_ = mat4(1.0f);
	mat4 model_translation_ = mat4(1.0f);
	mat4 model_scale_ = mat4(1.0f);
	mat4 model_rotate_ = mat4(1.0f);

	void PassUniforms() const {
		shader_.setMat4("projection", projection_mat_);
		shader_.setMat4("view", camera_.GetViewMatrix());
		shader_.setMat4("model", model_mat_);

		shader_.setVec3("lightColor", light_.color_);
		shader_.setVec3("lightPos", light_.pos_);

		shader_.setVec3("viewPos", camera_.Position);
	}
};
#endif