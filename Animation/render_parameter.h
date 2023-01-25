#ifndef RENDER_PARAMETER_H
#define RENDER_PARAMETER_H

#include<vector>
#include<string>

enum class EAnimtionPlayMode
{
	eSingle,
	eBlend,
	eTransition
};

struct PlaySingleAnimParameter
{
	int anim_index = 0;
	float normalized_time = 0.0f;
};

struct BlendAnimParameter
{
	int anim_index1 = 0;
	int anim_index2 = 0;
	float anim_blend_weight = 0.0f;
	float normalized_time = 0.0f;
};

struct TransitionAnimParameter
{
	int anim_index1 = 0;
	int anim_index2 = 0;
	float begin_trans_time_in_sec = 0.0f;
	float time_in_sec = 0.0f;
};

struct RenderParameter
{
	const bool have_animtion = true;
	const std::vector<std::string> anim_names;
	const std::vector<float> anim_durations;

	EAnimtionPlayMode eanim_play_mode = EAnimtionPlayMode::eSingle;

	union
	{
		PlaySingleAnimParameter play_single_anim_para;
		BlendAnimParameter blend_anim_para;
		TransitionAnimParameter transition_anim_para;
	};

	RenderParameter(bool _have_animation, const std::vector<std::string>& _anim_names, const std::vector<float>& _anim_durations)
		: have_animtion(_have_animation), anim_names(_anim_names), anim_durations(_anim_durations) {};
};

#endif