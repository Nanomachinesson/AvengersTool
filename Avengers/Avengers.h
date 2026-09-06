#pragma once
#include <memory>
#include <filesystem>
#include "Input.h"
#include "Render.h"
#include "ui_anglehelper.h"
#include "ui_position.h"
#include "ui_velocity.h"
#include "ui_view.h"
#include "ui_settings.h"
#include "Game.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"
#include "imgui-docking/backends/imgui_impl_win32.h"
#include "imgui-docking/backends/imgui_impl_dx9.h"
#include "hook_wrapper.h"
#include "memory.h"
#include "ui_90_lines.h"
#include "ui_demoplayer.h"
#include "ui_fps_image.h"
#include "ui_jump_target.h"
#include "ui_menu.h"
#include "ui_position_marker.h"
#include "ui_fpswheel.h"
#include "ui_strafedowntime.h"
#include "ui_bounceinfo.h"
#include "config_manager.h"
#include "collision.h"
#include "game_state.h"

extern "C" {
	bool __declspec(dllexport) __stdcall RIB_Main(int a, int b);
}

class Avengers
{
public:
	void loadConfiguration();
	void saveConfiguration();
	void saveMarkers();
	void loadMarkers();
	Avengers();
	~Avengers();

	std::shared_ptr<input> instInput;
	std::shared_ptr<render> instRender;
	std::shared_ptr<game> instGame;
	std::shared_ptr<hook_wrapper> instHooks;


	std::shared_ptr<ui_position> instUiPosition;
	std::shared_ptr<ui_velocity> instUiVelocity;
	std::shared_ptr<ui_anglehelper> instUiAnglehelper;
	std::shared_ptr<ui_view> instUiView;
	std::shared_ptr<ui_settings> instUiSettings;
	std::shared_ptr<ui_menu> instUiMenu;
	std::shared_ptr<ui_demoplayer> instUiDemoplayer;
	std::shared_ptr<ui_position_marker> instUiPositionMarker;
	std::shared_ptr<ui_fps_image> instUiFpsImage;
	std::shared_ptr<ui_jump_target> instUiJumpTarget;
	std::shared_ptr<ui_90_lines> instUi90Lines;
	std::shared_ptr<ui_fpswheel> instUiFpswheel;
	std::shared_ptr<ui_strafedowntime> instUiStrafedowntime;
	std::shared_ptr<ui_bounceinfo> instUiBounceinfo;
	std::shared_ptr<Collision> collision;
	std::shared_ptr<GameState> gameState;

	bool exit = false;
	bool wantInput = false;
	bool bindToggleInput(UINT keyState);
	bool bindTpToSavedPos(UINT keyState);
	bool bindClose(UINT keyState);
	static Avengers* instAvengers;
	static Avengers* getInstance();

	std::string markerDirectory = "AvengersMarkers";

	template<typename T>
	void registerConfig(const std::string& name, T* data)
	{
		configManager.registerConfig(name, data);
	}

private:
	ConfigManager configManager;

};

