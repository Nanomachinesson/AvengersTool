#include "pch.h"
#include "game_state.h"

void predictPlayerState(int unk)
{
	Avengers* avengers = Avengers::get_instance();
	auto game = avengers->inst_game;
	avengers->inst_hooks->hook_map["cg_predictPlayerState"]->original(predictPlayerState)(unk);
	
	pmove_t* pm = avengers->inst_game->get_pmove_current();
	if (!(pm && pm->ps)) {
		return;
	}

	auto gameState = avengers->gameState;

	gameState->onGround = game->isOnGround();
	gameState->lMove = game->get_lmove(true);
	gameState->origin = game->get_origin();
	gameState->velocity = game->get_velocity();
	gameState->weaponDelay = pm->ps->weaponDelay;
	gameState->pmFlags = pm->ps->pm_flags;
	gameState->weaponState = pm->ps->weaponstate;
}

int __cdecl FS_CompareIwds(char* needediwds, int len, int dlstring)
{
	Avengers* avengers = Avengers::get_instance();
	int* pIwdSums = (int*)addr_serveriwdsums;
	char** pIwdNames = (char**)addr_serveriwdnames;
	int numIds = *(int*)addr_servernumiwds;

	if (!avengers->inst_ui_menu->allow_impure_map_iwds) {
		int orig = avengers->inst_hooks->hook_map["FS_CompareIwds"]->original(FS_CompareIwds)(needediwds, len, dlstring);
		return orig;
	}

	//iwdGamename's are prefixed with usermaps/
	std::string iwdName = "";
	int iwdChecksum = 0;

	for (int i = 0; i < numIds; i++) {
		iwdName = pIwdNames[i];
		if (iwdName.starts_with("mp_")) {
			iwdName = "usermaps/" + std::string(pIwdNames[i]);
			iwdChecksum = pIwdSums[i];
			break;
		}
	}

	if (iwdChecksum != 0) {
		typedef void FS_AddUserMapDirIWD(void);
		FS_AddUserMapDirIWD* f = (FS_AddUserMapDirIWD*)addr_FS_AddUserMapDirIwd;

		//Call this function so the searchpath changes to our usermaps/<map> folder
		const char* folder = iwdName.data();
		_asm {
			mov edi, folder
		}
		f();

		//First searchpath is invalid
		Searchpath_s* searchPathBase = ((Searchpath_s*)addr_searchpathbase)->next;

		for (Searchpath_s* path = searchPathBase; path; path = path->next) {
			if (path->Iwd) {
				if (path->Iwd->iwdGamename == iwdName) {
					//Hack the loaded checksum to match the one sent by the server
					path->Iwd->checksum = iwdChecksum;
				}
			}
		}
	}

	int orig = avengers->inst_hooks->hook_map["FS_CompareIwds"]->original(FS_CompareIwds)(needediwds, len, dlstring);
	return orig;
}

GameState::GameState(Avengers* avengers)
{
	avengers->inst_hooks->Add("cg_predictPlayerState", addr_cg_predictPlayerState, predictPlayerState, hook_type_detour);
	avengers->inst_hooks->Add("FS_CompareIwds", addr_FS_CompareIwds, FS_CompareIwds, hook_type_detour);
}
