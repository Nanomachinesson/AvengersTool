#include "pch.h"
#include "ui_bounceinfo.h"

ui_bounceinfo::ui_bounceinfo(Avengers* avengers) :
	avengers(avengers)
{
}

ui_bounceinfo::~ui_bounceinfo()
{
}

void ui_bounceinfo::renderRpgTimer()
{
	static int pmFlagsLastFrame = 0;
	static float lastFrameWeaponDelay = 0.f;
	static int framesSinceLastBounce = 0;
	static std::chrono::time_point lastTimeShotRpg = std::chrono::system_clock::now();
	pmove_t* pm = avengers->inst_game->get_pmove_current();
	if (!(pm && pm->ps)) {
		return;
	}

	std::chrono::time_point now = std::chrono::system_clock::now();
	float VELO_CUTOFF = 5.f;
	int BOUNCE_RPG_TIME_CUTOFF = 350;
	bool onGround = avengers->inst_game->isOnGround();
	int pmFlags = pm->ps->pm_flags;
	float velo = avengers->inst_game->get_velocity().Length2D();
	float weaponDelay = pm->ps->weaponDelay;
	bool bounced = false;
	bool shotRpg = false;

	if (((pmFlags & PMF_JUMPING) == 0) && ((pmFlagsLastFrame & PMF_JUMPING) != 0) && velo >= VELO_CUTOFF && !onGround) {
		bounced = true;
	}
	if (weaponDelay == 0.f && lastFrameWeaponDelay != 0.f && pm->ps->weaponstate != 7) {
		shotRpg = true;
	}

	int timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeShotRpg).count();
	if (bounced) {
		if (timeDiff < 200 && velo > VELO_CUTOFF) {
			avengers->inst_game->add_obituary("^1Early");
		}
		framesSinceLastBounce = 0;
	}

	if (shotRpg && timeDiff > 20) {
		if (framesSinceLastBounce < BOUNCE_RPG_TIME_CUTOFF) {
			std::string switchtimeColor;

			if (framesSinceLastBounce < 15) {
				switchtimeColor = "^2";
			}
			else if (framesSinceLastBounce < 50) {
				switchtimeColor = "^3";
			}
			else {
				switchtimeColor = "^1";
			}

			std::string rpgTimerText;
			if (framesSinceLastBounce > 0) {
				rpgTimerText = "RPG Delay: " + switchtimeColor + std::to_string(framesSinceLastBounce) + " Frames";
			}
			else {
				rpgTimerText = "^2Perfect";
			}
			avengers->inst_game->add_obituary(rpgTimerText);
		}
		shotRpg = false;
		lastTimeShotRpg = now;
	}

	framesSinceLastBounce++;
	lastFrameWeaponDelay = weaponDelay;
	pmFlagsLastFrame = pmFlags;
}

void ui_bounceinfo::renderBounceVelocity()
{
	static int pmFlagsLastFrame = 0;
	static float lastFrameVelo = 0.f;
	pmove_t* pm = avengers->inst_game->get_pmove_current();
	if (!(pm && pm->ps)) {
		return;
	}

	float VELO_CUTOFF = 5.f;
	bool onGround = avengers->inst_game->isOnGround();
	int pmFlags = pm->ps->pm_flags;
	float velo = avengers->inst_game->get_velocity().Length2D();
	bool bounced = false;

	if (((pmFlags & PMF_JUMPING) == 0) && ((pmFlagsLastFrame & PMF_JUMPING) != 0) && velo >= VELO_CUTOFF && !onGround) {
		bounced = true;
	}

	if (bounced) {
		std::string bounceVeloText = "Velocity on bounce: ^5" + std::to_string(velo);
		float veloLost = lastFrameVelo - velo;
		if (veloLost > 0) {
			bounceVeloText += "^7, Velocity lost: ^1" + std::to_string(veloLost);
		}
		avengers->inst_game->add_obituary(bounceVeloText);
	}

	lastFrameVelo = velo;
	pmFlagsLastFrame = pmFlags;
}
