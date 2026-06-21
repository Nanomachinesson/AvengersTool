#include "pch.h"
#include "ui_position_marker.h"

ImU32 ui_position_marker::im_vec4_to_im_col32(const ImVec4& color)
{
    return IM_COL32(
        static_cast<int>(color.x * 255),
        static_cast<int>(color.y * 255),
        static_cast<int>(color.z * 255),
        static_cast<int>(color.w * 255));
}

ImVec4 ui_position_marker::invertColor(const ImVec4& color)
{
    return ImVec4(1.f - color.x, 1.f - color.y, 1.f - color.z, color.w);
}

ui_position_marker::ui_position_marker(Avengers* avengers) :
    avengers(avengers)
{
    avengers->inst_render->add_callback([this]() { this->menu(); });
}

ui_position_marker::~ui_position_marker()
{
}

void ui_position_marker::render()
{
    bool isConnected = avengers->inst_game->is_connected();

    if (isConnected) {
        std::string currentMap = avengers->inst_game->getMapName();
        if (initializedForMap != currentMap) {
            avengers->load_markers();
            initializedForMap = currentMap;
        }
    }
    else {
        return;
    }

    if (!avengers->inst_ui_menu->render_markers) {
        return;
    }

    constexpr float LINE_HEIGHT = 40.f;
    constexpr float LINE_WIDTH = 20.f;
    constexpr float CIRCLE_RADIUS = 10.f;
    constexpr float ANGLE_WIDTH = 20.f;
    constexpr float FADEOUT_DIST = 150.f;

    bool centerDrawn = false;
    for (std::size_t i = 0; i < markers.size(); i++) {
        vec3<float> playerPos = avengers->inst_game->get_origin();
        float lineWidth = LINE_WIDTH;

        Marker& marker = markers[i];
        if (marker.position.Dist(playerPos) >= avengers->inst_ui_menu->marker_render_distance) {
            continue;
        }

        float markerDist = marker.position.Dist(playerPos);
        if (markerDist >= FADEOUT_DIST) {
            lineWidth = std::fmaxf(LINE_WIDTH * (FADEOUT_DIST / (markerDist * 2.f)), 1.f);
        }

        ImVec4 color = marker.color;
        vec3<float> pos1 = marker.position;
        vec3<float> pos2 = pos1;
        pos2.z += LINE_HEIGHT;
        
        ImVec2 screen1;
        ImVec2 screen2;
        ImVec2 screenAngle1;
        ImVec2 screenAngle2;

        vec3<float> anglingHelper1(pos1.x, pos1.y, pos1.z + LINE_HEIGHT);
        vec3<float> anglingHelper2(pos1.x + ANGLE_WIDTH, pos1.y, pos1.z + LINE_HEIGHT);
        float angleToRotate = marker.angles.y;
        float angleToRotate180 = mm::normalise(marker.angles.y + 180.f, 0.f, 360.f);
        vec2<float> rotated = mm::rotate_point(vec2<float>(anglingHelper2.x, anglingHelper2.y), vec2<float>(anglingHelper1.x, anglingHelper1.y), angleToRotate);
        anglingHelper2.x = rotated.x;
        anglingHelper2.y = rotated.y;

        bool v1 = avengers->inst_game->world_to_screen(pos1, &screen1.x, &screen1.y);
        bool v2 = avengers->inst_game->world_to_screen(pos2, &screen2.x, &screen2.y);

        bool sa1 = avengers->inst_game->world_to_screen(anglingHelper1, &screenAngle1.x, &screenAngle1.y);
        bool sa2 = avengers->inst_game->world_to_screen(anglingHelper2, &screenAngle2.x, &screenAngle2.y);

        if (v1 && v2) {
            ImGui::GetBackgroundDrawList()->AddLine(screen1, screen2, im_vec4_to_im_col32(color), lineWidth);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(screen1, CIRCLE_RADIUS, im_vec4_to_im_col32(invertColor(color)), 36.f);
        }

        if (sa1 && sa2) {
            ImGui::GetBackgroundDrawList()->AddLine(screenAngle1, screenAngle2, im_vec4_to_im_col32(invertColor(color)), lineWidth);
        }

        constexpr float HELPER_CIRCLE_RADIUS = 30.f;
        constexpr float WINDOW_SIZE = 200.f;
        constexpr float WINDOW_OFFSET = 200.f;
        float widgetRenderDist = avengers->inst_ui_menu->widget_render_distance;

        if (((avengers->inst_ui_menu->positioning_helper_onlyonground && avengers->gameState->onGround) || !avengers->inst_ui_menu->positioning_helper_onlyonground)
            && avengers->inst_ui_menu->positioning_helper && marker.position.Dist(playerPos) <= widgetRenderDist) {
            vec3<float> dist = playerPos - marker.position;

            ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE, WINDOW_SIZE));
            ImGui::SetNextWindowPos(ImVec2(avengers->inst_game->get_screen_res().x / 2.f - WINDOW_SIZE / 2.f, avengers->inst_game->get_screen_res().y / 2.f - WINDOW_SIZE / 2.f + WINDOW_OFFSET));
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
            ImGui::Begin("Positioning Helper", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

            ImVec2 centerPos = ImVec2(ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x / 2.f), ImGui::GetWindowPos().y + (ImGui::GetWindowSize().y / 2.f));
            ImVec2 markerPos = ImVec2(centerPos.x - dist.x, centerPos.y + dist.y);

            ImGui::GetWindowDrawList()->AddCircleFilled(markerPos, HELPER_CIRCLE_RADIUS, im_vec4_to_im_col32(marker.color));
            vec2<float> rotated = mm::rotate_point(vec2<float>(markerPos.x - HELPER_CIRCLE_RADIUS - 20.f, markerPos.y), vec2<float>(markerPos.x, markerPos.y), mm::normalise(360.f - angleToRotate180, 0.f, 360.f));
            ImGui::GetWindowDrawList()->AddLine(markerPos, ImVec2(rotated.x, rotated.y), im_vec4_to_im_col32(invertColor(marker.color)), 1.f);

            if (!centerDrawn) {
                ImGui::GetWindowDrawList()->AddCircleFilled(centerPos, HELPER_CIRCLE_RADIUS, ImColor(0.5f, 0.5f, 0.5f, 0.7f));
                vec2<float> rotatedCenter = mm::rotate_point(vec2<float>(centerPos.x + HELPER_CIRCLE_RADIUS + 20.f, centerPos.y), vec2<float>(centerPos.x, centerPos.y), mm::normalise(-1.f * avengers->inst_game->get_view().y, 0.f, 360.f));
                
                ImGui::GetWindowDrawList()->AddLine(centerPos, ImVec2(rotatedCenter.x, rotatedCenter.y), ImColor(1.f, 1.f, 1.f, 1.f), 1.f);
                centerDrawn = true;
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }
}

