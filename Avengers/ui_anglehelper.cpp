#include "pch.h"
#include "ui_anglehelper.h"

void ui_anglehelper::render(Avengers*& hud, ImVec4& color)
{
	float width = 2.5f;

	vec2<float> screen;
	vec2<float> center(hud->inst_game->get_screen_res().x / 2 - width / 2, hud->inst_game->get_screen_res().y / 2);
	float yaw = hud->inst_game->get_view().y;
	float optAngle = hud->inst_game->get_optimal_angle();
	float fov = hud->inst_game->get_fov();

	float pixelScale = (hud->inst_game->get_screen_res().x / fov) * hud->inst_ui_menu->ah_pixel_scale;

	screen.y = hud->inst_game->get_screen_res().y / 2 + hud->inst_ui_menu->anglehelper_y_offset;

	float ahOffset = smallestAngleDiff(yaw, optAngle);
	if (mm::compare_angles(yaw, optAngle) == 1) {
		ahOffset *= -1.f;
	}

	screen.x = center.x + (ahOffset * pixelScale);

	if (hud->inst_ui_menu->currentAhStyle == "Style 1") {
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, -30 + screen.y), ImVec2(width + screen.x, 30 + screen.y), ImColor(color));
	}
	else {
		Lmove lmove = hud->inst_game->get_lmove(true);
		bool goingRight = (lmove.isRight && lmove.isForward) || (lmove.isRight && !lmove.isForward) || (lmove.isBack && !hud->inst_game->decideStechSide(lmove));
		float deltaMax = hud->inst_game->get_deltamax_bogus();
		float ahWidth = deltaMax * hud->inst_ui_menu->ah_pixel_scale * 5.f;

		bool clampLeft;
		bool clampRight;

		vec2<float> currentZoneBounds = hud->inst_ui_fpswheel->getCurrentZoneBounds();
		static vec2<float> prevZoneBounds = currentZoneBounds;

		if (hud->inst_ui_menu->clamp_to_next_zone) {
			float differencex = 180.f - abs(abs(currentZoneBounds.x - optAngle) - 180.f);
			float differencey = 180.f - abs(abs(currentZoneBounds.y - optAngle) - 180.f);

			clampLeft = differencey * pixelScale < ahWidth;
			clampRight = differencex * pixelScale < ahWidth;
			if (clampLeft) {
				ahWidth = differencey * pixelScale;
			}
			else if (clampRight) {
				ahWidth = differencex * pixelScale;
			}
		}

		ImColor zoneColor1 = color;
		ImColor zoneColor2 = color;
		zoneColor1.Value.w = 0.6f;
		zoneColor2.Value.w = 0.6f;
		zoneColor2.Value.x += fmodf(0.5f, 1.f);
		zoneColor2.Value.y += fmodf(0.5f, 1.f);
		zoneColor2.Value.z += fmodf(0.5f, 1.f);

		static ImColor currentZoneColor = zoneColor1;
		static ImColor nextZoneColor = zoneColor2;

		if (hud->inst_ui_menu->clamp_to_next_zone && (prevZoneBounds != currentZoneBounds)) {
			if (currentZoneColor == zoneColor1) {
				currentZoneColor = zoneColor2;
				nextZoneColor = zoneColor1;
			}
			else {
				currentZoneColor = zoneColor1;
				nextZoneColor = zoneColor2;
			}
		}

		if (!hud->inst_ui_menu->clamp_to_next_zone) {
			currentZoneColor = color;
			currentZoneColor.Value.w = 0.6f;
		}

		if (goingRight) {
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, -10 + screen.y), ImVec2(screen.x + ahWidth, 10 + screen.y), ImColor(currentZoneColor));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x - 1.f, -15.f + screen.y), ImVec2(screen.x + 1.f, 15.f + screen.y), ImColor(1.f, 1.f, 1.f, 0.7f));
			if (hud->inst_ui_menu->clamp_to_next_zone) {
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x + ahWidth, -10 + screen.y), ImVec2(screen.x + ahWidth + 5.f * pixelScale, 10 + screen.y), ImColor(nextZoneColor));
			}
		}
		else {
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, -10 + screen.y), ImVec2(screen.x - ahWidth, 10 + screen.y), ImColor(currentZoneColor));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x - 1.f, -15.f + screen.y), ImVec2(screen.x + 1.f, 15.f + screen.y), ImColor(1.f, 1.f, 1.f, 0.7f));
			if (hud->inst_ui_menu->clamp_to_next_zone) {
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x - ahWidth, -10 + screen.y), ImVec2(screen.x - ahWidth - 5.f * pixelScale, 10 + screen.y), ImColor(nextZoneColor));
			}
		}

		prevZoneBounds = currentZoneBounds;
	}
}

void ui_anglehelper::renderOnWheel(Avengers*& hud, ImVec4& color)  //TODO: refactor into 1 func w params
{
	float width = 2.5f;

	vec2<float> screen;
	vec2<float> center(hud->inst_game->get_screen_res().x / 2 - width / 2, hud->inst_game->get_screen_res().y / 2);
	float fpsWheelPos = hud->inst_game->get_screen_res().y / 2 + hud->inst_ui_menu->fpswheel_offset_y;
	float yaw = hud->inst_game->get_view().y;
	float optAngle = hud->inst_game->get_optimal_angle();
	float fov = hud->inst_game->get_fov();

	float pixelScale = (hud->inst_game->get_screen_res().x / fov) * hud->inst_ui_menu->wheel_ah_pixel_scale;

	screen.y = fpsWheelPos;
	float ahOffset = smallestAngleDiff(yaw, optAngle);
	if (mm::compare_angles(yaw, optAngle) == 1) {
		ahOffset *= -1.f;
	}

	screen.x = center.x + (ahOffset * pixelScale);

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen.x, screen.y), ImVec2(width + screen.x, hud->inst_ui_menu->fpswheel_size + screen.y), ImColor(color));
}

void ui_anglehelper::renderCenterLine(Avengers*& hud, ImVec4& color)
{
	float width = 2.5f;

	vec2<float> center(hud->inst_game->get_screen_res().x / 2 - width/2, hud->inst_game->get_screen_res().y / 2);

	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(center.x, -30 + center.y), ImVec2(width + center.x, 30 + center.y), ImColor(color));
}

void ui_anglehelper::renderCenterLineOnWheel(Avengers*& hud, ImVec4& color)
{
	float width = 2.5f;

	vec2<float> center(hud->inst_game->get_screen_res().x / 2 - width / 2, hud->inst_game->get_screen_res().y / 2);
	vec2<float> screen;
	float fpsWheelPos = hud->inst_game->get_screen_res().y / 2 + hud->inst_ui_menu->fpswheel_offset_y;

	screen.y = fpsWheelPos;
	screen.x = center.x - width / 2.f;

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen.x, screen.y), ImVec2(width + screen.x, hud->inst_ui_menu->fpswheel_size + screen.y), ImColor(color));
}

float ui_anglehelper::smallestAngleDiff(float a, float b)
{
	float c = 180.f;
	return c - fabs(fmod(fabs(a - b), 2 * c) - c);
}

ui_anglehelper::ui_anglehelper(Avengers* hud)
{
	
}

ui_anglehelper::~ui_anglehelper()
{

}
