#include "pch.h"
#include "ui_90_lines.h"
#include "Avengers.h"


void ui_90_lines::render()
{
	Avengers* hud = Avengers::getInstance();
	auto color = ImColor(hud->instUiMenu->linesColor);
	vec3<float> cDirection;
	std::vector<std::pair<game::GfxPointVertex, game::GfxPointVertex>> verts;

	cg_t* ref = (cg_t*)0x0074E338;


	for (int i = 0; i < 4; i++)
	{
		auto origin = ref->Refdef.Origin;
		origin.z -= 10;
		float length = 80000;
		mm::angleVectors({ 0, (float)(i * 90), 0 }, &cDirection, 0, 0);

		vec3<float> cPoint = origin + (cDirection * vec3<float>(length, length, length));
		verts.push_back({ game::GfxPointVertex(origin, color), game::GfxPointVertex(cPoint, color) });
	}

	hud->instGame->polyline(verts.size(), 1, reinterpret_cast<game::GfxPointVertex*>(verts.data()), true);
}
ui_90_lines::ui_90_lines(Avengers* hud)
{
	
}
ui_90_lines::~ui_90_lines()
{

}