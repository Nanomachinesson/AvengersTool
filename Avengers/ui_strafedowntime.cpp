#include "pch.h"
#include "ui_strafedowntime.h"

ui_strafedowntime::ui_strafedowntime(Avengers* avengers) :
	avengers(avengers)
{
}

ui_strafedowntime::~ui_strafedowntime()
{
}

void ui_strafedowntime::render()
{
	std::shared_ptr<game> inst_game = avengers->inst_game;

	Lmove lmove = inst_game->get_lmove(true);
	static Lmove acceleratingFrom = lmove;
	static int keysPressedLastFrame = 0;
	static float lastFrameVelocity = 0.f;
	static std::chrono::time_point lastAccelerationPoint = std::chrono::system_clock::now();
	static bool measuringSwitch = false;

	float velocity = inst_game->get_velocity().Length2D();
	int keysPressed = static_cast<int>(lmove.isBack) + static_cast<int>(lmove.isForward) * 2
		+ static_cast<int>(lmove.isLeft) * 4 + static_cast<int>(lmove.isRight) * 8;
	bool didAccelerate = velocity > lastFrameVelocity;
	std::chrono::time_point now = std::chrono::system_clock::now();

	if (!inst_game->isOnGround() && velocity > VELO_CUTOFF && !inst_game->is_spectating() && !inst_game->is_noclipping()) {
		if (keysPressed != keysPressedLastFrame) {
			measuringSwitch = true;
		}

		if (didAccelerate) {
			if (measuringSwitch) {
				std::string switchtimeColor;
				int switchTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAccelerationPoint).count();
				
				if (switchTime < SWITCHTIME_CUTOFF_GREEN) {
					switchtimeColor = "^2";
				}
				else if(switchTime < SWITCHTIME_CUTOFF_YELLOW) {
					switchtimeColor = "^3";
				}
				else {
					switchtimeColor = "^1";
				}

				std::string color = "^6";
				std::string fromText = lMoveToKey(acceleratingFrom);
				std::string toText = lMoveToKey(lmove);
				std::string switchTimeText = std::to_string(switchTime) + "ms";

				std::string switchText = color + fromText + "-" + toText + ": " + switchtimeColor + switchTimeText;
				if (switchTime < MAX_SWITCHTIME_CUTOFF) {
					inst_game->add_obituary(switchText);
				}
				measuringSwitch = false;
			}

			lastAccelerationPoint = now;
			acceleratingFrom = lmove;
		}
	}
	else {
		didAccelerate = false;
		measuringSwitch = false;
	}

	lastFrameVelocity = velocity;
	keysPressedLastFrame = keysPressed;
}

std::string ui_strafedowntime::lMoveToKey(const Lmove& lMove)
{
	if (!lMove.isForward) {
		if (!lMove.isBack) {
			if (lMove.isRight) {
				return "D";
			}
			else if (lMove.isLeft) {
				return "A";
			}
			else {
				return "";
			}
		}
		else {
			if (lMove.isRight) {
				return "SD";
			}
			else if (lMove.isLeft) {
				return "SA";
			}
			else {
				return "S";
			}
		}

	}
	else {
		if (lMove.isRight) {
			return "WD";
		}
		else if (lMove.isLeft) {
			return "WA";
		}
		else {
			return "W";
		}
	}
}
