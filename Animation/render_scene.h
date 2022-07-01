#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include"camera.h"
#include"model.h"
#include"light.h"

class RenderScene
{
public:
	RenderScene(Model model, Camera camera = Camera(), Light light = Light())
		: model_(model), camera_(camera), light_(light) {}

	Model model_;
	Camera camera_;
	Light light_;
};

#endif