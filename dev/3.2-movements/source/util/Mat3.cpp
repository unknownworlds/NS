#include "Mat3.h"
#include "MathUtil.h"
#include "common/mathlib.h"

Mat3::Mat3()
{
}


Mat3::Mat3(const float angles[3])
{
	SetEulerAngles(angles);
}


float& Mat3::operator()(int r, int c)
{
	return element[c][r];
}


float Mat3::operator()(int r, int c) const
{
	return element[c][r];
}

void Mat3::SetEulerAngles(const float angles[3])
{
	AngleVectors(angles,element[0],element[1],element[2]);
}

void Mat3::TransformVector(float vector[3]) const
{
	float result[3] = {0,0,0};
	float temp[3] = {0,0,0};

	VectorScale(element[0],vector[0],result);
	VectorScale(element[1],vector[1],temp);
	VectorAdd(result,temp,result);
	VectorScale(element[2],vector[2],temp);
	VectorAdd(result,temp,result);
	VectorCopy(result,vector);
}

void Mat3::GetEulerAngles(float angles[3]) const
{
	VectorsToAngles(element[0],element[1],element[2],angles);
}

Mat3 Mat3::Transpose() const
{

	Mat3 result;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			result.element[i][j] = element[j][i];
		}
	}
		
	return result;
}


Mat3 operator*(const Mat3& m1, const Mat3& m2)
{

	Mat3 result;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{

			result(i,j) = 0;

			for (int k = 0; k < 3; ++k)
			{
				result(i,j) += m1(i,k) * m2(k,j);
			}
		}
	}
	
	return result;

}