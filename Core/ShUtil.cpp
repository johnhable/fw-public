#include "ShUtil.h"

//#include "LinearAlgebraUtil.h"

const static float s_c0 = 0.28209479177f; // 1 / (2 * sqrt(pi))
const static float s_c1 = 0.4886025119f; // sqrt(3)/(2*sqrt(pi))
const static float s_c2 = 1.09254843059f; // sqrt(15)/(2*sqrt(pi))



const static float s_c3 = 0.94617469575; // (3*sqrt(5))/(4*sqrt(pi))
const static float s_c4 = -0.31539156525;// (-sqrt(5))/(4*sqrt(pi))
const static float s_c5 = 0.54627421529; // (sqrt(15))/(4*sqrt(pi))

const static float s_c_scale = 1.0/0.91529123286551084;
const static float s_c_scale_inv = 0.91529123286551084;

const static float s_rc2 = 1.5853309190550713*s_c_scale;
const static float s_c4_div_c3 = s_c4/s_c3;
const static float s_c4_div_c3_x2 = (s_c4/s_c3)*2.0;

const static float s_scale_dst2 = s_c3 * s_c_scale_inv;
const static float s_scale_dst4 = s_c5 * s_c_scale_inv;



const static float s_inv_sqrt_2 = 0.70710678118f;

float ShUtil::DotProduct(const GreySh3 & lhs, const GreySh3 & rhs)
{
	float sum = 0;
	for (int i = 0; i < 9; i++)
	{
		sum += lhs.m_coefs[i] * rhs.m_coefs[i];
	}

	return sum;
}




GreySh3 ShUtil::ProjectNormal(Vec3 N)
{
	GreySh3 ret;
	ret.m_coefs[0] = s_c0;
	ret.m_coefs[1] = -s_c1 * N.y;
	ret.m_coefs[2] = s_c1 * N.z;
	ret.m_coefs[3] = -s_c1 * N.x;
	ret.m_coefs[4] = s_c2*N.x*N.y;
	ret.m_coefs[5] = -s_c2*N.y*N.z;
	ret.m_coefs[6] = s_c3*N.z*N.z + s_c4;
	ret.m_coefs[7] = -s_c2*N.x*N.z;
	ret.m_coefs[8] = s_c5*(N.x*N.x - N.y*N.y);

	return ret;
}

static void ProjectBand1(float dst[3], const float src[3])
{
	dst[0] = -s_c1 * src[1];
	dst[1] = s_c1 * src[2];
	dst[2] = -s_c1 * src[0];
}


static void ProjectBand2(float dst[5], const float src[3])
{
	dst[0] = s_c2*src[0]*src[1];
	dst[1] = -s_c2*src[1]*src[2];
	dst[2] = s_c3*src[2]*src[2] + s_c4;
	dst[3] = -s_c2*src[0]*src[2];
	dst[4] = s_c5*(src[0]*src[0] - src[1]*src[1]);
}

static void MulMatVec3(float dst[3], const float lhs[3][3], const float rhs[3])
{
	for (int r = 0; r < 3; r++)
	{
		float sum = 0.0f;
		for (int c = 0; c < 3; c++)
			sum += lhs[r][c]*rhs[c];
		dst[r] = sum;
	}
}

static void MulMatVec3Flat(float dst[3], const float lhs[9], const float rhs[3])
{
	for (int r = 0; r < 3; r++)
	{
		float sum = 0.0f;
		for (int c = 0; c < 3; c++)
			sum += lhs[r*3+c]*rhs[c];
		dst[r] = sum;
	}
}

static void MulMatTransVec3(float dst[3], const float lhs[3][3], const float rhs[3])
{
	for (int r = 0; r < 3; r++)
	{
		float sum = 0.0f;
		for (int c = 0; c < 3; c++)
			sum += lhs[c][r]*rhs[c];
		dst[r] = sum;
	}
}

static void MulMatVec3(float dst[3], const std::vector < float > lhs, const float rhs[3])
{
	for (int r = 0; r < 3; r++)
	{
		float sum = 0.0f;
		for (int c = 0; c < 3; c++)
			sum += lhs[r*3+c]*rhs[c];
		dst[r] = sum;
	}
}
static void MulMatTransVec3(float dst[3], const std::vector < float > lhs, const float rhs[3])
{
	for (int r = 0; r < 3; r++)
	{
		float sum = 0.0f;
		for (int c = 0; c < 3; c++)
			sum += lhs[c*3+r]*rhs[c];
		dst[r] = sum;
	}
}


