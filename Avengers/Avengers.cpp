#include "pch.h"
#include "Avengers.h"

#include "ui_90_lines.h"
#include "ui_demoplayer.h"

//pretend its an audio codec for the miles sound system
bool __stdcall RIB_Main(int a, int b)
{
	return true;
}

Avengers* Avengers::inst_Avengers = nullptr;
bool Avengers::bind_toggle_input(UINT key_state)
{
	if (key_state == WM_KEYDOWN) //return true on key down just so nothing else receives the key down stroke
		return true; 
	if (key_state == WM_KEYUP)
	{
		want_input = !want_input;
		return true;
	}
}

bool Avengers::bind_tp_to_saved_pos(UINT key_state)
{
	if (!inst_game->isDevmap()) {
		return true;
	}

	if (key_state == WM_KEYDOWN) //return true on key down just so nothing else receives the key down stroke
		return true; 
	if (key_state == WM_KEYUP)
	{
		if (inst_game->is_connected()) {
			inst_game->setPosition(inst_ui_menu->copied_position_origin);
			inst_game->setView(inst_ui_menu->copied_position_view);
			inst_game->setVelocity(vec3<float>(0, 0, 0));
		}

		return true;
	}
}

void Avengers::load_configuration() {
	configManager.loadConfig();
}

void Avengers::save_configuration() {
	configManager.saveConfig();
}

Avengers::Avengers() :
	configManager("AvengersConfig.txt")
{
	exit = false;
	inst_Avengers = this;
	inst_hooks = std::shared_ptr<hook_wrapper>(new hook_wrapper);
	inst_game = std::shared_ptr<game>(new game());
	inst_input = std::shared_ptr<input>(new input(this));
	inst_render = std::shared_ptr<render>(new render(this));
	
	inst_ui_settings = std::shared_ptr<ui_settings>(new ui_settings(this));
	inst_ui_position = std::shared_ptr<ui_position>(new ui_position(this));
	inst_ui_velocity = std::shared_ptr<ui_velocity>(new ui_velocity(this));
	inst_ui_view = std::shared_ptr<ui_view>(new ui_view(this));
	inst_ui_menu = std::shared_ptr<ui_menu>(new ui_menu(this));
	inst_ui_demoplayer = std::shared_ptr<ui_demoplayer>(new ui_demoplayer(this));
	inst_ui_position_marker = std::shared_ptr<ui_position_marker>(new ui_position_marker(this));
	inst_ui_fps_image = std::shared_ptr<ui_fps_image>(new ui_fps_image(this));
	inst_ui_jump_target = std::shared_ptr<ui_jump_target>(new ui_jump_target(this));
	inst_ui_90_lines = std::shared_ptr<ui_90_lines>(new ui_90_lines(this));
	inst_ui_fpswheel = std::shared_ptr<ui_fpswheel>(new ui_fpswheel(this));
	inst_ui_strafedowntime = std::shared_ptr<ui_strafedowntime>(new ui_strafedowntime(this));
	inst_ui_bounceinfo = std::shared_ptr<ui_bounceinfo>(new ui_bounceinfo(this));
	collision = std::shared_ptr<Collision>(new Collision(this));
	gameState = std::shared_ptr<GameState>(new GameState(this));

	//Added both INSERT and F6 to open the menu for people who have smaller keyboards and cant find that INSERT key ¬_¬
	inst_input->add_callback(VK_INSERT, [this](UINT key_state) { return this->bind_toggle_input(key_state); });
	inst_input->add_callback(VK_F6, [this](UINT key_state) { return this->bind_toggle_input(key_state); });

	//Keybind to tp to the last saved postion
	inst_input->add_callback(VK_F3, [this](UINT key_state) { return this->bind_tp_to_saved_pos(key_state); });

	load_configuration();
}

Avengers::~Avengers()
{
}

Avengers* Avengers::get_instance()
{
	return inst_Avengers;
}
