#include "pch.h"
#include "ui_velocity.h"
#include "Avengers.h"

void ui_velocity::render(Avengers* &hud, bool &is_locked, vec2<float> &pos, float &scale, ImVec4 &color)
{
	ImGui::Begin("Velocity", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	auto gameState = hud->gameState;

	float velo = gameState->velocity.Length2D();
	static float prev_velo = gameState->velocity.Length2D();

	static int frames_to_wait_for_velo_decrease = 0;  //Velo drops. Wait X frames for velo to drop again.
	static int frames_checking_for_velo_decrease = 0;  // After having waited X frames, wait Y frames and check if it decreased again.
	static int frames_to_decrease_velo_for = 0;  //Mark velocity as decreasing for Z frames.
	static bool velocity_decreasing = false;

	static int frames_to_wait_for_velo_increase = 0;
	static int frames_checking_for_velo_increase = 0;
	static int frames_to_increase_velo_for = 0;
	static bool velocity_increasing = false;


	//////////////////////////////////////////////////////////////////DECREASE LOGIC
	if (velo < prev_velo && frames_to_decrease_velo_for > 0) {
		frames_to_decrease_velo_for = 50;
	}

	if (velo < prev_velo
		&& frames_to_wait_for_velo_decrease == 0 && frames_checking_for_velo_decrease == 0 && frames_to_decrease_velo_for == 0
		&& !gameState->onGround) {
		frames_to_wait_for_velo_decrease = 10;
		frames_checking_for_velo_decrease = 0;
		frames_to_decrease_velo_for = 0;
	}

	if (velo > prev_velo) {
		velocity_decreasing = false;
		frames_to_decrease_velo_for = 0;
		frames_to_wait_for_velo_decrease = 0;
		frames_checking_for_velo_decrease = 0;
	}

	if (frames_to_wait_for_velo_decrease > 0) {
		frames_to_wait_for_velo_decrease--;
		if (frames_to_wait_for_velo_decrease == 0) {
			frames_checking_for_velo_decrease = 10;
		}
	}

	if (frames_checking_for_velo_decrease > 0) {
		frames_checking_for_velo_decrease--;
		if (velo < prev_velo) {
			velocity_decreasing = true;
			frames_to_decrease_velo_for = 50;
		}
	}

	if (frames_to_decrease_velo_for > 0) {
		frames_to_decrease_velo_for--;
		if (frames_to_decrease_velo_for == 0) {
			velocity_decreasing = false;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (velo > prev_velo && frames_to_increase_velo_for > 0) {
		frames_to_increase_velo_for = 10;
	}

	if (velo > prev_velo
		&& frames_to_wait_for_velo_increase == 0 && frames_checking_for_velo_increase == 0 && frames_to_increase_velo_for == 0
		&& !gameState->onGround) {
		frames_to_wait_for_velo_increase = 10;
		frames_checking_for_velo_increase = 0;
		frames_to_increase_velo_for = 0;
	}

	if (velo < prev_velo) {
		velocity_increasing = false;
		frames_to_increase_velo_for = 0;
		frames_to_wait_for_velo_increase = 0;
		frames_checking_for_velo_increase = 0;
	}

	if (frames_to_wait_for_velo_increase > 0) {
		frames_to_wait_for_velo_increase--;
		if (frames_to_wait_for_velo_increase == 0) {
			frames_checking_for_velo_increase = 10;
		}
	}

	if (frames_checking_for_velo_increase > 0) {
		frames_checking_for_velo_increase--;
		if (velo > prev_velo) {
			velocity_increasing = true;
			frames_to_increase_velo_for = 10;
		}
	}

	if (frames_to_increase_velo_for > 0) {
		frames_to_increase_velo_for--;
		if (frames_to_increase_velo_for == 0) {
			velocity_increasing = false;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	prev_velo = velo;

	//Velocity converted to string
	std::string veloText = std::to_string(static_cast<int>(velo));
		
	ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
		
		
	// Check if the mouse is over the text and is being dragged
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !is_locked) {
		vec2<float> prevPos = pos;
		
		// Update the text position based on mouse drag
		pos.x += ImGui::GetIO().MouseDelta.x;
		pos.y += ImGui::GetIO().MouseDelta.y;

		if(prevPos != pos)
		{
			hud->save_configuration();
		}
	}

	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	ImGui::SetWindowFontScale(scale);
	if (hud->inst_ui_menu->sep_velo) {
		ImGui::PushFont(hud->sep_font);
	}
	else {
		ImGui::PushFont(hud->toxic_font);
	}

	vec2<float> adjustedPos = pos;
	/*https://stackoverflow.com/a/67855985*/
	if (hud->inst_ui_menu->keep_velo_centered) {
		float windowWidth = ImGui::GetWindowSize().x;
		float textWidth = ImGui::CalcTextSize(veloText.c_str()).x;

		adjustedPos.x += (windowWidth - textWidth) * 0.5f - (windowWidth - ImGui::CalcTextSize("0").x) * 0.5f;
	}

	ImVec2 outline_position(adjustedPos.x + 1, adjustedPos.y + 1);
	
	draw_list->AddText(outline_position, outlineColor, veloText.c_str());
	
	if (hud->inst_ui_menu->velo_show_deceleration && velocity_decreasing) {
		draw_list->AddText(ImVec2(adjustedPos.x, adjustedPos.y), hud->inst_ui_position_marker->im_vec4_to_im_col32(hud->inst_ui_menu->deceleration_color), veloText.c_str());
	}
	else if (hud->inst_ui_menu->velo_show_acceleration && velocity_increasing) {
		draw_list->AddText(ImVec2(adjustedPos.x, adjustedPos.y), hud->inst_ui_position_marker->im_vec4_to_im_col32(hud->inst_ui_menu->acceleration_color), veloText.c_str());
	}
	else {
		draw_list->AddText(ImVec2(adjustedPos.x, adjustedPos.y), hud->inst_ui_position_marker->im_vec4_to_im_col32(hud->inst_ui_menu->color), veloText.c_str());
	}

	ImGui::SetWindowFontScale(1.0f);

	prev_velo = velo;

	ImGui::PopFont();

	ImGui::End();
}

void ui_velocity::render_jumpoff_speed(Avengers*& hud, vec2<float>& pos, float& scale, ImVec4& color)
{
	auto gameState = hud->gameState;
	static bool onGroundLastFrame = true;
	static float jumpOffVelo = 0.0f;
	bool onGround = gameState->onGround;
	float offset = 50.f;
	ImVec2 position(pos.x, pos.y + offset);
	float VELO_CUTOFF = 5.f;

	float velo = gameState->velocity.Length2D();
	std::string veloText;

	if (onGroundLastFrame && !onGround && velo >= VELO_CUTOFF) {
		jumpOffVelo = velo;
		if (hud->inst_ui_menu->jumpoffspeed_display_bottom) {
			veloText = "Jump speed: ^5" + std::to_string(static_cast<int>(jumpOffVelo));
			hud->inst_game->add_obituary(veloText);
		}
	}
	veloText = std::to_string(static_cast<int>(jumpOffVelo));

	if (hud->inst_ui_menu->draw_jumpoff_speed) {
		ImGui::Begin("Jumpoff velocity", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

		ImGui::SetWindowFontScale(scale);
		if (hud->inst_ui_menu->sep_velo) {
			ImGui::PushFont(hud->sep_font);
		}
		else {
			ImGui::PushFont(hud->toxic_font);
		}

		ImGui::SetWindowFontScale(scale);

		if (hud->inst_ui_menu->keep_velo_centered) {
			float windowWidth = ImGui::GetWindowSize().x;
			float textWidth = ImGui::CalcTextSize(veloText.c_str()).x;

			position.x += (windowWidth - textWidth) * 0.5f - (windowWidth - ImGui::CalcTextSize("0").x) * 0.5f;
		}

		draw_list->AddText(position, hud->inst_ui_position_marker->im_vec4_to_im_col32(color), veloText.c_str());

		ImGui::SetWindowFontScale(1.f);

		ImGui::PopFont();
		ImGui::End();
	}

	onGroundLastFrame = onGround;
}

ui_velocity::ui_velocity(Avengers* hud)
{
	
}

ui_velocity::~ui_velocity()
{

}
