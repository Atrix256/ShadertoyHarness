#pragma once

#include <assert.h>
#include <cmath>

#define in // nothing to do for in

//-------------------------------------------------------------------------------------
template <size_t NumElements>
struct vecN
{
	static const size_t c_numElements = NumElements;
	virtual float& operator[] (size_t i) = 0;
	virtual float operator[] (size_t i) const = 0;

	vecN<NumElements>* GetBasePointer() { return this; }
};

//-------------------------------------------------------------------------------------
struct vec2 : public vecN<2>
{
	vec2(float _x, float _y)
		: x(_x)
		, y(_y)
	{ }

	vec2(float v = 0.0f)
	{
		for (size_t i = 0; i < c_numElements; ++i)
			(*this)[i] = v;
	}

	float x;
	float y;

	virtual float& operator[] (size_t i)
	{
		assert(i < c_numElements);
		switch (i)
		{
			case 0: return x;
			case 1: return y;
		}
		assert(false);
		return x;
	}

	virtual float operator[] (size_t i) const
	{
		return (*const_cast<vec2*>(this))[i];
	}
};

//-------------------------------------------------------------------------------------
struct vec3 : public vecN<3>
{
	vec3(float _x, float _y, float _z)
		: x(_x)
		, y(_y)
		, z(_z)
	{ }

	vec3(float v = 0.0f)
	{
		for (size_t i = 0; i < c_numElements; ++i)
			(*this)[i] = v;
	}

	float x;
	float y;
	float z;

	virtual float& operator[] (size_t i)
	{
		assert(i < c_numElements);
		switch (i)
		{
			case 0: return x;
			case 1: return y;
			case 2: return z;
		}
		assert(false);
		return x;
	}

	virtual float operator[] (size_t i) const
	{
		return (*const_cast<vec3*>(this))[i];
	}

	vec2 xy()
	{
		return vec2(x,y);
	}
};

//-------------------------------------------------------------------------------------
struct vec4 : public vecN<4>
{
	vec4(float _x, float _y, float _z, float _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{ }

	vec4(vec3 v3, float f)
	{
		float fz = v3[0];
		x = v3[0];
		y = v3[1];
		z = v3[2];
		w = f;
	}

	vec4(float v = 0.0f)
	{
		for (size_t i = 0; i < c_numElements; ++i)
			(*this)[i] = v;
	}

	float x;
	float y;
	float z;
	float w;

	virtual float& operator[] (size_t i)
	{
		assert(i < c_numElements);
		switch (i)
		{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			case 3: return w;
		}
		assert(false);
		return x;
	}

	virtual float operator[] (size_t i) const
	{
		return (*const_cast<vec4*>(this))[i];
	}

	vec2 xy()
	{
		return vec2(x, y);
	}
};

//-------------------------------------------------------------------------------------
template<typename T>
T operator - (const T& A, const T& B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] - B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator + (const T& A, const T& B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] + B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator / (const T& A, const T& B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] / B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator / (const T& A, float B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] / B;
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator * (const T& A, const T& B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] * B[i];
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
T operator + (const T& A, float B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

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
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

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
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	T ret;
	for (size_t i = 0; i < T::c_numElements; ++i)
		ret[i] = A[i] * B;
	return ret;
}

//-------------------------------------------------------------------------------------
template<typename T>
float dot (const T& A, const T& B)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

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
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &V;

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
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &V;

	// Do the operation
	return V / length(V);
}

//-------------------------------------------------------------------------------------
template<typename T>
T fract(const T& A)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

	// Do the operation
	return mod(A, 1.0f);
}

//-------------------------------------------------------------------------------------
template<typename T>
T mod (const T& A, float modulus)
{
	// Make sure T is a derivation of vecN
	const vecN<T::c_numElements>* vec = &A;

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
extern vec2 iResolution;
extern vec4 iMouse;

extern void mainImage(vec4& fragColor, in vec2 fragCoord);