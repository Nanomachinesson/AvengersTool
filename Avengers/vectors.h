#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <sstream>
#include <iomanip>
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"
typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

template <typename T>
struct vec2
{
public:
	vec2() { x = 0;  y = 0; };
	vec2(T vecX, T vecY) { x = vecX; y = vecY; }
	vec2(const vec2& v) { x = v.x; y = v.y; }
	inline vec2 operator + (const vec2& v) const { return vec2(x + v.x, y + v.y); }
	inline vec2 operator - (const vec2& v) const { return vec2(x - v.x, y - v.y); }
	inline vec2 operator / (const vec2& v) const { return vec2(x / v.x, y / v.y); }
	inline vec2 operator * (const vec2& v) const { return vec2(x * v.x, y * v.y); }
	inline bool operator == (const vec2& v) const { return x == v.x && y == v.y; }
	inline bool operator != (const vec2& v) const { return x != v.x && y != v.y; }
	inline vec2& operator += (const vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	inline vec2& operator -= (const vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	inline vec2& operator *= (const vec2& v) { this->x *= v.x, this->y *= v.y; return *this; }
	inline vec2& operator /= (const vec2& v) { this->x += v.x; this->y += v.y; return *this; }
	inline T& operator [] (int index) { T f = FLT_MIN; if (index == 0) return x;  if (index == 1) return y; return f; }
	const T& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; return FLT_MIN; }
	ImVec2 toImvec2() const { return ImVec2(x, y); }
	inline double length() { return sqrtf(pow(x, 2) + pow(y, 2)); }
	static vec2 tovec2(const ImVec2& v) { return vec2(v.x, v.y); }
	inline double dist(const vec2 v) { return (*this - v).length(); }
	inline vec2 snapped() { return vec2(roundf(x), roundf(y)); }
	T x, y;
};


template <typename T>
struct vec3
{
	T x, y, z;
	vec3() { x = 0; y = 0; z = 0; };
	vec3(vec3_t vec) { x = vec[0]; y = vec[1]; z = vec[2]; }
	vec3(T vecX, T vecY, T vecZ) { x = vecX; y = vecY; z = vecZ; }
	vec3(const vec3& v) { x = v.x; y = v.y; z = v.z; }
	inline vec3 operator + (const vec3& v) { return vec3(x + v.x, y + v.y, z + v.z); }
	inline vec3 operator - (const vec3& v) { return vec3(x - v.x, y - v.y, z - v.z); }
	inline vec3 operator / (const vec3& v) { return vec3(x / v.x, y / v.y, z / v.z); }
	inline vec3 operator / (T l) { return vec3(x / l, y / l, z / l); }
	inline vec3 operator * (const vec3& v) { return vec3(x * v.x, y * v.y, z * v.z); }
	inline bool operator == (const vec3& v) { return x == v.x && y == v.y && z == v.z; }
	inline bool operator != (const vec3& v) { return x != v.x || y != v.y || z != v.z; }
	inline T& operator [] (int index) { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; return z; }
	const T& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; return FLT_MIN; }
	inline vec3& operator += (const vec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
	inline vec3& operator -= (const vec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
	inline vec3& operator *= (const vec3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
	inline vec3& operator /= (const vec3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }
	inline vec3& operator /= (T l) { this->x /= l; this->y /= l; this->z /= l; return *this; }

	inline std::string toString() { std::stringstream ss; std::setprecision(6); ss << std::fixed << x << " " << y << " " << z; return ss.str(); }
	inline vec3 crossProduct(const vec3 v) { return vec3(v.y * this.z - v.z * this.y, -(v.x * this.z - v.z * this.x), v.x * this.y - v.y * this.x); }
	inline double length() { return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
	inline double length2D() { return sqrtf(pow(x, 2) + pow(y, 2)); }
	inline double length2DRounded() { return sqrtf(pow(roundf(x), 2) + pow(roundf(y), 2)); }
	inline double lengthZ() { return sqrt(pow(y, 2)); }
	inline T dotProduct(const vec3 v) { return (x * v.x + y * v.y + z * v.z); }
	inline vec3 rounded() { return vec3(roundf(this->x), roundf(this->y), roundf(this->z)); }
	inline vec3 floored() { return vec3(floorf(this->x), floorf(this->y), floorf(this->z)); }
	inline vec3 ceiled() { return vec3(ceilf(this->x), ceilf(this->y), ceilf(this->z)); }
	inline double dist(const vec3 v) { return (*this - v).length(); }
	inline double dist2D(const vec3 v) { return (vec2(x, y) - vec2(v.x, v.y)).length(); }
	inline T* v3t() { T x[3]; x[0] = this->x; x[1] = this->y; x[2] = this->z;  return x; }
	inline vec3 normalize360() { return vec3(fmodf(this->x + 360, 360), fmodf(this->y + 360, 360), fmodf(this->z + 360, 360)); }
	inline vec3 getFractional() { T whole; return { std::modf(x, &whole), std::modf(y, &whole),std::modf(z, &whole) }; }
	inline vec3 getFractionalAbs() { T whole; return { fabs(std::modf(x, &whole)), fabs(std::modf(y, &whole)), fabs(std::modf(z, &whole)) }; }
	inline vec3 normalized() { return vec3(x / length(), y / length(), z / length()); }
	inline void normalize() { *this = vec3(x / length(), y / length(), z / length()); }
	inline vec3 originFromStance(int stance) {
		if (stance == 0) { return { this->x, this->y, this->z - 60 }; };
		if (stance == 1) { return { this->x, this->y, this->z - 40 }; };
		if (stance == 2) { return { this->x, this->y, this->z - 11 }; };
		return *this;
	}
	inline vec3 clearXYZ() { return vec3(0,0,0); };
};

template <typename T>
struct vec4
{
	T x, y, z, w;
	vec4() { x = 0; y = 0; z = 0; w = 0; };
	vec4(vec4_t vec) { x = vec[0]; y = vec[1]; z = vec[2]; w = vec[3]; }
	vec4(T vecX, T vecY, T vecZ, T vecW) { x = vecX; y = vecY; z = vecZ; w = vecW; }
	vec4(const vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	inline vec4 operator + (const vec4& v) { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline vec4 operator - (const vec4& v) { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline vec4 operator / (const vec4& v) { return vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
	inline vec4 operator * (const vec4& v) { return vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
	inline bool operator == (const vec4& v) { return x == v.x && y == v.y && z == v.z && w == v.w; }
	inline bool operator != (const vec4& v) { return x != v.x || y != v.y || z != v.z || w != v.w; }
	inline T& operator [] (int index) { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; if (index == 3) return w; return w; }
	const T& operator [] (int index) const { if (index == 0) return x;  if (index == 1) return y; if (index == 2) return z; if (index == 3) return w; return FLT_MIN; }
	inline vec4& operator += (const vec4& v) { this->x += v.x; this->y += v.y; this->z += v.z; this->w += v.w; return *this; }
	inline vec4& operator -= (const vec4& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; this->w -= v.w; return *this; }
	inline vec4& operator *= (const vec4& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; this->w *= v.w; return *this; }
	inline vec4& operator /= (const vec4& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; this->w /= v.w; return *this; }
	inline T* v4t() { T x[4]; x[0] = this->x; x[1] = this->y; x[2] = this->z; x[3] = this->w; return x; }
	inline ImVec4 toImvec4() { return { x, y, z, w }; }
	inline ImColor toImColor() { return { x, y, z, w }; }
};



