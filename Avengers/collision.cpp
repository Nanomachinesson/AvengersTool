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
	float maxDist = avengers->instUiMenu->drawCollisionDistance;
	if (maxDist == 0.f) {
		maxDist = 99999999.f;
	}

	vec3<float> origin = avengers->instGame->getOrigin();
	bool drawOnlyClips = avengers->instUiMenu->drawCollisionOnlyClips;
	bool noDrawSky = avengers->instUiMenu->drawCollisionNoSky;

	if (!hasInitialized) {
		return;
	}

	for (ProcessedBrush& processedBrush : processedBrushes) {
		if ( ((drawOnlyClips && processedBrush.isClip) || !drawOnlyClips) 
			&& (! (noDrawSky && processedBrush.isSky) || !noDrawSky)
			&& processedBrush.center.dist(origin) <= maxDist) {
			drawCollision(processedBrush);
		}
	}
}

void Collision::init()
{
	///////////////////////////////////////////////////////////////////////////////
	bool connected = avengers->instGame->isConnected();

	if (avengers->instUiMenu->drawCollision && !hasInitialized && connected) {
		clipMap_t* cm = reinterpret_cast<clipMap_t*>(addr_clipmap_t);
		const char* mapentsPtr = cm->mapEnts->entityString;

		parseEntities(std::string(mapentsPtr));
		brushModels = getBrushmodels();
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
		avengers->instUiJumpTarget->resetBrushes();
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

	AxialPlane_t axialPlanes[6] {};
	getAxialPlanes(axialPlanes, brush);

	int ptCount = pts.size();

	// -------------------------------
	// brushside [0]-[5] (axialPlanes)
	for (auto windingSideIndex = 0u; windingSideIndex < 6; ++windingSideIndex) {
		vec3<float> planeNormal;
		planeNormal = axialPlanes[windingSideIndex].plane;

		if (buildBrushWindingForSide((winding_t*)&windingPool, reinterpret_cast<const float*>(&planeNormal), windingSideIndex, pts)) {
			processedBrush.sides.push_back(BrushSide());
			vec3<float> sideCenter(0.f, 0.f, 0.f);
			std::size_t sideIndex = processedBrush.sides.size() - 1;
			for (int i = 0; i < windingPool.numpoints; i++) {
				vec3<float> p(reinterpret_cast<vec3<float>*>(windingPool.p)[i]);
				sideCenter += p;
				processedBrush.sides[sideIndex].points.push_back(p);
			}

			sideCenter /= windingPool.numpoints;
			processedBrush.sides[sideIndex].center = sideCenter;
		}
	}

	// ---------------------------------
	// brushside [6] and up (additional)
	for (auto windingSideIndex = 6u; windingSideIndex < brush->numsides + 6; ++windingSideIndex) {
		if (buildBrushWindingForSide((winding_t*)&windingPool, brush->sides[windingSideIndex - 6].plane->normal, windingSideIndex, pts)) {
			processedBrush.sides.push_back(BrushSide());
			std::size_t sideIndex = processedBrush.sides.size() - 1;
			vec3<float> sideCenter(0.f, 0.f, 0.f);
			for (int i = 0; i < windingPool.numpoints; i++) {
				vec3<float> p(reinterpret_cast<vec3<float>*>(windingPool.p)[i]);
				sideCenter += p;
				processedBrush.sides[sideIndex].points.push_back(p);
			}

			sideCenter /= windingPool.numpoints;
			processedBrush.sides[sideIndex].center = sideCenter;
		}
	}
}

void Collision::drawCollision(ProcessedBrush& processedBrush)
{
	const auto polyLit = false;
	const auto polyOutlines = false;
	const auto polyLinecolor = ImColor(255, 255, 255, 255);
	const auto polyDepth = true;
	const auto polyFace = false;

	for (BrushSide& side : processedBrush.sides) {
		vec3<float>* points = side.points.data();
		avengers->instGame->drawPoly(side.points.size(), (float(*)[3]) points, (const float*)&processedBrush.color,
			polyLit, polyOutlines, (const float*)&polyLinecolor, polyDepth, polyFace);
	}
}

void Collision::drawCircle(const vec3<float>& pos, ImColor color)
{
	vec2<float> screenPos;
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	if (avengers->instGame->worldToScreen(pos, &screenPos.x, &screenPos.y)) {
		drawList->AddCircle(screenPos.toImvec2(), 20.f, color, 16, 2);
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

	const std::uint32_t clipmapMaterialIndex = cm->numMaterials;
	std::vector<dmaterial_t*> materials(clipmapMaterialIndex);

	for (auto num = 0u; num < clipmapMaterialIndex; num++) {
		materials[num] = &cm->materials[num];
	}

	mapMaterials = materials;
}

std::vector<ShowCollisionBrushPt> Collision::getPointsForBrush(cbrush_t* brush)
{
	std::vector<ShowCollisionBrushPt> brushPts(128);

	AxialPlane_t axialPlanes[6];
	axialPlanes[0].plane = vec3<float>(-1.0f, 0.0f, 0.0f);
	axialPlanes[0].dist = -brush->mins[0];

	axialPlanes[1].plane = vec3<float>(1.0f, 0.0f, 0.0f);
	axialPlanes[1].dist = brush->maxs[0];

	axialPlanes[2].plane = vec3<float>(0.0f, -1.0f, 0.0f);
	axialPlanes[2].dist = -brush->mins[1];

	axialPlanes[3].plane = vec3<float>(0.0f, 1.0f, 0.0f);
	axialPlanes[3].dist = brush->maxs[1];

	axialPlanes[4].plane = vec3<float>(0.0f, 0.0f, -1.0f);
	axialPlanes[4].dist = -brush->mins[2];

	axialPlanes[5].plane = vec3<float>(0.0f, 0.0f, 1.0f);
	axialPlanes[5].dist = brush->maxs[2];

	int ptCount = 0;
	int ptCountAll = 0;
	const int sideCount = brush->numsides + 6;

	std::int16_t windingSideIndex[3] = {};
	float expandedPlane[3][4] = {};
	constexpr int CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS = 128;

	// first loop should only get the axial planes till brush->numsides < 3
	for (windingSideIndex[0] = 0; windingSideIndex[0] < sideCount - 2; ++windingSideIndex[0]) {
		// sideIndex[0]-[5] are axial planes only; move the current plane into expandedPlane[0]
		getPlaneVec4(brush->sides, axialPlanes, windingSideIndex[0], (float*)expandedPlane);

		// get a plane 1 plane ahead of our first plane
		for (windingSideIndex[1] = windingSideIndex[0] + 1; windingSideIndex[1] < sideCount - 1; ++windingSideIndex[1]) {
			// check if we're using an axial plane and 2 different planes
			if (windingSideIndex[0] < 6 || windingSideIndex[1] < 6 || brush->sides[windingSideIndex[0] - 6].plane != brush->sides[windingSideIndex[1] - 6].plane) {
				// move the current plane into expandedPlane[1]
				getPlaneVec4(brush->sides, axialPlanes, windingSideIndex[1], expandedPlane[1]);

				// get a plane 1 plane ahead of our second plane
				for (windingSideIndex[2] = windingSideIndex[1] + 1; windingSideIndex[2] < sideCount - 0; ++windingSideIndex[2]) {
					// check if we use axial planes or atleast 3 different sides
					if ((windingSideIndex[0] < 6 || windingSideIndex[2] < 6 || brush->sides[windingSideIndex[0] - 6].plane != brush->sides[windingSideIndex[2] - 6].plane)
						&& (windingSideIndex[1] < 6 || windingSideIndex[2] < 6 || brush->sides[windingSideIndex[1] - 6].plane != brush->sides[windingSideIndex[2] - 6].plane)) {
						// move the current plane into expandedPlane[2]
						getPlaneVec4(brush->sides, axialPlanes, windingSideIndex[2], expandedPlane[2]);

						// intersect the 3 planes
						float xyz[3];

						if (intersectPlanes(expandedPlane[0], expandedPlane[1], expandedPlane[2], xyz)) {
							// snap our verts in xyz onto the grid
							snapPointToIntersectingPlanes(expandedPlane[0], expandedPlane[1], expandedPlane[2], xyz, 0.25f, 0.0099999998f);

							// if the planes intersected, put verts into brushPts and increase our pointCount
							ptCount = addSimpleBrushPoint(brush, axialPlanes, windingSideIndex, xyz, ptCount, brushPts);
							ptCountAll += ptCount;

							if (ptCount >= CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS - 1) {
								goto end;
							}
						}
					}
				}
			}
		}
	}

end:
	brushPts.resize(ptCount);
	return brushPts;
}

std::vector<brushmodel_entity_s> Collision::getBrushmodels()
{
	clipMap_t* cm = reinterpret_cast<clipMap_t*>(addr_clipmap_t);

	std::vector<brushmodel_entity_s> bmodels;

	// geting the total clipmap size would prob. be better
	const uintptr_t leafBrushesStart = reinterpret_cast<uintptr_t>(&*cm->leafbrushNodes);
	const uintptr_t leafBrushesEnd = leafBrushesStart + sizeof(cLeafBrushNode_s) * (cm->leafbrushNodesCount + cm->numLeafBrushes); // wrong

	// first element is always empty because
	// the first submodel within the entsMap starts at 1 and we want to avoid subtracting - 1 everywhere 
	bmodels.emplace_back(brushmodel_entity_s());

	for (auto& entity : this->entities) {
		if (entity.contains("model")) {
			std::string model = entity["model"];
			std::string origin = entity["origin"];

			// if ent is a brushmodel/submodel
			if (!model.empty() && model[0] == '*' && !origin.empty()) {
				auto currBmodel = brushmodel_entity_s();

				// get the submodel index 
				const auto pIndex = std::stoi(model.erase(0, 1));

				// the index should always match the size of our vector or we did something wrong
				if (pIndex != (int)bmodels.size()) {
					//game::Com_PrintMessage(0, utils::va("[Entities::getBrushModels]: Something went wrong while parsing submodels. (%d != %d)", pIndex, bmodels.size()), 0);
				}

				if (pIndex >= static_cast<int>(cm->numSubModels)) {
					//game::Com_PrintMessage(0, utils::va("[Entities::getBrushModels]: Something went wrong while parsing submodels. (%d >= %d numSubModels)", pIndex, game::cm->numSubModels), 0);
					break;
				}

				// assign indices and pointers to both the brush and the submodel
				currBmodel.cm_submodel_index = pIndex;

				if (&cm->cmodels[pIndex]) {
					currBmodel.cm_submodel = &cm->cmodels[pIndex];
				}

				// fix me daddy
				auto brushIndexPtr = cm->leafbrushNodes[cm->cmodels[pIndex].leaf.leafBrushNode].data.leaf.brushes;
				currBmodel.cm_brush_index = 0;

				// this is giving me cancer
				if (cm->cmodels[pIndex].leaf.leafBrushNode != 0 && brushIndexPtr) {
					if ((uintptr_t) & *brushIndexPtr >= leafBrushesStart && (uintptr_t) & *brushIndexPtr < leafBrushesEnd) {
						currBmodel.cm_brush_index = static_cast<int>(*cm->leafbrushNodes[cm->cmodels[pIndex].leaf.leafBrushNode].data.leaf.brushes);
					}
					else {
						//game::Com_PrintMessage(0, utils::va("[Entities::getBrushModels]: Skipping faulty brush-index pointer at leafbrushNodes[%d].data.leaf.brushes ...\n", pIndex), 0);
					}

					//currBModel.cmBrush = &Game::cm->brushes[*Game::cm->leafbrushNodes[Game::cm->cmodels[pIndex].leaf.leafBrushNode].data.leaf.brushes];
					currBmodel.cm_brush = &cm->brushes[currBmodel.cm_brush_index];

					// add the submodel index to the clipmap brush
					currBmodel.cm_brush->isSubmodel = true;
					currBmodel.cm_brush->cmSubmodelIndex = static_cast<std::int16_t>(pIndex);
				}


				// save entity origin
				if (!sscanf_s(origin.c_str(), "%f %f %f", &currBmodel.cm_submodel_origin[0], &currBmodel.cm_submodel_origin[1], &currBmodel.cm_submodel_origin[2])) {
					//game::Com_PrintMessage(0, utils::va("[!]: sscanf failed for submodel %d", pIndex), 0);
					currBmodel.cm_submodel_origin[0] = 0.0f;
					currBmodel.cm_submodel_origin[1] = 0.0f;
					currBmodel.cm_submodel_origin[2] = 0.0f;
				}

				bmodels.push_back(currBmodel);
			}
		}
	}

	return bmodels;
}

void Collision::parseEntities(const std::string& buffer)
{
	int parseState = 0;

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
			if (parseState == PARSE_AWAIT_KEY) {
				key.clear();
				parseState = PARSE_READ_KEY;
			}
			else if (parseState == PARSE_READ_KEY) {
				parseState = PARSE_AWAIT_VALUE;
			}
			else if (parseState == PARSE_AWAIT_VALUE) {
				value.clear();
				parseState = PARSE_READ_VALUE;
			}
			else if (parseState == PARSE_READ_VALUE) {
				entity[strToLower(key)] = value;
				parseState = PARSE_AWAIT_KEY;
			}
			else {
				throw std::runtime_error("Parsing error!");
			}
			break;
		}

		default:
		{
			if (parseState == PARSE_READ_KEY) {
				key.push_back(character);
			}
			else if (parseState == PARSE_READ_VALUE) {
				value.push_back(character);
			}

			break;
		}
		}
	}
}

