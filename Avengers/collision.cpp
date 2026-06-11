#include "pch.h"
#include "collision.h"

Collision::Collision(Avengers* avengers) :
	avengers(avengers)
{
}

Collision::~Collision()
{
}

void Collision::render()
{
	float maxDist = avengers->inst_ui_menu->draw_collision_distance;
	if (maxDist == 0.f) {
		maxDist = 99999999.f;
	}

	vec3<float> origin = avengers->inst_game->get_origin();
	bool drawOnlyClips = avengers->inst_ui_menu->draw_collision_only_clips;
	bool noDrawSky = avengers->inst_ui_menu->draw_collision_no_sky;

	if (!hasInitialized) {
		return;
	}

	for (ProcessedBrush& processedBrush : processedBrushes) {
		if ( ((drawOnlyClips && processedBrush.isClip) || !drawOnlyClips) 
			&& (! (noDrawSky && processedBrush.isSky) || !noDrawSky)
			&& processedBrush.center.Dist(origin) <= maxDist) {
			drawCollision(processedBrush);
		}
	}
}

void Collision::init()
{
	///////////////////////////////////////////////////////////////////////////////
	bool connected = avengers->inst_game->is_connected();

	if (avengers->inst_ui_menu->draw_collision && !hasInitialized && connected) {
		clipMap_t* cm = reinterpret_cast<clipMap_t*>(addr_clipmap_t);
		const char* mapents_ptr = cm->mapEnts->entityString;

		parseEntities(std::string(mapents_ptr));
		brushModels = get_brushmodels();
		createMaterials();
		buildBrushes();

		hasInitialized = true;
	}
	else if (!connected) {
		hasInitialized = false;
		processedBrushes.clear();
		entities.clear();
		brushModels.clear();
		mapMaterials.clear();
		avengers->inst_ui_jump_target->resetBrushes();
	}
	///////////////////////////////////////////////////////////////////////////////
}

bool Collision::isClip(const std::string& materialName)
{
	return materialName.contains("clip");
}

bool Collision::isSky(const std::string& materialName)
{
	return materialName.contains("sky");
}

void Collision::buildBrushes()
{

	clipMap_t* cm = reinterpret_cast<clipMap_t*>(addr_clipmap_t);
	int brushCount = cm->numBrushes;

	std::vector<cbrush_t*> brushes;
	brushes.reserve(25000);

	for (int i = 0; i < brushCount; i++) {
		brushes.push_back(&cm->brushes[i]);
	}

	const ImColor COLOR_RED = ImColor(1.f, 0.f, 0.f, 1.f);
	const ImColor COLOR_GREEN = ImColor(0.f, 1.f, 0.f, 1.f);
	const ImColor COLOR_BLUE = ImColor(0.f, 0.f, 1.f, 1.f);
	const float maxDist = 1200.f;

	for (cbrush_t* brush : brushes) {
		ProcessedBrush processedBrush;

		// if brush is part of a submodel, translate brushmodel bounds by the submodel origin [from iw3xo]
		if (brush->isSubmodel) {
			cbrush_t dupe = {};
			memcpy(&dupe, brush, sizeof(cbrush_t));

			dupe.mins += brushModels[dupe.cmSubmodelIndex].cm_submodel_origin;
			dupe.maxs += brushModels[dupe.cmSubmodelIndex].cm_submodel_origin;

			brush = &dupe;
		}

		vec3<float> center = (brush->mins + brush->maxs) / 2.f;
		processedBrush.center = center;

		std::vector<ShowCollisionBrushPt> points = getPointsForBrush(brush);
		processedBrush.points = points;

		if (points.size() < 4) {
			continue;
		}

		buildCollisionPoints(processedBrush, brush, points);

		// check if we are within array bounds
		if (static_cast<size_t>(brush->axialMaterialNum[0][0]) >= mapMaterials.size()) {
			return;
		}

		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 3; y++) {
				std::string material = std::string(mapMaterials[brush->axialMaterialNum[x][y]]->material);
				if (std::find(processedBrush.materials.begin(), processedBrush.materials.end(), material) == processedBrush.materials.end()) {
					processedBrush.materials.push_back(material);
					if (isClip(material)) {
						processedBrush.isClip = true;
					}
					if (isSky(material)) {
						processedBrush.isSky = true;
					}
				}
			}
		}

		processedBrushes.push_back(processedBrush);
	}
}

