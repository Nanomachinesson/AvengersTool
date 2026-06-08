#include "pch.h"
#include "ui_fpswheel.h"

void fillMapFromOneZone(std::multimap<int, vec2<float>>& map)
{
	auto mapCopy(map);

	for (auto it = mapCopy.begin(); it != mapCopy.end(); ++it) {
		for (int i = 1; i <= 3; i++) {
			int fps = it->first;
			vec2<float> zone = it->second;
			zone.x = mm::normalise(zone.x + 90 * i, 0.f, 360.f);
			zone.y = mm::normalise(zone.y + 90 * i, 0.f, 360.f);

			auto pair = std::pair<int, vec2<float>>(fps, zone);
			map.insert(pair);
		}
	}
}

void ui_fpswheel::render(Avengers* hud)
{
	std::map<int, ImU32> zoneColors
	{
		{125, IM_COL32(255, 0, 0, 200)},
		{200, IM_COL32(0, 255, 0, 200)},
		{250, IM_COL32(0, 0, 200, 200)},
		{251, IM_COL32(255, 255, 0, 200)},
		{333, IM_COL32(95, 0, 160, 200)}
	};

	cg_t* ref = (cg_t*)0x0074E338;
	vec2<float> center(hud->inst_game->get_screen_res().x / 2, hud->inst_game->get_screen_res().y / 2);
	float yaw = hud->inst_game->get_view().y;
	float fov = hud->inst_game->get_fov();

	float pixelScale = (hud->inst_game->get_screen_res().x / fov) * hud->inst_ui_menu->wheel_pixel_scale;

	for (auto const& [fps, zone] : fpsZones) {
		float onePx;
		float onePy;
		float zonex = zone.x;
		float zoney = zone.y;
		vec2<float> zones = moveZone(vec2<float>(zonex, zoney));
		zonex = zones.x;
		zoney = zones.y;

		float differencex = 180.f - abs(abs(zonex - yaw) - 180.f);
		float differencey = 180.f - abs(abs(zoney - yaw) - 180.f);

		if (differencex > fov && differencey > fov) {
			continue;
		}

		if (mm::compare_angles(yaw, zonex) == 1) {
			onePx = center.x - differencex * pixelScale;
		} else {
			onePx = center.x + differencex * pixelScale;
		}
		if (mm::compare_angles(yaw, zoney) == 1) {
			onePy = center.x - differencey * pixelScale;
		}
		else {
			onePy = center.x + differencey * pixelScale;
		}

		if (onePx > onePy) {
			std::swap(onePx, onePy);
		}

		if ((onePx < 0 && onePy > hud->inst_game->get_screen_res().x)
			|| ( (onePx > hud->inst_game->get_screen_res().x) && (onePy > hud->inst_game->get_screen_res().x) )
			|| ( (onePx < 0) && (onePy < 0)) )
				{  //Shouldnt draw stuff outside the screen
			continue;
		}

		int xCutoffLeft = hud->inst_game->get_screen_res().x - hud->inst_game->get_screen_res().x * hud->inst_ui_menu->fpswheel_offset_x;
		int xCutoffRight = hud->inst_game->get_screen_res().x * hud->inst_ui_menu->fpswheel_offset_x;

		float fpsWheelPos = center.y + hud->inst_ui_menu->fpswheel_offset_y;
		std::string fpsText = std::to_string(fps);
		if (fpsText == "251") {
			fpsText = "250";
		}

		ImVec2 textPos((onePx + onePy) / 2 - 15, fpsWheelPos);
		bool drawText = true;

		if ( (textPos.x < xCutoffLeft) || (textPos.x > ( xCutoffRight - 40 ) ) ) {
			drawText = false;
		}

		ImGui::SetNextWindowBgAlpha(0.f);
		ImVec2 windowSize(hud->inst_game->get_screen_res().x * hud->inst_ui_menu->fpswheel_offset_x + 5, hud->inst_game->get_screen_res().y);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(ImVec2(center.x - windowSize.x / 2, center.y - windowSize.y / 2));
		ImGui::Begin("FPSWheel", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

		ImGui::SetWindowFontScale(0.80f + hud->inst_ui_menu->fpswheel_size / 100.f);

		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(onePx, fpsWheelPos), ImVec2(onePy, fpsWheelPos + hud->inst_ui_menu->fpswheel_size), zoneColors[fps]);
		ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 200), fpsText.c_str());
		ImGui::SetWindowFontScale(1.f);

		ImVec4 color(0, 0, 0, 1.f);
		hud->inst_ui_anglehelper->renderOnWheel(hud, color);

		if (hud->inst_ui_menu->drawfpswheelcenterline) {
			hud->inst_ui_anglehelper->renderCenterLineOnWheel(hud, hud->inst_ui_menu->fpswheelcenterline_color);
		}

		ImGui::End();
	}
}

