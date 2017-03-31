#ifndef _MAT_3X3_H_
#define _MAT_3X3_H_

#include "CoreHelpers.h"

#include "Vec3.h"

class Mat33
{
public:
	Mat33();
	Mat33(const float srcData[9]);
	Mat33(const float srcData[3][3]);

	void InitZero();
	void InitIdentity();
	float Determinant() const;

	static Mat33 Inverse(const Mat33 & src);
	static Mat33 RotationAroundAxisL(Vec3 N, float cosA, float sinA);
	static Mat33 RotationAroundAxisR(Vec3 N, float cosA, float sinA);
	static Mat33 Transpose(const Mat33 & src);

	float m_data[9];
};

inline Vec3 operator*(const Mat33 & lhs, const Vec3 & rhs);

inline Mat33 operator*(const Mat33 & lhs, const Mat33 & rhs);


#include "Mat33.inl"

#endif
