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
	
	
	HWND get_window(); 
	bool is_focused();
	bool is_in_main_menu();
	LPDIRECT3DDEVICE9 get_device();
    bool is_connected();
    vec3<float> get_view();
    vec3<float> get_origin();
    vec3<float> get_velocity();
    float get_optimal_angle();
    float get_optimal_angle(const Lmove& lMove);
	bool isOnGround();
	static void send_command_to_console(const char* command);
	bool world_to_screen(vec3<float> world, float* screen_x, float* screen_y);
    int get_fps(bool adjustForSpectator = false);
	int get_fps_wtmod();
	int get_fps_3_xp();
	void add_obituary(const std::string& msg);
	int getJumpTime();
    vec2<float> get_screen_res();
	mem::function<void(int count, int width, GfxPointVertex* verts, bool depthTest)> polyline = 0x613040;
    Lmove get_lmove(bool adjustForSpectator = false);
	float get_fov();
	pmove_t* get_pmove_current();
	bool is_spectating();
	bool is_noclipping();
	cvar_t* getCvar(const char* name);
	bool decideStechSide(const Lmove& lMove);
	bool isDevmap();
	void setPosition(const vec3<float>& pos);
	void setView(const vec3<float>& angles);
	void setVelocity(const vec3<float>& velocity);
	vec3<float> toCodAngles(const vec3<float>& angles);
	float get_deltamax_bogus();
	//TAKEN FROM IW3XO
	void drawPoly(const int num_points, float(*points)[3], const float* brush_color, bool brush_lit, bool outlines, const float* outline_color, bool depth_check, bool two_sides_poly);
	mem::function<void __fastcall (const float* colorFloat, char* colorBytes)> R_ConvertColorToBytes = 0x493530;
	mem::function<Material* (const char* fontName, int fontSize)> Material_RegisterHandle = 0x5F2A80;
	//

private:
    vec3<float> get_delta_angles();
    float get_delta();
    float get_delta(const Lmove& lMove);
    float get_delta_optimal();
    float get_delta_optimal(const Lmove& lMove);
    float get_velocity_angle();
    float get_dir_diff();
    float get_dir_diff(const Lmove& lMove);
    float get_accel();
    constexpr static float g_speed = 190.f;

	//TAKEN FROM IW3XO
private:
	typedef void(*RB_EndTessSurface_t)();
	void set_poly_vert_with_normal(const float* xyz, const float* normal, GfxColor color, int vert_count, int vertNum);
	void set_poly_vert(const float* xyz, GfxColor color, int vertCount, int vertNum);
	void check_tess_overflow(int vertex_count);
	MaterialTechnique* RB_BeginSurface(MaterialTechniqueType techType, Material* material);
	MaterialTechnique* RB_BeginSurface_CustomMaterial(MaterialTechniqueType techType, Material* material);
	PackedUnitVec Vec3PackUnitVec(const float*);

};