vec2<float> ui_fpswheel::getCurrentZoneBounds()
{
	Avengers* hud = Avengers::get_instance();
	float optAngle = hud->inst_game->get_optimal_angle();
	Lmove lmove = hud->inst_game->get_lmove(true);

	bool goingRight = (lmove.isRight && lmove.isForward) || (lmove.isRight && !lmove.isForward) || (lmove.isBack && !hud->inst_game->decideStechSide(lmove));

	for (auto const& [fps, zone] : fpsZones) {
		vec2<float> zoneCopy = zone;
		zoneCopy = moveZone(zoneCopy);
		if (!goingRight && mm::compare_angles(zoneCopy.x, optAngle) == 1 && mm::compare_angles(zoneCopy.y, optAngle) == -1) {
			return zoneCopy;
		}
		else if (goingRight && mm::compare_angles(zoneCopy.x, optAngle) == -1 && mm::compare_angles(zoneCopy.y, optAngle) == 1) {
			return zoneCopy;
		}
	}
}

vec2<float> ui_fpswheel::moveZone(const vec2<float>& zone)
{
	float zonex = zone.x;
	float zoney = zone.y;

	Avengers* hud = Avengers::get_instance();

	Lmove lmove = hud->inst_game->get_lmove(true);

	if (!lmove.isForward) {
		if (lmove.isRight) {
			zonex = mm::normalise(zonex + 45.f, 0.f, 360.f);
			zoney = mm::normalise(zoney + 45.f, 0.f, 360.f);
		}
		else if (lmove.isLeft) {
			zonex = mm::normalise(zonex - 45.f, 0.f, 360.f);
			zoney = mm::normalise(zoney - 45.f, 0.f, 360.f);
		}
	}
	if (lmove.isRight) {
		zonex = mm::normalise(180 - zonex, 0, 360);
		zoney = mm::normalise(180 - zoney, 0, 360);
	}

	if (!lmove.isRight && !lmove.isForward && !lmove.isLeft) {
		if (lmove.isBack) {
			if (!hud->inst_game->decideStechSide(lmove)) {
				zonex = mm::normalise(180.f - zonex + 45.f, 0.f, 360.f);  //I don't really understand this anymore, and opted to bruteforce the values.
				zoney = mm::normalise(180.f - zoney + 45.f, 0.f, 360.f);
			}
			else {
				zonex = mm::normalise(zonex - 45.f, 0.f, 360.f);
				zoney = mm::normalise(zoney - 45.f, 0.f, 360.f);
			}
		}
		else {  //no keys => wd strafe
			zonex = mm::normalise(zonex + 45.f, 0.f, 360.f);
			zoney = mm::normalise(zoney + 45.f, 0.f, 360.f);
		}
	}

	return vec2<float>(zonex, zoney);
}

ui_fpswheel::ui_fpswheel(Avengers* hud)
{
	fillMapFromOneZone(fpsZones);
}

ui_fpswheel::~ui_fpswheel()
{
}
