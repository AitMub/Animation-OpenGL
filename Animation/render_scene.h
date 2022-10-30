#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include"camera.h"
#include"model.h"
#include"light.h"
#include"render_parameter.h"

class RenderScene
{
public:
	RenderScene(Model model, Camera camera = Camera(), Light light = Light())
		: model_(model), camera_(camera), light_(light), render_parameter_(model.HaveAnimation(), model.GetAnimationNameList()) {}

	Model model_;
	Camera camera_;
	Light light_;
	RenderParameter render_parameter_;
};

#endif