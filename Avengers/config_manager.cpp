#include "pch.h"
#include "config_manager.h"

ConfigManager::ConfigManager(const std::string& filePath) :
	filePath(filePath)
{
}

void ConfigManager::loadConfig()
{
	std::ifstream configFile(filePath);
	if (!configFile.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(configFile, line)) {
		std::string name;
		Type t{};
		std::smatch match;
		static const std::regex typeRegex(R"(TYPE:\s*([0-9]+))");

		if (std::regex_search(line, match, typeRegex) && match.size() > 1) {
			try {
				int typeNum = std::stoi(match[1].str());
				t = static_cast<Type>(typeNum);
			}
			catch (const std::exception&) {
				continue;
			}
		}
		else {
			continue;
		}

		Setting* currentSetting = nullptr;
		for (auto& setting : settings) {
			std::string firstPart = line.substr(0, line.find(" ") - 1);  //Ignore semicolon
			if (firstPart == setting.first) {
				currentSetting = &setting.second;
				name = setting.first;
				break;
			}
		}

		if (currentSetting == nullptr) {  //didn't find setting
			continue;
		}

		switch (t) {
			case TYPE_BOOL:
			{
				int loadedValue;
				std::string searchString = name + ": %d";
				sscanf_s(line.c_str(), searchString.c_str(), &loadedValue);

				bool loadedValueBool = loadedValue == 1;
				bool* settingValue = reinterpret_cast<bool*>(currentSetting->data);
				*settingValue = loadedValueBool;
				break;
			}
			case TYPE_FLOAT:
			{
				float loadedValue;
				std::string searchString = name + ": %f";
				sscanf_s(line.c_str(), searchString.c_str(), &loadedValue);

				float* settingValue = reinterpret_cast<float*>(currentSetting->data);
				*settingValue = loadedValue;
				break;
			}
			case TYPE_VEC2_FLOAT:
			{
				vec2<float> loadedValue;
				std::string searchString = name + ": %f %f";
				sscanf_s(line.c_str(), searchString.c_str(), &loadedValue.x, &loadedValue.y);

				vec2<float>* settingValue = reinterpret_cast<vec2<float>*>(currentSetting->data);
				*settingValue = loadedValue;
				break;
			}
			case TYPE_VEC3_FLOAT:
			{
				vec3<float> loadedValue;
				std::string searchString = name + ": %f %f %f";
				sscanf_s(line.c_str(), searchString.c_str(), &loadedValue.x, &loadedValue.y, &loadedValue.z);

				vec3<float>* settingValue = reinterpret_cast<vec3<float>*>(currentSetting->data);
				*settingValue = loadedValue;
				break;
			}
			case TYPE_IMGUI_VEC4:
			{
				ImVec4 loadedValue;
				std::string searchString = name + ": %f %f %f %f";
				sscanf_s(line.c_str(), searchString.c_str(), &loadedValue.x, &loadedValue.y, &loadedValue.z, &loadedValue.w);

				ImVec4* settingValue = reinterpret_cast<ImVec4*>(currentSetting->data);
				*settingValue = loadedValue;
				break;
			}
		}
	}
}

void ConfigManager::saveConfig()
{
	std::ofstream configFile(filePath);
	if (!configFile.is_open()) {
		return;
	}

	for (const auto& setting : settings) {
		std::string name = setting.first;
		Type type = setting.second.type;

		configFile << name << ": ";
		switch (type) {
			case TYPE_BOOL:
			{
				bool value = *reinterpret_cast<bool*>(setting.second.data);
				configFile << value;
				break;
			}
			case TYPE_FLOAT:
			{
				float value = *reinterpret_cast<float*>(setting.second.data);
				configFile << value;
				break;
			}
			case TYPE_VEC2_FLOAT:
			{
				vec2<float> value = *reinterpret_cast<vec2<float>*>(setting.second.data);
				configFile << value.x << " " << value.y;
				break;
			}
			case TYPE_VEC3_FLOAT:
			{
				vec3<float> value = *reinterpret_cast<vec3<float>*>(setting.second.data);
				configFile << value.x << " " << value.y << " " << value.z;
				break;
			}
			case TYPE_IMGUI_VEC4:
			{
				ImVec4 value = *reinterpret_cast<ImVec4*>(setting.second.data);
				configFile << value.x << " " << value.y << " " << value.z << " " << value.w;
				break;
			}
		}
		configFile << " TYPE: " << static_cast<int>(type) << "\n";
	}
}
