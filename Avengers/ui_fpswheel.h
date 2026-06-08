#pragma once
#include <map>
#include "cod4Structs.h"
#include "vectors.h"
#include "game_math.h"

class ui_fpswheel
{
public:
	ui_fpswheel(class Avengers* hud);
	~ui_fpswheel();
	void render(class Avengers* hud);
	vec2<float> getCurrentZoneBounds();

private:
	vec2<float> moveZone(const vec2<float>& zone);
	std::multimap<int, vec2<float>> fpsZones
	{
		//CENTER 90
		{125, vec2<float>(103.31f, 115.72f)},
		{200, vec2<float>(93.88f, 103.31f)},
		{250, vec2<float>(73.6f, 86.5f)},
		{251, vec2<float>(86.5f, 93.88f)},
		{333, vec2<float>(25.72f, 73.6f)},
	};;
};
