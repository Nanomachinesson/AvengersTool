#include "pch.h"
#include "vectors.h"
#include "game.h"


float convertToCodPitch(float pitch)
{
	if (pitch > 0)
		pitch = 360 - pitch;
	else
		pitch = fabs(pitch);
	return pitch;
}
