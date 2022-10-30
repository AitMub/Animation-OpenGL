#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include"camera.h"
#include"model.h"
#include"light.h"
#include"render_parameter.h"
#include"shader.h"

class RenderScene
{
public:
	RenderScene(Model model, Camera camera = Camera(), Light light = Light())
		: model_(model), camera_(camera), light_(light), render_parameter_(model.HaveAnimation(), model.GetAnimationNameList()) {}

	void Draw(Shader& shader) {
		static int temp_time = 0;
		temp_time++;
		float n_t = temp_time / 5000.0f;

		if (render_parameter_.have_animtion == true)
		{
			switch (render_parameter_.eanim_play_mode)
			{
			case EAnimtionPlayMode::eSingle:
				model_.PlaySingleAnimation(render_parameter_.play_single_anim.anim_index, n_t);
				break;
			case EAnimtionPlayMode::eBlend:
				model_.BlendAnimation1D(render_parameter_.blend_anim.anim_index1, render_parameter_.blend_anim.anim_index2, 
					n_t, render_parameter_.blend_anim.anim_blend_weight);
				break;
			case EAnimtionPlayMode::eTransition:
				model_.PlayAnimtionTransition(render_parameter_.transition_anim.anim_index1, render_parameter_.transition_anim.anim_index2,
					n_t, render_parameter_.transition_anim.begin_trans_norm_time);
				break;
			}
		}

		model_.Draw(shader);
	}

	Model model_;
	Camera camera_;
	Light light_;
	RenderParameter render_parameter_;
};

#endif