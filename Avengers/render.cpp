#include "pch.h"
#include "Render.h"
#include "Avengers.h"
#include <cctype>
#include <filesystem>

void initGraphicsStub()
{
	Avengers* hud = Avengers::getInstance();
	if (hud && hud->instRender)
		hud->instRender->initGraphics();
}

void __cdecl EngineDraw_Hook()
{
	Avengers* hud = Avengers::getInstance();
	if (hud && hud->instHooks && hud->instRender)
	{
		hud->instHooks->hookMap["EngineDraw"]->original(EngineDraw_Hook)();
		hud->instRender->enginedraw();
	}
}

HRESULT __stdcall EndScene_Hook(LPDIRECT3DDEVICE9 dev)
{
	Avengers* hud = Avengers::getInstance();

	typedef HRESULT __stdcall EndsceneFunc(LPDIRECT3DDEVICE9 dev);
	EndsceneFunc* endsceneFunc = (EndsceneFunc*)hud->instRender->endsceneAddress;
;
	hud->instInput->windowReady = true;

	if (hud && hud->instHooks && hud->instRender)
	{
		auto orig = endsceneFunc(dev);
		hud->instRender->endscene(dev);
		return orig;
	}

	return 1;
}

HRESULT __stdcall Reset_Hook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{

	Avengers* hud = Avengers::getInstance();
	if (hud && hud->instHooks && hud->instRender)
	{

		auto orig = hud->instHooks->hookMap["Reset"]->original(Reset_Hook);
		hud->instRender->invalidateObjects(pDevice);
		HRESULT rval = orig(pDevice, pPresentationParameters);
		hud->instRender->createObjects(pDevice);
		return rval;
	}
	return 1;
}
void imguiEasyTheming(ImVec4 colorForText, ImVec4 colorForHead, ImVec4 colorForArea, ImVec4 colorForBody, ImVec4 colorForPops)
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


void render::setupImGuiStyle2()
{
	static ImVec4 colorForText = ImVec4(.92f, .94f, .94f, 0);
	static ImVec4 colorForHead = ImVec4(ImColor(65, 60, 73, 255).Value);
	static ImVec4 colorForArea = ImVec4(ImColor(82, 75, 92, 255).Value);
	static ImVec4 colorForBody = ImVec4(ImColor(0, 0, 0, 255).Value);
	static ImVec4 colorForPops = ImColor(40, 37, 45, 255).Value;

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
	

	if (firstRunStyle)
	{
		imguiEasyTheming(colorForText, colorForHead, colorForArea, colorForBody, colorForPops);
		firstRunStyle = false;
	}

}

void render::initImgui(LPDIRECT3DDEVICE9 dev)
{
	if (!imguiInitialized)
	{
		Avengers* hud = Avengers::getInstance();
		if (ImGui::GetCurrentContext()) {
			hud->instUiMenu->menuFont = nullptr;
			hud->instUiMenu->loadedFonts.clear();
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			firstRunStyle = true;
		}

		ImGui_ImplDX9_InvalidateDeviceObjects();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = false;
		ImGui_ImplWin32_Init(Avengers::getInstance()->instGame->getWindow());
		ImGui_ImplDX9_Init(dev);

		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		hud->instUiMenu->loadedFonts.clear();
		ImFont* defaultHudFont = io.Fonts->AddFontFromMemoryTTF(
			(void*)(_acbahnschrift), sizeof(_acbahnschrift) - 1, 24.f, &fontConfig);
		if (defaultHudFont) {
			hud->instUiMenu->loadedFonts.emplace("Bahnschrift", defaultHudFont);
		}

		ImFont* awesomeFont = io.Fonts->AddFontFromMemoryTTF(
			(void*)(_acawesomefont1), sizeof(_acawesomefont1) - 1, 24.f, &fontConfig);
		if (awesomeFont) {
			hud->instUiMenu->loadedFonts.emplace("Awesome Font 1", awesomeFont);
		}

		char windowsDirectory[MAX_PATH] {};
		const UINT windowsDirectoryLength = GetWindowsDirectoryA(windowsDirectory, MAX_PATH);
		if (windowsDirectoryLength > 0 && windowsDirectoryLength < MAX_PATH) {
			const std::filesystem::path trebuchetPath =
				std::filesystem::path(windowsDirectory) / "Fonts" / "trebuc.ttf";
			if (std::filesystem::is_regular_file(trebuchetPath)) {
				hud->instUiMenu->menuFont =
					io.Fonts->AddFontFromFileTTF(trebuchetPath.string().c_str(), 16.f);
			}
		}

		const std::filesystem::path fontDirectory = "AvengersFonts";
		if (std::filesystem::is_directory(fontDirectory)) {
			for (const auto& entry : std::filesystem::directory_iterator(fontDirectory)) {
				if (!entry.is_regular_file()) continue;
				std::string extension = entry.path().extension().string();
				for (char& ch : extension) {
					ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
				}
				if (extension != ".ttf" && extension != ".otf") continue;

				ImFont* customFont = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 24.f);
				if (customFont) {
					hud->instUiMenu->loadedFonts.emplace(entry.path().stem().string(), customFont);
				}
			}
		}
		if (!defaultHudFont) {
			defaultHudFont = io.Fonts->AddFontDefault();
		}
		io.FontDefault = defaultHudFont;
		if (!hud->instUiMenu->menuFont) {
			hud->instUiMenu->menuFont = defaultHudFont;
		}
		
		ImGui_ImplDX9_CreateDeviceObjects();
		imguiInitialized = true;

		setupImGuiStyle2();
	}
	dev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFF);
}

