#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "win_header.h"

template<typename t>
struct Vector3D
{
	t x, y, z;

	Vector3D() = default;

	Vector3D(t a, t b, t c)
	{
		x = a;
		y = b;
		z = c;
	}

	t& operator [] (int i)
	{
		return ((&x)[i]);
	}

	const t& operator [] (int i) const // for const vectors
	{
		return ((&x)[i]);
	}

	Vector3D& operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return (*this);
	}

	Vector3D& operator /= (float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return (*this);
	}

	Vector3D& operator += (const Vector3D& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return (*this);
	}

	Vector3D& operator -= (const Vector3D& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return (*this);
	}
};

template<typename t>
inline Vector3D<t> operator * (const Vector3D<t>& v, real32 s)
{
	return (Vector3D<t>(v.x * s, v.y * s, v.z * s));
}

template<typename t>
inline Vector3D<t> operator / (const Vector3D<t>& v, real32 s)
{
	s = static_cast<t>(1) / s;
	return (Vector3D<t>(v.x * s, v.y * s, v.z * s)); //temporary object
}

template<typename t>
inline Vector3D<t> operator + (const Vector3D<t>& v1, const Vector3D<t>& v2)
{
	return (Vector3D<t>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
}

template<typename t>
inline Vector3D<t> operator - (const Vector3D<t>& v1, const Vector3D<t>& v2)
{
	return (Vector3D<t>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
}

template<typename t>
inline Vector3D<t> operator -(const Vector3D<t>& v)
{
	return (Vector3D<t>(-v.x, -v.y, -v.z));
}

template<typename t>
inline real32 Magnitude(const Vector3D<t>& v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

template<typename t>
inline Vector3D<t> Normalize(const Vector3D<t>& v)
{
	return (v / Magnitude(v));
}

typedef Vector3D<float> vec3f;
typedef Vector3D<int> vec3i;


#endif
