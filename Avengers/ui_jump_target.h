#pragma once
#include <chrono>
#include "collision.h"

class ui_jump_target
{
public:
	ui_jump_target(class Avengers* hud);
	~ui_jump_target();
	void render();
	void selectFacePlayerIsStandingOn();
	void addBrush();
	void removeBrush();
	void resetBrushes();
	vec2<float> getBrushDistance();  //Return: distance, Z-distance

	std::vector<BrushSide*> selectedBrushes;

private:
	float PolygonSideDist(const std::vector<vec3<float>>& polygon, const vec3<float>& point);
	float PolygonDist(const std::vector<vec3<float>>& polygon, const vec3<float>& point);
	bool isInPolygon(const std::vector<vec2<float>>& polygon, const vec2<float>& point);
	void extendPolygon(std::vector<vec2<float>>& polygon, float offset);
};

