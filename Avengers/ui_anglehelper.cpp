#include "pch.h"
#include "ui_anglehelper.h"

void ui_anglehelper::render(Avengers*& hud, ImVec4& color)
{
	Lmove lmove = hud->instGame->getLmove(true);
	bool goingRight = (lmove.isRight && lmove.isForward) || (lmove.isRight && !lmove.isForward && !lmove.isBack)
		|| (lmove.isBack && lmove.isLeft) || (lmove.isBack && !lmove.isRight && !lmove.isLeft && !hud->instGame->decideStechSide(lmove));
	float width = 2.5f;
	width *= hud->instUiMenu->anglehelperWidth;
	if (goingRight) {
		width *= -1.f;
	}

	vec2<float> screen;
	vec2<float> center(hud->instGame->getScreenRes().x / 2, hud->instGame->getScreenRes().y / 2);
	float yaw = hud->instGame->getView().y;
	float optAngle = hud->instGame->getOptimalAngle();
	float fov = hud->instGame->getFov();

	float pixelScale = (hud->instGame->getScreenRes().x / fov) * hud->instUiMenu->ahPixelScale;

	screen.y = hud->instGame->getScreenRes().y / 2 + hud->instUiMenu->anglehelperYOffset;

	float ahOffset = smallestAngleDiff(yaw, optAngle);
	if (mm::compareAngles(yaw, optAngle) == 1) {
		ahOffset *= -1.f;
	}

	screen.x = center.x + (ahOffset * pixelScale);

	if (hud->instUiMenu->currentAhStyle == "Style 1") {
		float ahHeightScalar = hud->instUiMenu->anglehelperHeight;
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, (-30 * ahHeightScalar) + screen.y), ImVec2(width + screen.x, (30 * ahHeightScalar) + screen.y), ImColor(color));
	}
	else {
		float deltaMax = hud->instGame->getDeltamaxBogus();
		float ahWidth = deltaMax * hud->instUiMenu->ahPixelScale * 5.f;

		bool clampLeft;
		bool clampRight;

		vec2<float> currentZoneBounds = hud->instUiFpswheel->getCurrentZoneBounds();
		static vec2<float> prevZoneBounds = currentZoneBounds;

		if (hud->instUiMenu->clampToNextZone) {
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

		if (hud->instUiMenu->clampToNextZone && (prevZoneBounds != currentZoneBounds)) {
			if (currentZoneColor == zoneColor1) {
				currentZoneColor = zoneColor2;
				nextZoneColor = zoneColor1;
			}
			else {
				currentZoneColor = zoneColor1;
				nextZoneColor = zoneColor2;
			}
		}

		if (!hud->instUiMenu->clampToNextZone) {
			currentZoneColor = color;
			currentZoneColor.Value.w = 0.6f;
		}

		if (goingRight) {
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, -10 + screen.y), ImVec2(screen.x + ahWidth, 10 + screen.y), ImColor(currentZoneColor));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x - 1.f, -15.f + screen.y), ImVec2(screen.x + 1.f, 15.f + screen.y), ImColor(1.f, 1.f, 1.f, 0.7f));
			if (hud->instUiMenu->clampToNextZone) {
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x + ahWidth, -10 + screen.y), ImVec2(screen.x + ahWidth + 5.f * pixelScale, 10 + screen.y), ImColor(nextZoneColor));
			}
		}
		else {
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, -10 + screen.y), ImVec2(screen.x - ahWidth, 10 + screen.y), ImColor(currentZoneColor));
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x - 1.f, -15.f + screen.y), ImVec2(screen.x + 1.f, 15.f + screen.y), ImColor(1.f, 1.f, 1.f, 0.7f));
			if (hud->instUiMenu->clampToNextZone) {
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
	vec2<float> center(hud->instGame->getScreenRes().x / 2 - width / 2, hud->instGame->getScreenRes().y / 2);
	float fpsWheelPos = hud->instGame->getScreenRes().y / 2 + hud->instUiMenu->fpswheelOffsetY;
	float yaw = hud->instGame->getView().y;
	float optAngle = hud->instGame->getOptimalAngle();
	float fov = hud->instGame->getFov();

	float pixelScale = (hud->instGame->getScreenRes().x / fov) * hud->instUiMenu->wheelAhPixelScale;

	screen.y = fpsWheelPos;
	float ahOffset = smallestAngleDiff(yaw, optAngle);
	if (mm::compareAngles(yaw, optAngle) == 1) {
		ahOffset *= -1.f;
	}

	screen.x = center.x + (ahOffset * pixelScale);

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen.x, screen.y), ImVec2(width + screen.x, hud->instUiMenu->fpswheelSize + screen.y), ImColor(color));
}

void ui_anglehelper::renderCenterLine(Avengers*& hud, ImVec4& color)
{
	float width = hud->instUiMenu->centerlineWidth;

	vec2<float> center(hud->instGame->getScreenRes().x / 2 - width/2, hud->instGame->getScreenRes().y / 2);

	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(center.x, -30 + center.y), ImVec2(width + center.x, 30 + center.y), ImColor(color));
}

void ui_anglehelper::renderCenterLineOnWheel(Avengers*& hud, ImVec4& color)
{
	float width = 2.5f;

	vec2<float> center(hud->instGame->getScreenRes().x / 2 - width / 2, hud->instGame->getScreenRes().y / 2);
	vec2<float> screen;
	float fpsWheelPos = hud->instGame->getScreenRes().y / 2 + hud->instUiMenu->fpswheelOffsetY;

	screen.y = fpsWheelPos;
	screen.x = center.x;

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen.x, screen.y), ImVec2(width + screen.x, hud->instUiMenu->fpswheelSize + screen.y), ImColor(color));
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
