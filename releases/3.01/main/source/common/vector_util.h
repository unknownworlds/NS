#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H

#include "common/vec_op.h"

float Length(const float *v);
void VectorMA (const float *veca, float scale, const float *vecb, float *vecc);
void VectorScale (const float *in, float scale, float *out);
float VectorNormalize (float *v);
void VectorInverse ( float *v );

extern vec3_t vec3_origin;

#endif