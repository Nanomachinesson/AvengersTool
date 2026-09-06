#include "pch.h"
#include "ui_theme.h"

static const UiTheme kTheme{};

const UiTheme& uiTheme()
{
	return kTheme;
}

ImU32 uiCol32(const ImVec4& c)
{
	return ImGui::ColorConvertFloat4ToU32(c);
}

ThemePush uiThemeApply()
{
	const UiTheme& t = kTheme;
	ThemePush pushed;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);        ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.f);         ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);         ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 2.f);          ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6.f);         ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.f);           ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 6.f);     ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 7.f)); ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.f, 8.f)); ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(8.f, 4.f)); ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 8.f);         ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);      ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);       ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);       ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.f);       ++pushed.vars;
	ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 4.f);           ++pushed.vars;

	ImGui::PushStyleColor(ImGuiCol_Text, t.text);                 ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, t.textMuted);    ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, t.bg);               ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ChildBg, t.bg);                ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_PopupBg, t.bgAlt);             ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_Border, t.border);             ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0)); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, t.surface);           ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, t.surfaceHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, t.surfaceHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TitleBg, t.bg);                ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, t.bg);          ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, t.bg);       ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, t.bgAlt);           ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, t.bgAlt);         ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, t.surfaceHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, t.accent); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, t.accentHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_CheckMark, t.accent);          ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, t.text);           ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, t.text);     ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_Button, t.surface);            ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, t.surfaceHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, t.accentDim);    ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_Header, t.surfaceHover);       ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, t.surfaceHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, t.accentDim);    ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_Separator, t.border);          ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, t.accent);   ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, t.accent);    ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, t.border);         ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, t.accent);  ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, t.accentHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_Tab, t.surface);               ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TabHovered, t.accent);         ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TabActive, t.accent);          ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TabUnfocused, t.surface);      ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, t.surface); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_PlotLines, t.accent);          ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, t.accent);      ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, t.surface);     ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, t.border);  ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TableBorderLight, t.border);   ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TableRowBg, t.bg);             ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, t.bgAlt);       ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, t.accentDim);  ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, t.accent);     ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_NavHighlight, t.accent);       ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, t.accentHover); ++pushed.colors;
	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.02f, 0.02f, 0.02f, 0.55f)); ++pushed.colors;

	return pushed;
}

void uiThemeRestore(const ThemePush& pushed)
{
	ImGui::PopStyleColor(pushed.colors);
	ImGui::PopStyleVar(pushed.vars);
}