void ui_position_marker::menu()
{
    if (!(avengers->want_input && avengers->inst_ui_menu->marker_menu)) {
        return;
    }
    bool isConnected = avengers->inst_game->is_connected();

    ImGui::Begin("Marker Menu", &avengers->inst_ui_menu->marker_menu);

    if (!isConnected) {
        ImGui::PushItemFlag(ImGuiItemFlags_::ImGuiItemFlags_Disabled, true);
    }
    if (ImGui::Button("Add Marker")) {
        Marker marker(avengers->gameState->origin, avengers->inst_game->get_view(), selectedColor);
        markers.push_back(marker);
        avengers->save_markers();
    }

    if (!isConnected) {
        ImGui::PopItemFlag();
    }

    ImGui::SameLine();
    ImGui::ColorButton("##Marker Color", selectedColor);

    if (ImGui::IsItemClicked()) {
        ImGui::OpenPopup("MarkerColorPickerPopup");
    }

    if (ImGui::BeginPopup("MarkerColorPickerPopup")) {
        ImGui::ColorPicker4("Marker Color Picker", &selectedColor.x);
        ImGui::EndPopup();
        avengers->save_configuration();
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Render Markers", &avengers->inst_ui_menu->render_markers)) {
        avengers->save_configuration();
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Allow Binds", &avengers->inst_ui_menu->use_marker_binds)) {
        avengers->save_configuration();
    }
    ImGui::PushItemWidth(430.f);
    if (ImGui::SliderFloat("Marker Render Distance", &avengers->inst_ui_menu->marker_render_distance, 0.f, 50000.f)) {
        avengers->save_configuration();
    }
    if (ImGui::Checkbox("Positioning Helper", &avengers->inst_ui_menu->positioning_helper)) {
        avengers->save_configuration();
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Only on ground", &avengers->inst_ui_menu->positioning_helper_onlyonground)) {
        avengers->save_configuration();
    }
    if (ImGui::SliderFloat("Widget Render Distance", &avengers->inst_ui_menu->widget_render_distance, 0.f, 500.f)) {
        avengers->save_configuration();
    }
    ImGui::PopItemWidth();

    ImGui::Separator();
    for (int i = 0; i < static_cast<int>(markers.size()); ++i) {
        Marker& m = markers[i];

        ImGui::PushID(i);

        ImGui::ColorButton("##MarkerColor", m.color);
        std::string popupName = std::string("MarkerColorPickerPopup_") + std::to_string(i);
        if (ImGui::IsItemClicked()) {
            ImGui::OpenPopup(popupName.c_str());
        }

        ImGui::SameLine();
        ImGui::Text("%.4f %.4f %.4f", m.position[0], m.position[1], m.position[2]);
        ImGui::SameLine();

        if (ImGui::Button("Delete")) {
            markers.erase(markers.begin() + i);
            avengers->save_markers();
            ImGui::PopID();
            --i;
            continue;
        }

        if (avengers->inst_game->isDevmap()) {
            ImGui::SameLine();
            if (ImGui::Button("Teleport")) {
                avengers->inst_game->setPosition(m.position);
                avengers->inst_game->setView(m.angles);
            }
        }

        if (ImGui::BeginPopup(popupName.c_str())) {
            ImGui::ColorPicker4("Marker Color Picker", &m.color.x);
            ImGui::EndPopup();
            avengers->save_markers();
        }

        ImGui::PopID();
    }

    ImGui::End();
}