std::string Collision::strToLower(std::string input)
{
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}

bool Collision::vecCompareCustomEpsilon(const vec3<float>* xyzList, const int xyzIndex, const float* v1, const float epsilon, const int coordCount)
{
	for (auto i = 0; i < coordCount; ++i) {
		if (((xyzList[xyzIndex][i] - v1[i]) * (xyzList[xyzIndex][i] - v1[i])) > (epsilon * epsilon)) {
			return false;
		}
	}

	return true;
}

// check if point exists (CM_PointInList)
int Collision::pointExistsInList(const float* point, const vec3<float>* xyzList, const int xyzCount)
{
	for (auto xyzIndex = 0; xyzIndex < xyzCount; ++xyzIndex) {
		if (vecCompareCustomEpsilon(xyzList, xyzIndex, point, 0.1f, 3)) // larger epsilon decreases quality
		{
			return 1;
		}
	}

	return 0;
}

// create a list of vertex points (CM_GetXyzList)
int Collision::getXyzList(const unsigned int windingSideIndex, const std::vector<ShowCollisionBrushPt>& pts, vec3<float>* xyzList, const int xyzLimit)
{
	int count = 0;
	int ptCount = pts.size();

	for (auto index = 0; index < ptCount; ++index) {
		if ((windingSideIndex == static_cast<std::uint16_t>(pts[index].sideIndex[0])
			|| windingSideIndex == static_cast<std::uint16_t>(pts[index].sideIndex[1])
			|| windingSideIndex == static_cast<std::uint16_t>(pts[index].sideIndex[2]))
			&& !pointExistsInList(reinterpret_cast<const float*>(&pts[index].xyz), xyzList, count)) {
			if (count == xyzLimit) {
				//game::Com_PrintMessage(0, utils::va("^1get_xyz_list L#%d ^7:: Winding point limit (%i) exceeded on brush face \n", __LINE__, xyzLimit), 0);
				return 0;
			}

			xyzList[count] = pts[index].xyz;
			++count;
		}
	}

	return count;
}

