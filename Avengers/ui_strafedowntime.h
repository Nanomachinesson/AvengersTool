#pragma once

#include "imgui.h"
#include "vectors.h"

class ui_strafedowntime
{
public:
	ui_strafedowntime(class Avengers* hud);
	~ui_strafedowntime();
	void render();

private:
	std::string lMoveToKey(const Lmove& lMove);
	Avengers* avengers;
	const float VELO_CUTOFF = 0.f;
	const int SWITCHTIME_CUTOFF_GREEN = 100.f;
	const int SWITCHTIME_CUTOFF_YELLOW = 150.f;
	const int MAX_SWITCHTIME_CUTOFF = 500.f;
};
