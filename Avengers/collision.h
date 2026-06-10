#pragma once
#include <vector>

struct AxialPlane_t
{
	vec3<float> plane;
	float dist;
};

struct BrushSide
{
	std::vector<vec3<float>> points;
};

struct ProcessedBrush
{
	std::vector<ShowCollisionBrushPt> points;
	std::vector<BrushSide> sides;
	vec3<float> center;
	ImColor color;
};

class Collision
{
public:
	Collision(Avengers* avengers);
	~Collision();
	void render();
	void init();  //once per map

private:
	void buildBrushes();
	void buildCollisionPoints(ProcessedBrush& processedBrush, cbrush_t* brush, const std::vector<ShowCollisionBrushPt>& pts);
	void drawCollision(ProcessedBrush& processedBrush);
	void drawCircle(const vec3<float>& pos, ImColor color);

	Avengers* avengers;
	std::vector<ProcessedBrush> processedBrushes;

private:
	/* These are ported from iw3xo:
	*
	* https://github.com/xoxor4d/iw3xo-dev
	*
	///////////////////////////////////////////////////////*/
	std::vector<ShowCollisionBrushPt> getPointsForBrush(cbrush_t* brush);
	int add_simple_brush_point(const cbrush_t* brush, const AxialPlane_t* axial_planes, const __int16* side_indices, const float* xyz, int pt_count, std::vector<ShowCollisionBrushPt>& brush_pts);
	void snap_point_to_intersecting_planes(const float* plane0, const float* plane1, const float* plane2, float* xyz, float snap_grid, const float snap_epsilon);
	bool is_on_grid(const float* snapped, const float* xyz);
	int intersect_planes(const float* plane0, const float* plane1, const float* plane2, float* xyz);
	void get_plane_vec4(const cbrushside_t* sides, const AxialPlane_t* axial_planes, const int index, float* expanded_plane);
	std::vector<brushmodel_entity_s> get_brushmodels();
	void parseEntities(const std::string& buffer);
	std::string str_to_lower(std::string input);
	bool build_brush_winding_for_side(winding_t* winding, const float* plane_normal, const int side_index, const std::vector<ShowCollisionBrushPt> pts);
	int get_xyz_list(const unsigned int side_index, const std::vector<ShowCollisionBrushPt>& pts, vec3<float>* xyz_list, const int xyz_limit);
	bool vec_compare_custom_epsilon(const vec3<float>* xyz_list, const int xyz_index, const float* v1, const float epsilon, const int coord_count);
	int point_exists_in_list(const float* point, const vec3<float>* xyz_list, const int xyz_count);
	void pick_projection_axes(const float* normal, int* i, int* j);
	void add_colinear_exterior_point_to_winding(winding_t* w, const vec3<float>& pt, int i, int j, int index0, int index1);
	void add_exterior_point_to_winding(winding_t* w, const vec3<float>& pt, int i, int j);
	float signed_area_for_points_projected(const float* pt0, const vec3<float>& pt1, const float* pt2, const int i, const int j);
	float representative_triangle_from_winding(const winding_t* w, const float* normal, int* i0, int* i1, int* i2);
	void cross3(const vec3_t v1, const vec3_t v2, vec3_t cross);
	bool plane_from_points(float* plane, const float* v0, const float* v1, const float* v2);
	vec_t length_squared3(const vec3_t v);
	vec_t dot3(const vec3_t v1, const vec3_t v2);
	void reverse_winding(winding_t* w);
	void get_axial_planes(AxialPlane_t* planes, const cbrush_t* brush);

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
	windingpool_t winding_pool;
};