void Collision::buildCollisionPoints(ProcessedBrush& processedBrush, cbrush_t* brush, const std::vector<ShowCollisionBrushPt>& pts)
{
	static int currentColorIndex = 0;

	constexpr int COLOR_COUNT = 6;
	const ImColor COLOR_RED = ImColor(1.f, 0.f, 0.f, 0.4f);
	const ImColor COLOR_GREEN = ImColor(0.f, 1.f, 0.f, 0.4f);
	const ImColor COLOR_BLUE = ImColor(0.f, 0.f, 1.f, 0.4f);
	const ImColor COLOR_RED2 = ImColor(0.5f, 0.5f, 0.f, 0.4f);
	const ImColor COLOR_GREEN2 = ImColor(0.0f, 0.5f, 0.5f, 0.4f);
	const ImColor COLOR_BLUE2 = ImColor(0.5f, 0.f, 0.5f, 0.4f);

	ImColor currentColor;
	switch (currentColorIndex) {
	case 0:
		currentColor = COLOR_RED;
		break;
	case 1:
		currentColor = COLOR_GREEN;
		break;
	case 2:
		currentColor = COLOR_BLUE;
		break;
	case 3:
		currentColor = COLOR_RED2;
		break;
	case 4:
		currentColor = COLOR_GREEN2;
		break;
	case 5:
		currentColor = COLOR_BLUE2;
		break;
	}
	currentColorIndex = (currentColorIndex + 1) % COLOR_COUNT;
	processedBrush.color = currentColor;

	AxialPlane_t axial_planes[6] {};
	get_axial_planes(axial_planes, brush);

	int pt_count = pts.size();

	// -------------------------------
	// brushside [0]-[5] (axialPlanes)
	for (auto side_index = 0u; side_index < 6; ++side_index) {
		vec3<float> plane_normal;
		plane_normal = axial_planes[side_index].plane;

		if (build_brush_winding_for_side((winding_t*)&winding_pool, reinterpret_cast<const float*>(&plane_normal), side_index, pts)) {
			processedBrush.sides.push_back(BrushSide());
			vec3<float> sideCenter(0.f, 0.f, 0.f);
			std::size_t sideIndex = processedBrush.sides.size() - 1;
			for (int i = 0; i < winding_pool.numpoints; i++) {
				vec3<float> p(reinterpret_cast<vec3<float>*>(winding_pool.p)[i]);
				sideCenter += p;
				processedBrush.sides[sideIndex].points.push_back(p);
			}

			sideCenter /= winding_pool.numpoints;
			processedBrush.sides[sideIndex].center = sideCenter;
		}
	}

	// ---------------------------------
	// brushside [6] and up (additional)
	for (auto side_index = 6u; side_index < brush->numsides + 6; ++side_index) {
		if (build_brush_winding_for_side((winding_t*)&winding_pool, brush->sides[side_index - 6].plane->normal, side_index, pts)) {
			processedBrush.sides.push_back(BrushSide());
			std::size_t sideIndex = processedBrush.sides.size() - 1;
			for (int i = 0; i < winding_pool.numpoints; i++) {
				vec3<float> p(reinterpret_cast<vec3<float>*>(winding_pool.p)[i]);
				processedBrush.sides[sideIndex].points.push_back(p);
			}
		}
	}
}

void Collision::drawCollision(ProcessedBrush& processedBrush)
{
	const auto poly_lit = false;
	const auto poly_outlines = false;
	const auto poly_linecolor = ImColor(255, 255, 255, 255);
	const auto poly_depth = true;
	const auto poly_face = false;

	for (BrushSide& side : processedBrush.sides) {
		vec3<float>* points = side.points.data();
		avengers->inst_game->drawPoly(side.points.size(), (float(*)[3]) points, (const float*)&processedBrush.color,
			poly_lit, poly_outlines, (const float*)&poly_linecolor, poly_depth, poly_face);
	}
}

void Collision::drawCircle(const vec3<float>& pos, ImColor color)
{
	vec2<float> screen_pos;
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	if (avengers->inst_game->world_to_screen(pos, &screen_pos.x, &screen_pos.y)) {
		drawList->AddCircle(screen_pos.toImvec2(), 20.f, color, 16, 2);
	}
}

/* These are ported from iw3xo:
* 
* https://github.com/xoxor4d/iw3xo-dev
*
///////////////////////////////////////////////////////*/

void Collision::createMaterials()
{
	clipMap_t* cm = reinterpret_cast<clipMap_t*>(addr_clipmap_t);

	const std::uint32_t clipmap_material_index = cm->numMaterials;
	std::vector<dmaterial_t*> map_materials(clipmap_material_index);

	for (auto num = 0u; num < clipmap_material_index; num++) {
		map_materials[num] = &cm->materials[num];
	}

	mapMaterials = map_materials;
}

