#ifndef _VEC3_H_
#define _VEC3_H_

#include "CoreHelpers.h"

struct Vec3
{
	Vec3() restrict(amp) restrict(cpu)
	{
		x = y = z = 0;
	}

	Vec3(float val) restrict(amp) restrict(cpu)
	{
		x = y = z = val;
	}

	Vec3(float _x, float _y, float _z) restrict(amp) restrict(cpu)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	union
	{
		float m_data[3];

		struct
		{
			float x, y, z;
		};
	};

	static inline Vec3 Cross(const Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
	{
		Vec3 ret;
		ret.x = lhs.y * rhs.z - lhs.z * rhs.y;
		ret.y = lhs.z * rhs.x - lhs.x * rhs.z;
		ret.z = lhs.x * rhs.y - lhs.y * rhs.x;
		return ret;
	}

	static inline float Dot(const Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
	{
		return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
	}

	inline float LengthSqr() const restrict(amp) restrict(cpu)
	{
		return Dot(*this,*this);
	}

	inline float Length() const
	{
		return sqrtf(LengthSqr());
	}

	inline float InvLength() const
	{
		return SafeInv(Length());
	}

	inline void NormalizeMe()
	{
		float invLen = InvLength();
		x *= invLen;
		y *= invLen;
		z *= invLen;
	}

	static inline Vec3 Normalize(const Vec3 & lhs)
	{
		Vec3 ret = lhs;
		ret.NormalizeMe();
		return ret;
	}

	static float Dist(const Vec3 & lhs, const Vec3 & rhs)
	{
		Vec3 sub;
		sub.x = lhs.x-rhs.x;
		sub.y = lhs.y-rhs.y;
		sub.z = lhs.z-rhs.z;
		return sub.Length();
	}

	static float DistSqr(const Vec3 & lhs, const Vec3 & rhs)
	{
		Vec3 sub;
		sub.x = lhs.x-rhs.x;
		sub.y = lhs.y-rhs.y;
		sub.z = lhs.z-rhs.z;
		return sub.LengthSqr();
	}

	static inline Vec3 ProjectPointIntoPlane(const Vec3 & p, const Vec3 & n, const Vec3 & x);

	static inline bool IsEqual(const Vec3 & lhs, const Vec3 & rhs)
	{
		if (lhs.x == rhs.x &&
			lhs.y == rhs.y &&
			lhs.z == rhs.z)
		{
			return true;
		}

		return false;
	}

};



inline Vec3 operator*(const Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x*rhs.x,lhs.y*rhs.y,lhs.z*rhs.z);
}

inline Vec3 operator*(float lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs*rhs.x,lhs*rhs.y,lhs*rhs.z);
}

inline Vec3 operator*(const Vec3 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x*rhs,lhs.y*rhs,lhs.z*rhs);
}

inline Vec3 operator+(const Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x+rhs.x,lhs.y+rhs.y,lhs.z+rhs.z);
}

inline Vec3 operator+(float lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs+rhs.x,lhs+rhs.y,lhs+rhs.z);
}

inline Vec3 operator+(const Vec3 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x+rhs,lhs.y+rhs,lhs.z+rhs);
}

inline Vec3 operator/(const Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x/rhs.x,lhs.y/rhs.y,lhs.z/rhs.z);
}

inline Vec3 operator/(float lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs/rhs.x,lhs/rhs.y,lhs/rhs.z);
}

inline Vec3 operator/(const Vec3 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x/rhs,lhs.y/rhs,lhs.z/rhs);
}

inline Vec3 operator-(const Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x-rhs.x,lhs.y-rhs.y,lhs.z-rhs.z);
}

inline Vec3 operator-(float lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs-rhs.x,lhs-rhs.y,lhs-rhs.z);
}

inline Vec3 operator-(const Vec3 & lhs, float rhs) restrict(amp) restrict(cpu)
{
	return Vec3(lhs.x-rhs,lhs.y-rhs,lhs.z-rhs);
}

inline Vec3 & operator+=(Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	lhs = lhs+rhs;
	return lhs;
}

inline Vec3 & operator-=(Vec3 & lhs, const Vec3 & rhs) restrict(amp) restrict(cpu)
{
	lhs = lhs-rhs;
	return lhs;
}

inline Vec3 Vec3::ProjectPointIntoPlane(const Vec3 & p, const Vec3 & n, const Vec3 & x)
{
	return x - Dot(x-p,n)*n;
}



#endif
