#include "pch.h"
#include "ui_velocity.h"
#include "Avengers.h"

void ui_velocity::render(Avengers* &hud, bool &isLocked, vec2<float> &pos, float &scale, ImVec4 &color)
{
	ImGui::Begin("Velocity", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
	auto gameState = hud->gameState;

	float velo = 0.f;
	if (hud->instGame->isDemoPlaying()) {
		velo = hud->instGame->getVelocity().length2D();
	}
	else {
		velo = gameState->velocity.length2D();
	}

	static float prevVelo = gameState->velocity.length2D();

	static int framesToWaitForVeloDecrease = 0;  //Velo drops. Wait X frames for velo to drop again.
	static int framesCheckingForVeloDecrease = 0;  // After having waited X frames, wait Y frames and check if it decreased again.
	static int framesToDecreaseVeloFor = 0;  //Mark velocity as decreasing for Z frames.
	static bool velocityDecreasing = false;

	static int framesToWaitForVeloIncrease = 0;
	static int framesCheckingForVeloIncrease = 0;
	static int framesToIncreaseVeloFor = 0;
	static bool velocityIncreasing = false;


	//////////////////////////////////////////////////////////////////DECREASE LOGIC
	bool onGround = gameState->onGround;
	if (hud->instUiMenu->enableDecelerationOnGround) {
		onGround = false;
	}

	if (velo < prevVelo && framesToDecreaseVeloFor > 0) {
		framesToDecreaseVeloFor = hud->instUiMenu->veloKeepDecelFor;
	}

	if (velo < prevVelo
		&& framesToWaitForVeloDecrease == 0 && framesCheckingForVeloDecrease == 0 && framesToDecreaseVeloFor == 0
		&& !onGround) {
		framesToWaitForVeloDecrease = hud->instUiMenu->veloDecelerationThreshold;
		framesCheckingForVeloDecrease = 0;
		framesToDecreaseVeloFor = 0;
	}

	if (velo > prevVelo) {
		velocityDecreasing = false;
		framesToDecreaseVeloFor = 0;
		framesToWaitForVeloDecrease = 0;
		framesCheckingForVeloDecrease = 0;
	}

	if (framesToWaitForVeloDecrease > 0) {
		framesToWaitForVeloDecrease--;
		if (framesToWaitForVeloDecrease == 0) {
			framesCheckingForVeloDecrease = hud->instUiMenu->veloDecelerationThreshold;
		}
	}

	if (framesCheckingForVeloDecrease > 0) {
		framesCheckingForVeloDecrease--;
		if (velo < prevVelo) {
			velocityDecreasing = true;
			framesToDecreaseVeloFor = hud->instUiMenu->veloKeepDecelFor;
		}
	}

	if (framesToDecreaseVeloFor > 0) {
		framesToDecreaseVeloFor--;
		if (framesToDecreaseVeloFor == 0) {
			velocityDecreasing = false;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	onGround = gameState->onGround;
	if (hud->instUiMenu->enableAccelerationOnGround) {
		onGround = false;
	}

	if (velo > prevVelo && framesToIncreaseVeloFor > 0) {
		framesToIncreaseVeloFor = hud->instUiMenu->veloKeepAccelFor;
	}

	if (velo > prevVelo
		&& framesToWaitForVeloIncrease == 0 && framesCheckingForVeloIncrease == 0 && framesToIncreaseVeloFor == 0
		&& !onGround) {
		framesToWaitForVeloIncrease = hud->instUiMenu->veloAccelerationThreshold;
		framesCheckingForVeloIncrease = 0;
		framesToIncreaseVeloFor = 0;
	}

	if (velo < prevVelo) {
		velocityIncreasing = false;
		framesToIncreaseVeloFor = 0;
		framesToWaitForVeloIncrease = 0;
		framesCheckingForVeloIncrease = 0;
	}

	if (framesToWaitForVeloIncrease > 0) {
		framesToWaitForVeloIncrease--;
		if (framesToWaitForVeloIncrease == 0) {
			framesCheckingForVeloIncrease = hud->instUiMenu->veloAccelerationThreshold;
		}
	}

	if (framesCheckingForVeloIncrease > 0) {
		framesCheckingForVeloIncrease--;
		if (velo > prevVelo) {
			velocityIncreasing = true;
			framesToIncreaseVeloFor = hud->instUiMenu->veloKeepAccelFor;
		}
	}

	if (framesToIncreaseVeloFor > 0) {
		framesToIncreaseVeloFor--;
		if (framesToIncreaseVeloFor == 0) {
			velocityIncreasing = false;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	prevVelo = velo;

	//Velocity converted to string
	std::string veloText = std::to_string(static_cast<int>(velo));
		
	ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
		
		
	// Check if the mouse is over the text and is being dragged
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !isLocked) {
		vec2<float> prevPos = pos;
		
		// Update the text position based on mouse drag
		pos.x += ImGui::GetIO().MouseDelta.x;
		pos.y += ImGui::GetIO().MouseDelta.y;

		if(prevPos != pos)
		{
			hud->saveConfiguration();
		}
	}

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	ImGui::SetWindowFontScale(scale);
	ImFont* speedometerFont = hud->instUiMenu->getSpeedometerFont();
	if (speedometerFont) ImGui::PushFont(speedometerFont);

	vec2<float> adjustedPos = pos;
	/*https://stackoverflow.com/a/67855985*/
	if (hud->instUiMenu->keepVeloCentered) {
		float windowWidth = ImGui::GetWindowSize().x;
		float textWidth = ImGui::CalcTextSize(veloText.c_str()).x;
		if (hud->instUiMenu->useStaticPositioning) {
			std::string widthHelper(veloText.length(), '5');  //Different digits may have different sizes
			textWidth = ImGui::CalcTextSize(widthHelper.c_str()).x;
		}

		adjustedPos.x += (windowWidth - textWidth) * 0.5f - (windowWidth - ImGui::CalcTextSize("0").x) * 0.5f;
	}

	ImVec2 outlinePosition(adjustedPos.x + 1, adjustedPos.y + 1);
	
	drawList->AddText(outlinePosition, outlineColor, veloText.c_str());
	
	if (hud->instUiMenu->veloShowDeceleration && velocityDecreasing) {
		drawList->AddText(ImVec2(adjustedPos.x, adjustedPos.y), hud->instUiPositionMarker->imVec4ToImCol32(hud->instUiMenu->decelerationColor), veloText.c_str());
	}
	else if (hud->instUiMenu->veloShowAcceleration && velocityIncreasing) {
		drawList->AddText(ImVec2(adjustedPos.x, adjustedPos.y), hud->instUiPositionMarker->imVec4ToImCol32(hud->instUiMenu->accelerationColor), veloText.c_str());
	}
	else {
		drawList->AddText(ImVec2(adjustedPos.x, adjustedPos.y), hud->instUiPositionMarker->imVec4ToImCol32(hud->instUiMenu->color), veloText.c_str());
	}

	ImGui::SetWindowFontScale(1.0f);

	prevVelo = velo;

	if (speedometerFont) ImGui::PopFont();

	ImGui::End();
}

void ui_velocity::renderJumpoffSpeed(Avengers*& hud, vec2<float>& pos, float& scale, ImVec4& color)
{
	auto gameState = hud->gameState;
	static bool onGroundLastFrame = true;
	static float jumpOffVelo = 0.0f;
	bool onGround = gameState->onGround;
	float offset = 50.f;
	ImVec2 position(pos.x, pos.y + offset);
	float VELO_CUTOFF = 5.f;

	float velo = gameState->velocity.length2D();
	std::string veloText;

	if (onGroundLastFrame && !onGround && velo >= VELO_CUTOFF) {
		jumpOffVelo = velo;
		if (hud->instUiMenu->jumpoffspeedDisplayBottom) {
			veloText = "Jump speed: ^5" + std::to_string(static_cast<int>(jumpOffVelo));
			hud->instGame->addObituary(veloText);
		}
	}
	veloText = std::to_string(static_cast<int>(jumpOffVelo));

	if (hud->instUiMenu->drawJumpoffSpeed) {
		ImGui::Begin("Jumpoff velocity", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		ImGui::SetWindowFontScale(scale);
		ImFont* speedometerFont = hud->instUiMenu->getSpeedometerFont();
		if (speedometerFont) ImGui::PushFont(speedometerFont);

		ImGui::SetWindowFontScale(scale);

		if (hud->instUiMenu->keepVeloCentered) {
			float windowWidth = ImGui::GetWindowSize().x;
			float textWidth = ImGui::CalcTextSize(veloText.c_str()).x;
			if (hud->instUiMenu->useStaticPositioning) {
				std::string widthHelper(veloText.length(), '5');  //Different digits may have different sizes
				textWidth = ImGui::CalcTextSize(widthHelper.c_str()).x;
			}
			position.x += (windowWidth - textWidth) * 0.5f - (windowWidth - ImGui::CalcTextSize("0").x) * 0.5f;
		}

		drawList->AddText(speedometerFont, ImGui::GetFontSize(), position, hud->instUiPositionMarker->imVec4ToImCol32(color), veloText.c_str());

		ImGui::SetWindowFontScale(1.f);

		if (speedometerFont) ImGui::PopFont();
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
