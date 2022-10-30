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

struct PlaySingleAnim
{
	int anim_index;
};

struct BlendAnim
{
	int anim_index1;
	int anim_index2;
	float anim_blend_weight;
};

struct TransitionAnim
{
	int anim_index1;
	int anim_index2;
	float begin_trans_norm_time;
};

struct RenderParameter
{
	const bool have_animtion = true;
	const std::vector<std::string> anim_names;

	float play_speed = 1.0f;

	EAnimtionPlayMode eanim_play_mode = EAnimtionPlayMode::eSingle;

	union
	{
		PlaySingleAnim play_single_anim;
		BlendAnim blend_anim;
		TransitionAnim transition_anim;
	};

	RenderParameter(bool _have_animation, const std::vector<std::string>& _anim_names)
		: have_animtion(_have_animation), anim_names(_anim_names) {};
};

#endif