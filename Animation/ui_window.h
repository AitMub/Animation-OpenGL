#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include "render_parameter.h"

class UIWindow
{
public:
	virtual void Render(RenderParameter&) = 0;

	virtual void SetWindowFlags(ImGuiWindowFlags window_flags) {
		window_flags_ = window_flags;
	}

protected:
	ImGuiWindowFlags window_flags_ = 0;
};

#endif