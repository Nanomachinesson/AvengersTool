#pragma once

#include "imgui.h"
#include "vectors.h"

class ui_bounceinfo
{
public:
	ui_bounceinfo(Avengers* avengers);
	~ui_bounceinfo();
	void renderRpgTimer();
	void renderBounceVelocity();

private:
	Avengers* avengers;

};
