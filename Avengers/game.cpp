#include "pch.h"
#include "Game.h"


HWND game::get_window()
{
	return *(HWND*)0xCC1B6FC;
}
bool game::is_focused()
{
	return *(bool*)0x0CC1B704;
}

bool game::is_in_main_menu()
{
	return *reinterpret_cast<int*>(addr_ingame) == connection_state_uninitialized;
}

LPDIRECT3DDEVICE9 game::get_device()
{
	return *(LPDIRECT3DDEVICE9*)0xCC9A408;
}

bool game::is_connected()
{
	int state = *(int*)addr_ingame;
	return state == connection_state_connected;
}

vec3<float> game::get_view()
{
	vec3<float> deltaAngles = *reinterpret_cast<vec3<float>*>(addr_deltaAngles);
	vec3<float> cameraAngles = *reinterpret_cast<vec3<float>*>(addr_writeableAngles);
	vec3<float> normalized;
	pmove_t* pm = get_pmove_current();

	if (!is_spectating()) {
		normalized.x = mm::normalise(cameraAngles.x + deltaAngles.x, 0.f, 360.f);
		normalized.y = mm::normalise(cameraAngles.y + deltaAngles.y, 0.f, 360.f);
		normalized.z = mm::normalise(cameraAngles.z + deltaAngles.z, 0.f, 360.f);
	}
	else if (pm && pm->ps) {
		normalized.x = mm::normalise(pm->ps->viewangles[0], 0.f, 360.f);
		normalized.y = mm::normalise(pm->ps->viewangles[1], 0.f, 360.f);
		normalized.z = mm::normalise(pm->ps->viewangles[2], 0.f, 360.f);
	}

	return normalized;
}

vec3<float> game::get_origin()
{
	pmove_t* pm = get_pmove_current();
	vec3<float> origin{};
	if (pm && pm->ps) {
		origin = pm->ps->origin;
	}

	return origin;
}

vec3<float> game::get_velocity()
{
	pmove_t* pm = get_pmove_current();
	vec3<float> velocity{};
	if (pm && pm->ps) {
		velocity = pm->ps->velocity;
	}

	return velocity;
}

float game::get_dir_diff()
{
	return get_dir_diff(get_lmove(true));
}

float game::get_dir_diff(const Lmove& lMove)
{
	if (lMove.isForward) {
		return (lMove.isRight) ? -45.f : 45.f;
	}
	else if (lMove.isBack) {
		return 180.f;
	}
	else {
		return (lMove.isRight) ? -90.f : +90.f;
	}

	return 0;
}

