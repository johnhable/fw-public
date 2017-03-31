#ifndef _VEC2_H_
#define _VEC2_H_

#include "CoreHelpers.h"

struct Vec2
{
	Vec2() restrict(amp) restrict(cpu)
	{
		x = y = 0;
	}

	Vec2(float v) restrict(amp) restrict(cpu)
	{
		x = y = v;
	}

	Vec2(float _x, float _y) restrict(amp) restrict(cpu)
	{
		x = _x;
		y = _y;
	}

	float LengthSqr() const restrict(amp) restrict(cpu)
	{
		return x*x + y*y;
	}

	float Length() const
	{
		return sqrtf(LengthSqr());
	}

	void NormalizeMe()
	{
		float len = Length();
		float invLen = SafeInv(len);
		x *= invLen;
		y *= invLen;
	}

	static float Dist(Vec2 lhs, Vec2 rhs);
	static float DistSqr(Vec2 lhs, Vec2 rhs)
	{
		float offsetX = lhs.x-rhs.x;
		float offsetY = lhs.y-rhs.y;
		return offsetX*offsetX + offsetY*offsetY;
	}

	float x, y;

};


struct IntVec2
{
	IntVec2() restrict(amp) restrict(cpu)
	{
		x = 0;
		y = 0;
	}

	IntVec2(int _x, int _y) restrict(amp) restrict(cpu)
	{
		x = _x;
		y = _y;
	}

	int x;
	int y;
};



inline Vec2 operator*(const Vec2 & lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x*rhs.x,lhs.y*rhs.y);
}

inline Vec2 operator*(float lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs*rhs.x,lhs*rhs.y);
}

inline Vec2 operator*(const Vec2 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x*rhs,lhs.y*rhs);
}

inline Vec2 operator+(const Vec2 & lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x+rhs.x,lhs.y+rhs.y);
}

inline Vec2 operator+(float lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs+rhs.x,lhs+rhs.y);
}

inline Vec2 operator+(const Vec2 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x+rhs,lhs.y+rhs);
}

inline Vec2 operator/(const Vec2 & lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x/rhs.x,lhs.y/rhs.y);
}

inline Vec2 operator/(float lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs/rhs.x,lhs/rhs.y);
}

inline Vec2 operator/(const Vec2 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x/rhs,lhs.y/rhs);
}

inline Vec2 operator-(const Vec2 & lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x-rhs.x,lhs.y-rhs.y);
}

inline Vec2 operator-(float lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs-rhs.x,lhs-rhs.y);
}

inline Vec2 operator-(const Vec2 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec2(lhs.x-rhs,lhs.y-rhs);
}

inline Vec2 & operator+=(Vec2 & lhs, const Vec2 & rhs) restrict(amp) restrict(cpu)
{
	lhs = lhs+rhs;
	return lhs;
}

inline float Vec2::Dist(Vec2 lhs, Vec2 rhs)
{
	Vec2 offset = lhs-rhs;
	return offset.Length();
}



inline IntVec2 FloatVecToInt(Vec2 src)
{
	IntVec2 ret;
	ret.x = (int)src.x;
	ret.y = (int)src.y;
	return ret;
}

inline Vec2 IntVecToFloat (IntVec2 src)
{
	Vec2 ret;
	ret.x = (float)src.x;
	ret.y = (float)src.y;
	return ret;
}



#endif



