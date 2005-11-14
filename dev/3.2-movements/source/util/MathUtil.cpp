//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: MathUtil.cpp $
// $Date: 2002/07/25 16:59:03 $
//
//-------------------------------------------------------------------------------
// $Log: MathUtil.cpp,v $
// Revision 1.7  2002/07/25 16:59:03  flayra
// -Linux changes
//
// Revision 1.6  2002/07/23 16:53:46  Flayra
// - Added VectorDistance2D, added document headers
//
//===============================================================================
#include <cmath>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

//#include "hud.h"
//#include "cl_util.h"
#include <string.h>
#include "nowarnings.h"
#include "util/MathUtil.h"
#include "common/vec_op.h"
#include "common/mathlib.h"

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

// Ignore "double to float possible loss of data" warning
#pragma warning (disable: 4244)

double sqrt(double x);

bool FindCollisionPointOnPlane(const float* inOrigin, const float* inRayVector, const float* inPlaneABCD, float* outPoint)
{
	bool theSuccess = false;
	
	// Solve for parametric t
	float thePlaneA = inPlaneABCD[0];
	float thePlaneB = inPlaneABCD[1];
	float thePlaneC = inPlaneABCD[2];
	float thePlaneD = inPlaneABCD[3];

	float theDenom = (thePlaneA*inRayVector[0] + thePlaneB*inRayVector[1] + thePlaneC*inRayVector[2]);
	if(fabs(theDenom) > kFloatTolerance)
	{
		float theT = -(thePlaneA*inOrigin[0] + thePlaneB*inOrigin[1] + thePlaneC*inOrigin[2] + thePlaneD)/theDenom;
		
		// Now we have t, solve for the endpoint
		outPoint[0] = inOrigin[0] + theT*inRayVector[0];
		outPoint[1] = inOrigin[1] + theT*inRayVector[1];
		outPoint[2] = inOrigin[2] + theT*inRayVector[2];
		theSuccess = true;
	}
	
	//	float theA = inPlaneABCD[0];
	//	float theB = inPlaneABCD[1];
	//	float theC = inPlaneABCD[2];
	//	float theD = inPlaneABCD[3];
	//
	//	float theDenom = (theA*inRayVector[0] + theB*inRayVector[1] + theC*inRayVector[2]);
	//	if(fabs(theDenom) > kFloatTolerance)
	//	{
	//		float theT = -1*(theA*inOrigin[0] + theB*inOrigin[1] + theC*inOrigin[2] + theD)/theDenom;
	//		outPoint[0] = inOrigin[0] + inRayVector[0]*theT;
	//		outPoint[1] = inOrigin[1] + inRayVector[1]*theT;
	//		outPoint[2] = inOrigin[2] + inRayVector[2]*theT;
	//		theSuccess = true;
	//	}
	
	return theSuccess;
}

