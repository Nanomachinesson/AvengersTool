#pragma once
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "imgui.h"
#include "ui_theme.h"
#include "vectors.h"

namespace uiw
{
	constexpr float kSettingRowGap = 18.f;
	constexpr float kGridGap = 16.f;
	constexpr float kGridMinColumnWidth = 220.f;
	constexpr float kCardPadding = 16.f;
	constexpr float kCardGap = 14.f;

	struct CardLayoutState
	{
		float minimumHeight = 0.f;
	};

	inline float& gridColumnWidth()
	{
		static float width = 0.f;
		return width;
	}

	inline CardLayoutState& cardLayoutState()
	{
		static CardLayoutState state;
		return state;
	}

	inline float currentCardWidth()
	{
		const float avail = ImGui::GetContentRegionAvail().x;
		if (gridColumnWidth() > 1.f && gridColumnWidth() < avail + 0.5f) {
			return gridColumnWidth();
		}
		return avail;
	}

	inline void finishSetting()
	{
		const ImVec2 p = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(p.x, p.y - ImGui::GetStyle().ItemSpacing.y + kSettingRowGap));
	}

	inline float lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	inline ImVec4 lerp(const ImVec4& a, const ImVec4& b, float t)
	{
		return ImVec4(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t), lerp(a.w, b.w, t));
	}

	inline float saturate(float v)
	{
		if (v < 0.f) return 0.f;
		if (v > 1.f) return 1.f;
		return v;
	}

	inline bool& tooltipsEnabled()
	{
		static bool enabled = true;
		return enabled;
	}

	inline void tooltip(const char* text, ImGuiHoveredFlags flags = ImGuiHoveredFlags_None)
	{
		if (tooltipsEnabled() && text && text[0] && ImGui::IsItemHovered(flags)) {
			ImGui::SetTooltip("%s", text);
		}
	}

	inline void checkboxTooltip(const char* text)
	{
		if (!tooltipsEnabled() || !text || !text[0]) {
			return;
		}

		const ImVec2 controlMin = ImGui::GetItemRectMin();
		const float controlSize = ImGui::GetFrameHeight();
		const ImVec2 controlMax(controlMin.x + controlSize, controlMin.y + controlSize);
		if (ImGui::IsMouseHoveringRect(controlMin, controlMax)) {
			ImGui::SetTooltip("%s", text);
		}
	}

	inline void beginGrid()
	{
		gridColumnWidth() = (ImGui::GetContentRegionAvail().x - kGridGap) * 0.5f;
		if (gridColumnWidth() < kGridMinColumnWidth) {
			gridColumnWidth() = 0.f;
			return;
		}
		ImGui::BeginGroup();
	}

	inline void nextGridColumn()
	{
		if (gridColumnWidth() <= 0.f) {
			return;
		}
		ImGui::EndGroup();
		ImGui::SameLine(0.f, kGridGap);
		ImGui::BeginGroup();
	}

	inline void endGrid()
	{
		if (gridColumnWidth() > 0.f) {
			ImGui::EndGroup();
		}
		gridColumnWidth() = 0.f;
	}

	inline void beginCard(const char* id, float minimumHeight = 0.f)
	{
		ImGui::PushID(id);
		cardLayoutState().minimumHeight = minimumHeight;
		if (minimumHeight > 0.f) {
			ImGui::BeginGroup();
		}

		ImDrawList* draw = ImGui::GetWindowDrawList();
		draw->ChannelsSplit(2);
		draw->ChannelsSetCurrent(1);
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(currentCardWidth(), kCardPadding));
		ImGui::Indent(kCardPadding);
	}

	inline void endCard()
	{
		const UiTheme& t = uiTheme();
		ImGui::Unindent(kCardPadding);
		ImGui::Dummy(ImVec2(0.f, kCardPadding));
		ImGui::EndGroup();

		ImVec2 rmin = ImGui::GetItemRectMin();
		ImVec2 rmax = ImGui::GetItemRectMax();
		const float minimumHeight = cardLayoutState().minimumHeight;
		if (minimumHeight > 0.f) {
			const float naturalHeight = rmax.y - rmin.y;
			if (naturalHeight < minimumHeight) {
				const float fillerHeight = minimumHeight - naturalHeight - ImGui::GetStyle().ItemSpacing.y;
				if (fillerHeight > 0.f) {
					ImGui::Dummy(ImVec2(0.f, fillerHeight));
				}
			}
			ImGui::EndGroup();
			rmin = ImGui::GetItemRectMin();
			rmax = ImGui::GetItemRectMax();
		}

		const float width = currentCardWidth();
		if (width > 1.f) {
			rmax.x = rmin.x + width;
		}

		ImDrawList* draw = ImGui::GetWindowDrawList();
		draw->ChannelsSetCurrent(0);
		draw->AddRectFilled(rmin, rmax, uiCol32(t.surface), 6.f);
		draw->ChannelsMerge();

		ImGui::Dummy(ImVec2(0.f, kCardGap));
		cardLayoutState().minimumHeight = 0.f;
		ImGui::PopID();
	}

	inline bool pillToggle(const char* id, bool* value)
	{
		const UiTheme& t = uiTheme();
		const ImVec2 size(38.f, 20.f);
		const float knob = 14.f;

		ImGui::InvisibleButton(id, size);
		const bool pressed = ImGui::IsItemClicked();
		if (pressed) {
			*value = !*value;
		}

		const ImGuiID itemId = ImGui::GetItemID();
		ImGuiStorage* storage = ImGui::GetStateStorage();
		float anim = storage->GetFloat(itemId, *value ? 1.f : 0.f);
		const float target = *value ? 1.f : 0.f;
		const float speed = (ImGui::GetIO().DeltaTime * 14.f < 1.f) ? (ImGui::GetIO().DeltaTime * 14.f) : 1.f;
		anim = lerp(anim, target, speed);
		if (anim > 0.995f && target == 1.f) anim = 1.f;
		if (anim < 0.005f && target == 0.f) anim = 0.f;
		storage->SetFloat(itemId, anim);

		const ImVec2 pmin = ImGui::GetItemRectMin();
		const ImVec2 pmax = ImGui::GetItemRectMax();
		ImDrawList* draw = ImGui::GetWindowDrawList();
		draw->AddRectFilled(pmin, pmax, uiCol32(lerp(t.toggleOff, t.accent, anim)), 10.f);

		const float pad = (size.y - knob) * 0.5f;
		const float knobX = lerp(pmin.x + pad, pmax.x - pad - knob, anim);
		const float knobY = pmin.y + pad;
		draw->AddCircleFilled(ImVec2(knobX + knob * 0.5f, knobY + knob * 0.5f), knob * 0.5f, IM_COL32(255, 255, 255, 255));
		return pressed;
	}

	inline void mutedText(const char* text)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, uiTheme().textMuted);
		ImGui::TextUnformatted(text);
		ImGui::PopStyleColor();
	}

	inline float rightControlX(float controlW)
	{
		return ImGui::GetCursorScreenPos().x + currentCardWidth() - 16.f - 16.f - controlW;
	}

	inline bool settingToggle(const char* label, const char* desc, bool* value)
	{
		ImGui::PushID(label);
		const float toggleW = 38.f;
		const float wrapW = currentCardWidth() - toggleW - 40.f;

		ImGui::BeginGroup();
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapW);
		ImGui::TextUnformatted(label);
		ImGui::PopTextWrapPos();
		ImGui::EndGroup();
		tooltip(desc);

		const ImVec2 labelMin = ImGui::GetItemRectMin();
		const ImVec2 labelMax = ImGui::GetItemRectMax();
		const float rowH = labelMax.y - labelMin.y;
		float toggleY = labelMin.y + (rowH - 20.f) * 0.5f;
		if (toggleY < labelMin.y) {
			toggleY = labelMin.y;
		}

		const ImVec2 afterLabel = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(rightControlX(toggleW), toggleY));
		const bool changed = pillToggle("##toggle", value);
		tooltip(desc);
		const ImVec2 toggleMax = ImGui::GetItemRectMax();
		const float rowBottom = (afterLabel.y > toggleMax.y) ? afterLabel.y : toggleMax.y;
		ImGui::SetCursorScreenPos(ImVec2(afterLabel.x, rowBottom + kSettingRowGap));
		ImGui::PopID();
		return changed;
	}

	inline void pushInputStyle()
	{
		const UiTheme& t = uiTheme();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, t.bgAlt);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, t.surfaceHover);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, t.surfaceHover);
		ImGui::PushStyleColor(ImGuiCol_Border, t.border);
		ImGui::PushStyleColor(ImGuiCol_Text, t.text);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 6.f));
	}

	inline void popInputStyle()
	{
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(5);
	}

	inline bool settingTextInput(const char* label, const char* desc, char* buf, int bufSize, const char* hint = nullptr)
	{
		ImGui::PushID(label);
		ImGui::TextUnformatted(label);
		pushInputStyle();
		ImGui::SetNextItemWidth(currentCardWidth() - 32.f);
		const bool changed = hint && hint[0]
			? ImGui::InputTextWithHint("##input", hint, buf, bufSize)
			: ImGui::InputText("##input", buf, bufSize);
		popInputStyle();
		tooltip(desc);
		finishSetting();
		ImGui::PopID();
		return changed;
	}

	inline bool tryParseBoundedFloat(const char* text, float minV, float maxV, bool integer, float* out)
	{
		if (!text || !out) {
			return false;
		}

		const char* cursor = text;
		while (*cursor && std::isspace(static_cast<unsigned char>(*cursor))) {
			++cursor;
		}
		if (*cursor == '\0') {
			return false;
		}

		char* end = nullptr;
		const float parsed = std::strtof(cursor, &end);
		if (end == cursor) {
			return false;
		}
		while (*end && std::isspace(static_cast<unsigned char>(*end))) {
			++end;
		}
		if (*end != '\0') {
			return false;
		}
		if (!std::isfinite(parsed)) {
			return false;
		}

		float value = parsed;
		if (integer) {
			value = std::floor(value + (value >= 0.f ? 0.5f : -0.5f));
		}
		if (value < minV) {
			value = minV;
		}
		if (value > maxV) {
			value = maxV;
		}
		*out = value;
		return true;
	}

	inline bool thinSlider(const char* id, float* value, float minV, float maxV)
	{
		const UiTheme& t = uiTheme();
		const float width = currentCardWidth() - 32.f;
		ImGui::InvisibleButton(id, ImVec2(width, 16.f));
		const float previousValue = *value;
		const bool active = ImGui::IsItemActive();
		if (active && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			const ImVec2 pmin = ImGui::GetItemRectMin();
			const ImVec2 pmax = ImGui::GetItemRectMax();
			const float t01 = saturate((ImGui::GetIO().MousePos.x - pmin.x) / (pmax.x - pmin.x));
			*value = minV + t01 * (maxV - minV);
		}

		const ImVec2 pmin = ImGui::GetItemRectMin();
		const ImVec2 pmax = ImGui::GetItemRectMax();
		const float midY = (pmin.y + pmax.y) * 0.5f;
		const float span = (maxV > minV) ? (maxV - minV) : 1.f;
		const float t01 = saturate((*value - minV) / span);
		const float knobX = pmin.x + t01 * (pmax.x - pmin.x);

		ImDrawList* draw = ImGui::GetWindowDrawList();
		draw->AddLine(ImVec2(pmin.x, midY), ImVec2(pmax.x, midY), uiCol32(t.toggleOff), 2.f);
		draw->AddLine(ImVec2(pmin.x, midY), ImVec2(knobX, midY), uiCol32(t.accent), 2.f);
		draw->AddRectFilled(ImVec2(knobX - 1.f, midY - 6.f), ImVec2(knobX + 1.f, midY + 6.f), uiCol32(t.text), 1.f);
		return *value != previousValue;
	}

	inline bool settingSlider(const char* label, const char* desc, float* value, float minV, float maxV, const char* fmt = "%.2f")
	{
		ImGui::PushID(label);
		const bool integer = (std::strcmp(fmt, "%.0f") == 0);
		ImGuiStorage* storage = ImGui::GetStateStorage();
		const ImGuiID editingId = ImGui::GetID("##editing");
		const ImGuiID focusId = ImGui::GetID("##focus");
		bool editing = storage->GetBool(editingId, false);

		struct SliderEditBuffer
		{
			ImGuiID owner = 0;
			char text[32]{};
		};
		static SliderEditBuffer edit;

		char valueText[32];
		std::snprintf(valueText, sizeof(valueText), fmt, *value);
		const ImVec2 valueTextSize = ImGui::CalcTextSize(editing ? edit.text : valueText);
		const float valueFieldW = (valueTextSize.x + 16.f > 56.f) ? valueTextSize.x + 16.f : 56.f;
		const float valueFieldH = ImGui::GetTextLineHeight() + 6.f;

		ImGui::TextUnformatted(label);
		const ImVec2 afterLabel = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(
			rightControlX(valueFieldW),
			afterLabel.y - ImGui::GetTextLineHeight() - ImGui::GetStyle().ItemSpacing.y - 2.f));

		bool changed = false;
		if (editing && edit.owner == editingId) {
			if (storage->GetBool(focusId, false)) {
				ImGui::SetKeyboardFocusHere();
				storage->SetBool(focusId, false);
			}
			pushInputStyle();
			ImGui::SetNextItemWidth(valueFieldW);
			ImGui::InputText("##value", edit.text, sizeof(edit.text),
				ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue);
			popInputStyle();
			tooltip("Enter a number. Invalid input is ignored; values are clamped to the slider range.");

			if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsItemDeactivated()) {
				float parsed = *value;
				if (tryParseBoundedFloat(edit.text, minV, maxV, integer, &parsed) && parsed != *value) {
					*value = parsed;
					changed = true;
				}
				storage->SetBool(editingId, false);
				editing = false;
				edit.owner = 0;
			}
		}
		else {
			const ImVec2 fieldMin = ImGui::GetCursorScreenPos();
			ImGui::InvisibleButton("##valueHit", ImVec2(valueFieldW, valueFieldH));
			const bool hovered = ImGui::IsItemHovered();
			if (ImGui::IsItemClicked()) {
				edit.owner = editingId;
				std::snprintf(edit.text, sizeof(edit.text), fmt, *value);
				storage->SetBool(editingId, true);
				storage->SetBool(focusId, true);
			}

			const ImVec2 fieldMax = ImGui::GetItemRectMax();
			ImDrawList* draw = ImGui::GetWindowDrawList();
			draw->AddRectFilled(fieldMin, fieldMax, uiCol32(hovered ? uiTheme().surfaceHover : uiTheme().bgAlt), 4.f);
			draw->AddRect(fieldMin, fieldMax, uiCol32(uiTheme().border), 4.f);
			const ImVec2 textPos(
				fieldMin.x + (valueFieldW - valueTextSize.x) * 0.5f,
				fieldMin.y + (valueFieldH - valueTextSize.y) * 0.5f);
			draw->AddText(textPos, uiCol32(uiTheme().textMuted), valueText);
			tooltip("Click to type a value.");
		}

		ImGui::SetCursorScreenPos(afterLabel);
		if (thinSlider("##slider", value, minV, maxV)) {
			changed = true;
			if (editing) {
				storage->SetBool(editingId, false);
				edit.owner = 0;
			}
		}
		tooltip(desc);
		finishSetting();
		ImGui::PopID();
		return changed;
	}

	inline bool settingColor(const char* label, const char* desc, ImVec4* color)
	{
		ImGui::PushID(label);
		ImGui::TextUnformatted(label);

		const ImVec2 afterLabel = ImGui::GetCursorScreenPos();
		const float swatch = 16.f;
		float swatchY = afterLabel.y - ImGui::GetTextLineHeight() - ImGui::GetStyle().ItemSpacing.y;
		ImGui::SetCursorScreenPos(ImVec2(rightControlX(swatch), swatchY));
		const bool opened = ImGui::ColorButton("##swatch", *color, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview, ImVec2(swatch, swatch));
		tooltip(desc);
		if (opened) {
			ImGui::OpenPopup("##colorPopup");
		}
		ImGui::SetCursorScreenPos(afterLabel);

		bool changed = false;
		if (ImGui::BeginPopup("##colorPopup")) {
			changed = ImGui::ColorPicker4("##picker", &color->x, ImGuiColorEditFlags_AlphaBar);
			ImGui::EndPopup();
		}
		ImGui::SetCursorScreenPos(ImVec2(afterLabel.x, afterLabel.y + kSettingRowGap));
		ImGui::PopID();
		return changed;
	}

	inline bool settingCombo(const char* label, const char* desc, std::string* current, const char* const* items, int count)
	{
		ImGui::PushID(label);
		ImGui::TextUnformatted(label);
		ImGui::SetNextItemWidth(currentCardWidth() - 32.f);
		bool changed = false;
		const bool comboOpen = ImGui::BeginCombo("##combo", current->c_str());
		const bool comboHovered = ImGui::IsItemHovered();
		if (comboOpen) {
			for (int i = 0; i < count; ++i) {
				const bool selected = (*current == items[i]);
				if (ImGui::Selectable(items[i], selected)) {
					*current = items[i];
					changed = true;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (tooltipsEnabled() && comboHovered && desc && desc[0]) {
			ImGui::SetTooltip("%s", desc);
		}
		finishSetting();
		ImGui::PopID();
		return changed;
	}

	inline bool navButton(const char* label, bool active)
	{
		const UiTheme& t = uiTheme();
		const ImVec2 size(ImGui::GetContentRegionAvail().x, 40.f);
		ImGui::InvisibleButton(label, size);
		const bool pressed = ImGui::IsItemClicked();
		const bool hovered = ImGui::IsItemHovered();

		const ImVec2 pmin = ImGui::GetItemRectMin();
		const ImVec2 pmax = ImGui::GetItemRectMax();
		ImDrawList* draw = ImGui::GetWindowDrawList();

		if (active || hovered) {
			draw->AddRectFilled(pmin, pmax, uiCol32(active ? t.surfaceHover : t.surface), 8.f);
		}

		const ImVec2 textSize = ImGui::CalcTextSize(label);
		const ImVec2 textPos(pmin.x + 12.f, pmin.y + (size.y - textSize.y) * 0.5f);
		draw->AddText(textPos, uiCol32(active ? t.accent : t.text), label);
		return pressed;
	}

	inline void vec3Readonly(const char* label, const vec3<float>& value)
	{
		ImGui::TextUnformatted(label);
		ImGui::PushStyleColor(ImGuiCol_Text, uiTheme().textMuted);
		ImGui::Text("%.4f   %.4f   %.4f", value.x, value.y, value.z);
		ImGui::PopStyleColor();
		finishSetting();
	}

	inline void pageHeader(const char* title, const char* subtitle)
	{
		char headed[64];
		std::snprintf(headed, sizeof(headed), "[%s]", title);
		ImGui::TextUnformatted(headed);
		if (subtitle && subtitle[0]) {
			mutedText(subtitle);
		}
		ImGui::Dummy(ImVec2(0.f, 12.f));
	}

	inline bool accentButton(const char* label, const ImVec2& size = ImVec2(0.f, 0.f))
	{
		const UiTheme& t = uiTheme();
		ImGui::PushStyleColor(ImGuiCol_Button, t.accent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, t.accentHover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, t.accent);
		ImGui::PushStyleColor(ImGuiCol_Text, t.text);
		const bool pressed = ImGui::Button(label, size);
		ImGui::PopStyleColor(4);
		return pressed;
	}
}
