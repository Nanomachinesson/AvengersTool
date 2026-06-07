#pragma once
#include <unordered_map>
#include <string>
#include <fstream>
#include <imgui.h>
#include <type_traits>
#include <regex>
#include "game_math.h"

enum Type { TYPE_BOOL, TYPE_FLOAT, TYPE_VEC2_FLOAT, TYPE_VEC3_FLOAT, TYPE_IMGUI_VEC4, TYPE_STRING };

struct Setting
{
	Type type;
	void* data;
};

class ConfigManager
{
public:
	ConfigManager(const std::string& fileName);
	void loadConfig();
	void saveConfig();

	template<typename T>
	void registerConfig(const std::string& name, T* value)
	{
		if (name.contains("TYPE")) {
			std::runtime_error("NAME CANNOT CONTAIN THE STRING \"TYPE\"");
		}

		Setting setting;
		setting.data = reinterpret_cast<void*>(value);

		if (std::is_same<T, bool>::value ) {
			setting.type = TYPE_BOOL;
		}
		else if (std::is_same<T, float>::value ) {
			setting.type = TYPE_FLOAT;
		}
		else if (std::is_same<T, vec2<float>>::value ) {
			setting.type = TYPE_VEC2_FLOAT;
		}
		else if (std::is_same<T, vec3<float>>::value) {
			setting.type = TYPE_VEC3_FLOAT;
		}
		else if (std::is_same<T, ImVec4>::value ) {
			setting.type = TYPE_IMGUI_VEC4;
		}
		else if (std::is_same<T, std::string>::value ) {
			setting.type = TYPE_STRING;
		}
		else {
			throw std::runtime_error("Unsupported config type");
		}

		settings.emplace(name, setting);
	}

private:
	std::unordered_map<std::string, Setting> settings {};
	std::string filePath;

};
