#include "pch.h"
#include "ui_bounceinfo.h"

bool bouncedInProjVel = false;
void __cdecl PM_StepSlideMove_hook(pmove_t* pm, void* pml, bool gravity)
{
	Avengers* hud = Avengers::get_instance();
	bouncedInProjVel = false;
	hud->inst_hooks->hook_map["PM_StepSlideMove"]->original(PM_StepSlideMove_hook)(pm, pml, gravity);
}

int tramp;

__declspec(naked)
void PM_ProjectVelocity()
{
	_asm {
		mov bouncedInProjVel, 1
		jmp addr_projectvelocity
	}
}

ui_bounceinfo::ui_bounceinfo(Avengers* avengers) :
	avengers(avengers)
{
	avengers->inst_hooks->Add("PM_StepSlideMove", addr_stepslidemove, PM_StepSlideMove_hook, hook_type_detour);

	DWORD dwOldProtect;
	_MEMORY_BASIC_INFORMATION mbi = { 0,0,0,0,0,0,0 };
	VirtualQuery((LPVOID)addr_projectvelocity_in_stepslidemove, &mbi, sizeof(mbi));
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	BYTE* callInstruction = (BYTE*)addr_projectvelocity_in_stepslidemove;
	*callInstruction = 0xE8;
	DWORD relativeAddress = (DWORD)PM_ProjectVelocity - (DWORD)addr_projectvelocity_in_stepslidemove - 5;
	*(DWORD*)((DWORD)addr_projectvelocity_in_stepslidemove + 1) = relativeAddress;

	VirtualProtect((LPVOID)addr_projectvelocity_in_stepslidemove, 1000, dwOldProtect, &dwOldProtect);
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
	auto gameState = avengers->gameState;

	std::chrono::time_point now = std::chrono::system_clock::now();
	float VELO_CUTOFF = 5.f;
	int BOUNCE_RPG_TIME_CUTOFF = 350;
	bool onGround = gameState->onGround;
	int pmFlags = gameState->pmFlags;
	float velo = gameState->velocity.Length2D();
	float weaponDelay = gameState->weaponDelay;
	bool bounced = bouncedInProjVel && !onGround && !avengers->inst_game->is_noclipping() && !avengers->inst_game->is_spectating() && velo > 0.f;
	bool shotRpg = false;

	if (weaponDelay == 0.f && lastFrameWeaponDelay != 0.f && gameState->weaponState != 7) {
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
				std::string frameText = "Frames";
				if (framesSinceLastBounce == 1) {
					frameText = "Frame";
				}
				rpgTimerText = "RPG Delay: " + switchtimeColor + std::to_string(framesSinceLastBounce) + " " + frameText;
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

void ui_bounceinfo::renderRpgAngle()
{
	static float lastFrameWeaponDelay = 0.f;
	auto gameState = avengers->gameState;

	pmove_t* pm = avengers->inst_game->get_pmove_current();
	if (!(pm && pm->ps)) {
		return;
	}

	float weaponDelay = gameState->weaponDelay;
	float pitch = pm->ps->viewangles[0];

	if (weaponDelay == 0.f && lastFrameWeaponDelay != 0.f && gameState->weaponState != 7) {
		avengers->inst_game->add_obituary("RPGAngle: ^5" + getStringWithSignificantFigures(pitch, 3));
	}

	lastFrameWeaponDelay = weaponDelay;
}

void ui_bounceinfo::renderBounceVelocity()
{
	static int pmFlagsLastFrame = 0;
	static float lastFrameVelo = 0.f;
	auto gameState = avengers->gameState;

	pmove_t* pm = avengers->inst_game->get_pmove_current();
	if (!(pm && pm->ps)) {
		return;
	}

	float VELO_CUTOFF = 5.f;
	bool onGround = gameState->onGround;
	int pmFlags = gameState->pmFlags;
	float velo = gameState->velocity.Length2D();
	bool bounced = bouncedInProjVel && !onGround && !avengers->inst_game->is_noclipping() && !avengers->inst_game->is_spectating() && velo > 0.f;  //JH noclip sets your velo to 0 but is_noclipping doesn't work there

	if (bounced) {
		std::string veloText = getStringWithSignificantFigures(velo, 3);
		std::string veloTextZ = getStringWithSignificantFigures(gameState->velocity.z, 0);

		std::string bounceVeloText = "Bounce: ^5" + veloText + " ^7z: " + "^5" + veloTextZ;
		float veloLost = lastFrameVelo - velo;
		if (veloLost > 0) {
			std::string veloLostString = getStringWithSignificantFigures(veloLost, 3);
			bounceVeloText += "^7, Velocity lost: ^1" + veloLostString;
		}
		avengers->inst_game->add_obituary(bounceVeloText);
	}

	lastFrameVelo = velo;
	pmFlagsLastFrame = pmFlags;
}

std::string ui_bounceinfo::getStringWithSignificantFigures(float a, int precision)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << a;
	std::string text = stream.str();

	return text;
}
