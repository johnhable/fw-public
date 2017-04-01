#include "Mat44.h"

#include "Mat33.h"

void Mat44::InitZero()
{
	for (int i = 0; i < 16; i++)
		m_data[i] = 0.0f;
}

void Mat44::InitIdentity()
{
	for (int i = 0; i < 16; i++)
	{
		m_data[i] = (i/4 == i%4) ? 1.0f : 0.0f;
	}
}

// return lhs + scale*rhs
Mat44 Mat44::MulAdd(const Mat44 & lhs, const Mat44 & rhs, float scale)
{
	Mat44 ret;
	for (int i = 0; i < 16; i++)
	{
		ret.m_data[i] = lhs.m_data[i]   + rhs.m_data[i]*scale;
	}
	return ret;
}



Mat44 Mat44::Lerp(const Mat44 & lhs, const Mat44 & rhs, float t)
{
	Mat44 ret;
	for (int i = 0; i < 16; i++)
	{
		ret.m_data[i] = lhs.m_data[i] * (1.0f-t) + rhs.m_data[i] * t;
	}

	return ret;
}



Mat44 Mat44::MulMatMat(const Mat44 & lhs, const Mat44 & rhs)
{
	Mat44 dst;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			float sum = 0;
			for (int i = 0; i < 4; i++)
			{
				sum += lhs.At(r,i) * rhs.At(i,c);
			}

			dst.At(r,c) = sum;
		}
	}

	return dst;
}

Vec4 Mat44::MulMatVec(const Mat44 & lhs, const Vec4 & rhs)
{
	Vec4 ret;

	float data[4];
	for (int r = 0; r < 4; r++)
	{
		float sum = 0.0f;
		sum += lhs.At(r,0) * rhs.x;
		sum += lhs.At(r,1) * rhs.y;
		sum += lhs.At(r,2) * rhs.z;
		sum += lhs.At(r,3) * rhs.w;

		data[r] = sum;
	}

	ret.x = data[0];
	ret.y = data[1];
	ret.z = data[2];
	ret.w = data[3];

	return ret;
}

Vec3 Mat44::MulMatVecUpper3x3(const Mat44 & lhs, const Vec3 & rhs)
{
	Vec3 ret;
	ret.x = lhs.At(0,0) * rhs.x + lhs.At(0,1) * rhs.y + lhs.At(0,2) * rhs.z;
	ret.y = lhs.At(1,0) * rhs.x + lhs.At(1,1) * rhs.y + lhs.At(1,2) * rhs.z;
	ret.z = lhs.At(2,0) * rhs.x + lhs.At(2,1) * rhs.y + lhs.At(2,2) * rhs.z;
	return ret;
}

Vec3 Mat44::MulMatVecUpper3x3Transpose(const Mat44 & lhs, const Vec3 & rhs)
{
	Vec3 ret;
	ret.x = lhs.At(0,0) * rhs.x + lhs.At(1,0) * rhs.y + lhs.At(2,0) * rhs.z;
	ret.y = lhs.At(0,1) * rhs.x + lhs.At(1,1) * rhs.y + lhs.At(2,1) * rhs.z;
	ret.z = lhs.At(0,2) * rhs.x + lhs.At(1,2) * rhs.y + lhs.At(2,2) * rhs.z;
	return ret;
}

Vec3 Mat44::MulMatVecUpper3x3AndRenormalize(const Mat44 & lhs, const Vec3 & rhs)
{
	Vec3 ret;
	ret = MulMatVecUpper3x3(lhs,rhs);
	ret.NormalizeMe();
	return ret;
}

Vec3 Mat44::MulMatPointAndDivide(const Mat44 & lhs, const Vec3 & rhs)
{
	// impled 1.0f on w
	Vec3 ret;
	ret.x = lhs.At(0,0) * rhs.x + lhs.At(0,1) * rhs.y + lhs.At(0,2) * rhs.z + lhs.At(0,3);
	ret.y = lhs.At(1,0) * rhs.x + lhs.At(1,1) * rhs.y + lhs.At(1,2) * rhs.z + lhs.At(1,3);
	ret.z = lhs.At(2,0) * rhs.x + lhs.At(2,1) * rhs.y + lhs.At(2,2) * rhs.z + lhs.At(2,3);
	float w = lhs.At(3,0) * rhs.x + lhs.At(3,1) * rhs.y + lhs.At(3,2) * rhs.z + lhs.At(3,3);
	float invW = SafeInv(w);

	ret = ret * invW;
	return ret;
}

