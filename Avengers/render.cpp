#include "pch.h"
#include "Render.h"
#include "Avengers.h"

void init_graphics_stub()
{
	Avengers* hud = Avengers::get_instance();
	if (hud && hud->inst_render)
		hud->inst_render->init_graphics();
}

void __cdecl EngineDraw_Hook()
{
	Avengers* hud = Avengers::get_instance();
	if (hud && hud->inst_hooks && hud->inst_render)
	{
		hud->inst_hooks->hook_map["EngineDraw"]->original(EngineDraw_Hook)();
		hud->inst_render->enginedraw();
	}
}

HRESULT __stdcall EndScene_Hook(LPDIRECT3DDEVICE9 dev)
{
	int ret = reinterpret_cast<int>(_ReturnAddress());
	bool correctCaller = ret == 6379331;

	Avengers* hud = Avengers::get_instance();
	hud->inst_input->windowReady = true;

	if (hud && hud->inst_hooks && hud->inst_render)
	{
		auto orig = hud->inst_hooks->hook_map["EndScene"]->original(EndScene_Hook)(dev);
		if (correctCaller) {
			hud->inst_render->endscene(dev);
		}
		return orig;
	}

	return 1;
}

HRESULT __stdcall Reset_Hook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{

	Avengers* hud = Avengers::get_instance();
	if (hud && hud->inst_hooks && hud->inst_render)
	{

		auto orig = hud->inst_hooks->hook_map["Reset"]->original(Reset_Hook);
		hud->inst_render->invalidate_objects(pDevice);
		HRESULT rval = orig(pDevice, pPresentationParameters);
		hud->inst_render->create_objects(pDevice);
		return rval;
	}
	return 1;
}
void imgui_easy_theming(ImVec4 color_for_text, ImVec4 color_for_head, ImVec4 color_for_area, ImVec4 color_for_body, ImVec4 color_for_pops)
{
	// DUCK RED nope! is DARK RED style by for40255 from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 0.85f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.05490196123719215f, 0.05490196123719215f, 0.05490196123719215f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.321f, 0.321f, 0.321f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.5647059082984924f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.5647059082984924f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9411764740943909f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.5647059082984924f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.501960813999176f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.8154506683349609f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8156862854957581f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.501960813999176f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.7450980544090271f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.6566523313522339f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.8039215803146362f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.501960813999176f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.6695278882980347f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9570815563201904f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1019607856869698f, 0.1137254908680916f, 0.1294117718935013f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2039215713739395f, 0.2078431397676468f, 0.2156862765550613f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 0.2000000029802322f);
	style.Colors[ImGuiCol_Tab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4392156898975372f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 0.9725490212440491f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9490196108818054f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9490196108818054f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4274509847164154f, 0.3607843220233917f, 0.3607843220233917f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.7124463319778442f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.196078434586525f, 0.196078434586525f, 0.196078434586525f, 0.6274510025978088f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.2588235437870026f, 0.2705882489681244f, 0.3803921639919281f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1803921610116959f, 0.2274509817361832f, 0.2784313857555389f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}


void render::SetupImGuiStyle2()
{
	static ImVec4 color_for_text = ImVec4(.92f, .94f, .94f, 0);
	static ImVec4 color_for_head = ImVec4(ImColor(65, 60, 73, 255).Value);
	static ImVec4 color_for_area = ImVec4(ImColor(82, 75, 92, 255).Value);
	static ImVec4 color_for_body = ImVec4(ImColor(0, 0, 0, 255).Value);
	static ImVec4 color_for_pops = ImColor(40, 37, 45, 255).Value;

	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().GrabRounding = 0.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;
	ImGui::GetStyle().FramePadding = ImVec2(2, 2);
	ImGui::GetStyle().WindowPadding = ImVec2(5, 5);
	ImGui::GetStyle().ItemInnerSpacing = ImVec2(15, 0);
	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().AntiAliasedLines = true;
	

	if (first_run_style)
	{
		imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
		first_run_style = false;
	}

}

void render::init_imgui(LPDIRECT3DDEVICE9 dev)
{
	if (!imgui_initialized)
	{
		if (ImGui::GetCurrentContext()) {
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			first_run_style = true;
		}

		Avengers* hud = Avengers::get_instance();
		
		ImGui_ImplDX9_InvalidateDeviceObjects();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = false;
		ImGui_ImplWin32_Init(Avengers::get_instance()->inst_game->get_window());
		ImGui_ImplDX9_Init(dev);

		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		hud->toxic_font = io.Fonts->AddFontFromMemoryTTF((void*)(_acbahnschrift), sizeof(_acbahnschrift) - 1, 24.f, &fontConfig);
		hud->sep_font = io.Fonts->AddFontFromMemoryTTF((void*)(_acawesomefont1), sizeof(_acawesomefont1) - 1, 24.f, &fontConfig);
		
		ImGui_ImplDX9_CreateDeviceObjects();
		imgui_initialized = true;

		SetupImGuiStyle2();
	}
	dev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFF);
}