std::vector<ShowCollisionBrushPt> Collision::getPointsForBrush(cbrush_t* brush)
{
	std::vector<ShowCollisionBrushPt> brush_pts(128);

	AxialPlane_t axial_planes[6];
	axial_planes[0].plane = vec3<float>(-1.0f, 0.0f, 0.0f);
	axial_planes[0].dist = -brush->mins[0];

	axial_planes[1].plane = vec3<float>(1.0f, 0.0f, 0.0f);
	axial_planes[1].dist = brush->maxs[0];

	axial_planes[2].plane = vec3<float>(0.0f, -1.0f, 0.0f);
	axial_planes[2].dist = -brush->mins[1];

	axial_planes[3].plane = vec3<float>(0.0f, 1.0f, 0.0f);
	axial_planes[3].dist = brush->maxs[1];

	axial_planes[4].plane = vec3<float>(0.0f, 0.0f, -1.0f);
	axial_planes[4].dist = -brush->mins[2];

	axial_planes[5].plane = vec3<float>(0.0f, 0.0f, 1.0f);
	axial_planes[5].dist = brush->maxs[2];

	int pt_count = 0;
	int pt_count_all = 0;
	const int side_count = brush->numsides + 6;

	std::int16_t side_index[3] = {};
	float expanded_plane[3][4] = {};
	constexpr int CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS = 128;

	// first loop should only get the axial planes till brush->numsides < 3
	for (side_index[0] = 0; side_index[0] < side_count - 2; ++side_index[0]) {
		// sideIndex[0]-[5] are axial planes only; move the current plane into expandedPlane[0]
		get_plane_vec4(brush->sides, axial_planes, side_index[0], (float*)expanded_plane);

		// get a plane 1 plane ahead of our first plane
		for (side_index[1] = side_index[0] + 1; side_index[1] < side_count - 1; ++side_index[1]) {
			// check if we're using an axial plane and 2 different planes
			if (side_index[0] < 6 || side_index[1] < 6 || brush->sides[side_index[0] - 6].plane != brush->sides[side_index[1] - 6].plane) {
				// move the current plane into expandedPlane[1]
				get_plane_vec4(brush->sides, axial_planes, side_index[1], expanded_plane[1]);

				// get a plane 1 plane ahead of our second plane
				for (side_index[2] = side_index[1] + 1; side_index[2] < side_count - 0; ++side_index[2]) {
					// check if we use axial planes or atleast 3 different sides
					if ((side_index[0] < 6 || side_index[2] < 6 || brush->sides[side_index[0] - 6].plane != brush->sides[side_index[2] - 6].plane)
						&& (side_index[1] < 6 || side_index[2] < 6 || brush->sides[side_index[1] - 6].plane != brush->sides[side_index[2] - 6].plane)) {
						// move the current plane into expandedPlane[2]
						get_plane_vec4(brush->sides, axial_planes, side_index[2], expanded_plane[2]);

						// intersect the 3 planes
						float xyz[3];

						if (intersect_planes(expanded_plane[0], expanded_plane[1], expanded_plane[2], xyz)) {
							// snap our verts in xyz onto the grid
							snap_point_to_intersecting_planes(expanded_plane[0], expanded_plane[1], expanded_plane[2], xyz, 0.25f, 0.0099999998f);

							// if the planes intersected, put verts into brushPts and increase our pointCount
							pt_count = add_simple_brush_point(brush, axial_planes, side_index, xyz, pt_count, brush_pts);
							pt_count_all += pt_count;

							if (pt_count >= CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS - 1) {
								goto end;
							}
						}
					}
				}
			}
		}
	}

end:
	brush_pts.resize(pt_count);
	return brush_pts;
}

