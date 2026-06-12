#include "pch.h"
#include "ui_jump_target.h"

#include "Avengers.h"


ui_jump_target::ui_jump_target(Avengers* hud)
{

}
ui_jump_target::~ui_jump_target()
{

}

void ui_jump_target::render()
{
	Avengers* hud = Avengers::get_instance();

	static float minDist = 99999999.f;
	static float minDistZ = 99999999.f;
	static float globalMinZ = 9999999.f;
	static bool onGroundLastFrame = true;
	static std::chrono::time_point measureStartPoint = std::chrono::system_clock::now();
	constexpr int minimumAirtimeMs = 500;

	bool onGround = hud->gameState->onGround;
	vec3<float> jumpTargetOrigin = hud->inst_ui_menu->jump_target_origin;
	vec3<float> origin = hud->gameState->origin;
	float dist;
	float distZ;

	if (hud->inst_game->is_spectating() || hud->inst_game->is_noclipping()) {
		return;
	}

	if ((hud->inst_ui_menu->brush_mode && selectedBrushes.size() == 0) || (!hud->inst_ui_menu->brush_mode && jumpTargetOrigin == vec3<float>(0.f, 0.f, 0.f))) {
		return;
	}

	if (hud->inst_ui_menu->brush_mode) {
		vec2<float> brushDist = getBrushDistance();
		dist = brushDist.x;
		distZ = brushDist.y;

		if (brushDist.y < globalMinZ) {
			globalMinZ = brushDist.y;
		}
	}
	else {
		dist = origin.Dist(jumpTargetOrigin);
		distZ = jumpTargetOrigin.z - origin.z;

		if (distZ < globalMinZ) {
			globalMinZ = distZ;
		}
	}

	if (dist < minDist) {
		minDist = dist;
		minDistZ = distZ;
	}

	std::chrono::time_point now = std::chrono::system_clock::now();

	if (!onGroundLastFrame && onGround) {
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - measureStartPoint).count() >= minimumAirtimeMs) {
			std::stringstream ss;
			ss << globalMinZ << " too low" << std::endl;
			ss << minDist << " units off" << std::endl;
			ss << minDistZ << " too low at closest position";
			hud->inst_game->add_obituary(ss.str());
		}

		minDist = 99999999.f;
		minDistZ = 99999999.f;
		globalMinZ = 9999999.f;
	}

	if (onGround) {
		measureStartPoint = now;
	}

	onGroundLastFrame = onGround;
}

void ui_jump_target::addBrush()
{
	selectFacePlayerIsStandingOn();
}

void ui_jump_target::removeBrush()
{
	if (selectedBrushes.size() > 0) {
		selectedBrushes.pop_back();
	}
}

void ui_jump_target::resetBrushes()
{
	selectedBrushes.clear();
}

void ui_jump_target::selectFacePlayerIsStandingOn()
{
	Avengers* avengers = Avengers::get_instance();
	auto gameState = avengers->gameState;

	vec3<float> origin = gameState->origin;
	BrushSide* face = nullptr;

	for (auto& brush : avengers->collision->processedBrushes) {
		for (auto& brushSide : brush.sides) {
			constexpr float LEEWAY = 0.01f;
			constexpr float EXTEND_WIDTH = 15.f;
			if (fabsf(fabsf(brushSide.center.z + 0.125f) - fabsf(origin.z)) <= LEEWAY) {
				vec2<float> origin2D = vec2<float>(origin.x, origin.y);
				std::vector<vec2<float>> points2D;
				for (const vec3<float>& vertex: brushSide.points) {
					points2D.push_back(vec2<float>(vertex.x, vertex.y));
				}

				extendPolygon(points2D, EXTEND_WIDTH);
				if (isInPolygon(points2D, origin2D)) {
					face = &brushSide;
					break;
				}
			}
		}
	}
	
	if (face && std::find(selectedBrushes.begin(), selectedBrushes.end(), face) == selectedBrushes.end()) {
		selectedBrushes.push_back(face);
	}
}

vec2<float> ui_jump_target::getBrushDistance()
{
	float closest = 9999999999.f;
	float closestZ = 9999999999.f;
	constexpr float EXTEND_WIDTH = 15.f;

	Avengers* avengers = Avengers::get_instance();
	auto gameState = avengers->gameState;
	vec3<float> origin = gameState->origin;
	BrushSide* selectedBrush;

	for (BrushSide* brush : selectedBrushes) {
		std::vector<vec2<float>> points2D;
		std::vector<vec3<float>> points;

		for (const vec3<float>& vertex : brush->points) {
			points2D.push_back(vec2<float>(vertex.x, vertex.y));
		}
		extendPolygon(points2D, EXTEND_WIDTH);

		for (int i = 0; i < points2D.size(); i++) {
			points.push_back(vec3<float>(points2D[i].x, points2D[i].y, brush->points[i].z + 0.125f));  //Add 0.125 because were looking at collisions
		}

		float dist = PolygonDist(points, origin);
		if (dist < closest) {
			closest = dist;
			closestZ = points[0].z - origin.z;
			selectedBrush = brush;
		}
	}

	return vec2<float>(closest, closestZ);
}