void Collision::pickProjectionAxes(const float* normal, int* i, int* j)
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
void Collision::addColinearExteriorPointToWinding(winding_t* w, const vec3<float>& pt, int i, int j, int index0, int index1)
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
float Collision::signedAreaForPointsProjected(const float* pt0, const vec3<float>& pt1, const float* pt2, const int i, const int j)
{
	return (pt2[j] - pt1[j]) * pt0[i] + (pt0[j] - pt2[j]) * pt1[i] + (pt1[j] - pt0[j]) * pt2[i];
}

vec_t Collision::lengthSquared3(const vec3_t v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

// Source :: PolyFromPlane || Q3 :: RemoveColinearPoints? (CM_AddExteriorPointToWindingProjected)
void Collision::addExteriorPointToWinding(winding_t* w, const vec3<float>& pt, int i, int j)
{
	int bestIndex = -1;
	float bestSignedArea = FLT_MAX;

	int indexPrev = w->numpoints - 1;

	for (auto index = 0; index < w->numpoints; ++index) {
		const float signedArea = signedAreaForPointsProjected(w->p[indexPrev], pt, w->p[index], i, j);

		if (bestSignedArea > signedArea) {
			bestSignedArea = signedArea;
			bestIndex = index;
		}

		indexPrev = index;
	}

	if (bestSignedArea < -0.001f) {
		memmove((char*)w->p[bestIndex + 1], (char*)w->p[bestIndex], 12 * (w->numpoints - bestIndex));

	    *reinterpret_cast<vec3<float>*>(w->p[bestIndex]) = pt;
		++w->numpoints;
	}

	else if (bestSignedArea <= 0.001f) {
		addColinearExteriorPointToWinding(w, pt, i, j, (bestIndex + w->numpoints - 1) % w->numpoints, bestIndex);
	}
}

// create a triangle to check the winding order (CM_RepresentativeTriangleFromWinding)
float Collision::representativeTriangleFromWinding(const winding_t* w, const float* normal, int* i0, int* i1, int* i2)
{
	float areaBest = 0.0f;
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
				const float testAgainst = fabs(((vc[0] * normal[0]) + (vc[1] * normal[1])) + (vc[2] * normal[2]));

				if (testAgainst > 0.0f) {
					areaBest = testAgainst;
					*i0 = i;
					*i1 = j;
					*i2 = k;
				}
			}
		}
	}

	return areaBest;
}

