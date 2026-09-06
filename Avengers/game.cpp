#include "pch.h"
#include "Game.h"


HWND game::getWindow()
{
	return *(HWND*)0xCC1B6FC;
}
bool game::isFocused()
{
	return *(bool*)0x0CC1B704;
}

bool game::isInMainMenu()
{
	return *reinterpret_cast<int*>(addr_ingame) == connection_state_uninitialized;
}

LPDIRECT3DDEVICE9 game::getDevice()
{
	return *(LPDIRECT3DDEVICE9*)0xCC9A408;
}

bool game::isConnected()
{
	int state = *(int*)addr_ingame;
	return state == connection_state_connected;
}

vec3<float> game::getView()
{
	vec3<float> deltaAngles = *reinterpret_cast<vec3<float>*>(addr_deltaAngles);
	vec3<float> cameraAngles = *reinterpret_cast<vec3<float>*>(addr_writeableAngles);
	vec3<float> normalized;
	pmove_t* pm = getPmoveCurrent();

	if (!isSpectating()) {
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

vec3<float> game::getOrigin()
{
	pmove_t* pm = getPmoveCurrent();
	vec3<float> origin{};

	if (!isDemoPlaying()) {
		if (pm && pm->ps) {
			origin = pm->ps->origin;
		}
	}
	else {
		origin = *reinterpret_cast<vec3<float>*>(addr_demovelo - 12);
	}

	return origin;
}

vec3<float> game::getVelocity()
{
	vec3<float> velocity{};
	cvar_t* cvar = getCvar("cl_demoplaying");
	if (!isDemoPlaying()) {
		pmove_t* pm = getPmoveCurrent();
		if (pm && pm->ps) {
			velocity = pm->ps->velocity;
		}
	}
	else {
		velocity = *reinterpret_cast<vec3<float>*>(addr_demovelo);
	}

	return velocity;
}

bool game::isDemoPlaying()
{
	cvar_t* cvar = getCvar("cl_demoplaying");
	return cvar->current.enabled;
}

float game::getDirDiff()
{
	return getDirDiff(getLmove(true));
}

float game::getDirDiff(const Lmove& lMove)
{
	if (lMove.isForward) {
		return (lMove.isRight) ? -45.f : 45.f;
	}
	else if (lMove.isBack) {
		if (lMove.isRight) {
			return -135.f;
		}
		else if (lMove.isLeft) {
			return 135.f;
		}
		else {
			return 180.f;
		}
	}
	else {
		return (lMove.isRight) ? -90.f : +90.f;
	}

	return 0;
}

Lmove game::getLmove(bool adjustForSpectator)
{
	using namespace mm;
	input_s* input = (input_s*)addr_usercmd;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	Lmove lMove;
	static Lmove prevLmove {};

	static float prevYaw = 0.f;
	float yaw = getView().y;

	lMove.isInAir = !Avengers::getInstance()->instGame->isOnGround();
	lMove.isSprint = *reinterpret_cast<int*>(addr_sprint) >= 20 || *reinterpret_cast<int*>(addr_sprint) == 5 ? true : false;
	lMove.isMoving = fabsf(getVelocity().length2D()) > 0 ? true : false;
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
	if (adjustForSpectator && game::isSpectating() && fabsf(yaw - prevYaw) <= CHANGE_TOLERANCE && getVelocity().length2D() >= MINIMUM_VELO) {
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

		float optimalAngleForward = getOptimalAngle(forwardLmove);
		float optimalAngleNoForward = getOptimalAngle(noForwardLmove);
		float optimalAngleBack = getOptimalAngle(backMove);

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

float game::getVelocityAngle()
{
	using namespace mm;
	vec3<float> velocity = getVelocity();
	return normalise(tiltAngle(truncateVector(getVelocity())), 0.f, 360.f);
}

float game::getDelta()
{
	return getDelta(getLmove(true));
}

float game::getDelta(const Lmove& lMove)
{
	float accelAngle = 0.f;

	accelAngle = mm::normalise(getView().y + getDirDiff(lMove), 0.f, 360.f);

	float delta = getVelocityAngle() - accelAngle;
	return mm::normalise(delta, -180.f, 180.f);
}

float game::getDeltaOptimal()
{
	return getDeltaOptimal(getLmove(true));
}

float game::getDeltaOptimal(const Lmove& lMove)
{
	constexpr float g_speed = 190.f;

	float speed = getVelocity().length2D();
	float deltaOpt = mm::toDegrees(acosf((g_speed - getAccel()) / speed));

	if ((lMove.isLeft && !lMove.isBack) || (lMove.isBack && lMove.isRight))
	{
		deltaOpt *= -1.f;
	}

	return deltaOpt;
}

int game::getFps(bool adjustForSpectator)
{
	int maxFps = 0;  //TODO: make this stuff work for spectator
	if (adjustForSpectator && isSpectating()) {
		maxFps = getFps3Xp();
	}
	else {
		cvar_t* maxFpsCvar = getCvar("com_maxfps");
		maxFps = maxFpsCvar->current.integer;
	}
	return maxFps;
}

float game::getAccel()
{
	return g_speed / getFps();
}

float game::getDeltamaxBogus()
{
	constexpr float g_speed = 190.f;

	float speed = getVelocity().length2D();
	float deltaMax = mm::toDegrees(acosf( ( (-1.f * getAccel()) / (2 * speed) ) ));

	return deltaMax;
}

std::string game::getMapName()
{
	return std::string(getCvar("mapname")->current.string);
}

MaterialTechnique* game::RB_BeginSurface(MaterialTechniqueType techType, Material* material)
{
	const static uint32_t RB_BeginSurface_func = 0x61A220;
	__asm
	{
		pushad;
		mov		edi, techType;
		mov		esi, material;
		call	RB_BeginSurface_func;
		popad;
	}
}

MaterialTechnique* game::RB_BeginSurface_CustomMaterial(MaterialTechniqueType techType, Material* material)
{
	const static uint32_t RB_BeginSurface_func = 0x61A220;
	__asm
	{
		pushad;
		mov		edi, techType;
		mov		esi, material;
		call	RB_BeginSurface_func;
		popad;
	}
}

void game::checkTessOverflow(int vertex_count)
{
	materialCommands_t* tess = reinterpret_cast<materialCommands_t*>(addr_material_commands);
	GfxCmdBufState* gfxCmdBufState = reinterpret_cast<GfxCmdBufState*>(addr_gfxcmdbufstate);
	RB_EndTessSurface_t RB_EndTessSurface = (RB_EndTessSurface_t)addr_rb_endtesssurface;

	if (vertex_count + tess->vertexCount > 5450 || tess->indexCount + 6 > 0x100000) {
		RB_EndTessSurface();
		RB_BeginSurface(gfxCmdBufState->origTechType, gfxCmdBufState->origMaterial);
	}
}

// set vertices for current render-surface
void game::setPolyVert(const float* xyz, GfxColor color, int vertCount, int vertNum)
{
	materialCommands_t* tess = reinterpret_cast<materialCommands_t*>(addr_material_commands);

	tess->verts[vertCount].xyzw[0] = xyz[0];
	tess->verts[vertCount].xyzw[1] = xyz[1];
	ga:tess->verts[vertCount].xyzw[2] = xyz[2];
	tess->verts[vertCount].xyzw[3] = 1.0f; // 0.0 to make it a sky
	tess->verts[vertCount].color.packed = color.packed;

	switch (vertNum) {
	case 0:
		tess->verts[vertCount].texCoord[0] = 0.0f;
		tess->verts[vertCount].texCoord[1] = 0.0f;
		break;

	case 1:
		tess->verts[vertCount].texCoord[0] = 0.0f;
		tess->verts[vertCount].texCoord[1] = 1.0f;
		break;
	case 2:
		tess->verts[vertCount].texCoord[0] = 1.0f;
		tess->verts[vertCount].texCoord[1] = 1.0f;
		break;
	case 3:
		tess->verts[vertCount].texCoord[0] = 1.0f;
		tess->verts[vertCount].texCoord[1] = 0.0f;
		break;

	default:
		tess->verts[vertCount].texCoord[0] = 0.0f;
		tess->verts[vertCount].texCoord[1] = 0.0f;
		break;
	}

	tess->verts[vertCount].normal.packed = 1073643391;
}

__declspec(naked) PackedUnitVec game::Vec3PackUnitVec(const float*)
{
	const static uint32_t funcAddr = 0x5645A0;
	__asm
	{
		mov		eax, [esp + 4h];
		call	funcAddr;
		retn;

		//push	0x5645A0;
		//retn;
	}
}

// *
// set vertices for current render-surface
void game::setPolyVertWithNormal(const float* xyz, const float* normal, GfxColor color, int vert_count, int vertNum)
{
	materialCommands_t* tess = reinterpret_cast<materialCommands_t*>(addr_material_commands);

	tess->verts[vert_count].xyzw[0] = xyz[0];
	tess->verts[vert_count].xyzw[1] = xyz[1];
	tess->verts[vert_count].xyzw[2] = xyz[2];
	tess->verts[vert_count].xyzw[3] = 1.0f; // 0.0 to make it a sky
	tess->verts[vert_count].color.packed = color.packed;

	switch (vertNum) {
	case 0:
		tess->verts[vert_count].texCoord[0] = 0.0f;
		tess->verts[vert_count].texCoord[1] = 0.0f;
		break;

	case 1:
		tess->verts[vert_count].texCoord[0] = 0.0f;
		tess->verts[vert_count].texCoord[1] = 1.0f;
		break;
	case 2:
		tess->verts[vert_count].texCoord[0] = 1.0f;
		tess->verts[vert_count].texCoord[1] = 1.0f;
		break;
	case 3:
		tess->verts[vert_count].texCoord[0] = 1.0f;
		tess->verts[vert_count].texCoord[1] = 0.0f;
		break;

	default:
		tess->verts[vert_count].texCoord[0] = 0.0f;
		tess->verts[vert_count].texCoord[1] = 0.0f;
		break;
	}

	tess->verts[vert_count].normal = Vec3PackUnitVec(normal);
}

void game::drawPoly(const int numPoints, float(*points)[3], const float* brushColor, bool brushLit, bool outlines, const float* outlineColor, bool depthCheck, bool twoSidesPoly)
{
	if (numPoints < 3) {
		return;
	}

	int vertIndex;

	GfxColor color = {};
	R_ConvertColorToBytes(brushColor, (char*)&color);

	GfxCmdBufState* gfxCmdBufState = reinterpret_cast<GfxCmdBufState*>(addr_gfxcmdbufstate);
	r_global_permanent_t* rgp = reinterpret_cast<r_global_permanent_t*>(addr_r_global_permanent_t);
	materialCommands_t* tess = reinterpret_cast<materialCommands_t*>(addr_material_commands);
	GfxCmdBufSourceState* gfxCmdBufSourceState = reinterpret_cast<GfxCmdBufSourceState*>(addr_gfxcmdbufsourcestate);
	GfxWorld* gfxWorld = reinterpret_cast<GfxWorld*>(addr_gfxworld);
	RB_EndTessSurface_t RB_EndTessSurface = (RB_EndTessSurface_t)addr_rb_endtesssurface;

	// check render-surface overflow
	if (gfxCmdBufState->origMaterial != rgp->lineMaterial || gfxCmdBufState->origTechType != MaterialTechniqueType::TECHNIQUE_UNLIT) {
		// draw / skip left over polys
		if (tess->indexCount) {
			RB_EndTessSurface();
		}

		if (brushLit) {
			// use a custom material for polygons
			Material* unlitMaterial = Material_RegisterHandle("iw3xo_showcollision_fakelight", 3);

			if (!unlitMaterial) {
				//Com_Error(0, utils::va("^1_debug::draw_poly L#%d ^7:: unlitMaterial was null\n", __LINE__));
			}

			// dirty shader constants for our fakelight shader
			gfxCmdBufSourceState->input.consts[ShaderCodeConstants::CONST_SRC_CODE_FILTER_TAP_3][0] = gfxWorld->sunParse.ambientScale * gfxWorld->sunParse.ambientColor[0];
			gfxCmdBufSourceState->input.consts[ShaderCodeConstants::CONST_SRC_CODE_FILTER_TAP_3][1] = gfxWorld->sunParse.ambientScale * gfxWorld->sunParse.ambientColor[1];
			gfxCmdBufSourceState->input.consts[ShaderCodeConstants::CONST_SRC_CODE_FILTER_TAP_3][2] = gfxWorld->sunParse.ambientScale * gfxWorld->sunParse.ambientColor[2];

			// start poly
			RB_BeginSurface_CustomMaterial(MaterialTechniqueType::TECHNIQUE_UNLIT, unlitMaterial);
		}
		else {
			// patch default line material so that it uses Blend and PolyOffset
			const auto unlitMaterial = rgp->lineMaterial;//reinterpret_cast<game::Material*>(*(DWORD32*)(game::builtInMaterialUnlitDepth));

			// fill poly on both sides
			if (twoSidesPoly) {
				// blendFunc Blend + cullFace "None"
				unlitMaterial->stateBitsTable->loadBits[0] = 422072677;
			}
			else // 1 sided poly
			{
				// blendFunc Blend + cullFace "Back"
				unlitMaterial->stateBitsTable->loadBits[0] = 422089061;
			}

			// give poly a slight offset to stop z-fighting :: polyOffset StaticDecal
			unlitMaterial->stateBitsTable->loadBits[1] = 44;

			// start poly
			RB_BeginSurface(MaterialTechniqueType::TECHNIQUE_UNLIT, depthCheck ? rgp->lineMaterial : rgp->lineMaterialNoDepth);
		}
	}


	// render all added polys if we would overflow the surface by adding new ones
	checkTessOverflow(numPoints);

	if (brushLit) {
		vec3_t pt1, pt2, normal;

		mm::subtract3(points[1], points[0], pt1);
		mm::subtract3(points[2], points[0], pt2);
		mm::cross3(pt1, pt2, normal);

		for (vertIndex = 0; vertIndex < numPoints; ++vertIndex) {
			setPolyVertWithNormal(&(*points)[3 * vertIndex], normal, color, tess->vertexCount + vertIndex, vertIndex);
		}
	}
	else {
		for (vertIndex = 0; vertIndex < numPoints; ++vertIndex) {
			setPolyVert(&(*points)[3 * vertIndex], color, tess->vertexCount + vertIndex, vertIndex);
		}
	}

	for (vertIndex = 0; vertIndex < numPoints - 2; ++vertIndex) {
		tess->indices[tess->indexCount + 0] = (unsigned short int)(0);
		tess->indices[tess->indexCount + 1] = (unsigned short int)(vertIndex + 2);
		tess->indices[tess->indexCount + 2] = (unsigned short int)(vertIndex + 1);
		tess->indexCount += 3;
	}

	tess->vertexCount += numPoints;

	// draw all added polys
	RB_EndTessSurface();


	// ------------------------------
	// Draw Polys as wireframe on top

	if (outlines) {
		// check render-surface overflow
		if (gfxCmdBufState->origMaterial != rgp->lineMaterial || gfxCmdBufState->origTechType != MaterialTechniqueType::TECHNIQUE_WIREFRAME_SOLID) {
			// draw / skip left over polys
			if (tess->indexCount) {
				RB_EndTessSurface();
			}

			// use a custom material for outlines
			const auto unlitMaterial = game::Material_RegisterHandle("iw3xo_showcollision_wire", 3);

			if (!unlitMaterial) {
				//Com_Error(0, utils::va("^1_debug::draw_poly L#%d ^7:: unlitMaterial was null\n", __LINE__));
				return;
			}

			RB_BeginSurface_CustomMaterial(MaterialTechniqueType::TECHNIQUE_UNLIT, unlitMaterial);
		}

		// Check if we would overflow our Surface and if we would, render all added polys
		checkTessOverflow(numPoints);

		if (outlineColor == nullptr) {
			float tempColor[3] = { 1.0f, 0.0f, 0.0f };
			game::R_ConvertColorToBytes(tempColor, (char*)&color);
		}
		else {
			game::R_ConvertColorToBytes(outlineColor, (char*)&color);
		}

		// set our surface verts (tess->verts)
		for (vertIndex = 0; vertIndex < numPoints; ++vertIndex) {
			setPolyVert(&(*points)[3 * vertIndex], color, tess->vertexCount + vertIndex, 0);
		}

		// counter-clockwise polys?
		for (vertIndex = 0; vertIndex < numPoints - 2; ++vertIndex) {
			tess->indices[tess->indexCount + 0] = (unsigned short int)(0);
			tess->indices[tess->indexCount + 1] = (unsigned short int)(vertIndex + 2);
			tess->indices[tess->indexCount + 2] = (unsigned short int)(vertIndex + 1);

			tess->indexCount += 3;
		}

		tess->vertexCount += numPoints;

		// Draw all added polys
		RB_EndTessSurface();
	}
}

float game::getOptimalAngle()
{
	return getOptimalAngle(getLmove(true));
}

float game::getOptimalAngle(const Lmove& lMove)
{
	float delta = getDelta(lMove);
	float deltaOptimal = getDeltaOptimal(lMove);

	float yaw = getView().y;

	if (lMove.isBack && !lMove.isLeft && !lMove.isRight) {  //If the user is doing s-tech we need to account for both strafe sides
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
	pmove_t* pm = getPmoveCurrent();
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

vec2<float> game::getScreenRes()
{
	cg_t* ref = (cg_t*)0x0074E338;
	vec2<float> screenres{};
	screenres.x = ref->Refdef.ScreenWidth;
	screenres.y = ref->Refdef.ScreenHeight;
	return screenres;
}

vec3<float> game::getDeltaAngles()
{
	return *reinterpret_cast<vec3<float>*>(addr_delta_angles);
}

float game::getFov()
{
	cvar_t* fov = getCvar("cg_fov");
	cvar_t* fovScale = getCvar("cg_fovScale");
	return fov->current.value * fovScale->current.value;
}

pmove_t* game::getPmoveCurrent()
{
	return reinterpret_cast<pmove_t*>(addr_pMove_current);
}

bool game::isSpectating()
{
	pmove_t* pm = getPmoveCurrent();
	bool spectating = false;
	if (pm && pm->ps) {
		spectating = pm->ps->otherFlags & PMF_FOLLOW;
	}
	return spectating;
}

bool game::isNoclipping()
{
	pmove_t* pm = getPmoveCurrent();
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
	float delta = getDelta(lMove);
	float deltaOptimal = getDeltaOptimal(lMove);

	float deltaDiff = fabsf(delta - deltaOptimal);
	float deltaDiffOtherSide = fabsf(delta - deltaOptimal * -1.f);
	return deltaDiffOtherSide < deltaDiff;
}

bool game::isDevmap()
{
	return *reinterpret_cast<int*>(0x01288600);
}

void game::setPosition(const vec3<float>& pos)
{
	vec3<float>* position = reinterpret_cast<vec3<float>*>(addr_position_writeable);
	*position = pos;
}

void game::setView(const vec3<float>& angles)
{
	vec3<float>* view = reinterpret_cast<vec3<float>*>(addr_writeableAngles);
	*view = toCodAngles(angles);
}

void game::setVelocity(const vec3<float>& velocity)
{
	vec3<float>* gameVelocity = reinterpret_cast<vec3<float>*>(addr_velocity_writeable);
	*gameVelocity = velocity;
}

vec3<float> game::toCodAngles(const vec3<float>& angles)
{
	vec3<float> delta = getDeltaAngles();
	vec3<float> view;
	view.x = angles.x - delta.x;
	view.y = angles.y - delta.y;
	view.z = angles.z - delta.z;

	return view;
}

void game::sendCommandToConsole(const char* command)
{
	DWORD bufferCmd = 0x4f8d90;
	__asm
	{
		mov eax, command
		mov ecx, 0
		call bufferCmd
	}
}

bool game::worldToScreen(vec3<float> world, float* screenX, float* screenY)
{
	cg_t* ref = (cg_t*)0x0074E338;
	
	vec3<float> position = world - ref->Refdef.Origin;
	
	vec3<float> transform;
	transform.x = position.dotProduct(ref->Refdef.ViewAxis[1]);
	transform.y = position.dotProduct(ref->Refdef.ViewAxis[2]);
	transform.z = position.dotProduct(ref->Refdef.ViewAxis[0]);
	
	if (transform.z < 0.0f)
		return false;

	vec2 center = { ref->Refdef.ScreenWidth * 0.5f, ref->Refdef.ScreenHeight * 0.5f };

	*screenX = center.x * (1 - (transform.x / ref->Refdef.FOV.x / transform.z));
	*screenY = center.y * (1 - (transform.y / ref->Refdef.FOV.y / transform.z));

	return true;
}

int game::getFpsWtmod()
{
	int maxFps = (int)*reinterpret_cast<float*>(addr_maxfps_wtmod);
	return maxFps;
}

int game::getFps3Xp()
{
	int maxFps = (int)*reinterpret_cast<float*>(addr_maxfps_3xp);
	return maxFps;
}

void game::addObituary(const std::string& msg)
{
	std::string finalMsg = msg + "\n";
	reinterpret_cast<void(__cdecl*)(conChannel_t, const char*, msgtype_t)>(0x4FCA50)(conChannel_t::CON_CHANNEL_GAMENOTIFY, finalMsg.c_str(), msgtype_t::MSG_DEFAULT);
}