Vec3 Mat44::MulMatPointAndDivideTranspose(const Mat44 & srcLhs, const Vec3 & rhs)
{
	Mat44 lhs = srcLhs.GetTranspose();
	Vec3 ret = MulMatPointAndDivide(lhs,rhs);
	return ret;
}


bool Mat44::Invert(Mat44 & dst, const Mat44 & src)
{
	float a00 = src.m_data[0*4+0];
	float a01 = src.m_data[0*4+1];
	float a02 = src.m_data[0*4+2];
	float a03 = src.m_data[0*4+3];
	float a10 = src.m_data[1*4+0];
	float a11 = src.m_data[1*4+1];
	float a12 = src.m_data[1*4+2];
	float a13 = src.m_data[1*4+3];
	float a20 = src.m_data[2*4+0];
	float a21 = src.m_data[2*4+1];
	float a22 = src.m_data[2*4+2];
	float a23 = src.m_data[2*4+3];
	float a30 = src.m_data[3*4+0];
	float a31 = src.m_data[3*4+1];
	float a32 = src.m_data[3*4+2];
	float a33 = src.m_data[3*4+3];

	float d0 = a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a11*a23*a32 - a12*a21*a33 - a13*a22*a31;
	float d1 = a10*a23*a32 + a12*a20*a33 + a13*a22*a30 - a10*a22*a33 - a12*a23*a30 - a13*a20*a32;
	float d2 = a10*a21*a33 + a11*a23*a30 + a13*a20*a31 - a10*a23*a31 - a11*a20*a33 - a13*a21*a30;
	float d3 = a10*a22*a31 + a11*a20*a32 + a12*a21*a30 - a10*a21*a32 - a11*a22*a30 - a12*a20*a31;
	float det = a00*d0 + a01*d1 + a02*d2 + a03*d3;

	if (det == 0.0f)
		return false;

	dst.m_data[0*4+0] = (a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a11*a23*a32 - a12*a21*a33 - a13*a22*a31)/det;
	dst.m_data[0*4+1] = (a01*a23*a32 + a02*a21*a33 + a03*a22*a31 - a01*a22*a33 - a02*a23*a31 - a03*a21*a32)/det;
	dst.m_data[0*4+2] = (a01*a12*a33 + a02*a13*a31 + a03*a11*a32 - a01*a13*a32 - a02*a11*a33 - a03*a12*a31)/det;
	dst.m_data[0*4+3] = (a01*a13*a22 + a02*a11*a23 + a03*a12*a21 - a01*a12*a23 - a02*a13*a21 - a03*a11*a22)/det;
	dst.m_data[1*4+0] = (a10*a23*a32 + a12*a20*a33 + a13*a22*a30 - a10*a22*a33 - a12*a23*a30 - a13*a20*a32)/det;
	dst.m_data[1*4+1] = (a00*a22*a33 + a02*a23*a30 + a03*a20*a32 - a00*a23*a32 - a02*a20*a33 - a03*a22*a30)/det;
	dst.m_data[1*4+2] = (a00*a13*a32 + a02*a10*a33 + a03*a12*a30 - a00*a12*a33 - a02*a13*a30 - a03*a10*a32)/det;
	dst.m_data[1*4+3] = (a00*a12*a23 + a02*a13*a20 + a03*a10*a22 - a00*a13*a22 - a02*a10*a23 - a03*a12*a20)/det;
	dst.m_data[2*4+0] = (a10*a21*a33 + a11*a23*a30 + a13*a20*a31 - a10*a23*a31 - a11*a20*a33 - a13*a21*a30)/det;
	dst.m_data[2*4+1] = (a00*a23*a31 + a01*a20*a33 + a03*a21*a30 - a00*a21*a33 - a01*a23*a30 - a03*a20*a31)/det;
	dst.m_data[2*4+2] = (a00*a11*a33 + a01*a13*a30 + a03*a10*a31 - a00*a13*a31 - a01*a10*a33 - a03*a11*a30)/det;
	dst.m_data[2*4+3] = (a00*a13*a21 + a01*a10*a23 + a03*a11*a20 - a00*a11*a23 - a01*a13*a20 - a03*a10*a21)/det;
	dst.m_data[3*4+0] = (a10*a22*a31 + a11*a20*a32 + a12*a21*a30 - a10*a21*a32 - a11*a22*a30 - a12*a20*a31)/det;
	dst.m_data[3*4+1] = (a00*a21*a32 + a01*a22*a30 + a02*a20*a31 - a00*a22*a31 - a01*a20*a32 - a02*a21*a30)/det;
	dst.m_data[3*4+2] = (a00*a12*a31 + a01*a10*a32 + a02*a11*a30 - a00*a11*a32 - a01*a12*a30 - a02*a10*a31)/det;
	dst.m_data[3*4+3] = (a00*a11*a22 + a01*a12*a20 + a02*a10*a21 - a00*a12*a21 - a01*a10*a22 - a02*a11*a20)/det;

	return true;
}

