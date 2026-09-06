#pragma once
#include <d3d9.h>
#include <string>

// struct demo_player_states
// {
// 	bool playingDemos = false;
// 	int demoNum;
// 	float timescale = 1;
// 	char* extraCmd;
// 	bool simF9 = false;
// 	bool justFinished = false;
// 	int playDemosFrom = 1;
// 	int playDemosIndex = 1;
// 	bool demoPlaying = false;
// 	bool showFpsImage = false;
// 	bool wtmod = false;
// 	bool threexp = true;
// 	float imageScale = 0.7f;
// };

class ui_demoplayer
{
public:
	ui_demoplayer(class Avengers* hud);
	~ui_demoplayer();
	void render();
	void menu(Avengers* hud);
	void playAllDemos();

	//demo_player_states demo_player_states;

	bool playingDemos = false;
	int demoNum = 1;
	float timescale = 1;
	char* extraCmd = nullptr;
	bool simF9 = false;
	bool obsWebsocket = false;
	std::string obsHost = "127.0.0.1";
	std::string obsPort = "4455";
	std::string obsPassword;
	bool justFinished = false;
	int playDemosFrom = 1;
	int playDemosIndex = 1;
	bool demoPlaying = false;
	bool showFpsImage = false;
	bool wtmod = false;
	bool threexp = true;
	float imageScale = 0.7f;

	char timescaleInput[128] = "1";
	char demoCountInput[128] = "1";
	char extraCommandInput[1024]{};
	char playFromInput[10] = "1";
};