Lmove game::get_lmove(bool adjustForSpectator)
{
	using namespace mm;
	input_s* input = (input_s*)addr_usercmd;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	Lmove lMove;
	static Lmove prevLmove {};

	static float prevYaw = 0.f;
	float yaw = get_view().y;

	lMove.isInAir = !Avengers::get_instance()->inst_game->isOnGround();
	lMove.isSprint = *reinterpret_cast<int*>(addr_sprint) >= 20 || *reinterpret_cast<int*>(addr_sprint) == 5 ? true : false;
	lMove.isMoving = fabsf(get_velocity().Length2D()) > 0 ? true : false;
	lMove.fullLean = *reinterpret_cast<float*>(addr_lean) >= 0.5f || *reinterpret_cast<float*>(addr_lean) >= 0.25f ? true : false;

	if (cmd->forward != 0)
	{
		lMove.isBack = cmd->forward == 129 ? true : false;
		lMove.isForward = cmd->forward == 127 ? true : false;
	}
	else
	{
		lMove.isBack = false;
		lMove.isForward = false;
	}

	if (cmd->side != 0)
	{
		lMove.isRight = cmd->side == 127 ? true : false;
		lMove.isLeft = cmd->side == 129 ? true : false;
	}
	else
	{
		lMove.isLeft = false;
		lMove.isRight = false;
	}


	float CHANGE_TOLERANCE = 4.f;
	float MINIMUM_VELO = 200.f;
	if (adjustForSpectator && game::is_spectating() && fabsf(yaw - prevYaw) <= CHANGE_TOLERANCE && get_velocity().Length2D() >= MINIMUM_VELO) {
		if (yaw > prevYaw) {
			lMove.isLeft = true;
			lMove.isRight = false;
		}
		else if (yaw < prevYaw) {
			lMove.isLeft = false;
			lMove.isRight = true;
		}
		else {
			lMove.isRight = prevLmove.isRight;
			lMove.isLeft = prevLmove.isLeft;
		}

		Lmove forwardLmove = lMove;
		Lmove noForwardLmove = lMove;
		Lmove backMove = lMove;
		forwardLmove.isForward = true;
		noForwardLmove.isForward = false;
		backMove.isForward = false;
		backMove.isLeft = false;
		backMove.isRight = false;
		backMove.isBack = true;

		float optimalAngleForward = get_optimal_angle(forwardLmove);
		float optimalAngleNoForward = get_optimal_angle(noForwardLmove);
		float optimalAngleBack = get_optimal_angle(backMove);

		float forwardDiff = fabsf(yaw - optimalAngleForward);
		float noForwardDiff = fabsf(yaw - optimalAngleNoForward);
		float backDiff = fabsf(yaw - optimalAngleBack);

		if (forwardDiff <= noForwardDiff && forwardDiff <= backDiff) {
			lMove.isForward = true;
			lMove.isBack = false;
		}
		else if (noForwardDiff <= forwardDiff && noForwardDiff <= backDiff) {
			lMove.isForward = false;
			lMove.isBack = false;
		}
		else {
			lMove.isForward = false;
			lMove.isBack = true;
		}
	}

	prevYaw = yaw;
	prevLmove = lMove;
	return lMove;
}

float game::get_velocity_angle()
{
	using namespace mm;
	vec3<float> velocity = get_velocity();
	return normalise(tilt_angle(truncate_vector(get_velocity())), 0.f, 360.f);
}

float game::get_delta()
{
	return get_delta(get_lmove(true));
}

float game::get_delta(const Lmove& lMove)
{
	float accelAngle = 0.f;

	accelAngle = mm::normalise(get_view().y + get_dir_diff(lMove), 0.f, 360.f);

	float delta = get_velocity_angle() - accelAngle;
	return mm::normalise(delta, -180.f, 180.f);
}

float game::get_delta_optimal()
{
	return get_delta_optimal(get_lmove(true));
}

float game::get_delta_optimal(const Lmove& lMove)
{
	constexpr float g_speed = 190.f;

	float speed = get_velocity().Length2D();
	float deltaOpt = mm::to_degrees(acosf((g_speed - get_accel()) / speed));

	if (lMove.isLeft)
	{
		deltaOpt *= -1.f;
	}

	return deltaOpt;
}

int game::get_fps(bool adjustForSpectator)
{
	int maxFps = 0;  //TODO: make this stuff work for spectator
	if (adjustForSpectator && is_spectating()) {
		maxFps = get_fps_3_xp();
	}
	else {
		cvar_t* maxFpsCvar = getCvar("com_maxfps");
		maxFps = maxFpsCvar->current.integer;
	}
	return maxFps;
}

float game::get_accel()
{
	return g_speed / get_fps();
}

float game::get_optimal_angle()
{
	return get_optimal_angle(get_lmove(true));
}

float game::get_optimal_angle(const Lmove& lMove)
{
	float delta = get_delta(lMove);
	float deltaOptimal = get_delta_optimal(lMove);

	float yaw = get_view().y;

	if (lMove.isBack) {  //If the user is doing s-tech we need to account for both strafe sides
		if (decideStechSide(lMove)) {
			deltaOptimal *= -1.f;
		}
	}

	yaw += delta - deltaOptimal;

	return mm::normalise(yaw, 0.f, 360.f);
}