Mat44 Mat44::XformFromRt(const Mat33 & R, const Vec3 & t)
{
	Mat44 ret;
	ret.InitIdentity();
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			ret.At(r,c) = R.m_data[r*3+c];

	ret.At(0,3) = t.x;
	ret.At(1,3) = t.y;
	ret.At(2,3) = t.z;

	return ret;
}

void Mat44::RtFromXform(Mat33 & R, Vec3 & t, const Mat44 & M)
{
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			R.m_data[r*3+c] = M.At(r,c);

	t.x = M.At(0,3);
	t.y = M.At(1,3);
	t.z = M.At(2,3);
}

static const float s_degToRad = 3.14159f/180.0f;

Mat44 Mat44::RotationX(float angleDeg)
{
	Mat44 ret = RotationArbitrary(angleDeg,Vec3(1,0,0));
	return ret;
}

Mat44 Mat44::RotationY(float angleDeg)
{
	Mat44 ret = RotationArbitrary(angleDeg,Vec3(0,1,0));
	return ret;
}

Mat44 Mat44::RotationZ(float angleDeg)
{
	Mat44 ret = RotationArbitrary(angleDeg,Vec3(0,0,1));
	return ret;
}

Mat44 Mat44::RotationArbitrary(float angleDeg, Vec3 N)
{
	Mat33 rot;
	float rad = angleDeg * s_degToRad;
	float c = cosf(rad);
	float s = sinf(rad);
	rot = Mat33::RotationAroundAxisL(N,c,s);

	Mat44 ret;
	ret.InitIdentity();
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			ret.At(r,c) = rot.m_data[r*3+c];

	return ret;
}

Mat44 Mat44::TranslationFlip(Vec3 p)
{
	Mat44 ret;
	ret.InitIdentity();
	ret.At(3,0) = p.x;
	ret.At(3,1) = p.y;
	ret.At(3,2) = p.z;
	return ret;
}

Mat44 Mat44::TranslationStd(Vec3 p)
{
	Mat44 ret;
	ret.InitIdentity();
	ret.At(0,3) = p.x;
	ret.At(1,3) = p.y;
	ret.At(2,3) = p.z;
	return ret;
}

Mat44 Mat44::Scale(float s)
{
	Mat44 ret;
	ret.InitIdentity();
	ret.At(0,0) = s;
	ret.At(1,1) = s;
	ret.At(2,2) = s;
	return ret;
}

Mat44 Mat44::ScaleVec(Vec3 s)
{
	Mat44 ret;
	ret.InitIdentity();
	ret.At(0,0) = s.x;
	ret.At(1,1) = s.y;
	ret.At(2,2) = s.z;
	return ret;
}


Mat44 Mat44::LoadXformFromFile(std::string fileName)
{
	FILE * fin = fopen(fileName.c_str(),"r");
	ASSERT_ALWAYS(fin != nullptr);

	Mat44 ret;
	for (int i = 0; i < 16; i++)
	{
		int parsed = fscanf(fin,"%f",&ret.m_data[i]);
		ASSERT_ALWAYS(parsed == 1);
	}

	fclose(fin);
	fin = nullptr;

	return ret;
}


