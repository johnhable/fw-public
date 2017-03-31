#ifndef _MAT44_H_
#define _MAT44_H_

#include "CoreHelpers.h"
#include "Vec3.h"
#include "Vec4.h"


struct Mat33;

struct Mat44
{
	float m_data[16];

	float & At(int r, int c)
	{
		return m_data[r*4+c];
	}

	float At(int r, int c) const
	{
		return m_data[r*4+c];
	}


	void InitZero();
	void InitIdentity();

	Mat44 GetTranspose() const
	{
		Mat44 ret;
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
				ret.At(r,c) = At(c,r);
		
		return ret;
	}



	// return lhs + scale*rhs
	static Mat44 MulAdd(const Mat44 & lhs, const Mat44 & rhs, float scale);

	static Mat44 Lerp(const Mat44 & lhs, const Mat44 & rhs, float t);

	static Mat44 MulMatMat(const Mat44 & lhs, const Mat44 & rhs);
	static Vec4 MulMatVec(const Mat44 & lhs, const Vec4 & rhs);
	static Vec3 MulMatVecUpper3x3(const Mat44 & lhs, const Vec3 & rhs);
	static Vec3 MulMatVecUpper3x3Transpose(const Mat44 & lhs, const Vec3 & rhs);
	static Vec3 MulMatVecUpper3x3AndRenormalize(const Mat44 & lhs, const Vec3 & rhs);
	static Vec3 MulMatPointAndDivide(const Mat44 & lhs, const Vec3 & rhs);
	static Vec3 MulMatPointAndDivideTranspose(const Mat44 & lhs, const Vec3 & rhs);

	static bool Invert(Mat44 & dst, const Mat44 & src);

	static Mat44 RotationX(float angleDeg);
	static Mat44 RotationY(float angleDeg);
	static Mat44 RotationZ(float angleDeg);
	static Mat44 RotationArbitrary(float angleDeg, Vec3 N);

	static Mat44 TranslationFlip(Vec3 p);
	static Mat44 TranslationStd(Vec3 p);
	static Mat44 Scale(float s);
	static Mat44 ScaleVec(Vec3 s);


	static Mat44 XformFromRt(const Mat33 & R, const Vec3 & t);
	static void RtFromXform(Mat33 & R, Vec3 & t, const Mat44 & M);

	static Mat44 LoadXformFromFile(std::string fileName);

};


inline Mat44 operator*(const Mat44 & lhs, const Mat44 & rhs)
{
	Mat44 ret = Mat44::MulMatMat(lhs,rhs);
	return ret;
}

inline Mat44 operator+(const Mat44 & lhs, const Mat44 & rhs)
{
	Mat44 ret;
	for (int i = 0; i < 16; i++)
		ret.m_data[i] = lhs.m_data[i] + rhs.m_data[i];

	return ret;
}

inline Mat44 operator-(const Mat44 & lhs, const Mat44 & rhs)
{
	Mat44 ret;
	for (int i = 0; i < 16; i++)
		ret.m_data[i] = lhs.m_data[i] - rhs.m_data[i];

	return ret;
}

inline Mat44 operator*(const Mat44 & lhs, float rhs)
{
	Mat44 ret;
	for (int i = 0; i < 16; i++)
		ret.m_data[i] = lhs.m_data[i] * rhs;

	return ret;
}

inline Mat44 operator*(float rhs, const Mat44 & lhs)
{
	Mat44 ret;
	for (int i = 0; i < 16; i++)
		ret.m_data[i] = lhs.m_data[i] * rhs;

	return ret;
}

#endif
