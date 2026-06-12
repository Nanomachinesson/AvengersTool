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

GameState::GameState(Avengers* avengers)
{
	avengers->inst_hooks->Add("cg_predictPlayerState", addr_cg_predictPlayerState, predictPlayerState, hook_type_detour);
}
