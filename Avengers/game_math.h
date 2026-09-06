#pragma once
#include "vectors.h"
#include "cod4Structs.h"

namespace mm
{
	constexpr float PI = 3.14159265358979323846f;
	vec2<float> truncateVector(const vec3<float>& v);
	vec2<float> rotatePoint(const vec2<float>& p1, const vec2<float>& p2, float angle);  //p1 around p2
	float normalise(float value, float start, float end);
	double shortToAngle(int angle);
	float tiltAngle(const vec2<float>& v);
	float toDegrees(float radians);
	void angleVectors(vec3<float> angles, float* forward, float* right, float* up);
	void angleVectors(vec3<float> angles, vec3<float>* forward, vec3<float>* right, vec3<float>* up);
	int compareAngles(float sourceAngle, float otherAngle);
	void subtract3(const vec3_t v1, const vec3_t v2, vec3_t out);
	void cross3(const vec3_t v1, const vec3_t v2, vec3_t cross);
	vec_t dot3(const vec3_t v1, const vec3_t v2);
}