bool game::isOnGround()
{
	bool onGround = false;
	pmove_t* pm = get_pmove_current();
	if (pm && pm->ps) {
		onGround = pm->ps->groundEntityNum != 1023;
	}
	return onGround;
}

int game::getJumpTime()
{
	auto ps = (playerState_t*)(0x794474);

	return ps->JumpTime;
}

vec2<float> game::get_screen_res()
{
	cg_t* ref = (cg_t*)0x0074E338;
	vec2<float> screenres{};
	screenres.x = ref->Refdef.ScreenWidth;
	screenres.y = ref->Refdef.ScreenHeight;
	return screenres;
}

vec3<float> game::get_delta_angles()
{
	return *reinterpret_cast<vec3<float>*>(addr_delta_angles);
}

float game::get_fov()
{
	cvar_t* fov = getCvar("cg_fov");
	cvar_t* fovScale = getCvar("cg_fovScale");
	return fov->current.value * fovScale->current.value;
}

pmove_t* game::get_pmove_current()
{
	return reinterpret_cast<pmove_t*>(addr_pMove_current);
}

bool game::is_spectating()
{
	pmove_t* pm = get_pmove_current();
	bool spectating = false;
	if (pm->ps) {
		spectating = pm->ps->otherFlags & PMF_FOLLOW;
	}
	return spectating;
}

bool game::is_noclipping()
{
	pmove_t* pm = get_pmove_current();
	bool nocliping = false;
	if (pm->ps) {
		nocliping = pm->ps->pm_type & PM_NOCLIP;
	}
	return nocliping;
}

cvar_t* game::getCvar(const char* name)
{
	DWORD addr = 0x56b5d0;
	__asm
	{
		mov edi, name
		call[addr]
	}
}

bool game::decideStechSide(const Lmove& lMove)  //True = left, False = right
{
	float delta = get_delta(lMove);
	float deltaOptimal = get_delta_optimal(lMove);

	float deltaDiff = fabsf(delta - deltaOptimal);
	float deltaDiffOtherSide = fabsf(delta - deltaOptimal * -1.f);
	return deltaDiffOtherSide < deltaDiff;
}

void game::send_command_to_console(const char* command)
{
	DWORD buffer_cmd = 0x4f8d90;
	__asm
	{
		mov eax, command
		mov ecx, 0
		call buffer_cmd
	}
}

bool game::world_to_screen(vec3<float> world, float* screen_x, float* screen_y)
{
	cg_t* ref = (cg_t*)0x0074E338;
	
	vec3<float> position = world - ref->Refdef.Origin;
	
	vec3<float> transform;
	transform.x = position.DotProduct(ref->Refdef.ViewAxis[1]);
	transform.y = position.DotProduct(ref->Refdef.ViewAxis[2]);
	transform.z = position.DotProduct(ref->Refdef.ViewAxis[0]);
	
	if (transform.z < 0.1f)
		return false;

	vec2 center = { ref->Refdef.ScreenWidth * 0.5f, ref->Refdef.ScreenHeight * 0.5f };

	*screen_x = center.x * (1 - (transform.x / ref->Refdef.FOV.x / transform.z));
	*screen_y = center.y * (1 - (transform.y / ref->Refdef.FOV.y / transform.z));

	return true;
}

int game::get_fps_wtmod()
{
	int maxFps = (int)*reinterpret_cast<float*>(addr_maxfps_wtmod);
	return maxFps;
}

int game::get_fps_3_xp()
{
	int maxFps = (int)*reinterpret_cast<float*>(addr_maxfps_3xp);
	return maxFps;
}

void game::add_obituary(const std::string& msg)
{
	std::string final_msg = msg + "\n";
	reinterpret_cast<void(__cdecl*)(conChannel_t, const char*, msgtype_t)>(0x4FCA50)(conChannel_t::CON_CHANNEL_GAMENOTIFY, final_msg.c_str(), msgtype_t::MSG_DEFAULT);
}
