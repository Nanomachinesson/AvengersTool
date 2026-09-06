#pragma once
#include "imgui.h"
#include "vectors.h"

class ui_velocity
{
public:
	ui_velocity(class Avengers* hud);
	~ui_velocity();
	void render(Avengers*& hud, bool& isLocked, vec2<float>& pos, float& scale, ImVec4& color);
	void renderJumpoffSpeed(Avengers*& hud, vec2<float>& pos, float& scale, ImVec4& color);
};