// create a plane from points
bool Collision::planeFromPoints(float* plane, const float* v0, const float* v1, const float* v2)
{
	float v2V0[3], v1V0[3];
	v1V0[0] = v1[0] - v0[0];
	v1V0[1] = v1[1] - v0[1];
	v1V0[2] = v1[2] - v0[2];
	v2V0[0] = v2[0] - v0[0];
	v2V0[1] = v2[1] - v0[1];
	v2V0[2] = v2[2] - v0[2];

	mm::cross3(v2V0, v1V0, plane);
	const float lengthSqr = ((plane[0] * plane[0]) + (plane[1] * plane[1])) + (plane[2] * plane[2]);

	if (lengthSqr < 2.0f) {
		if (lengthSqr == 0.0f) {
			return false;
		}

		if (lengthSquared3(v2V0) * lengthSquared3(v1V0) * 0.0000010000001f >= lengthSqr) {
			v1V0[0] = v2[0] - v1[0];
			v1V0[1] = v2[1] - v1[1];
			v1V0[2] = v2[2] - v1[2];
			v2V0[0] = v0[0] - v1[0];
			v2V0[1] = v0[1] - v1[1];
			v2V0[2] = v0[2] - v1[2];

			mm::cross3(v2V0, v1V0, plane);

			if (lengthSquared3(v2V0) * lengthSquared3(v1V0) * 0.0000010000001f >= lengthSqr) {
				return false;
			}
		}
	}

	const float length = sqrt(lengthSqr);
	plane[0] = plane[0] / length;
	plane[1] = plane[1] / length;
	plane[2] = plane[2] / length;
	plane[3] = ((v0[0] * plane[0]) + (v0[1] * plane[1])) + (v0[2] * plane[2]);

	return true;
}

