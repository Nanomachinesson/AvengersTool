#pragma once
#include "CodAddresses.h"
#include "vectors.h"
#include "Lmove.h"

class GameState
{
public:
	GameState(Avengers* avengers);
	vec3<float> velocity;
	vec3<float> origin;
	bool onGround = true;
	Lmove lMove{};
	int pmFlags;
	float weaponDelay;
	int weaponState;
};
