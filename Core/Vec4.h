#ifndef _VEC4_H_
#define _VEC4_H_

#include "CoreHelpers.h"
#include "Vec3.h"

struct Vec4
{
	Vec4() restrict(amp) restrict(cpu)
	{
		x = y = z = w = 0;
	}

	Vec4(float val) restrict(amp) restrict(cpu)
	{
		x = y = z = w = val;
	}

	Vec4(float _x, float _y, float _z, float _w) restrict(amp) restrict(cpu)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	float x, y, z, w;

	static Vec4 Min(const Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
	{
		Vec4 ret;
		ret.x = MinFloat(lhs.x,rhs.x);
		ret.y = MinFloat(lhs.y,rhs.y);
		ret.z = MinFloat(lhs.z,rhs.z);
		ret.w = MinFloat(lhs.w,rhs.w);

		return ret;
	}

	static Vec4 Max(const Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
	{
		Vec4 ret;
		ret.x = MaxFloat(lhs.x,rhs.x);
		ret.y = MaxFloat(lhs.y,rhs.y);
		ret.z = MaxFloat(lhs.z,rhs.z);
		ret.w = MaxFloat(lhs.w,rhs.w);

		return ret;
	}

	Vec3 GetVec3() const
	{
		return Vec3(x,y,z);
	}

	static float Dot(const Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
	{
		return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w;
	}
};

inline Vec4 operator*(const Vec4 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs.x*rhs,lhs.y*rhs,lhs.z*rhs,lhs.w*rhs);
}

inline Vec4 operator*(float lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs*rhs.x,lhs*rhs.y,lhs*rhs.z,lhs*rhs.w);
}

inline Vec4 operator*(const Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs.x*rhs.x,lhs.y*rhs.y,lhs.z*rhs.z,lhs.w*rhs.w);
}


inline Vec4 operator+(const Vec4 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs.x+rhs,lhs.y+rhs,lhs.z+rhs,lhs.w+rhs);
}

inline Vec4 operator+(const Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs.x+rhs.x,lhs.y+rhs.y,lhs.z+rhs.z,lhs.w+rhs.w);
}

inline Vec4 operator-(const Vec4 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs.x-rhs,lhs.y-rhs,lhs.z-rhs,lhs.w-rhs);
}

inline Vec4 operator-(const Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
{
	return Vec4(lhs.x-rhs.x,lhs.y-rhs.y,lhs.z-rhs.z,lhs.w-rhs.w);
}

inline Vec4 & operator+=(Vec4 & lhs, const Vec4 & rhs) restrict(amp) restrict(cpu)
{
	lhs = lhs + rhs;
	return lhs;
}


#endif

