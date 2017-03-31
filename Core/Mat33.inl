
#include "Mat33.h"

inline Vec3 operator*(const Mat33 & lhs, const Vec3 & rhs)
{
	Vec3 ret;
	ret.x = lhs.m_data[0]*rhs.x + lhs.m_data[1]*rhs.y + lhs.m_data[2]*rhs.z;
	ret.y = lhs.m_data[3]*rhs.x + lhs.m_data[4]*rhs.y + lhs.m_data[5]*rhs.z;
	ret.z = lhs.m_data[6]*rhs.x + lhs.m_data[7]*rhs.y + lhs.m_data[8]*rhs.z;

	return ret;
}

inline Mat33 operator*(const Mat33 & lhs, const Mat33 & rhs)
{
	Mat33 ret;

	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			float sum = 0;
			for (int i = 0; i < 3; i++)
			{
				sum += lhs.m_data[r*3+i] * rhs.m_data[i*3+c];
			}
			ret.m_data[r*3+c] = sum;
		}
	}

	return ret;
}

inline float Mat33::Determinant() const
{
	float ret = 0.0f;
	ret += m_data[0*3+0] * m_data[1*3+1] * m_data[2*3+2];
	ret += m_data[0*3+1] * m_data[1*3+2] * m_data[2*3+0];
	ret += m_data[0*3+2] * m_data[1*3+0] * m_data[2*3+1];

	ret -= m_data[0*3+0] * m_data[1*3+2] * m_data[2*3+1];
	ret -= m_data[0*3+1] * m_data[1*3+0] * m_data[2*3+2];
	ret -= m_data[0*3+2] * m_data[1*3+1] * m_data[2*3+0];

	return ret;
}

inline Mat33 Mat33::Transpose(const Mat33 & src)
{
	Mat33 ret;
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			ret.m_data[3*r+c] = src.m_data[3*c+r];
		}
	}
	return ret;
}



//http://stackoverflow.com/questions/983999/simple-3x3-matrix-inverse-code-c
inline Mat33 Mat33::Inverse(const Mat33 & src)
{
	Mat33 dst;
	float determinant =    +src.m_data[0*3+0]*(src.m_data[1*3+1]*src.m_data[2*3+2]-src.m_data[2*3+1]*src.m_data[1*3+2])
							-src.m_data[0*3+1]*(src.m_data[1*3+0]*src.m_data[2*3+2]-src.m_data[1*3+2]*src.m_data[2*3+0])
							+src.m_data[0*3+2]*(src.m_data[1*3+0]*src.m_data[2*3+1]-src.m_data[1*3+1]*src.m_data[2*3+0]);
	float invdet = 1/determinant;
	dst.m_data[0*3+0] =  (src.m_data[1*3+1]*src.m_data[2*3+2]-src.m_data[2*3+1]*src.m_data[1*3+2])*invdet;
	dst.m_data[1*3+0] = -(src.m_data[0*3+1]*src.m_data[2*3+2]-src.m_data[0*3+2]*src.m_data[2*3+1])*invdet;
	dst.m_data[2*3+0] =  (src.m_data[0*3+1]*src.m_data[1*3+2]-src.m_data[0*3+2]*src.m_data[1*3+1])*invdet;
	dst.m_data[0*3+1] = -(src.m_data[1*3+0]*src.m_data[2*3+2]-src.m_data[1*3+2]*src.m_data[2*3+0])*invdet;
	dst.m_data[1*3+1] =  (src.m_data[0*3+0]*src.m_data[2*3+2]-src.m_data[0*3+2]*src.m_data[2*3+0])*invdet;
	dst.m_data[2*3+1] = -(src.m_data[0*3+0]*src.m_data[1*3+2]-src.m_data[1*3+0]*src.m_data[0*3+2])*invdet;
	dst.m_data[0*3+2] =  (src.m_data[1*3+0]*src.m_data[2*3+1]-src.m_data[2*3+0]*src.m_data[1*3+1])*invdet;
	dst.m_data[1*3+2] = -(src.m_data[0*3+0]*src.m_data[2*3+1]-src.m_data[2*3+0]*src.m_data[0*3+1])*invdet;
	dst.m_data[2*3+2] =  (src.m_data[0*3+0]*src.m_data[1*3+1]-src.m_data[1*3+0]*src.m_data[0*3+1])*invdet;

	return dst;
}

inline Mat33::Mat33()
{
	InitZero();
}

inline Mat33::Mat33(const float srcData[9])
{
	for (int i = 0; i < 9; i++)
		m_data[i] = srcData[i];
}

inline Mat33::Mat33(const float srcData[3][3])
{
	for (int i = 0; i < 9; i++)
		m_data[i] = srcData[i/3][i%3];
}

inline void Mat33::InitZero()
{
	for (int i = 0; i < 9; i++)
		m_data[i] = 0.0f;
}

inline void Mat33::InitIdentity()
{
	for (int i = 0; i < 9; i++)
		m_data[i] = ((i%3)==(i/3)) ? 1.0f : 0.0f;
}


inline Mat33 Mat33::RotationAroundAxisL(Vec3 N, float cosA, float sinA)
{
	float c = cosA;
	float s = sinA;
	float t = 1.0f-c;

	float x = N.x;
	float y = N.y;
	float z = N.z;

	Mat33 ret;

	ret.m_data[0] = t*x*x + c  ;
	ret.m_data[1] = t*x*y - s*z;
	ret.m_data[2] = t*x*z + s*y;
	ret.m_data[3] = t*x*y + s*z;
	ret.m_data[4] = t*y*y + c  ;
	ret.m_data[5] = t*y*z - s*x;
	ret.m_data[6] = t*x*z - s*y;
	ret.m_data[7] = t*y*z + s*x;
	ret.m_data[8] = t*z*z + c  ;

	return ret;
}

inline Mat33 Mat33::RotationAroundAxisR(Vec3 N, float cosA, float sinA)
{
	float c = cosA;
	float s = sinA;
	float t = 1.0f-c;

	float x = N.x;
	float y = N.y;
	float z = N.z;

	Mat33 ret;

	ret.m_data[0] = t*x*x + c  ;
	ret.m_data[1] = t*x*y + s*z;
	ret.m_data[2] = t*x*z - s*y;
	ret.m_data[3] = t*x*y - s*z;
	ret.m_data[4] = t*y*y + c  ;
	ret.m_data[5] = t*y*z + s*x;
	ret.m_data[6] = t*x*z + s*y;
	ret.m_data[7] = t*y*z - s*x;
	ret.m_data[8] = t*z*z + c  ;

	return ret;
}

