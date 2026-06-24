#pragma once
#include <vector>
#include "imgui.h"
#include "vectors.h"

class Marker
{
public:
    Marker()
    {
    }
    Marker(const vec3<float>& position, const vec3<float>& angles, const ImVec4& color) :
        position(position), angles(angles), color(color)
    {
    }

    vec3<float> position;
    vec3<float> angles;
    ImVec4 color;
};

class ui_position_marker
{
public:
    ui_position_marker(class Avengers* avengers);
    ~ui_position_marker();
    void menu();
    void render();
    void addMarker();
    bool bindSetMarker(UINT keyState);
    bool bindToggleWidget(UINT keyState);
    bool bindToggleRenderMarkers(UINT keyState);

public:
    static ImU32 im_vec4_to_im_col32(const ImVec4& color);
    ImVec4 invertColor(const ImVec4& color);
    std::vector<Marker> markers;
    ImVec4 selectedColor = ImVec4(0.5f, 0.5f, 1.f, 1.f);  //Should be public so we can add it to the config manager

private:
    Avengers* avengers;
    std::string initializedForMap = "";

};
