//-------------------------------------------------------------------------------------------
// File: MathHelper.h
//
// This class is used for math calculations
//-------------------------------------------------------------------------------------------

#ifndef MATHHELPER_H_
#define MATHHELPER_H_

#include <Windows.h>
#include <xnamath.h>

class MathHelper
{
public:
	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

	template <typename T>
	static T getMin(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template <typename T>
	static T getMax(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	static float RadiansToDegrees(float radians)
	{
		return radians * degreesPerRadian;
	}

	static float DegreesToRadians(float degrees)
	{
		return degrees * radiansPerDegree;
	}

	static const float pi;
	static const float infinity;
	static const float degreesPerRadian;
	static const float radiansPerDegree;
};

#endif