void Collision::reverseWinding(winding_t* w)
{
	for (auto i = 0; i < w->numpoints / 2; ++i) {
		const float sWinding[3] =
		{
			w->p[i][0], w->p[i][1], w->p[i][2]
		};

		w->p[i][0] = w->p[w->numpoints - 1 - i][0];
		w->p[i][1] = w->p[w->numpoints - 1 - i][1];
		w->p[i][2] = w->p[w->numpoints - 1 - i][2];

		w->p[w->numpoints - 1 - i][0] = sWinding[0];
		w->p[w->numpoints - 1 - i][1] = sWinding[1];
		w->p[w->numpoints - 1 - i][2] = sWinding[2];
	}
}

void Collision::getAxialPlanes(AxialPlane_t* planes, const cbrush_t* brush)
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

bool Collision::buildBrushWindingForSide(winding_t* winding, const float* planeNormal, const int windingSideIndex, const std::vector<ShowCollisionBrushPt> pts)
{
	int i, i0, i1, i2, j;
	int ptCount = pts.size();
	vec4_t plane {};

	if (!winding) {
		//game::Com_Error(0, utils::va("CM_BuildBrushWindingForSide L#%d :: winding \n", __LINE__));
		return false;
	}

	if (!planeNormal) {
		//game::Com_Error(0, COM_ERROR_MSG);
		return false;
	}

	// create a list of vertex points
	vec3<float> xyzList[1024];
	const int xyzCount = getXyzList(windingSideIndex, pts, xyzList, 1024);

	// we need atleast a triangle to create a poly
	if (xyzCount < 3) {
		return false;
	}

	/*/ direction of camera plane
	const vec3<float> cameraDirectionToPlane = xyzList[0] - glob::lpmoveCameraOrigin;

	// dot product between line from camera to the plane and the normal
	// if dot > 0 then the plane is facing away from the camera (dot = 1 = plane is facing the same way as the camera; dot = -1 = plane looking directly towards the camera)
	if (glm::dot(glm::vec3(planeNormal[0], planeNormal[1], planeNormal[2]), cameraDirectionToPlane) > 0.0f && !dvars::rDrawCollisionPolyFace->current.enabled) {
		return false;
	}*/

	// find the major axis
	pickProjectionAxes(planeNormal, &i, &j);

    *reinterpret_cast<vec3<float>*>(winding->p[0]) = xyzList[0];
	*reinterpret_cast<vec3<float>*>(winding->p[1]) = xyzList[1];

	winding->numpoints = 2;

	for (auto k = 2; k < xyzCount; ++k) {
		addExteriorPointToWinding(winding, xyzList[k], i, j);
	}

	// build a triangle of our winding points so we can check if the winding is clock-wise
	if (representativeTriangleFromWinding(winding, planeNormal, &i0, &i1, &i2) < 0.001f) {
		// do nothing if it is counter clock-wise
		return false;
	}

	// *
	// winding is clock-wise ..

	// create a temp plane
	planeFromPoints(&*plane, winding->p[i0], winding->p[i1], winding->p[i2]);

	// if our winding has a clock-wise winding, reverse it
	if (mm::dot3(plane, planeNormal) > 0.0f) {
		reverseWinding(winding);
	}

	return true;
}