static void RotateBand1Opt(float dst[3], const float src[3], const float mat[3][3])
{
	dst[0] = ( mat[1][1])*src[0] + (-mat[1][2])*src[1] + ( mat[1][0])*src[2];
	dst[1] = (-mat[2][1])*src[0] + ( mat[2][2])*src[1] + (-mat[2][0])*src[2];
	dst[2] = ( mat[0][1])*src[0] + (-mat[0][2])*src[1] + ( mat[0][0])*src[2];
}


static void RotateBand1(float dst[3], const float src[3], const float mat[3][3])
{
	// fic rotations!
	ASSERT_ALWAYS(0);


	float r0[3] = {1,0,0};
	float r1[3] = {0,1,0};
	float r2[3] = {0,0,1};

	float data_trans[3][3];

	ProjectBand1(data_trans[0],r0);
	ProjectBand1(data_trans[1],r1);
	ProjectBand1(data_trans[2],r2);

	// make columns
	std::vector < float > matData(9);
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			matData[r*3+c] = data_trans[c][r];
		}
	}

	std::vector < float > matInv(9);

	// LU factorization not included in this library, but you would put it here
	//LinearAlgebraUtil::CalcInverseLUDirect(matInv,matData,3);

	float rot0[3];
	float rot1[3];
	float rot2[3];
	MulMatVec3(rot0,mat,r0);
	MulMatVec3(rot1,mat,r1);
	MulMatVec3(rot2,mat,r2);

	float proj_trans[3][3];
	ProjectBand1(proj_trans[0],rot0);
	ProjectBand1(proj_trans[1],rot1);
	ProjectBand1(proj_trans[2],rot2);


	std::vector < float > projData(9);
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			projData[r*3+c] = proj_trans[c][r];
		}
	}

	float tempVec[3];
	MulMatVec3Flat(tempVec,&matInv[0],src);
	MulMatVec3Flat(dst,&projData[0],tempVec);
}

static void MulMatVec5(float dst[5], const float lhs[25], const float rhs[5])
{
	for (int r = 0; r < 5; r++)
	{
		float sum = 0.0f;
		for (int c = 0; c < 5; c++)
			sum += lhs[r*5+c]*rhs[c];
		dst[r] = sum;
	}
}

static void Rotate3rdOrderScalar(float & d0, float & d1, float & d2, float & d3, float & d4, float & d5, float & d6, float & d7, float & d8,
								 float s0, float s1, float s2, float s3, float s4, float s5, float s6, float s7, float s8,
							  float m00, float m01, float m02,
							  float m10, float m11, float m12,
							  float m20, float m21, float m22)
{
	// fic rotations!
	ASSERT_ALWAYS(0);

	// band 0
	{
		d0 = s0;
	}

	// band 1
	{
		// 9 mul
		d1 = m10*s3 + m11*s1 + m12*s2;
		d2 = m20*s3 + m21*s1 + m22*s2;
		d3 = m00*s3 + m01*s1 + m02*s2;
	}

	// band 2
	{
		static float s_other = 1.73205113f;

		static float c3_div_c2 = 0.866025329f;
		static float c5_div_c2 = .5f;

		static float dst3_rhs = -0.3333333333f;
		static float dst3_rhs_m2 = -.666666666f;

		// 1 mad, 5 add/subtract
		float x0 = (s5 - s7 + s8 + s8);
		float x1 = (s_other*s6 + s4 - s7 + s8);
		float x2 = s4;
		float x3 = s7;
		float x4 = s5;

		// 9 add
		float r2_0 = m00+m01;
		float r2_1 = m10+m11;
		float r2_2 = m20+m21;

		float r3_0 = m00+m02;
		float r3_1 = m10+m12;
		float r3_2 = m20+m22;

		float r4_0 = m01+m02;
		float r4_1 = m11+m12;
		float r4_2 = m21+m22;

		// 55 mul/mad
		d4 =  x0 * m00 * m10;
		d5 =  x0 * m10 * m20;
		d6 =  x0 * (m20 * m20 + dst3_rhs);
		d7 =  x0 * m00 * m20;
		d8 =  x0 * (m00 * m00 - m10 * m10);

		d4 += x1 * m02 * m12;
		d5 += x1 * m12 * m22;
		d6 += x1 * (m22 * m22 + dst3_rhs);
		d7 += x1 * m02 * m22;
		d8 += x1 * (m02 * m02 - m12 * m12);

		d4 += x2 * r2_0 * r2_1;
		d5 += x2 * r2_1 * r2_2;
		d6 += x2 * (r2_2 * r2_2 + dst3_rhs_m2);
		d7 += x2 * r2_0 * r2_2;
		d8 += x2 * (r2_0 * r2_0 - r2_1 * r2_1);

		d4 += x3 * r3_0 * r3_1;
		d5 += x3 * r3_1 * r3_2;
		d6 += x3 * (r3_2 * r3_2 + dst3_rhs_m2);
		d7 += x3 * r3_0 * r3_2;
		d8 += x3 * (r3_0 * r3_0 - r3_1 * r3_1);

		d4 += x4 * r4_0 * r4_1;
		d5 += x4 * r4_1 * r4_2;
		d6 += x4 * (r4_2 * r4_2 + dst3_rhs_m2);
		d7 += x4 * r4_0 * r4_2;
		d8 += x4 * (r4_0 * r4_0 - r4_1 * r4_1);

		// two mul
		d6 *= c3_div_c2;
		d8 *= c5_div_c2;
	}

}