//I had copilot write these, hopefully they work
float ui_jump_target::PolygonSideDist(const std::vector<vec3<float>>& polygon, const vec3<float>& point)
{
	if (polygon.empty())
		return 0.0f;

	const size_t n = polygon.size();
	if (n == 1) {
		// distance to the single vertex
		const float dx = point.x - polygon[0].x;
		const float dy = point.y - polygon[0].y;
		const float dz = point.z - polygon[0].z;
		return std::sqrt(dx*dx + dy*dy + dz*dz);
	}

	float minDist2 = 999999999999.f;

	for (size_t i = 0; i < n; ++i) {
		const size_t j = (i + 1) % n;
		const vec3<float>& A = polygon[i];
		const vec3<float>& B = polygon[j];

		// AB = B - A
		const float abx = B.x - A.x;
		const float aby = B.y - A.y;
		const float abz = B.z - A.z;

		// AP = P - A
		const float apx = point.x - A.x;
		const float apy = point.y - A.y;
		const float apz = point.z - A.z;

		const float ab_len2 = abx*abx + aby*aby + abz*abz;

		float t;
		if (ab_len2 <= 1e-12f) {
			// Degenerate edge: treat as distance to A
			t = 0.0f;
		} else {
			const float dot = apx*abx + apy*aby + apz*abz;
			t = dot / ab_len2;
			if (t < 0.0f) t = 0.0f;
			else if (t > 1.0f) t = 1.0f;
		}

		// Closest point = A + t * AB
		const float cx = A.x + t * abx;
		const float cy = A.y + t * aby;
		const float cz = A.z + t * abz;

		// squared distance from point to closest
		const float dx = point.x - cx;
		const float dy = point.y - cy;
		const float dz = point.z - cz;
		const float dist2 = dx*dx + dy*dy + dz*dz;

		if (dist2 < minDist2)
			minDist2 = dist2;
	}

	return std::sqrt(minDist2);
}

float ui_jump_target::PolygonDist(const std::vector<vec3<float>>& polygon, const vec3<float>& point)
{
	if (polygon.empty())
		return 0.0f;

	const size_t n = polygon.size();
	if (n < 3) {
		// line or point: distance to segments/points
		return PolygonSideDist(polygon, point);
	}

	constexpr float EPS = 1e-8f;

	// origin for plane = first vertex
	const vec3<float>& O = polygon[0];

	// Find two non-collinear edges to form a normal
	float nx = 0.0f, ny = 0.0f, nz = 0.0f;
	float norm2 = 0.0f;
	bool found = false;
	for (size_t i = 1; i + 1 < n; ++i) {
		// v1 = polygon[i] - O
		const float v1x = polygon[i].x - O.x;
		const float v1y = polygon[i].y - O.y;
		const float v1z = polygon[i].z - O.z;
		// v2 = polygon[i+1] - O
		const float v2x = polygon[i + 1].x - O.x;
		const float v2y = polygon[i + 1].y - O.y;
		const float v2z = polygon[i + 1].z - O.z;

		// normal = v1 cross v2
		nx = v1y * v2z - v1z * v2y;
		ny = v1z * v2x - v1x * v2z;
		nz = v1x * v2y - v1y * v2x;

		norm2 = nx * nx + ny * ny + nz * nz;
		if (norm2 > EPS) {
			found = true;
			break;
		}
	}

	if (!found) {
		// Degenerate / collinear polygon => distance to edges/vertices
		return PolygonSideDist(polygon, point);
	}

	// Compute signed distance numerator: dot(AP, normal)
	const float apx = point.x - O.x;
	const float apy = point.y - O.y;
	const float apz = point.z - O.z;
	const float signedDot = apx * nx + apy * ny + apz * nz;

	// perpendicular distance magnitude
	const float invSqrtNorm = 1.0f / std::sqrt(norm2);
	const float perpDist = std::fabs(signedDot) * invSqrtNorm;

	// Build local 2D basis: u = normalize(first non-zero edge we used), v = normalize(cross(normal, u))
	// Recompute the first non-zero edge used above as vector e = polygon[i] - O (we can reuse v1 from loop if we had saved it;
	// recompute by finding any edge with length > EPS)
	float ex = 0.0f, ey = 0.0f, ez = 0.0f;
	bool gotEdge = false;
	for (size_t i = 1; i < n; ++i) {
		ex = polygon[i].x - O.x;
		ey = polygon[i].y - O.y;
		ez = polygon[i].z - O.z;
		const float elen2 = ex * ex + ey * ey + ez * ez;
		if (elen2 > EPS) {
			const float invElen = 1.0f / std::sqrt(elen2);
			ex *= invElen;
			ey *= invElen;
			ez *= invElen;
			gotEdge = true;
			break;
		}
	}
	if (!gotEdge) {
		// fallback
		return PolygonSideDist(polygon, point);
	}

	// u = normalized edge (ex,ey,ez)
	const float ux = ex;
	const float uy = ey;
	const float uz = ez;

	// v = normalize(normal cross u)
	float vx = ny * uz - nz * uy;
	float vy = nz * ux - nx * uz;
	float vz = nx * uy - ny * ux;
	const float vlen2 = vx * vx + vy * vy + vz * vz;
	if (vlen2 <= EPS) {
		// numerical degeneracy fallback
		return PolygonSideDist(polygon, point);
	}
	const float invVlen = 1.0f / std::sqrt(vlen2);
	vx *= invVlen;
	vy *= invVlen;
	vz *= invVlen;

	// Project polygon vertices and the projected point into 2D coords
	std::vector<vec2<float>> poly2d;
	poly2d.reserve(n);
	for (const auto& vert : polygon) {
		const float rx = vert.x - O.x;
		const float ry = vert.y - O.y;
		const float rz = vert.z - O.z;
		const float px = rx * ux + ry * uy + rz * uz;
		const float py = rx * vx + ry * vy + rz * vz;
		poly2d.push_back(vec2<float>(px, py));
	}

	// projected point: subtract projection along normal: point_proj = point - (signedDot / norm2) * normal
	const float projScale = signedDot / norm2;
	const float projx = point.x - projScale * nx;
	const float projy = point.y - projScale * ny;
	const float projz = point.z - projScale * nz;

	const float rpx = projx - O.x;
	const float rpy = projy - O.y;
	const float rpz = projz - O.z;
	const float point2dx = rpx * ux + rpy * uy + rpz * uz;
	const float point2dy = rpx * vx + rpy * vy + rpz * vz;
	const vec2<float> p2{ point2dx, point2dy };

	// If projected point lies within polygon, perpendicular distance is the answer
	if (isInPolygon(poly2d, p2)) {
		return fabsf(polygon[0].z - point.z);
	}

	// Otherwise distance is to polygon sides/vertices
	return PolygonSideDist(polygon, point);
}