void __cdecl render::enginedraw()
{
	Avengers* hud = Avengers::get_instance();

	if(hud->inst_game->is_connected() && hud->inst_ui_menu->lines_toggle)
	{
		hud->inst_ui_90_lines->render();
	}

	if (hud->inst_game->is_connected() && hud->inst_ui_menu->draw_collision) {
		hud->collision->render();
	}

	if (hud->inst_ui_jump_target->selectedBrushes.size() > 0 && hud->inst_ui_menu->brush_mode && hud->inst_ui_menu->draw_selected_brushes) {
		const auto poly_lit = false;
		const auto poly_outlines = false;
		const auto poly_linecolor = ImColor(255, 255, 255, 255);
		const auto poly_depth = true;
		const auto poly_face = false;
		ImColor color(0.3f, 1.f, 0.f, 0.4f);

		for (BrushSide* face : hud->inst_ui_jump_target->selectedBrushes) {
			vec3<float>* points = face->points.data();
			hud->inst_game->drawPoly(face->points.size(), (float(*)[3]) points, (const float*)&color,
				poly_lit, poly_outlines, (const float*)&poly_linecolor, poly_depth, poly_face);
		}
	}
}

void render::endscene(LPDIRECT3DDEVICE9 dev)
{
	init_imgui(dev);
	auto& io = ImGui::GetIO();

	Avengers* hud = Avengers::get_instance();
	if (hud->want_input)
		io.MouseDrawCursor = true;
	else
		io.MouseDrawCursor = false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	for (auto& fn : callbacks_render)
		fn();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void render::add_callback(RenderCallback render)
{
	callbacks_render.push_back(render);
}

void render::invalidate_objects(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}
void render::create_objects(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui_ImplDX9_CreateDeviceObjects();
}
void render::init_graphics()
{
	Avengers* hud = Avengers::get_instance();
	//call the original function first
	hud->inst_hooks->hook_map["InitGraphics"]->original(init_graphics_stub)();
	static LPDIRECT3DDEVICE9 current_device = nullptr;
	if (current_device != hud->inst_game->get_device())
	{
		Avengers* hud = Avengers::get_instance();

		if (hud && hud->inst_hooks) //remove the old hooks
		{
			if (hud->inst_hooks->hook_map.find("EndScene") != hud->inst_hooks->hook_map.end())
				hud->inst_hooks->hook_map["EndScene"]->remove();
			if (hud->inst_hooks->hook_map.find("Reset") != hud->inst_hooks->hook_map.end())
				hud->inst_hooks->hook_map["Reset"]->remove();
			if (hud->inst_hooks->hook_map.find("EngineDraw") != hud->inst_hooks->hook_map.end())
				hud->inst_hooks->hook_map["EngineDraw"]->remove();
		}

		current_device = hud->inst_game->get_device();
		uint32_t* g_methodsTable = (uint32_t*)::calloc(119, sizeof(uint32_t));
		if (g_methodsTable)
		{
			imgui_initialized = false;
			::memcpy(g_methodsTable, *(uint32_t**)(hud->inst_game->get_device()), 119 * sizeof(uint32_t));
			hud->inst_hooks->Add("EndScene", g_methodsTable[42], EndScene_Hook, hook_type_detour);
			hud->inst_hooks->Add("Reset", g_methodsTable[16], Reset_Hook, hook_type_detour);
			mem::mem_set(0x6496d8, 0x90, 3); //disable check for developer to engine draw
			hud->inst_hooks->Add("EngineDraw", addr_engine_draw, EngineDraw_Hook, hook_type_detour);
			//update the wndproc hook on init
			hud->inst_input->update_wndproc(hud->inst_game->get_window());
		}
	}
}



render::render(Avengers* hud)
{
	//doing it this way only works if its loaded before initgraphics is called
	hud->inst_hooks->Add("InitGraphics", 0x5f4f09, init_graphics_stub, hook_type_replace_call);
}

render::~render() //hooks are removed when the hook wrapper is destroyed
{
	Avengers* hud = Avengers::get_instance();
	if (hud && hud->inst_hooks)
	{
		if (hud->inst_hooks->hook_map.count("InitGraphics") > 0)
			hud->inst_hooks->hook_map["InitGraphics"]->remove(); //remove hook here in case of a race condition on destructors
		if (hud->inst_hooks->hook_map.count("EndScene") > 0)
			hud->inst_hooks->hook_map["EndScene"]->remove(); //remove hook here in case of a race condition on destructors
		if (hud->inst_hooks->hook_map.count("Reset") > 0)
			hud->inst_hooks->hook_map["Reset"]->remove(); //remove hook here in case of a race condition on destructors
		if (hud->inst_hooks->hook_map.count("EngineDraw") > 0)
			hud->inst_hooks->hook_map["EngineDraw"]->remove(); //remove hook here in case of a race condition on destructors

	}
	ImGui::DestroyContext();
}