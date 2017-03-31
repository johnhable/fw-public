#pragma once

#include "CoreHelpers.h"
#include "Mat33.h"
#include "Mat44.h"
#include "Vec3.h"

struct Quat
{
	Quat()
	{
		x = y = z = w = 0;
	}

	Quat(float val)
	{
		x = y = z = w = val;
	}

	Quat(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	static Mat44 ToMat44(Quat quat);
	static Quat FromMat44(Mat44 m);

	static Mat33 ToMat33(Quat quat);
	static Quat FromMat33(Mat33 m);

	// matches maya
	enum eRotationOrder
	{
		kRotationOrder_XYZ,
		kRotationOrder_YZX,
		kRotationOrder_ZXY,
		kRotationOrder_XZY,
		kRotationOrder_YXZ,
		kRotationOrder_ZYX,
		kRotationOrder_Num
	};

	static Vec3 ToEuler(Quat quat, eRotationOrder order);
	static Quat FromEuler(Vec3 v, eRotationOrder order);

	
	float x, y, z, w;
};