void __cdecl render::enginedraw()
{
	Avengers* hud = Avengers::getInstance();

	if (hud->instGame->isConnected() && hud->instUiMenu->drawCollision) {
		hud->collision->render();
	}

	if (hud->instUiJumpTarget->selectedBrushes.size() > 0 && hud->instUiMenu->brushMode && hud->instUiMenu->drawSelectedBrushes) {
		const auto polyLit = false;
		const auto polyOutlines = false;
		const auto polyLinecolor = ImColor(255, 255, 255, 255);
		const auto polyDepth = true;
		const auto polyFace = false;
		ImColor color(0.3f, 1.f, 0.f, 0.4f);

		for (BrushSide* face : hud->instUiJumpTarget->selectedBrushes) {
			vec3<float>* points = face->points.data();
			hud->instGame->drawPoly(face->points.size(), (float(*)[3]) points, (const float*)&color,
				polyLit, polyOutlines, (const float*)&polyLinecolor, polyDepth, polyFace);
		}
	}

	if (hud->instGame->isConnected() && hud->instUiMenu->linesToggle) {
		hud->instUi90Lines->render();
	}
}

void render::endscene(LPDIRECT3DDEVICE9 dev)
{
	initImgui(dev);
	auto& io = ImGui::GetIO();

	Avengers* hud = Avengers::getInstance();
	if (hud->wantInput)
		io.MouseDrawCursor = true;
	else
		io.MouseDrawCursor = false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	if (!hud->wantInput) {
		io.ClearInputKeys();
	}

	for (auto& fn : callbacksRender)
		fn();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void render::addCallback(RenderCallback render)
{
	callbacksRender.push_back(render);
}

void render::invalidateObjects(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void render::createObjects(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void render::initGraphics()
{
	Avengers* hud = Avengers::getInstance();
	//call the original function first
	hud->instHooks->hookMap["InitGraphics"]->original(initGraphicsStub)();
	static LPDIRECT3DDEVICE9 currentDevice = nullptr;
	if (currentDevice != hud->instGame->getDevice())
	{
		Avengers* hud = Avengers::getInstance();

		if (hud && hud->instHooks) //remove the old hooks
		{
			if (hud->instHooks->hookMap.find("EndScene") != hud->instHooks->hookMap.end())
				hud->instHooks->hookMap["EndScene"]->remove();
			if (hud->instHooks->hookMap.find("Reset") != hud->instHooks->hookMap.end())
				hud->instHooks->hookMap["Reset"]->remove();
			if (hud->instHooks->hookMap.find("EngineDraw") != hud->instHooks->hookMap.end())
				hud->instHooks->hookMap["EngineDraw"]->remove();
		}

		currentDevice = hud->instGame->getDevice();
		uint32_t* gMethodsTable = (uint32_t*)::calloc(119, sizeof(uint32_t));
		if (gMethodsTable)
		{
			imguiInitialized = false;
			::memcpy(gMethodsTable, *(uint32_t**)(hud->instGame->getDevice()), 119 * sizeof(uint32_t));
			endsceneAddress = gMethodsTable[42];
			hud->instHooks->add("Reset", gMethodsTable[16], Reset_Hook, hook_type_detour);
			mem::memSet(0x6496d8, 0x90, 3); //disable check for developer to engine draw
			hud->instHooks->add("EngineDraw", addr_engine_draw, EngineDraw_Hook, hook_type_detour);
			//update the wndproc hook on init
			hud->instInput->updateWndproc(hud->instGame->getWindow());

			//Hook endscene call in RB_CallExecuteRenderCommands

			DWORD dwOldProtect;
			_MEMORY_BASIC_INFORMATION mbi = { 0,0,0,0,0,0,0 };
			VirtualQuery((LPVOID)addr_rb_callexecuterendercommands_callafter, &mbi, sizeof(mbi));
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

			BYTE* callInstruction = (BYTE*)addr_rb_callexecuterendercommands_callafter;
			*callInstruction = 0xE8;
			DWORD relativeAddress = (DWORD)EndScene_Hook - (DWORD)addr_rb_callexecuterendercommands_callafter - 5;
			*(DWORD*)((DWORD)addr_rb_callexecuterendercommands_callafter + 1) = relativeAddress;

			VirtualProtect((LPVOID)addr_rb_callexecuterendercommands_callafter, 1000, dwOldProtect, &dwOldProtect);

			///////////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
}



render::render(Avengers* hud)
{
	//doing it this way only works if its loaded before initgraphics is called
	hud->instHooks->add("InitGraphics", 0x5f4f09, initGraphicsStub, hook_type_replace_call);
}

render::~render() //hooks are removed when the hook wrapper is destroyed
{
	Avengers* hud = Avengers::getInstance();
	if (hud && hud->instHooks)
	{
		if (hud->instHooks->hookMap.count("InitGraphics") > 0)
			hud->instHooks->hookMap["InitGraphics"]->remove(); //remove hook here in case of a race condition on destructors
		if (hud->instHooks->hookMap.count("Reset") > 0)
			hud->instHooks->hookMap["Reset"]->remove(); //remove hook here in case of a race condition on destructors
		if (hud->instHooks->hookMap.count("EngineDraw") > 0)
			hud->instHooks->hookMap["EngineDraw"]->remove(); //remove hook here in case of a race condition on destructors

	}
	ImGui::DestroyContext();
}