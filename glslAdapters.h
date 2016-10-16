#pragma once

#include <assert.h>
#include <cmath>

struct vec2
{
	vec2(float v = 0.0f)
	{
		for (size_t i = 0; i < c_numElements; ++i)
			(*this)[i] = v;
	}
	vec2(float _x, float _y)
		: x(_x)
		, y(_y)
	{ }

	float x, y;

	static const size_t c_numElements = 2;

	float& operator[] (size_t i) { return (&x)[i]; }
	float operator[] (size_t i) const { return (&x)[i]; }

	// xy
	__declspec(property(get = get_xy, put = put_xy)) vec2 xy;
	inline vec2 get_xy() const { return{ x, y }; }
	inline void put_xy(vec2 v) { *this = v.xy; }
	// yx
	__declspec(property(get = get_yx, put = put_yx)) vec2 yx;
	inline vec2 get_yx() const { return{ y, x }; }
	inline void put_yx(vec2 v) { *this = v.yx; }
};

struct vec3
{
	vec3(float v = 0.0f)
	{
		for (size_t i = 0; i < c_numElements; ++i)
			(*this)[i] = v;
	}
	vec3(float _x, float _y, float _z)
		: x(_x)
		, y(_y)
		, z(_z)
	{ }

	float x, y, z;

	static const size_t c_numElements = 3;

	float& operator[] (size_t i) { return (&x)[i]; }
	float operator[] (size_t i) const { return (&x)[i]; }

	// xy
	__declspec(property(get = get_xy, put = put_xy)) vec2 xy;
	inline vec2 get_xy() const { return{ x, y }; }
	inline void put_xy(vec2 v) { this->x = v.x;  this->y = v.y; }
	// yx
	__declspec(property(get = get_yx, put = put_yx)) vec2 yx;
	inline vec2 get_yx() const { return{ y, x }; }
	inline void put_yx(vec2 v) { this->y = v.x;  this->x = v.y; }
	// xz
	__declspec(property(get = get_xz, put = put_xz)) vec2 xz;
	inline vec2 get_xz() const { return{ x, z }; }
	inline void put_xz(vec2 v) { this->x = v.x;  this->z = v.y; }
	// zx
	__declspec(property(get = get_zx, put = put_zx)) vec2 zx;
	inline vec2 get_zx() const { return{ z, x }; }
	inline void put_zx(vec2 v) { this->z = v.x;  this->x = v.y; }
};

struct vec4
{
	vec4(float v = 0.0f)
	{
		for (size_t i = 0; i < c_numElements; ++i)
			(*this)[i] = v;
	}
	vec4(const vec2& v2, float _z, float _w)
		: x(v2.x)
		, y(v2.y)
		, z(_z)
		, w(_w)
	{ }
	vec4(const vec3& v3, float _w)
		: x(v3.x)
		, y(v3.y)
		, z(v3.z)
		, w(w)
	{ }
	vec4(float _x, float _y, float _z, float _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{ }

	float x, y, z, w;

	static const size_t c_numElements = 4;

	float& operator[] (size_t i) { return (&x)[i]; }
	float operator[] (size_t i) const { return (&x)[i]; }

	// xy
	__declspec(property(get = get_xy, put = put_xy)) vec2 xy;
	inline vec2 get_xy() const { return{ x, y }; }
	inline void put_xy(vec2 v) { this->x = v.x;  this->y = v.y; }
};

//-------------------------------------------------------------------------------------
// Vec vs Vec operations
//-------------------------------------------------------------------------------------
template<typename T>
T operator + (const T& A, const T& B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] + B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator - (const T& A, const T& B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] - B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator -= (T& A, const T& B)
{
	// Do the operation
	for (size_t i = 0; i < T::c_numElements; ++i)
		A[i] -= B[i];
	return A;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator * (const T& A, const T& B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] * B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator / (const T& A, const T& B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] / B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
// Vec vs Float operations
//-------------------------------------------------------------------------------------
template<typename T>
T operator + (const T& A, float B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] + B;
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator - (const T& A, float B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] - B;
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator * (const T& A, float B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] * B;
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator *= (T& A, float B)
{
	// Do the operation
	for (size_t i = 0; i < T::c_numElements; ++i)
		A[i] *= B;
	return A;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator / (const T& A, float B)
{
	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] / B;
	return ret;
}

//-------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------
template<typename T>
float dot (const T& A, const T& B)
{
	// Do the operation
	float ret = 0.0;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret += A[i] * B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
float length(const T& V)
{
	// Do the operation
	float length = 0.0;
	for (size_t i = 0; i < T::c_numElements; ++i)
		length += V[i] * V[i];
	length = sqrt(length);
	return length;
}

//-------------------------------------------------------------------------------------
template<typename T>
T normalize(const T& V)
{
	// Do the operation
	return V / length(V);
}

//-------------------------------------------------------------------------------------
template<typename T>
T fract(const T& A)
{
	// Do the operation
	return mod(A, 1.0f);
}

//-------------------------------------------------------------------------------------
template<typename T>
T mod (const T& A, float modulus)
{
	// Do the operation
	T ret(0.0f);
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = mod(A[i], modulus);
	return ret;
}

//-------------------------------------------------------------------------------------
inline float clamp(float value, float min, float max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

//-------------------------------------------------------------------------------------
inline float min (float a, float b)
{
	return a < b ? a : b;
}

//-------------------------------------------------------------------------------------
inline float step (float threshold, float value)
{
	return value >= threshold ? 1.0f : 0.0f;
}

//-------------------------------------------------------------------------------------
inline float mod (float value, float modulus)
{
	float ret = std::fmodf(value, modulus);
	if (ret < 0.0f)
		ret += modulus;
	return ret;
}

//-------------------------------------------------------------------------------------
inline vec3 cross (const vec3& a, const vec3& b)
{
	return vec3
	(
		a[1] * b[2] - a[2] * b[1],
		a[2] * b[0] - a[0] * b[2],
		a[0] * b[1] - a[1] * b[0]
	);
}

//-------------------------------------------------------------------------------------
extern float iGlobalTime;
extern vec3 iResolution;
extern vec4 iMouse;

extern void mainImage(vec4& fragColor, vec2 fragCoord);