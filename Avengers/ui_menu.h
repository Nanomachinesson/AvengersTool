#pragma once
#include "vectors.h"

class ui_menu
{
	public:
	ui_menu(class Avengers* hud);
	~ui_menu();
	void menu(Avengers* hud);
	void render();

	bool demoplayer_menu = false;
	bool show_position = false;
	bool velo_meter = false;
	bool velo_show_acceleration = false;
	bool velo_show_deceleration = false;
	bool draw_jumpoff_speed = false;
	bool jumpoffspeed_display_bottom = false;
	bool sep_velo = false;
	ImVec4 color = { 0.0f, 0.0f, 1.0f, 1.0f };
	ImVec4 acceleration_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 deceleration_color = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImVec4 lines_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 anglehelper_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	float velo_scale = 1.5;
	bool lock_velo_pos = true;
	bool anglehelper_toggle = false;
	bool drawcenterline = false;
	ImVec4 centerline_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool drawfpswheelcenterline = false;
	ImVec4 fpswheelcenterline_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool fpswheel_toggle = false;
	float fpswheel_size = 20.f;
	float fpswheel_offset_y = 0.f;
	float fpswheel_offset_x = 1.f;
	float ah_pixel_scale = 0.4f;
	float wheel_ah_pixel_scale = 0.4f;
	float wheel_pixel_scale = 0.4f;
	bool lines_toggle = false;
	bool strafedowntime_toggle = false;
	bool rpgtimer_toggle = false;
	bool rpgangle_toggle = false;
	bool bouncevelocity_toggle = false;
	// Default the position to the center of the screen if there is no position in the config file
	vec2<float> velo_pos = vec2<float>(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
	
	bool draw_marker1 = false;
	bool draw_marker2 = false;
	bool draw_marker3 = false;
	vec3<float> marker1;
	vec3<float> marker2;
	vec3<float> marker3;

	float marker_size = 20.0f;
	ImVec4 marker1_color = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImVec4 marker2_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 marker3_color = { 0.0f, 0.0f, 1.0f, 1.0f };

	char* demo_name;
	std::string copied_position;
	
	vec2<float> pos1;
	vec2<float> pos2;
	vec2<float> pos3;

	bool jump_target = false;
	vec3<float> jump_target_origin;
	bool drawfps_toggle = false;
	bool drawfps_spectateonly = false;
	float fpsScale = 1.f;
	ImVec4 fpsColor = { 0.0f, 1.0f, 0.0f, 1.0f };

	bool should_focus_next_frame = false;
};