std::vector<brushmodel_entity_s> Collision::get_brushmodels()
{
	clipMap_t* cm = reinterpret_cast<clipMap_t*>(addr_clipmap_t);

	std::vector<brushmodel_entity_s> bmodels;

	// geting the total clipmap size would prob. be better
	const uintptr_t leaf_brushes_start = reinterpret_cast<uintptr_t>(&*cm->leafbrushNodes);
	const uintptr_t leaf_brushes_end = leaf_brushes_start + sizeof(cLeafBrushNode_s) * (cm->leafbrushNodesCount + cm->numLeafBrushes); // wrong

	// first element is always empty because
	// the first submodel within the entsMap starts at 1 and we want to avoid subtracting - 1 everywhere 
	bmodels.emplace_back(brushmodel_entity_s());

	for (auto& entity : this->entities) {
		if (entity.contains("model")) {
			std::string model = entity["model"];
			std::string origin = entity["origin"];

			// if ent is a brushmodel/submodel
			if (!model.empty() && model[0] == '*' && !origin.empty()) {
				auto curr_bmodel = brushmodel_entity_s();

				// get the submodel index 
				const auto p_index = std::stoi(model.erase(0, 1));

				// the index should always match the size of our vector or we did something wrong
				if (p_index != (int)bmodels.size()) {
					//game::Com_PrintMessage(0, utils::va("[Entities::getBrushModels]: Something went wrong while parsing submodels. (%d != %d)", p_index, bmodels.size()), 0);
				}

				if (p_index >= static_cast<int>(cm->numSubModels)) {
					//game::Com_PrintMessage(0, utils::va("[Entities::getBrushModels]: Something went wrong while parsing submodels. (%d >= %d numSubModels)", p_index, game::cm->numSubModels), 0);
					break;
				}

				// assign indices and pointers to both the brush and the submodel
				curr_bmodel.cm_submodel_index = p_index;

				if (&cm->cmodels[p_index]) {
					curr_bmodel.cm_submodel = &cm->cmodels[p_index];
				}

				// fix me daddy
				auto brush_index_ptr = cm->leafbrushNodes[cm->cmodels[p_index].leaf.leafBrushNode].data.leaf.brushes;
				curr_bmodel.cm_brush_index = 0;

				// this is giving me cancer
				if (cm->cmodels[p_index].leaf.leafBrushNode != 0 && brush_index_ptr) {
					if ((uintptr_t) & *brush_index_ptr >= leaf_brushes_start && (uintptr_t) & *brush_index_ptr < leaf_brushes_end) {
						curr_bmodel.cm_brush_index = static_cast<int>(*cm->leafbrushNodes[cm->cmodels[p_index].leaf.leafBrushNode].data.leaf.brushes);
					}
					else {
						//game::Com_PrintMessage(0, utils::va("[Entities::getBrushModels]: Skipping faulty brush-index pointer at leafbrushNodes[%d].data.leaf.brushes ...\n", p_index), 0);
					}

					//currBModel.cmBrush = &Game::cm->brushes[*Game::cm->leafbrushNodes[Game::cm->cmodels[p_index].leaf.leafBrushNode].data.leaf.brushes];
					curr_bmodel.cm_brush = &cm->brushes[curr_bmodel.cm_brush_index];

					// add the submodel index to the clipmap brush
					curr_bmodel.cm_brush->isSubmodel = true;
					curr_bmodel.cm_brush->cmSubmodelIndex = static_cast<std::int16_t>(p_index);
				}


				// save entity origin
				if (!sscanf_s(origin.c_str(), "%f %f %f", &curr_bmodel.cm_submodel_origin[0], &curr_bmodel.cm_submodel_origin[1], &curr_bmodel.cm_submodel_origin[2])) {
					//game::Com_PrintMessage(0, utils::va("[!]: sscanf failed for submodel %d", p_index), 0);
					curr_bmodel.cm_submodel_origin[0] = 0.0f;
					curr_bmodel.cm_submodel_origin[1] = 0.0f;
					curr_bmodel.cm_submodel_origin[2] = 0.0f;
				}

				bmodels.push_back(curr_bmodel);
			}
		}
	}

	return bmodels;
}

void Collision::parseEntities(const std::string& buffer)
{
	int parse_state = 0;

	std::string key;
	std::string value;

	std::unordered_map<std::string, std::string> entity;

	for (unsigned int i = 0; i < buffer.size(); ++i) {
		char character = buffer[i];
		if (character == '{') {
			entity.clear();
		}

		switch (character) {
		case '{':
		{
			entity.clear();
			break;
		}

		case '}':
		{
			entities.push_back(entity);
			entity.clear();
			break;
		}

		case '"':
		{
			if (parse_state == PARSE_AWAIT_KEY) {
				key.clear();
				parse_state = PARSE_READ_KEY;
			}
			else if (parse_state == PARSE_READ_KEY) {
				parse_state = PARSE_AWAIT_VALUE;
			}
			else if (parse_state == PARSE_AWAIT_VALUE) {
				value.clear();
				parse_state = PARSE_READ_VALUE;
			}
			else if (parse_state == PARSE_READ_VALUE) {
				entity[str_to_lower(key)] = value;
				parse_state = PARSE_AWAIT_KEY;
			}
			else {
				throw std::runtime_error("Parsing error!");
			}
			break;
		}

		default:
		{
			if (parse_state == PARSE_READ_KEY) {
				key.push_back(character);
			}
			else if (parse_state == PARSE_READ_VALUE) {
				value.push_back(character);
			}

			break;
		}
		}
	}
}

std::string Collision::str_to_lower(std::string input)
{
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}

