#pragma once

#include <sstream>

class ui_bounceinfo
{
public:
	ui_bounceinfo(Avengers* avengers);
	~ui_bounceinfo();
	void renderRpgTimer();
	void renderRpgAngle();
	void render5Timing();
	void renderBounceVelocity();

private:
	Avengers* avengers;
	std::string getStringWithSignificantFigures(float a, int precision);

};
