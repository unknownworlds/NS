#ifndef ANIMATION_UTIL_H
#define ANIMATION_UTIL_H

#include "engine/studio.h"

/**
 *
 */
typedef float NS_Matrix3x4[3][4];

/**
 *
 */
struct NS_AnimationData
{
    NS_Matrix3x4    mMatrix;
	float           mTime;
	float           mFrame;
    float           mFrameRate;
    studiohdr_t*    mModelHeader;
    int             mSequence;
    int             mGaitSequence;
    vec3_t          mMins;
    vec3_t          mMaxs;
};

/**
 * Returns false if the animation data could not be retrieved for the specified entity.
 */
bool NS_GetEntityAnimationData(int inEntityIndex, NS_AnimationData& outAnimationData);

/**
 * outBoneMatrix array should have as many elements as there are bones in the model (MAXSTUDIOBONES)
 */
void NS_GetBoneMatrices(const NS_AnimationData& inAnimationData, float time, NS_Matrix3x4 outBoneMatrix[]);

#endif