// create plane for intersection (CM_GetPlaneVec4Form)
void Collision::getPlaneVec4(const cbrushside_t* sides, const AxialPlane_t* axialPlanes, const int index, float* expandedPlane)
{
	if (index >= 6) {
		if (!sides) {
			return;
		}

		expandedPlane[0] = sides[index - 6].plane->normal[0];
		expandedPlane[1] = sides[index - 6].plane->normal[1];
		expandedPlane[2] = sides[index - 6].plane->normal[2];
		expandedPlane[3] = sides[index - 6].plane->dist;
	}
	else {
		expandedPlane[0] = axialPlanes[index].plane.x;
		expandedPlane[1] = axialPlanes[index].plane.y;
		expandedPlane[2] = axialPlanes[index].plane.z;
		expandedPlane[3] = axialPlanes[index].dist;
	}
}

// intersect 3 planes
int Collision::intersectPlanes(const float* plane0, const float* plane1, const float* plane2, float* xyz)
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

bool Collision::isOnGrid(const float* snapped, const float* xyz)
{
	return xyz[0] == snapped[0] && xyz[1] == snapped[1] && xyz[2] == snapped[2];
}

void Collision::snapPointToIntersectingPlanes(const float* plane0, const float* plane1, const float* plane2, float* xyz, float snapGrid, const float snapEpsilon)
{
	float snapped[3], currentPlane[4];

	snapGrid = 1.0f / snapGrid;

	// cod4map
	for (auto axis = 0; axis < 3; ++axis) {
		const float rounded = round(xyz[axis] * snapGrid) / snapGrid;
		const float delta = fabs(rounded - xyz[axis]);

		if (snapEpsilon <= delta) {
			snapped[axis] = xyz[axis];
		}
		else {
			snapped[axis] = rounded;
		}
	}

	if (!isOnGrid(snapped, xyz)) {
		float maxSnapError = 0.0f;
		float maxBaseError = snapEpsilon;

		for (auto planeIndex = 0; planeIndex < 3; ++planeIndex) {
			if (planeIndex == 0) {
				memcpy(&currentPlane, plane0, sizeof(currentPlane));
			}
			else if (planeIndex == 1) {
				memcpy(&currentPlane, plane1, sizeof(currentPlane));
			}
			else if (planeIndex == 2) {
				memcpy(&currentPlane, plane2, sizeof(currentPlane));
			}

			const float snapError = log((currentPlane[0] * snapped[0] + currentPlane[1] * snapped[1] + currentPlane[2] * snapped[2]) - currentPlane[3]);
			if (snapError > maxSnapError) {
				maxSnapError = snapError;
			}

			const float baseError = log((currentPlane[0] * xyz[0] + currentPlane[1] * xyz[1] + currentPlane[2] * xyz[2]) - currentPlane[3]);
			if (baseError > maxBaseError) {
				maxBaseError = baseError;
			}
		}

		if (maxBaseError > maxSnapError) {
			xyz[0] = snapped[0];
			xyz[1] = snapped[1];
			xyz[2] = snapped[2];
		}
	}
}