bool ui_jump_target::isInPolygon(const std::vector<vec2<float>>& polygon, const vec2<float>& point)
{
	if (polygon.size() < 3)
		return false;

	const float EPS = 1e-6f;

	// Compute centroid
	vec2<float> center{};
	for (const auto& p : polygon) {
		// assume vec2 has members x and y
		center.x += p.x;
		center.y += p.y;
	}
	center.x /= static_cast<float>(polygon.size());
	center.y /= static_cast<float>(polygon.size());

	// Make a copy and sort by angle around centroid to form a consistent polygon order
	std::vector<vec2<float>> poly = polygon;
	std::sort(poly.begin(), poly.end(), [&](const vec2<float>& a, const vec2<float>& b) {
		return std::atan2f(a.y - center.y, a.x - center.x) < std::atan2f(b.y - center.y, b.x - center.x);
		});

	auto pointOnSegment = [&](const vec2<float>& a, const vec2<float>& b, const vec2<float>& p) -> bool {
		// Check if p is colinear with a-b: cross product near zero
		const float cross = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
		if (std::fabs(cross) > EPS)
			return false;
		// Check if p is within bounding box of a and b
		const float minX = std::fmin(a.x, b.x) - EPS;
		const float maxX = std::fmax(a.x, b.x) + EPS;
		const float minY = std::fmin(a.y, b.y) - EPS;
		const float maxY = std::fmax(a.y, b.y) + EPS;
		return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY);
		};

	bool inside = false;
	const size_t n = poly.size();
	for (size_t i = 0; i < n; ++i) {
		size_t j = (i + 1) % n;
		const vec2<float>& a = poly[i];
		const vec2<float>& b = poly[j];

		// If point is exactly on an edge, consider it inside
		if (pointOnSegment(a, b, point))
			return true;

		// Ray-casting: check if edge crosses the horizontal ray to the right of the point
		const bool yi_gt = a.y > point.y;
		const bool yj_gt = b.y > point.y;

		// Edge crosses the horizontal line of point if yi_gt != yj_gt
		if (yi_gt != yj_gt) {
			// Compute x coordinate of intersection of edge with horizontal line at point.y
			const float intersectX = a.x + (b.x - a.x) * ((point.y - a.y) / (b.y - a.y));
			if (intersectX > point.x)
				inside = !inside;
		}
	}

	return inside;
}

void ui_jump_target::extendPolygon(std::vector<vec2<float>>& polygon, float offset)
{
	if (polygon.empty())
		return;

	// Compute centroid (arithmetic mean)
	vec2<float> center{};
	for (const auto& p : polygon) {
		center.x += p.x;
		center.y += p.y;
	}
	const float invN = 1.0f / static_cast<float>(polygon.size());
	center.x *= invN;
	center.y *= invN;

	// Offset each vertex radially from the centroid
	for (auto& p : polygon) {
		const float dx = p.x - center.x;
		const float dy = p.y - center.y;
		if (dx > 0) {
			p.x = center.x + dx + offset;
		}
		else {
			p.x = center.x + dx - offset;
		}

		if (dy > 0) {
			p.y = center.y + dy + offset;
		}
		else {
			p.y = center.y + dy - offset;
		}
	}
}