bool Collision::vec_compare_custom_epsilon(const vec3<float>* xyz_list, const int xyz_index, const float* v1, const float epsilon, const int coord_count)
{
	for (auto i = 0; i < coord_count; ++i) {
		if (((xyz_list[xyz_index][i] - v1[i]) * (xyz_list[xyz_index][i] - v1[i])) > (epsilon * epsilon)) {
			return false;
		}
	}

	return true;
}

// check if point exists (CM_PointInList)
int Collision::point_exists_in_list(const float* point, const vec3<float>* xyz_list, const int xyz_count)
{
	for (auto xyz_index = 0; xyz_index < xyz_count; ++xyz_index) {
		if (vec_compare_custom_epsilon(xyz_list, xyz_index, point, 0.1f, 3)) // larger epsilon decreases quality
		{
			return 1;
		}
	}

	return 0;
}

// create a list of vertex points (CM_GetXyzList)
int Collision::get_xyz_list(const unsigned int side_index, const std::vector<ShowCollisionBrushPt>& pts, vec3<float>* xyz_list, const int xyz_limit)
{
	int count = 0;
	int pt_count = pts.size();

	for (auto index = 0; index < pt_count; ++index) {
		if ((side_index == static_cast<std::uint16_t>(pts[index].sideIndex[0])
			|| side_index == static_cast<std::uint16_t>(pts[index].sideIndex[1])
			|| side_index == static_cast<std::uint16_t>(pts[index].sideIndex[2]))
			&& !point_exists_in_list(reinterpret_cast<const float*>(&pts[index].xyz), xyz_list, count)) {
			if (count == xyz_limit) {
				//game::Com_PrintMessage(0, utils::va("^1get_xyz_list L#%d ^7:: Winding point limit (%i) exceeded on brush face \n", __LINE__, xyz_limit), 0);
				return 0;
			}

			xyz_list[count] = pts[index].xyz;
			++count;
		}
	}

	return count;
}

void Collision::pick_projection_axes(const float* normal, int* i, int* j)
{
	int k = 0;

	if (fabs(normal[1]) > fabs(normal[0])) {
		k = 1;
	}

	if (fabs(normal[2]) > fabs(normal[k])) {
		k = 2;
	}

	*i = ~k & 1;
	*j = ~k & 2;
}

// add a point that intersected behind another plane that still is within the bounding box? (CM_AddColinearExteriorPointToWindingProjected)
void Collision::add_colinear_exterior_point_to_winding(winding_t* w, const vec3<float>& pt, int i, int j, int index0, int index1)
{
	float delta; int axis;

	if (fabs(float(uint32_t(w->p[index1][i] - w->p[index0][i]))) < fabs(float(uint32_t(w->p[index1][j] - w->p[index0][j])))) {
		axis = j;
		delta = w->p[index1][j] - w->p[index0][j];
	}
	else {
		axis = i;
		delta = w->p[index1][i] - w->p[index0][i];
	}

	if (delta <= 0.0f) {
		if (pt[axis] <= w->p[index0][axis]) {
			if (w->p[index1][axis] > pt[axis]) {
				*reinterpret_cast<vec3<float>*>(w->p[index1]) = pt;
			}
		}
		else {
			*reinterpret_cast<vec3<float>*>(w->p[index0]) = pt;
		}
	}

	else {
		if (w->p[index0][axis] <= pt[axis]) {
			if (pt[axis] > w->p[index1][axis]) {
				*reinterpret_cast<vec3<float>*>(w->p[index1]) = pt;
			}
		}
		else {
			*reinterpret_cast<vec3<float>*>(w->p[index0]) = pt;
		}
	}
}

// cross product (CM_SignedAreaForPointsProjected)
float Collision::signed_area_for_points_projected(const float* pt0, const vec3<float>& pt1, const float* pt2, const int i, const int j)
{
	return (pt2[j] - pt1[j]) * pt0[i] + (pt0[j] - pt2[j]) * pt1[i] + (pt1[j] - pt0[j]) * pt2[i];
}