static void RotateBand2Scalar(float dst[5],
							  const float x[5],
							  float m00, float m01, float m02,
							  float m10, float m11, float m12,
							  float m20, float m21, float m22)
{
	// Sparse matrix multiply
	double sh0 =  x[3] + x[4] + x[4] - x[1];
	double sh1 =  x[0] + s_rc2*x[2] +  x[3] + x[4];
	double sh2 =  x[0];
	double sh3 = -x[3];
	double sh4 = -x[1];

	// Rotations.  R0 and R1 just use the raw matrix columns
	double r2x = m00 + m01;
	double r2y = m10 + m11;
	double r2z = m20 + m21;

	double r3x = m00 + m02;
	double r3y = m10 + m12;
	double r3z = m20 + m22;

	double r4x = m01 + m02;
	double r4y = m11 + m12;
	double r4z = m21 + m22;

	// dense matrix multiplication one column at a time
	
	// column 0
	double sh0_x = sh0 * m00;
	double sh0_y = sh0 * m10;
	double d0 = sh0_x * m10;
	double d1 = sh0_y * m20;
	double d2 = sh0 * (m20 * m20 + s_c4_div_c3);
	double d3 = sh0_x * m20;
	double d4 = sh0_x * m00 - sh0_y * m10;
	
	// column 1
	double sh1_x = sh1 * m02;
	double sh1_y = sh1 * m12;
	d0 += sh1_x * m12;
	d1 += sh1_y * m22;
	d2 += sh1 * (m22 * m22 + s_c4_div_c3);
	d3 += sh1_x * m22;
	d4 += sh1_x * m02 - sh1_y * m12;
	
	// column 2
	double sh2_x = sh2 * r2x;
	double sh2_y = sh2 * r2y;
	d0 += sh2_x * r2y;
	d1 += sh2_y * r2z;
	d2 += sh2 * (r2z * r2z + s_c4_div_c3_x2);
	d3 += sh2_x * r2z;
	d4 += sh2_x * r2x - sh2_y * r2y;

	// column 3
	double sh3_x = sh3 * r3x;
	double sh3_y = sh3 * r3y;
	d0 += sh3_x * r3y;
	d1 += sh3_y * r3z;
	d2 += sh3 * (r3z * r3z + s_c4_div_c3_x2);
	d3 += sh3_x * r3z;
	d4 += sh3_x * r3x - sh3_y * r3y;

	// column 4
	double sh4_x = sh4 * r4x;
	double sh4_y = sh4 * r4y;
	d0 += sh4_x * r4y;
	d1 += sh4_y * r4z;
	d2 += sh4 * (r4z * r4z + s_c4_div_c3_x2);
	d3 += sh4_x * r4z;
	d4 += sh4_x * r4x - sh4_y * r4y;

	// extra multipliers
	dst[0] = d0;
	dst[1] = -d1;
	dst[2] = d2 * s_scale_dst2;
	dst[3] = -d3;
	dst[4] = d4 * s_scale_dst4;
}

static void RotateBand2Opt(float dst[5], const float src[5], const float mat[3][3])
{
	RotateBand2Scalar(dst,
					  src,
					  mat[0][0], mat[0][1], mat[0][2],
					  mat[1][0], mat[1][1], mat[1][2],
					  mat[2][0], mat[2][1], mat[2][2]);
}