bool IsVectorBetweenBoundingVectors(const float* inOrigin, const float* inRay, const float* inVecOne, const float* inVecTwo)
{
	bool theSuccess = false;
	
	// The plane normal is opposite to our view
	float thePlaneABCD[4];
	thePlaneABCD[0] = 0;//inRay[0];
	thePlaneABCD[1] = 0;//inRay[1];
	thePlaneABCD[2] = 1;//inRay[2];
	thePlaneABCD[3] = 0;

	// Put plane far away by plugging in far away point (ax + by + cz + d = 0)
//	float theT = 5000;
//	float thePoint[3];
//	thePoint[0] = inOrigin[0] + theT*thePlaneABCD[0];
//	thePoint[1] = inOrigin[1] + theT*thePlaneABCD[1];
//	thePoint[2] = inOrigin[2] + theT*thePlaneABCD[2];
//
//	// Find plane D using this point
//	thePlaneABCD[3] = -(thePlaneABCD[0]*thePoint[0] + thePlaneABCD[1]*thePoint[1] + thePlaneABCD[2]*thePoint[2]);

	// Solve for each vector hitting the plane
	float theVecPoint[3];
	float theVecOnePoint[3];
	float theVecTwoPoint[3];
	
	// If they all hit the plane
	if(FindCollisionPointOnPlane(inOrigin, inRay, thePlaneABCD, theVecPoint))
	{
		if(FindCollisionPointOnPlane(inOrigin, inVecOne, thePlaneABCD, theVecOnePoint))
		{
			if(FindCollisionPointOnPlane(inOrigin, inVecTwo, thePlaneABCD, theVecTwoPoint))
			{
				// Get the collision point for each solution
				float theMaxX = max(theVecOnePoint[0], theVecTwoPoint[0]);
				float theMaxY = max(theVecOnePoint[1], theVecTwoPoint[1]);
				float theMaxZ = max(theVecOnePoint[2], theVecTwoPoint[2]);
				
				float theMinX = min(theVecOnePoint[0], theVecTwoPoint[0]);
				float theMinY = min(theVecOnePoint[1], theVecTwoPoint[1]);
				float theMinZ = min(theVecOnePoint[2], theVecTwoPoint[2]);
				
				// If it is between them, the vector is between them
				float theVecX = theVecPoint[0];
				float theVecY = theVecPoint[1];
				float theVecZ = theVecPoint[2];
				if(	(theVecX >= (theMinX - kFloatTolerance)) && (theVecX <= (theMaxX + kFloatTolerance)) && 
					(theVecY >= (theMinY - kFloatTolerance)) && (theVecY <= (theMaxY + kFloatTolerance)) && 
					(theVecZ >= (theMinZ - kFloatTolerance)) && (theVecZ <= (theMaxZ + kFloatTolerance)))
				{
					theSuccess = true;
				}
			}
		}
	}
	
	return theSuccess;
}

void AngleMatrix(const float* angles, float matrix[3][4])
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[2] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);
	
	// matrix = (Z * Y) * X
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

float Length(const float *v)
{
	int		i;
	float	length;
	
	length = 0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = sqrt (length);		// FIXME
	
	return length;
}

void RotateFloatValuesByVector(float& ioX, float& ioY, float& ioZ, /*const float* inBaseVector,*/ const float* inVector)
{
	// Get rotation vector
	//float theBaseRotationAngles[3];
	//VectorAngles((float*)inBaseVector, theBaseRotationAngles);
	
	// Get rotation vector
	float theRotationAngles[3];
	VectorAngles((float*)inVector, theRotationAngles);

	// Subtract out frame of reference
	//theRotationAngles[0] -= theBaseRotationAngles[0];
	//theRotationAngles[1] -= theBaseRotationAngles[1];
	//theRotationAngles[2] -= theBaseRotationAngles[2];
	
	// Rotate the first three parameters as a point
	float theSourceValues[3] = {ioX, ioY, ioZ};
				
	float theMatrix[3][4];
	AngleMatrix(theRotationAngles, theMatrix);
	
	float theRotatedValues[3];
	VectorRotate(theSourceValues, theMatrix, theRotatedValues);
	
	ioX = (int)theRotatedValues[0];
	ioY = (int)theRotatedValues[1];
	ioZ = (int)theRotatedValues[2];
}

float WrapFloat(float inValue, float inMin, float inMax)
{
    const float theRange = inMax - inMin;
	
    if (inValue < inMin)
    {
        inValue += floor((inMax - inValue) / theRange) * theRange;
    }    
	
    if (inValue >= inMax)
    {
        inValue -= floor(((inValue - inMin) / theRange)) * theRange;
    }
    
    return inValue;
}

