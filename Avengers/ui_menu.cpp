#include "pch.h"
#include "ui_menu.h"
#include "Avengers.h"

void ui_menu::menu(Avengers* hud)
{
	ImGui::Begin("Avengers Helper");

	if (should_focus_next_frame) {
		ImGui::SetWindowFocus();
		should_focus_next_frame = false;
	}

	//################ Demoplayer menu toggle ###############
	if(ImGui::Button("Demo Player"))
	{
		demoplayer_menu = !demoplayer_menu;
	}
	//#######################################################

	//################ Bind demo to load key ###############
	static char demoName[128] = "";
	ImGui::InputText("Demo Name", demoName, 128);
	ImGui::SameLine(); if (ImGui::Button("Bind demo to load key"))
	{
		if (std::string(demoName) != "")
		{
			std::stringstream ss;
			ss << "bind f \"openscriptmenu cj load;stoprecord;record " << demoName << "\"";
			
			hud->inst_game->send_command_to_console(ss.str().c_str());
		}
		else
		{
			std::string cmd = "bind f \"openscriptmenu cj load;stoprecord;record\"";

			hud->inst_game->send_command_to_console(cmd.c_str());
		}
	}
	//#######################################################

	//################# Position ############################
	if (ImGui::Button("Copy position"))
	{
		vec3<float> pos = hud->inst_game->get_origin();
		vec3<float> view = hud->inst_game->get_view();

		copied_position_origin = pos;
		copied_position_view = view;
		hud->save_configuration();
		
		pos.z += 60.f;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(6) << pos.x <<  " " << pos.y << " " << pos.z << " " << view.y << " " << view.x;

		ImGui::SetClipboardText(ss.str().c_str());
	}
	ImGui::SameLine(); if(ImGui::Checkbox("Show Coordinates", &show_position))
	{
		hud->save_configuration();
	}

	//#######################################################
	
	//################# SPEEDOMETER ########################
	if (ImGui::Checkbox("Speedometer", &velo_meter))
	{
		hud->save_configuration();
	}
	
	ImGui::SameLine(); ImGui::ColorButton("Color Button", color);


	if(ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("ColorPickerPopup");
	}

	if(ImGui::BeginPopup("ColorPickerPopup"))
	{
		ImGui::ColorPicker4("Color Picker", &color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}

	ImGui::SameLine(); ImGui::Checkbox("Lock Speed Position", &lock_velo_pos);
	ImGui::SameLine();
	if (ImGui::Checkbox("Keep Centered", &keep_velo_centered)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	if (ImGui::Button("Center")) {
		ImGui::SetWindowFontScale(velo_scale);
		if (hud->inst_ui_menu->sep_velo) {
			ImGui::PushFont(hud->sep_font);
		}
		else {
			ImGui::PushFont(hud->toxic_font);
		}

		if (keep_velo_centered) {
			velo_pos.x = hud->inst_game->get_screen_res().x / 2.f - ImGui::CalcTextSize("0").x / 2.f;
		}
		velo_pos.y = hud->inst_game->get_screen_res().y / 2.f;

		ImGui::SetWindowFontScale(1.f);
		ImGui::PopFont();

		hud->save_configuration();
	}

	ImGui::SameLine();
	static std::string currentSpeedoStyle = sep_velo ? "Style 2" : "Style 1";
	std::array<std::string, 2> speedoStyles = { "Style 1", "Style 2" };
	ImGui::PushItemWidth(200.f);
	if (ImGui::BeginCombo("##Speedometer style", currentSpeedoStyle.c_str())) {
		for (int n = 0; n < speedoStyles.size(); n++) {
			bool isSelected = (currentSpeedoStyle == speedoStyles[n]);
			if (ImGui::Selectable(speedoStyles[n].c_str(), isSelected)) {
				currentSpeedoStyle = speedoStyles[n].c_str();
				ImGui::SetItemDefaultFocus();
				if (currentSpeedoStyle == "Style 2") {
					sep_velo = true;
				}
				else {
					sep_velo = false;
				}

				hud->save_configuration();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();


	//Acceleration
	if (ImGui::Checkbox("Show acceleration", &velo_show_acceleration)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	ImGui::ColorButton("Acceleration Color Button", acceleration_color);

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("AccelerationColorPickerPopup");
	}

	if (ImGui::BeginPopup("AccelerationColorPickerPopup")) {
		ImGui::ColorPicker4("Acceleration Color Picker", &acceleration_color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}

	ImGui::SameLine();

	//Deceleration
	if (ImGui::Checkbox("Show deceleration", &velo_show_deceleration)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	ImGui::ColorButton("Deceleration Color Button", deceleration_color);

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("DecelerationColorPickerPopup");
	}

	if (ImGui::BeginPopup("DecelerationColorPickerPopup")) {
		ImGui::ColorPicker4("Deceleration Color Picker", &deceleration_color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}
	
	/////////////
	if(ImGui::SliderFloat("Speed Size", &velo_scale, 0.01f, 10.f))
	{
		hud->save_configuration();
	}

	if (ImGui::Checkbox("Jumpoff speed", &draw_jumpoff_speed)) {
		hud->save_configuration();
	}

	ImGui::SameLine();
	if (ImGui::Checkbox("Display bottom left", &jumpoffspeed_display_bottom)) {
		hud->save_configuration();
	}

	if (ImGui::Checkbox("Strafe downtime", &strafedowntime_toggle)) {
		hud->save_configuration();
	}
	//#######################################################

	//################# JUMP TARGET ########################
	ImGui::Checkbox("Enable Jump Target", &jump_target);
	
	ImGui::SameLine();
	if(ImGui::Button("Set Jump target"))
	{
		jump_target_origin = hud->inst_game->get_origin();
	}

	//#######################################################

	//################# POSITION MARKERS ########################
	if (ImGui::Button("Mark Position 1"))
	{
		draw_marker1 = true;
		marker1 = hud->inst_game->get_origin();
	}
	ImGui::SameLine(); ImGui::ColorButton("Marker 1 Colour Button", marker1_color);

	if(ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("Marker1ColorPickerPopup");
	}

	if(ImGui::BeginPopup("Marker1ColorPickerPopup"))
	{
		ImGui::ColorPicker4("Color Picker", &marker1_color.x);

		ImGui::EndPopup();
	}
	ImGui::SameLine(); if (ImGui::Button("Remove Marker 1"))
	{
		draw_marker1 = false;
	}
	
	if (ImGui::Button("Mark Position 2"))
	{
		draw_marker2 = true;
		marker2 = hud->inst_game->get_origin();
	}
	ImGui::SameLine(); ImGui::ColorButton("Marker 2 Colour Button", marker2_color);

	if(ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("Marker2ColorPickerPopup");
	}

	if(ImGui::BeginPopup("Marker2ColorPickerPopup"))
	{
		ImGui::ColorPicker4("Color Picker", &marker2_color.x);

		ImGui::EndPopup();
	}
	ImGui::SameLine(); if (ImGui::Button("Remove Marker 2"))
	{
		draw_marker2 = false;
	}
	
	if (ImGui::Button("Mark Position 3"))
	{
		draw_marker3 = true;
		marker3 = hud->inst_game->get_origin();
	}
	ImGui::SameLine(); ImGui::ColorButton("Marker 3 Colour Button", marker3_color);

	if(ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("Marker3ColorPickerPopup");
	}

	if(ImGui::BeginPopup("Marker3ColorPickerPopup"))
	{
		ImGui::ColorPicker4("Color Picker", &marker3_color.x);

		ImGui::EndPopup();
	}
	ImGui::SameLine(); if (ImGui::Button("Remove Marker 3"))
	{
		draw_marker3 = false;
	}

	ImGui::SliderFloat("Marker Size", &marker_size, 5.0f, 100.0f);
	
	//#######################################################

	//################# ANGLE HELPER ########################
	if (ImGui::Checkbox("Anglehelper", &anglehelper_toggle))
	{
		hud->save_configuration();
	}
	ImGui::SameLine();
	ImGui::ColorButton("Anglehelper color", anglehelper_color);

	if (ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("AnglehelperColorPickerPopup");
	}

	if (ImGui::BeginPopup("AnglehelperColorPickerPopup"))
	{
		ImGui::ColorPicker4("Anglehelper Color Picker", &anglehelper_color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}

	ImGui::SameLine();

	std::array<std::string, 2> ahStyles = { "Style 1", "Style 2" };
	ImGui::PushItemWidth(200.f);
	if (ImGui::BeginCombo("##Anglehelper style", currentAhStyle.c_str())) {
		for (int n = 0; n < ahStyles.size(); n++) {
			bool isSelected = (currentAhStyle == ahStyles[n]);
			if (ImGui::Selectable(ahStyles[n].c_str(), isSelected)) {
				currentAhStyle = ahStyles[n];
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}

				hud->save_configuration();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Checkbox("Clamp to next zone", &clamp_to_next_zone)) {
		hud->save_configuration();
	}

	if (ImGui::SliderFloat("Anglehelper y-offset", &anglehelper_y_offset, -1000.f, 1000.f)) {
		hud->save_configuration();
	}

	if (ImGui::Checkbox("Draw centerline", &drawcenterline)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	ImGui::ColorButton("Centerline color", centerline_color);

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("CenterLineColorPickerPopup");
	}

	if (ImGui::BeginPopup("CenterLineColorPickerPopup")) {
		ImGui::ColorPicker4("Centerline Color Picker", &centerline_color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}

	//#######################################################

	//################# 90 lines ########################
	if (ImGui::Checkbox("90 lines", &lines_toggle))
	{
		hud->save_configuration();
	}
	
	ImGui::SameLine(); ImGui::ColorButton("90 Lines Color Button", lines_color);

	if (ImGui::IsItemClicked())
	{
		ImGui::OpenPopup("90LinesColorPickerPopup");
	}

	if (ImGui::BeginPopup("90LinesColorPickerPopup"))
	{
		ImGui::ColorPicker4("90 Lines Color Picker", &lines_color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}

	//#######################################################

	//################# Misc ################################

	if (ImGui::Checkbox("RPG Timer", &rpgtimer_toggle)) {
		hud->save_configuration();
	}
	if (ImGui::Checkbox("RPG angle", &rpgangle_toggle)) {
		hud->save_configuration();
	}
	if (ImGui::Checkbox("Show velocity on bounce", &bouncevelocity_toggle)) {
		hud->save_configuration();
	}
	/*
	if (ImGui::Checkbox("Draw FPS", &drawfps_toggle)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	if (ImGui::SliderFloat("##Scale", &fpsScale, 0.3f, 2.5f)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	ImGui::ColorButton("FPS color", fpsColor);

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("FpsColorPopup");
	}

	if (ImGui::BeginPopup("FpsColorPopup")) {
		ImGui::ColorPicker4("Fps color picker", &fpsColor.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}


	ImGui::Indent(50.f);
	if (ImGui::Checkbox("Only while spectating", &drawfps_spectateonly)) {
		hud->save_configuration();
	}
	ImGui::Indent(-50.f);*/
	//#######################################################

	//################# FPS Wheel #######################
	if (ImGui::Checkbox("FPS Wheel", &fpswheel_toggle)) {
		hud->save_configuration();
		if (fpswheel_toggle) {
			should_focus_next_frame = true;
		}
	}
	if (ImGui::Checkbox("Draw wheel centerline", &drawfpswheelcenterline)) {
		hud->save_configuration();
	}
	ImGui::SameLine();
	ImGui::ColorButton("wheel centerline color", fpswheelcenterline_color);

	if (ImGui::IsItemClicked()) {
		ImGui::OpenPopup("WheelCenterLineColorPickerPopup");
	}

	if (ImGui::BeginPopup("WheelCenterLineColorPickerPopup")) {
		ImGui::ColorPicker4("Wheel Centerline Color Picker", &fpswheelcenterline_color.x);

		ImGui::EndPopup();

		hud->save_configuration();
	}

	if (ImGui::SliderFloat("FPS Wheel height", &fpswheel_size, 1.f, 100.f)) {
		hud->save_configuration();
	}

	if (ImGui::SliderFloat("FPS Wheel y-offset", &fpswheel_offset_y, -200.f, 200.f)) {
		hud->save_configuration();
	}

	if (ImGui::SliderFloat("FPS Wheel x-offset", &fpswheel_offset_x, 0.45f, 1.f)) {
		hud->save_configuration();
	}

	if (ImGui::SliderFloat("Anglehelper pixel scale", &ah_pixel_scale, 0.3, 1.f)) {
		hud->save_configuration();
	}

	if (ImGui::SliderFloat("Wheel anglehelper pixel scale", &wheel_ah_pixel_scale, 0.3, 1.f)) {
		hud->save_configuration();
	}

	if (ImGui::SliderFloat("Wheel pixel scale", &wheel_pixel_scale, 0.3, 1.f)) {
		hud->save_configuration();
	}
	//#######################################################
	
	ImGui::End();
}

void ui_menu::render()
{
	Avengers* hud = Avengers::get_instance();
	
	if(hud->want_input)
	{
		menu(hud);
	}

	if (show_position) {
		hud->inst_ui_position->render();
		hud->inst_ui_view->render();

		hud->save_configuration();
	}


	/* WIP
	if (drawfps_toggle && hud->inst_game->is_connected()) {
		if ((drawfps_spectateonly && hud->inst_game->is_spectating()) || !drawfps_spectateonly) {
			ImGui::SetNextWindowSize(ImVec2(500, 500));
			ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
			ImGui::SetWindowFontScale(fpsScale);
			std::string fps = std::to_string(hud->inst_game->get_fps(true));

			ImGui::PushStyleColor(ImGuiCol_Text, fpsColor);
			ImGui::Text(fps.c_str());
			ImGui::PopStyleColor();

			ImGui::SetWindowFontScale(1.f);
			ImGui::End();
		}
	}*/

	//Render speedometer
	if ((velo_meter) && hud->inst_game->is_connected())
	{
		hud->inst_ui_velocity->render(hud, lock_velo_pos, velo_pos, velo_scale, color);
	}

	if ((draw_jumpoff_speed || jumpoffspeed_display_bottom) && hud->inst_game->is_connected()) {
		ImVec4 color(0.9f, 0.3f, 0.75f, 1.f);
		hud->inst_ui_velocity->render_jumpoff_speed(hud, velo_pos, velo_scale, color);
	}

	//Render anglehelper
	if (anglehelper_toggle && hud->inst_game->is_connected())
	{
		hud->inst_ui_anglehelper->render(hud, anglehelper_color);
	}

	if (fpswheel_toggle && hud->inst_game->is_connected())
	{
		hud->inst_ui_fpswheel->render(hud);
	}

	if (drawcenterline && hud->inst_game->is_connected()) {
		hud->inst_ui_anglehelper->renderCenterLine(hud, centerline_color);
	}

	//Jump Target
	if(jump_target && hud->inst_game->is_connected())
	{
		hud->inst_ui_jump_target->render();
	}

	//Strafe downtime
	if (strafedowntime_toggle && hud->inst_game->is_connected()) {
		hud->inst_ui_strafedowntime->render();
	}

	//bounce info
	if (rpgtimer_toggle && hud->inst_game->is_connected()) {
		hud->inst_ui_bounceinfo->renderRpgTimer();
	}

	if (bouncevelocity_toggle && hud->inst_game->is_connected()) {
		hud->inst_ui_bounceinfo->renderBounceVelocity();
	}

	if (rpgangle_toggle && hud->inst_game->is_connected()) {
		hud->inst_ui_bounceinfo->renderRpgAngle();
	}

	//Draw markers
	//This can probably be made much better using an array to draw as many markers as needed if they shared the same color values
	if (draw_marker1 && hud->inst_game->is_connected())
	{
		hud->inst_ui_position_marker->render(marker1, pos1, marker1_color, marker_size);
	}
	if (draw_marker2 && hud->inst_game->is_connected())
	{
		hud->inst_ui_position_marker->render(marker2, pos2, marker2_color, marker_size);
	}
	if (draw_marker3 && hud->inst_game->is_connected())
	{
		hud->inst_ui_position_marker->render(marker3, pos3, marker3_color, marker_size);
	}
}

void ui_menu::registerConfigs(Avengers* hud)
{
	hud->registerConfig("Speedometer", &velo_meter);
	hud->registerConfig("SepVelo", &sep_velo);
	hud->registerConfig("Position", &velo_pos);
	hud->registerConfig("Color_anglehelper", &anglehelper_color);
	hud->registerConfig("Color_90_lines", &lines_color);
	hud->registerConfig("Color", &color);
	hud->registerConfig("Scale", &velo_scale);
	hud->registerConfig("PosHud", &show_position);
	hud->registerConfig("LastCopiedPositionOrigin", &copied_position_origin);
	hud->registerConfig("LastCopiedPositionView", &copied_position_view);
	hud->registerConfig("Anglehelper", &anglehelper_toggle);
	hud->registerConfig("90_Lines", &lines_toggle);
	hud->registerConfig("FPSWheel", &fpswheel_toggle);
	hud->registerConfig("FPSWheelOffsetY", &fpswheel_offset_y);
	hud->registerConfig("FPSWheelOffsetX", &fpswheel_offset_x);
	hud->registerConfig("FPSWheelSize", &fpswheel_size);
	hud->registerConfig("Anglehelper_pixel_scale", &ah_pixel_scale);
	hud->registerConfig("Wheel_anglehelper_pixel_scale", &wheel_ah_pixel_scale);
	hud->registerConfig("Wheel_pixel_scale", &wheel_pixel_scale);
	hud->registerConfig("JumpoffSpeed", &draw_jumpoff_speed);
	hud->registerConfig("JumpoffSpeed_bottom", &jumpoffspeed_display_bottom);
	hud->registerConfig("Strafedowntime", &strafedowntime_toggle);
	hud->registerConfig("rpgtimer", &rpgtimer_toggle);
	hud->registerConfig("bouncevelocity", &bouncevelocity_toggle);
	hud->registerConfig("drawfps", &drawfps_toggle);
	hud->registerConfig("color_fps", &fpsColor);
	hud->registerConfig("scale_fps", &fpsScale);
	hud->registerConfig("drawfps_spectateonly", &drawfps_spectateonly);
	hud->registerConfig("rpgangle", &rpgangle_toggle);
	hud->registerConfig("centerline_toggle", &drawcenterline);
	hud->registerConfig("color_centerline", &centerline_color);
	hud->registerConfig("centerline_toggle_fpswheel", &drawfpswheelcenterline);
	hud->registerConfig("color_fpswheelcenterline", &fpswheelcenterline_color);
	hud->registerConfig("velo_acceleration_toggle", &velo_show_acceleration);
	hud->registerConfig("velo_deceleration_toggle", &velo_show_deceleration);
	hud->registerConfig("color_acceleration", &acceleration_color);
	hud->registerConfig("color_deceleration", &deceleration_color);
	hud->registerConfig("keep_velo_centered_toggle", &keep_velo_centered);
	hud->registerConfig("ah_style", &currentAhStyle);
	hud->registerConfig("clamp_to_next_zone", &clamp_to_next_zone);
	hud->registerConfig("anglehelper_y_offset", &anglehelper_y_offset);
}

ui_menu::ui_menu(Avengers* hud)
{
	hud->inst_render->add_callback([this]() { this->render(); });
	registerConfigs(hud);
}
ui_menu::~ui_menu()
{

}