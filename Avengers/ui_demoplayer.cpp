#include "ui_demoplayer.h"

#include "pch.h"
#include "Avengers.h"
#include "obs_websocket.h"

namespace
{
	void pressF9()
	{
		INPUT press[2]{};
		press[0].type = INPUT_KEYBOARD;
		press[0].ki.wVk = VK_F9;
		press[0].ki.wScan = static_cast<WORD>(MapVirtualKeyW(VK_F9, MAPVK_VK_TO_VSC));
		press[1] = press[0];
		press[1].ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(2, press, sizeof(INPUT));
	}

	void notifyRecord(ui_demoplayer* demo, bool start)
	{
		if (demo->simF9) {
			pressF9();
		}
		if (demo->obsWebsocket) {
			obsWebsocketConfigure(true, demo->obsHost.c_str(), demo->obsPort.c_str(), demo->obsPassword.c_str());
			if (start) {
				obsWebsocketStartRecord();
			}
			else {
				obsWebsocketStopRecord();
			}
		}
	}
}

void ui_demoplayer::render()
{
	Avengers* hud = Avengers::getInstance();
	if (playingDemos && !hud->wantInput)
	{
		playAllDemos();
	}

	if (demoPlaying && hud->instGame->isInMainMenu()) {
		notifyRecord(this, false);
		demoPlaying = false;
	}

	if(showFpsImage)
	{
		hud->instUiFpsImage->render();
	}
}

void ui_demoplayer::menu(Avengers* hud)
{
	if (ImGui::Button("Play Demos")) {
		hud->wantInput = false;
		playingDemos = true;
	}

	ImGui::Checkbox("F9 on all demos played", &simF9);

	ImGui::InputText("Timescale", timescaleInput, sizeof(timescaleInput));
	timescale = static_cast<float>(atof(timescaleInput));
	
	ImGui::InputText("Demo count", demoCountInput, sizeof(demoCountInput));
	demoNum = atoi(demoCountInput);
	
	ImGui::InputText("Execute every demo", extraCommandInput, sizeof(extraCommandInput));
	
	extraCmd = extraCommandInput;
	
	ImGui::InputText("Play demos from", playFromInput, sizeof(playFromInput));
	
	ImGui::Checkbox("Show FPS image", &showFpsImage);
	ImGui::Checkbox("WTMOD", &wtmod);
	ImGui::Checkbox("3XP", &threexp);
	ImGui::SliderFloat("FPS Image scale", &imageScale, 0.01f, 10.f);

	if (playDemosFrom != atoi(playFromInput)) {
		playDemosIndex = atoi(playFromInput);
	}
	
	playDemosFrom = atoi(playFromInput);

	if (wtmod && threexp) {
		threexp = false;
	}

}

void ui_demoplayer::playAllDemos()
{
	Avengers* hud = Avengers::getInstance();
	static bool demoPlayed = true;
	static bool cmdExecuted = false;

	*reinterpret_cast<float*>(addr_timescale) = timescale;

	std::string a = "demo ";
	a += std::to_string(playDemosIndex);

	if (hud->instGame->isConnected() && !demoPlayed) {
		demoPlayed = true;
		playDemosIndex++;
	}

	if (playDemosIndex > demoNum) {
		playDemosIndex = playDemosFrom;
		playingDemos = false;
		demoPlayed = true;
		cmdExecuted = false;
	}

	if (hud->instGame->isInMainMenu()) {
		using namespace std;
		static auto t = chrono::system_clock::now();
		int timeCount = abs(chrono::duration_cast<chrono::milliseconds>(t - chrono::system_clock::now()).count());
		if (timeCount > 1000.f) {
			hud->instGame->sendCommandToConsole(a.c_str());
			demoPlayed = false;
			cmdExecuted = false;
			t = chrono::system_clock::now();
		}
	}

	if (hud->instGame->isConnected() && !cmdExecuted) {
		cmdExecuted = true;
		if (!demoPlaying) {
			notifyRecord(this, true);
		}
		demoPlaying = true;
		if (std::string(extraCmd) != "") {
			hud->instGame->sendCommandToConsole(extraCmd);
		}
	}
}

ui_demoplayer::ui_demoplayer(Avengers* hud)
{
	extraCmd = extraCommandInput;
	hud->instRender->addCallback([this]() { this->render(); });
}

ui_demoplayer::~ui_demoplayer()
{
	obsWebsocketShutdown();
}