static void RotateBand2(float dst[5], const float src[5], const float mat[3][3])
{
	float r0[3] = {1,0,0};
	float r1[3] = {0,0,1};
	float r2[3] = {s_inv_sqrt_2,s_inv_sqrt_2,0};
	float r3[3] = {s_inv_sqrt_2,0,s_inv_sqrt_2};
	float r4[3] = {0,s_inv_sqrt_2,s_inv_sqrt_2};

	float data_trans[5][5];

	ProjectBand2(data_trans[0],r0);
	ProjectBand2(data_trans[1],r1);
	ProjectBand2(data_trans[2],r2);
	ProjectBand2(data_trans[3],r3);
	ProjectBand2(data_trans[4],r4);

	// make columns
	std::vector < float > matData(25);
	for (int r = 0; r < 5; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			matData[r*5+c] = data_trans[c][r];
		}
	}

	std::vector < float > matInv(25);

	// LU factorization not included in this library, but you would put it here
	//LinearAlgebraUtil::CalcInverseLUDirect(matInv,matData,5);


	float rot0[3];
	float rot1[3];
	float rot2[3];
	float rot3[3];
	float rot4[3];
	MulMatVec3(rot0,mat,r0);
	MulMatVec3(rot1,mat,r1);
	MulMatVec3(rot2,mat,r2);
	MulMatVec3(rot3,mat,r3);
	MulMatVec3(rot4,mat,r4);

	float proj_trans[5][5];
	ProjectBand2(proj_trans[0],rot0);
	ProjectBand2(proj_trans[1],rot1);
	ProjectBand2(proj_trans[2],rot2);
	ProjectBand2(proj_trans[3],rot3);
	ProjectBand2(proj_trans[4],rot4);


	std::vector < float > projData(25);
	for (int r = 0; r < 5; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			projData[r*5+c] = proj_trans[c][r];
		}
	}

	float tempVec[5];
	MulMatVec5(tempVec,&matInv[0],src);
	MulMatVec5(dst,&projData[0],tempVec);

}

static void MulMatMat3(float dst[3][3], const float lhs[3][3], const float rhs[3][3])
{
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			float sum = 0;
			for (int i = 0; i < 3; i++)
			{
				sum += lhs[r][i] * rhs[i][c];
			}
			dst[r][c] = sum;
		}
	}
}

void ShUtil::MakeRotationMatrixRadians(float dst[3][3], float thetaX, float thetaY, float thetaZ)
{
	float rotX[3][3];
	float rotY[3][3];
	float rotZ[3][3];

	float cx = cosf(thetaX);
	float cy = cosf(thetaY);
	float cz = cosf(thetaZ);

	float sx = sinf(thetaX);
	float sy = sinf(thetaY);
	float sz = sinf(thetaZ);

	memset(rotX,0,sizeof(rotX));
	memset(rotY,0,sizeof(rotY));
	memset(rotZ,0,sizeof(rotZ));

	rotX[0][0] = 1.0f;
	rotX[1][1] = cx;
	rotX[1][2] = -sx;
	rotX[2][1] = sx;
	rotX[2][2] = cx;

	rotY[0][0] = cy;
	rotY[0][2] = sy;
	rotY[1][1] = 1.0f;
	rotY[2][0] = -sy;
	rotY[2][2] = cy;

	rotZ[0][0] = cz;
	rotZ[0][1] = -sz;
	rotZ[1][0] = sz;
	rotZ[1][1] = cz;
	rotZ[2][2] = 1.0f;

	float rotZY[3][3];
	MulMatMat3(rotZY,rotZ,rotY);
	MulMatMat3(dst,rotZY,rotX);

}

static const float s_degToRad = 3.14159f/180.0f;

void ShUtil::MakeRotationMatrixDegrees(float dst[3][3], float thetaX, float thetaY, float thetaZ)
{
	float thetaRadX = s_degToRad * thetaX;
	float thetaRadY = s_degToRad * thetaY;
	float thetaRadZ = s_degToRad * thetaZ;

	MakeRotationMatrixRadians(dst,thetaRadX,thetaRadY,thetaRadZ);
}


GreySh3 ShUtil::RotateSh(const GreySh3 & lhs, const float mat[3][3])
{

	GreySh3 ret;


	Rotate3rdOrderScalar(ret.m_coefs[0],ret.m_coefs[1],ret.m_coefs[2],ret.m_coefs[3],ret.m_coefs[4],ret.m_coefs[5],ret.m_coefs[6],ret.m_coefs[7],ret.m_coefs[8],
						 lhs.m_coefs[0],lhs.m_coefs[1],lhs.m_coefs[2],lhs.m_coefs[3],lhs.m_coefs[4],lhs.m_coefs[5],lhs.m_coefs[6],lhs.m_coefs[7],lhs.m_coefs[8],
						 mat[0][0],mat[0][1],mat[0][2],
						 mat[1][0],mat[1][1],mat[1][2],
						 mat[2][0],mat[2][1],mat[2][2]);

	return ret;
}



