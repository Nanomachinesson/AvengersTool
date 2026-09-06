#pragma once
#include "cod4Structs.h"
#include "d3dx9/d3dx9.h"
#include "vectors.h"
#include "game_math.h"
#include "Lmove.h"
#include "memory.h"
#include "cod4Structs.h"

enum connection_state_ : int
{
    connection_state_uninitialized,
    connection_state_disconnected,
    connection_state_authorizing,
    connection_state_connecting,
    connection_state_challenging,
    connection_state_established,
    connection_state_loading,
    connection_state_primed,
    connection_state_active,
    connection_state_connected
};

class game
{
public:
	struct GfxPointVertex
	{
		vec3<float> location;
		char color[4];
		GfxPointVertex() : location(0, 0, 0), color{ 0, 0, 0, 0 } {};
		GfxPointVertex(vec3<float> loc, ImColor col)
		{
			location = loc;
			color[0] = col.Value.z * 255;
			color[1] = col.Value.y * 255;
			color[2] = col.Value.x * 255;
			color[3] = col.Value.w * 255;
		}
		GfxPointVertex(vec3<float> loc, char r, char g, char b, char a)
		{
			location = loc;
			color[0] = b;
			color[1] = g;
			color[2] = r;
			color[3] = a;
		}
	};
	
	bool isDemoPlaying();
	HWND getWindow(); 
	bool isFocused();
	bool isInMainMenu();
	LPDIRECT3DDEVICE9 getDevice();
    bool isConnected();
    vec3<float> getView();
    vec3<float> getOrigin();
    vec3<float> getVelocity();
    float getOptimalAngle();
    float getOptimalAngle(const Lmove& lMove);
	bool isOnGround();
	static void sendCommandToConsole(const char* command);
	bool worldToScreen(vec3<float> world, float* screenX, float* screenY);
    int getFps(bool adjustForSpectator = false);
	int getFpsWtmod();
	int getFps3Xp();
	void addObituary(const std::string& msg);
	int getJumpTime();
    vec2<float> getScreenRes();
	mem::function<void(int count, int width, GfxPointVertex* verts, bool depthTest)> polyline = 0x613040;
    Lmove getLmove(bool adjustForSpectator = false);
	float getFov();
	pmove_t* getPmoveCurrent();
	bool isSpectating();
	bool isNoclipping();
	cvar_t* getCvar(const char* name);
	bool decideStechSide(const Lmove& lMove);
	bool isDevmap();
	void setPosition(const vec3<float>& pos);
	void setView(const vec3<float>& angles);
	void setVelocity(const vec3<float>& velocity);
	vec3<float> toCodAngles(const vec3<float>& angles);
	float getDeltamaxBogus();
	std::string getMapName();
	//TAKEN FROM IW3XO
	void drawPoly(const int numPoints, float(*points)[3], const float* brushColor, bool brushLit, bool outlines, const float* outlineColor, bool depthCheck, bool twoSidesPoly);
	mem::function<void __fastcall (const float* colorFloat, char* colorBytes)> R_ConvertColorToBytes = 0x493530;
	mem::function<Material* (const char* fontName, int fontSize)> Material_RegisterHandle = 0x5F2A80;
	//

private:
    vec3<float> getDeltaAngles();
    float getDelta();
    float getDelta(const Lmove& lMove);
    float getDeltaOptimal();
    float getDeltaOptimal(const Lmove& lMove);
    float getVelocityAngle();
    float getDirDiff();
    float getDirDiff(const Lmove& lMove);
    float getAccel();
    constexpr static float g_speed = 190.f;

	//TAKEN FROM IW3XO
private:
	typedef void(*RB_EndTessSurface_t)();
	void setPolyVertWithNormal(const float* xyz, const float* normal, GfxColor color, int vert_count, int vertNum);
	void setPolyVert(const float* xyz, GfxColor color, int vertCount, int vertNum);
	void checkTessOverflow(int vertex_count);
	MaterialTechnique* RB_BeginSurface(MaterialTechniqueType techType, Material* material);
	MaterialTechnique* RB_BeginSurface_CustomMaterial(MaterialTechniqueType techType, Material* material);
	PackedUnitVec Vec3PackUnitVec(const float*);

};

