#pragma once

#include <sstream>
#include "imgui.h"
#include "vectors.h"

class ui_bounceinfo
{
public:
	ui_bounceinfo(Avengers* avengers);
	~ui_bounceinfo();
	void renderRpgTimer();
	void renderRpgAngle();
	void renderBounceVelocity();

private:
	Avengers* avengers;
	std::string getStringWithSignificantFigures(float a, int precision);

};