// add valid vertices from 3 plane intersections (CM_AddSimpleBrushPoint)
int Collision::addSimpleBrushPoint(const cbrush_t* brush, const AxialPlane_t* axialPlanes, const __int16* sideIndices, const float* xyz, int ptCount, std::vector<ShowCollisionBrushPt>& brushPts)
{
	constexpr int CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS = 128;
	if (!brush) {
		return 0;
	}

	for (auto windingSideIndex = 0u; windingSideIndex < 6; ++windingSideIndex) {
		if (((axialPlanes[windingSideIndex].plane.x * xyz[0] + axialPlanes[windingSideIndex].plane.y * xyz[1] + axialPlanes[windingSideIndex].plane.z * xyz[2])
			- axialPlanes[windingSideIndex].dist) > 0.1f) {
			return ptCount;
		}
	}

	for (auto windingSideIndex = 0u; windingSideIndex < brush->numsides; ++windingSideIndex) {
		const auto plane = brush->sides[windingSideIndex].plane;

		if (plane != brush->sides[sideIndices[0] - 6].plane
			&& plane != brush->sides[sideIndices[1] - 6].plane
			&& plane != brush->sides[sideIndices[2] - 6].plane
			&& ((plane->normal[0] * xyz[0]) + (plane->normal[1] * xyz[1]) + (plane->normal[2] * xyz[2]) - plane->dist) > 0.1f) {
			return ptCount;
		}
	}

	if (ptCount > CM_MAX_BRUSHPOINTS_FROM_INTERSECTIONS - 2) // T5: 1024
	{
		return ptCount;
	}

	brushPts[ptCount].xyz[0] = xyz[0];
	brushPts[ptCount].xyz[1] = xyz[1];
	brushPts[ptCount].xyz[2] = xyz[2];

	brushPts[ptCount].sideIndex[0] = sideIndices[0];
	brushPts[ptCount].sideIndex[1] = sideIndices[1];
	brushPts[ptCount].sideIndex[2] = sideIndices[2];

	return ptCount + 1;
}
