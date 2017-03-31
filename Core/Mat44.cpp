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




// http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix/1148405#1148405
static bool my_gluInvertMatrix(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

bool Mat44::Invert(Mat44 & dst, const Mat44 & src)
{
	bool bRet = my_gluInvertMatrix(&src.m_data[0],&dst.m_data[0]);
	return bRet;
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

#pragma optimize("",off)

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