vec_t Collision::length_squared3(const vec3_t v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

// Source :: PolyFromPlane || Q3 :: RemoveColinearPoints? (CM_AddExteriorPointToWindingProjected)
void Collision::add_exterior_point_to_winding(winding_t* w, const vec3<float>& pt, int i, int j)
{
	int best_index = -1;
	float best_signed_area = FLT_MAX;

	int index_prev = w->numpoints - 1;

	for (auto index = 0; index < w->numpoints; ++index) {
		const float signed_area = signed_area_for_points_projected(w->p[index_prev], pt, w->p[index], i, j);

		if (best_signed_area > signed_area) {
			best_signed_area = signed_area;
			best_index = index;
		}

		index_prev = index;
	}

	if (best_signed_area < -0.001f) {
		memmove((char*)w->p[best_index + 1], (char*)w->p[best_index], 12 * (w->numpoints - best_index));

	    *reinterpret_cast<vec3<float>*>(w->p[best_index]) = pt;
		++w->numpoints;
	}

	else if (best_signed_area <= 0.001f) {
		add_colinear_exterior_point_to_winding(w, pt, i, j, (best_index + w->numpoints - 1) % w->numpoints, best_index);
	}
}

// create a triangle to check the winding order (CM_RepresentativeTriangleFromWinding)
float Collision::representative_triangle_from_winding(const winding_t* w, const float* normal, int* i0, int* i1, int* i2)
{
	float area_best = 0.0f;
	float va[3], vb[3], vc[3];

	*i0 = 0;
	*i1 = 1;
	*i2 = 2;

	for (auto k = 2; k < w->numpoints; ++k) {
		for (auto j = 1; j < k; ++j) {
			vb[0] = w->p[k][0] - w->p[j][0];
			vb[1] = w->p[k][1] - w->p[j][1];
			vb[2] = w->p[k][2] - w->p[j][2];

			for (auto i = 0; i < j; ++i) {
				va[0] = w->p[i][0] - w->p[j][0];
				va[1] = w->p[i][1] - w->p[j][1];
				va[2] = w->p[i][2] - w->p[j][2];

				mm::cross3(va, vb, vc);
				const float test_against = fabs(((vc[0] * normal[0]) + (vc[1] * normal[1])) + (vc[2] * normal[2]));

				if (test_against > 0.0f) {
					area_best = test_against;
					*i0 = i;
					*i1 = j;
					*i2 = k;
				}
			}
		}
	}

	return area_best;
}

// create a plane from points
bool Collision::plane_from_points(float* plane, const float* v0, const float* v1, const float* v2)
{
	float v2_v0[3], v1_v0[3];
	v1_v0[0] = v1[0] - v0[0];
	v1_v0[1] = v1[1] - v0[1];
	v1_v0[2] = v1[2] - v0[2];
	v2_v0[0] = v2[0] - v0[0];
	v2_v0[1] = v2[1] - v0[1];
	v2_v0[2] = v2[2] - v0[2];

	mm::cross3(v2_v0, v1_v0, plane);
	const float length_sqr = ((plane[0] * plane[0]) + (plane[1] * plane[1])) + (plane[2] * plane[2]);

	if (length_sqr < 2.0f) {
		if (length_sqr == 0.0f) {
			return false;
		}

		if (length_squared3(v2_v0) * length_squared3(v1_v0) * 0.0000010000001f >= length_sqr) {
			v1_v0[0] = v2[0] - v1[0];
			v1_v0[1] = v2[1] - v1[1];
			v1_v0[2] = v2[2] - v1[2];
			v2_v0[0] = v0[0] - v1[0];
			v2_v0[1] = v0[1] - v1[1];
			v2_v0[2] = v0[2] - v1[2];

			mm::cross3(v2_v0, v1_v0, plane);

			if (length_squared3(v2_v0) * length_squared3(v1_v0) * 0.0000010000001f >= length_sqr) {
				return false;
			}
		}
	}

	const float length = sqrt(length_sqr);
	plane[0] = plane[0] / length;
	plane[1] = plane[1] / length;
	plane[2] = plane[2] / length;
	plane[3] = ((v0[0] * plane[0]) + (v0[1] * plane[1])) + (v0[2] * plane[2]);

	return true;
}

void Collision::reverse_winding(winding_t* w)
{
	for (auto i = 0; i < w->numpoints / 2; ++i) {
		const float s_winding[3] =
		{
			w->p[i][0], w->p[i][1], w->p[i][2]
		};

		w->p[i][0] = w->p[w->numpoints - 1 - i][0];
		w->p[i][1] = w->p[w->numpoints - 1 - i][1];
		w->p[i][2] = w->p[w->numpoints - 1 - i][2];

		w->p[w->numpoints - 1 - i][0] = s_winding[0];
		w->p[w->numpoints - 1 - i][1] = s_winding[1];
		w->p[w->numpoints - 1 - i][2] = s_winding[2];
	}
}

void Collision::get_axial_planes(AxialPlane_t* planes, const cbrush_t* brush)
{
	planes[0].plane = vec3<float>(-1.0f, 0.0f, 0.0f);
	planes[0].dist = -brush->mins[0];

	planes[1].plane = vec3<float>(1.0f, 0.0f, 0.0f);
	planes[1].dist = brush->maxs[0];

	planes[2].plane = vec3<float>(0.0f, -1.0f, 0.0f);
	planes[2].dist = -brush->mins[1];

	planes[3].plane = vec3<float>(0.0f, 1.0f, 0.0f);
	planes[3].dist = brush->maxs[1];

	planes[4].plane = vec3<float>(0.0f, 0.0f, -1.0f);
	planes[4].dist = -brush->mins[2];

	planes[5].plane = vec3<float>(0.0f, 0.0f, 1.0f);
	planes[5].dist = brush->maxs[2];
}

bool Collision::build_brush_winding_for_side(winding_t* winding, const float* plane_normal, const int side_index, const std::vector<ShowCollisionBrushPt> pts)
{
	int i, i0, i1, i2, j;
	int pt_count = pts.size();
	vec4_t plane {};

	if (!winding) {
		//game::Com_Error(0, utils::va("CM_BuildBrushWindingForSide L#%d :: winding \n", __LINE__));
		return false;
	}

	if (!plane_normal) {
		//game::Com_Error(0, COM_ERROR_MSG);
		return false;
	}

	// create a list of vertex points
	vec3<float> xyz_list[1024];
	const int xyzCount = get_xyz_list(side_index, pts, xyz_list, 1024);

	// we need atleast a triangle to create a poly
	if (xyzCount < 3) {
		return false;
	}

	/*/ direction of camera plane
	const vec3<float> camera_direction_to_plane = xyz_list[0] - glob::lpmove_camera_origin;

	// dot product between line from camera to the plane and the normal
	// if dot > 0 then the plane is facing away from the camera (dot = 1 = plane is facing the same way as the camera; dot = -1 = plane looking directly towards the camera)
	if (glm::dot(glm::vec3(plane_normal[0], plane_normal[1], plane_normal[2]), camera_direction_to_plane) > 0.0f && !dvars::r_drawCollision_polyFace->current.enabled) {
		return false;
	}*/

	// find the major axis
	pick_projection_axes(plane_normal, &i, &j);

    *reinterpret_cast<vec3<float>*>(winding->p[0]) = xyz_list[0];
	*reinterpret_cast<vec3<float>*>(winding->p[1]) = xyz_list[1];

	winding->numpoints = 2;

	for (auto k = 2; k < xyzCount; ++k) {
		add_exterior_point_to_winding(winding, xyz_list[k], i, j);
	}

	// build a triangle of our winding points so we can check if the winding is clock-wise
	if (representative_triangle_from_winding(winding, plane_normal, &i0, &i1, &i2) < 0.001f) {
		// do nothing if it is counter clock-wise
		return false;
	}

	// *
	// winding is clock-wise ..

	// create a temp plane
	plane_from_points(&*plane, winding->p[i0], winding->p[i1], winding->p[i2]);

	// if our winding has a clock-wise winding, reverse it
	if (mm::dot3(plane, plane_normal) > 0.0f) {
		reverse_winding(winding);
	}

	return true;
}

// create plane for intersection (CM_GetPlaneVec4Form)
void Collision::get_plane_vec4(const cbrushside_t* sides, const AxialPlane_t* axial_planes, const int index, float* expanded_plane)
{
	if (index >= 6) {
		if (!sides) {
			return;
		}

		expanded_plane[0] = sides[index - 6].plane->normal[0];
		expanded_plane[1] = sides[index - 6].plane->normal[1];
		expanded_plane[2] = sides[index - 6].plane->normal[2];
		expanded_plane[3] = sides[index - 6].plane->dist;
	}
	else {
		expanded_plane[0] = axial_planes[index].plane.x;
		expanded_plane[1] = axial_planes[index].plane.y;
		expanded_plane[2] = axial_planes[index].plane.z;
		expanded_plane[3] = axial_planes[index].dist;
	}
}

// intersect 3 planes
int Collision::intersect_planes(const float* plane0, const float* plane1, const float* plane2, float* xyz)
{
	float determinant;

	determinant = (((plane1[1] * plane2[2]) - (plane2[1] * plane1[2])) * plane0[0])
		+ (((plane2[1] * plane0[2]) - (plane0[1] * plane2[2])) * plane1[0])
		+ (((plane0[1] * plane1[2]) - (plane1[1] * plane0[2])) * plane2[0]);

	if (fabs(determinant) < 0.001f) {
		return 0;
	}

	determinant = 1.0f / determinant;

	xyz[0] = ((((plane1[1] * plane2[2]) - (plane2[1] * plane1[2])) * plane0[3])
		+ (((plane2[1] * plane0[2]) - (plane0[1] * plane2[2])) * plane1[3])
		+ (((plane0[1] * plane1[2]) - (plane1[1] * plane0[2])) * plane2[3])) * determinant;

	xyz[1] = ((((plane1[2] * plane2[0]) - (plane2[2] * plane1[0])) * plane0[3])
		+ (((plane2[2] * plane0[0]) - (plane0[2] * plane2[0])) * plane1[3])
		+ (((plane0[2] * plane1[0]) - (plane1[2] * plane0[0])) * plane2[3])) * determinant;

	xyz[2] = ((((plane1[0] * plane2[1]) - (plane2[0] * plane1[1])) * plane0[3])
		+ (((plane2[0] * plane0[1]) - (plane0[0] * plane2[1])) * plane1[3])
		+ (((plane0[0] * plane1[1]) - (plane1[0] * plane0[1])) * plane2[3])) * determinant;

	return 1;
}

bool Collision::is_on_grid(const float* snapped, const float* xyz)
{
	return xyz[0] == snapped[0] && xyz[1] == snapped[1] && xyz[2] == snapped[2];
}

void Collision::snap_point_to_intersecting_planes(const float* plane0, const float* plane1, const float* plane2, float* xyz, float snap_grid, const float snap_epsilon)
{
	float snapped[3], current_plane[4];

	snap_grid = 1.0f / snap_grid;

	// cod4map
	for (auto axis = 0; axis < 3; ++axis) {
		const float rounded = round(xyz[axis] * snap_grid) / snap_grid;
		const float delta = fabs(rounded - xyz[axis]);

		if (snap_epsilon <= delta) {
			snapped[axis] = xyz[axis];
		}
		else {
			snapped[axis] = rounded;
		}
	}

	if (!is_on_grid(snapped, xyz)) {
		float max_snap_error = 0.0f;
		float max_base_error = snap_epsilon;

		for (auto plane_index = 0; plane_index < 3; ++plane_index) {
			if (plane_index == 0) {
				memcpy(&current_plane, plane0, sizeof(current_plane));
			}
			else if (plane_index == 1) {
				memcpy(&current_plane, plane1, sizeof(current_plane));
			}
			else if (plane_index == 2) {
				memcpy(&current_plane, plane2, sizeof(current_plane));
			}

			const float snap_error = log((current_plane[0] * snapped[0] + current_plane[1] * snapped[1] + current_plane[2] * snapped[2]) - current_plane[3]);
			if (snap_error > max_snap_error) {
				max_snap_error = snap_error;
			}

			const float base_error = log((current_plane[0] * xyz[0] + current_plane[1] * xyz[1] + current_plane[2] * xyz[2]) - current_plane[3]);
			if (base_error > max_base_error) {
				max_base_error = base_error;
			}
		}

		if (max_base_error > max_snap_error) {
			xyz[0] = snapped[0];
			xyz[1] = snapped[1];
			xyz[2] = snapped[2];
		}
	}
}

// add valid vertices from 3 plane intersections (CM_AddSimpleBrushPoint)
int Collision::add_simple_brush_point(const cbrush_t* brush, const AxialPlane_t* axial_planes, const __int16* side_indices, const float* xyz, int pt_count, std::vector<ShowCollisionBrushPt>& brush_pts)
{
	constexpr int CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS = 128;
	if (!brush) {
		return 0;
	}

	for (auto side_index = 0u; side_index < 6; ++side_index) {
		if (((axial_planes[side_index].plane.x * xyz[0] + axial_planes[side_index].plane.y * xyz[1] + axial_planes[side_index].plane.z * xyz[2])
			- axial_planes[side_index].dist) > 0.1f) {
			return pt_count;
		}
	}

	for (auto side_index = 0u; side_index < brush->numsides; ++side_index) {
		const auto plane = brush->sides[side_index].plane;

		if (plane != brush->sides[side_indices[0] - 6].plane
			&& plane != brush->sides[side_indices[1] - 6].plane
			&& plane != brush->sides[side_indices[2] - 6].plane
			&& ((plane->normal[0] * xyz[0]) + (plane->normal[1] * xyz[1]) + (plane->normal[2] * xyz[2]) - plane->dist) > 0.1f) {
			return pt_count;
		}
	}

	if (pt_count > CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS - 2) // T5: 1024
	{
		return pt_count;
	}

	brush_pts[pt_count].xyz[0] = xyz[0];
	brush_pts[pt_count].xyz[1] = xyz[1];
	brush_pts[pt_count].xyz[2] = xyz[2];

	brush_pts[pt_count].sideIndex[0] = side_indices[0];
	brush_pts[pt_count].sideIndex[1] = side_indices[1];
	brush_pts[pt_count].sideIndex[2] = side_indices[2];

	return pt_count + 1;
}
