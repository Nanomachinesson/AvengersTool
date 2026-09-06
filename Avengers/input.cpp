#include "pch.h"
#include "Input.h"
#include "Avengers.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND WINAPI getForegroundWindow()
{
	Avengers* openhud = Avengers::getInstance();
	HWND orig = openhud->instHooks->hookMap["GetForegroundWindow"]->original(getForegroundWindow)();
	HWND currentWindow = openhud->instGame->getWindow();

	if (currentWindow != orig) {
		openhud->instInput->windowReady = false;
	}

	if (openhud->wantInput && openhud->instInput->windowReady) {
		return 0; //tell the game that it isn't the foreground window
	}

	return orig;
}

input::input(Avengers* openhud)
{
	windowHandle = nullptr;
	openhud->instHooks->add("GetForegroundWindow", GetProcAddress(GetModuleHandleA("user32.dll"), "GetForegroundWindow"), getForegroundWindow, hook_type_detour);
}

input::~input()
{
	Avengers* openhud = Avengers::getInstance();
	if (openhud && openhud->instHooks)
	{
		openhud->instHooks->hookMap["GetForegroundWindow"]->remove(); //remove hook here in case of a race condition on destructors
	}
	SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)pWndproc);
}

static void clearImguiKeys()
{
	if (!ImGui::GetCurrentContext()) {
		return;
	}
	ImGui::GetIO().ClearInputKeys();
	ImGui::GetIO().AddKeyEvent(ImGuiKey_Escape, false);
}

static bool isReleaseMessage(UINT uMsg)
{
	switch (uMsg) {
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		return true;
	default:
		return false;
	}
}

bool input::handleKey(UINT keyCode, UINT state)
{

	for (auto &[ key, fn ] : callbacksInput)
	{
		if (key == keyCode && fn(state)) //if the callback returns true then return true here as well so we can block the input
			return true;
	}
	if (keyCode == VK_ESCAPE && Avengers::getInstance()->wantInput)
	{
		Avengers::getInstance()->wantInput = false;
		clearImguiKeys();
		return true;
	}
	return false;
}

LRESULT __stdcall wndprocHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Avengers* hud = Avengers::getInstance();
	if (!hud)
		return 1;

	// Escape must close the menu without ImGui latching the key. WndProc only
	// forwards messages to ImGui while wantInput is true, so a KEYUP after this
	// would never arrive and InputText would treat Escape as cancel forever.
	if ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_ESCAPE && hud->wantInput) {
		hud->wantInput = false;
		clearImguiKeys();
		return 1;
	}

	const bool feedImgui = hud->wantInput || isReleaseMessage(uMsg);
	if (feedImgui && ImGui::GetCurrentContext()
		&& ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) && hud->wantInput) {
		return true;
	}

	if (hud->instGame->isFocused())
	{
		if (uMsg == WM_KEYUP || uMsg == WM_KEYDOWN)
		{
			if (hud->instInput->handleKey(wParam, uMsg))
				return 1; //the key was handled and blocked by the handle key function
		}
	}

	if (hud->wantInput && hud->instInput->windowReady
		&& (uMsg != WM_ACTIVATEAPP)) {  //block input unless its a window change msg
		return 1;
	}

	return CallWindowProc(hud->instInput->pWndproc, hWnd, uMsg, wParam, lParam);
}
void input::addCallback(UINT key, InputCallback fn)
{
	callbacksInput.push_back({ key, fn });
}
void input::updateWndproc(HWND handle)
{
	if (windowHandle != handle)
	{
		if (pWndproc)
			SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)pWndproc);
		windowHandle = handle;
		pWndproc = (WNDPROC)(SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)wndprocHook));
	}
}
