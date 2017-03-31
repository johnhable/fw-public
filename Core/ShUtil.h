#ifndef _SH_UTIL_H_
#define _SH_UTIL_H_

#include "CoreHelpers.h"
#include "Vec3.h"


struct GreySh3
{
	GreySh3()
	{
		for (int i = 0; i < 9; i++)
			m_coefs[i] = 0;
	}

	float m_coefs[9];
};

struct ColorSh3;

class ShUtil
{
public:
	static float DotProduct(const GreySh3 & lhs, const GreySh3 & rhs);
	static GreySh3 ProjectNormal(Vec3 N);

	static GreySh3 RotateSh(const GreySh3 & lhs, const float mat[3][3]);

	// assumes input in radians
	static void MakeRotationMatrixRadians(float dst[3][3], float thetaX, float thetaY, float thetaZ);
	static void MakeRotationMatrixDegrees(float dst[3][3], float thetaX, float thetaY, float thetaZ);

};

struct ColorSh3
{
	ColorSh3()
	{
		for (int i = 0; i < 9; i++)
			m_coefs[i] = Vec3(0,0,0);
	}

	void FromColorAndSh(const GreySh3 & lhs, Vec3 & rhs)
	{
		for (int i = 0; i < 9; i++)
			m_coefs[i] = lhs.m_coefs[i] * rhs;
	}

	Vec3 ProjectAndDot(Vec3 N)
	{
		N.NormalizeMe();
		GreySh3 greySh = ShUtil::ProjectNormal(N);
		Vec3 sum = 0;
		for (int i = 0; i < 9; i++)
			sum += m_coefs[i] * greySh.m_coefs[i];

		return sum;
	}

	Vec3 m_coefs[9];
};




#endif
