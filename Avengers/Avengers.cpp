#include "pch.h"
#include "Avengers.h"

#include "ui_90_lines.h"
#include "ui_demoplayer.h"

//pretend its an audio codec for the miles sound system
bool __stdcall RIB_Main(int a, int b)
{
	return true;
}

Avengers* Avengers::instAvengers = nullptr;
bool Avengers::bindToggleInput(UINT keyState)
{
	if (keyState == WM_KEYDOWN) //return true on key down just so nothing else receives the key down stroke
		return true; 
	if (keyState == WM_KEYUP)
	{
		wantInput = !wantInput;
		return true;
	}
}

bool Avengers::bindTpToSavedPos(UINT keyState)
{
	if (!instGame->isDevmap()) {
		return true;
	}

	if (keyState == WM_KEYDOWN) //return true on key down just so nothing else receives the key down stroke
		return true; 
	if (keyState == WM_KEYUP)
	{
		if (instGame->isConnected()) {
			instGame->setPosition(instUiMenu->copiedPositionOrigin);
			instGame->setView(instUiMenu->copiedPositionView);
			instGame->setVelocity(vec3<float>(0, 0, 0));
		}

		return true;
	}
}

void Avengers::loadConfiguration() {
	configManager.loadConfig();
}

void Avengers::saveConfiguration() {
	configManager.saveConfig();
}

void Avengers::saveMarkers()
{
	if (!std::filesystem::exists(markerDirectory)) {
		std::filesystem::create_directory(markerDirectory);
	}

	std::string mapName = instGame->getMapName();
	std::string filePath = markerDirectory + "/" + mapName + "_markers";

	std::ofstream markerFile(filePath);
	if (!markerFile.is_open()) {
		return;
	}

	for (Marker& marker : instUiPositionMarker->markers) {
		markerFile << "POS: " << marker.position.x << "," << marker.position.y << "," << marker.position.z << ","
				   << "ANGLES: " << marker.angles.x << "," << marker.angles.y << "," << marker.angles.z << ","
				   << "COL: " << marker.color.x << "," << marker.color.y << "," << marker.color.z << "," << marker.color.w << "\n";
	}
}

void Avengers::loadMarkers()
{
	std::string mapName = instGame->getMapName();
	std::string filePath = markerDirectory + "/" + mapName + "_markers";
	instUiPositionMarker->markers.clear();

	std::ifstream configFile(filePath);
	if (!configFile.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(configFile, line)) {
		if (line.empty())
			continue;

		vec3<float> position;
		vec3<float> angles;
		ImVec4 color;

		int scanned = sscanf_s(line.c_str(), "POS: %f,%f,%f,ANGLES: %f,%f,%f,COL: %f,%f,%f,%f",
			&position.x, &position.y, &position.z,
			&angles.x, &angles.y, &angles.z,
			&color.x, &color.y, &color.z, &color.w);

		if (scanned == 10) {
			Marker marker;
			marker.position = position;
			marker.angles = angles;
			marker.color = color;
			instUiPositionMarker->markers.push_back(marker);
		}
	}
}

Avengers::Avengers() :
	configManager("AvengersConfig.txt")
{
	exit = false;
	instAvengers = this;
	instHooks = std::shared_ptr<hook_wrapper>(new hook_wrapper);
	instGame = std::shared_ptr<game>(new game());
	instInput = std::shared_ptr<input>(new input(this));
	instRender = std::shared_ptr<render>(new render(this));
	
	instUiSettings = std::shared_ptr<ui_settings>(new ui_settings(this));
	instUiPosition = std::shared_ptr<ui_position>(new ui_position(this));
	instUiVelocity = std::shared_ptr<ui_velocity>(new ui_velocity(this));
	instUiView = std::shared_ptr<ui_view>(new ui_view(this));
	instUiMenu = std::shared_ptr<ui_menu>(new ui_menu(this));
	instUiDemoplayer = std::shared_ptr<ui_demoplayer>(new ui_demoplayer(this));
	instUiPositionMarker = std::shared_ptr<ui_position_marker>(new ui_position_marker(this));
	instUiFpsImage = std::shared_ptr<ui_fps_image>(new ui_fps_image(this));
	instUiJumpTarget = std::shared_ptr<ui_jump_target>(new ui_jump_target(this));
	instUi90Lines = std::shared_ptr<ui_90_lines>(new ui_90_lines(this));
	instUiFpswheel = std::shared_ptr<ui_fpswheel>(new ui_fpswheel(this));
	instUiStrafedowntime = std::shared_ptr<ui_strafedowntime>(new ui_strafedowntime(this));
	instUiBounceinfo = std::shared_ptr<ui_bounceinfo>(new ui_bounceinfo(this));
	collision = std::shared_ptr<Collision>(new Collision(this));
	gameState = std::shared_ptr<GameState>(new GameState(this));

	//Added both INSERT and F6 to open the menu for people who have smaller keyboards and cant find that INSERT key ¬_¬
	instInput->addCallback(VK_INSERT, [this](UINT keyState) { return this->bindToggleInput(keyState); });
	instInput->addCallback(VK_F6, [this](UINT keyState) { return this->bindToggleInput(keyState); });

	//Keybind to tp to the last saved postion
	instInput->addCallback(VK_F3, [this](UINT keyState) { return this->bindTpToSavedPos(keyState); });

	instUiMenu->registerConfigs(this);
	loadConfiguration();
}

Avengers::~Avengers()
{
}

Avengers* Avengers::getInstance()
{
	return instAvengers;
}
