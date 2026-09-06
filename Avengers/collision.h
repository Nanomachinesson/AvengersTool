#pragma once
#include <vector>
#include <algorithm>

struct AxialPlane_t
{
	vec3<float> plane;
	float dist;
};

struct BrushSide
{
	vec3<float> center;
	std::vector<vec3<float>> points;
};

struct ProcessedBrush
{
	std::vector<ShowCollisionBrushPt> points;
	std::vector<BrushSide> sides;
	std::vector<std::string> materials;
	vec3<float> center;
	ImColor color;

	bool isClip = false;
	bool isSky = false;
};

class Collision
{
public:
	Collision(Avengers* avengers);
	~Collision();
	void render();
	void init();  //once per map
	std::vector<ProcessedBrush> processedBrushes;
	bool hasInitialized = false;

private:
	bool isClip(const std::string& materialName);
	bool isSky(const std::string& materialName);
	void buildBrushes();
	void buildCollisionPoints(ProcessedBrush& processedBrush, cbrush_t* brush, const std::vector<ShowCollisionBrushPt>& pts);
	void drawCollision(ProcessedBrush& processedBrush);
	void drawCircle(const vec3<float>& pos, ImColor color);
	void createMaterials();

	Avengers* avengers;

private:
	/* These are ported from iw3xo:
	*
	* https://github.com/xoxor4d/iw3xo-dev
	*
	///////////////////////////////////////////////////////*/
	std::vector<ShowCollisionBrushPt> getPointsForBrush(cbrush_t* brush);
	int addSimpleBrushPoint(const cbrush_t* brush, const AxialPlane_t* axialPlanes, const __int16* sideIndices, const float* xyz, int ptCount, std::vector<ShowCollisionBrushPt>& brushPts);
	void snapPointToIntersectingPlanes(const float* plane0, const float* plane1, const float* plane2, float* xyz, float snapGrid, const float snapEpsilon);
	bool isOnGrid(const float* snapped, const float* xyz);
	int intersectPlanes(const float* plane0, const float* plane1, const float* plane2, float* xyz);
	void getPlaneVec4(const cbrushside_t* sides, const AxialPlane_t* axialPlanes, const int index, float* expandedPlane);
	std::vector<brushmodel_entity_s> getBrushmodels();
	void parseEntities(const std::string& buffer);
	std::string strToLower(std::string input);
	bool buildBrushWindingForSide(winding_t* winding, const float* planeNormal, const int windingSideIndex, const std::vector<ShowCollisionBrushPt> pts);
	int getXyzList(const unsigned int windingSideIndex, const std::vector<ShowCollisionBrushPt>& pts, vec3<float>* xyzList, const int xyzLimit);
	bool vecCompareCustomEpsilon(const vec3<float>* xyzList, const int xyzIndex, const float* v1, const float epsilon, const int coordCount);
	int pointExistsInList(const float* point, const vec3<float>* xyzList, const int xyzCount);
	void pickProjectionAxes(const float* normal, int* i, int* j);
	void addColinearExteriorPointToWinding(winding_t* w, const vec3<float>& pt, int i, int j, int index0, int index1);
	void addExteriorPointToWinding(winding_t* w, const vec3<float>& pt, int i, int j);
	float signedAreaForPointsProjected(const float* pt0, const vec3<float>& pt1, const float* pt2, const int i, const int j);
	float representativeTriangleFromWinding(const winding_t* w, const float* normal, int* i0, int* i1, int* i2);
	void cross3(const vec3_t v1, const vec3_t v2, vec3_t cross);
	bool planeFromPoints(float* plane, const float* v0, const float* v1, const float* v2);
	vec_t lengthSquared3(const vec3_t v);
	vec_t dot3(const vec3_t v1, const vec3_t v2);
	void reverseWinding(winding_t* w);
	void getAxialPlanes(AxialPlane_t* planes, const cbrush_t* brush);

	struct windingpool_t
	{
		int numpoints;
		float p[256][4][3];
	};

	enum
	{
		PARSE_AWAIT_KEY,
		PARSE_READ_KEY,
		PARSE_AWAIT_VALUE,
		PARSE_READ_VALUE,
	};

	std::vector<std::unordered_map<std::string, std::string>> entities;
	std::vector<brushmodel_entity_s> brushModels;
	std::vector<dmaterial_t*> mapMaterials;
	windingpool_t windingPool;
};
