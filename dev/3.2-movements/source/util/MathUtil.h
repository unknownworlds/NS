//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: MathUtil.h $
// $Date: 2002/07/23 16:53:46 $
//
//-------------------------------------------------------------------------------
// $Log: MathUtil.h,v $
// Revision 1.6  2002/07/23 16:53:46  Flayra
// - Added VectorDistance2D, added document headers
//
//===============================================================================
#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "types.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

void AngleMatrix(const float* angles, float matrix[3][4]);
double sqrt(double x);
float Length(const float *v);
void CreateOrthoNormalBasis(float inZAxis[3], float outXAxis[3], float outYAxis[3]);

/**
 * Transforms a vector by an orthonormal basis.
 */
void TransformVector(const float v[3], const float xAxis[3], const float yAxis[3], const float zAxis[3], float result[3]);
int RoundIntToNearestIncrementOf(int inValue, int inIncrement);
void RotateValuesByVector(int32& ioX, int32& ioY, int32& ioZ, /*const float* inBaseVector,*/ const float* inVector);
void RotateFloatValuesByVector(float& ioX, float& ioY, float& ioZ, /*const float* inBaseVector,*/ const float* inVector);
float WrapFloat(float inValue, float inMin, float inMax);
void VectorAngles( const float *forward, float *angles );
float VectorNormalize (float *v);
void VectorInverse ( float *v );
void VectorScale (const float *in, float scale, float *out);
void VectorMA (const float *veca, float scale, const float *vecb, float *vecc);
bool IsVectorBetweenBoundingVectors(const float* inOrigin, const float* inRay, const float* inVecOne, const float* inVecTwo);
void VectorRotate (const float* in1, const float in2[3][4], float* out);
double VectorDistance(const float* in1, const float* in2);
double VectorDistance2D(const float* in1, const float* in2);

// Added by mmcguire.
/**
 * Compute the Euler angles which represent the orientation defined by the
 * three basis vectors. This is the mathematical inverse of the AngleVectors
 * function.
 */
void VectorsToAngles(const float forward[3], const float right[3], const float up[3], float angles[3]);

const float kFloatTolerance								= 0.001f;

#endif