void CreateOrthoNormalBasis(float inZAxis[3], float outXAxis[3], float outYAxis[3])
{
	VectorNormalize(inZAxis);

	// check if in vector is z
	float theUp[3] = { 0, 0, 1 };
	if(fabs(DotProduct(theUp, inZAxis)) >= (1.0 - kFloatTolerance))
	{
		// Use y instead
		theUp[0] = 0;
		theUp[1] = 1;
		theUp[2] = 0;
	}

	CrossProduct(inZAxis, theUp, outXAxis);
	VectorNormalize(outXAxis);

	CrossProduct(outXAxis, inZAxis, outYAxis);
	VectorNormalize(outYAxis);
}

int RoundIntToNearestIncrementOf(int inValue, int inIncrement)
{
	int theValue = inValue;
	if(inIncrement > 0)
	{
		theValue = ((inValue + inIncrement/2)/inIncrement)*inIncrement;
	}
	return theValue;
}

void TransformVector(const float v[3], const float xAxis[3], const float yAxis[3], const float zAxis[3], float result[3])
{
	float temp[3];
	for (int i = 0; i < 3; ++i)
	{
		temp[i] = v[0] * xAxis[i] + v[1] * yAxis[i] + v[2] * zAxis[i];
	}
	for (int j = 0; j < 3; ++j)
	{
		result[j] = temp[j];
	}
}

void RotateValuesByVector(int32& ioX, int32& ioY, int32& ioZ, /*const float* inBaseVector,*/ const float* inVector)
{
	float ioFloatX = ioX;
	float ioFloatY = ioY;
	float ioFloatZ = ioZ;

	RotateFloatValuesByVector(ioFloatX, ioFloatY, ioFloatZ, inVector);

	ioX = (int32)ioFloatX;
	ioY = (int32)ioFloatY;
	ioZ = (int32)ioFloatZ;
}

void VectorAngles( const float *forward, float *angles )
{
	float	tmp, yaw, pitch;
	
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt (forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void VectorInverse ( float *v )
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

void VectorMA (const float *veca, float scale, const float *vecb, float *vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}

float VectorNormalize (float *v)
{
	float	length, ilength;
	
	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt (length);		// FIXME
	
	if (length)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
	
	return length;
	
}

void VectorRotate (const float* in1, const float in2[3][4], float* out)
{
	out[0] = DotProduct(in1, in2[0]);
	out[1] = DotProduct(in1, in2[1]);
	out[2] = DotProduct(in1, in2[2]);
}

void VectorScale (const float *in, float scale, float *out)
{
	out[0] = in[0]*scale;
	out[1] = in[1]*scale;
	out[2] = in[2]*scale;
}

double VectorDistance(const float* in1, const float* in2)
{
	float theXDiff = in1[0] - in2[0];
	float theYDiff = in1[1] - in2[1];
	float theZDiff = in1[2] - in2[2];

	return sqrt(theXDiff*theXDiff + theYDiff*theYDiff + theZDiff*theZDiff);
}

double VectorDistance2D(const float* in1, const float* in2)
{
	float theXDiff = in1[0] - in2[0];
	float theYDiff = in1[1] - in2[1];
	
	return sqrt(theXDiff*theXDiff + theYDiff*theYDiff);
}

// Added by mmcguire.
void VectorsToAngles(const float forward[3], const float right[3], const float up[3], float angles[3])
{
   
    float y,r,p;
    float sy;
    
	if(abs(forward[2]) < 0.9999)
	{
		y = atan2(forward[1], forward[0]);
	    sy = sin(y);
		if (abs(sy) < 0.1)
		{
			p = atan2(-forward[2], forward[0] / cos(y));
		}
		else
		{
			p = atan2(-forward[2], forward[1] / sy);
		}
		r = atan2(-right[2], up[2]);
	}
	else //gimbal lock; best we can do is assume roll = 0 and set pitch = pitch + actual roll
	{
		p = forward[2] > 0 ? -M_PI/2 : M_PI/2;
		y = atan2(right[0],-right[1]);
		r = 0;
	}

    angles[2] = r * (180 / M_PI); // Roll
    angles[0] = p * (180 / M_PI); // Pitch
    angles[1] = y * (180 / M_PI); // Yaw
}
