//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParticleSystem.h $
// $Date: 2002/10/16 01:03:04 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleSystem.h,v $
// Revision 1.11  2002/10/16 01:03:04  Flayra
// - Added new particle flag for particles to lie flat on ground ("faceup"), needed for scanner sweep
//
// Revision 1.10  2002/07/10 14:43:40  Flayra
// - Visibility fixes (too many PSs drawing, now they expire when not visible for awhile), removed cl_particleinfo drawing
//
//===============================================================================
#ifndef AVH_PARTICLESYSTEM_H
#define AVH_PARTICLESYSTEM_H

#include "util/nowarnings.h"
#include "types.h"

#ifdef AVH_CLIENT
#include "common/triangleapi.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#elif defined AVH_SERVER
  #include "dlls/extdll.h"
#endif

#include <p_vector.h>
#include "mod/AvHParticleTemplate.h"

typedef uint32 ParticleSystemHandle;

class AvHParticleTemplate;

// Represents an instantiated, active particle system
class AvHParticleSystem
{
public:
							AvHParticleSystem(AvHParticleTemplate* inTemplate, uint32 inIndex);

	void					Draw(const pVector &inView);

	ParticleSystemHandle	GetHandle() const;

	bool					GetHasGeneratedParticles() const;

	bool					GetIsMarkedForDeletion(void) const;

	#ifdef AVH_CLIENT
	float					GetLastTimeVisibilityLastSetTrue() const;
	
	bool					GetIsVisible() const;
	#endif

	int						GetNumberOfParticles(void) const;

	float					GetParticleSystemLifetime() const;

	float					GetParticleLifetime() const;

	uint32					GetTemplateIndex() const;

	float					GetTimeCreated() const;

	void					Kill();

	void					LoadFromTemplate(AvHParticleTemplate* inTemplate);

	bool					GetGenerationEntity(int& outIndex) const;
	//void					SetEntity(int inEntity);

	void					SetCustomData(uint16 inCustomData);

	void					SetGenerationEntityExtents(vec3_t& inMin, vec3_t& inMax);

	void					SetHandle(ParticleSystemHandle inHandle);

	void					SetIsMarkedForDeletion(void);

	#ifdef AVH_CLIENT
	void					SetIsVisible(bool inVisibilityState, float inTime);
	#endif

	void					SetNormal(const vec3_t& inOrigin);
	
	void					SetParticleSystemLifetime(float inLifetime);

	void					SetParticleLifetime(float inLifetime);

	void					SetPosition(vec3_t inOrigin);

	void					SetTimeCreated(float inTime);

	void					Update(float inTime);

	#ifdef AVH_SERVER
	void					UpdatePhysics(entvars_t* inPEV);
	#endif
	
private:

	void					Collide(float inTime);

	#ifdef AVH_CLIENT
	void					DrawGroup(const pVector &inView);

	void					LoadSpriteIfNeeded(AvHParticleTemplate* inTemplate);
	#endif

	void					GenerateParticles(int inNumberParticles);

	float					GetParticleScale() const;

	void					SetStartingVelocity();
	
	void					UpdateFromWorld();

	void					UpdateFirst();

	uint32					mGroup;

	uint32					mTemplateIndex;

	ParticleSystemHandle	mHandle;

	pVector					mBaseEntityPos;

	#ifdef AVH_CLIENT
	HSPRITE					mSprite;
	bool					mIsVisible;
	float					mLastTimeVisibilitySetTrue;
	#endif

	#ifdef AVH_SERVER
	float					mTimeOfLastPhysicsUpdate;
	float					mPhysicsUpdateTime;
	#endif

	//int						mEntity;

	//vec3_t					mOrigin;

	float					mTimeCreated;

	float					mParticleSystemLifetime;

	float					mParticleLifetime;

	bool					mIsMarkedForDeletion;

	int						mNumSpriteFrames;

	float					mAnimationSpeed;

	float					mParticleScaling;

	int						mRenderMode;

	bool					mFadeIn;
	bool					mFadeOut;
	bool					mUseWorldGravity;
	bool					mUseDensity;
	bool					mUseTrisNotQuads;
	bool					mMinimizeEdges;
	bool					mConstrainPitch;
	bool					mFaceUp;
	bool					mCollide;

	int						mParticleSystemIndexToGenerate;

	ShapeType				mGenerationShape;
	ParticleParams			mGenerationParams;
	int						mGenerationEntityIndex;
	int						mGenerationRate;
	float					mGenerationEntityParam;
	float					mGenerationEntityVolumeFactor;
	float					mParticleSize;
	float					mMaxAlpha;
	bool					mHasGeneratedParticles;
	PSVector				mParticleGravity;

	int						mGroupMaxParticles;
	int						mMaxParticles;

	vec3_t					mGenerationEntityAbsMin;
	vec3_t					mGenerationEntityAbsMax;

	ShapeType				mStartingVelocityShape;
	ParticleParams			mStartingVelocityParams;

	bool					mHasNormal;
	vec3_t					mNormal;

	bool					mUpdateFirst;

	uint16					mCustomData;

};

#endif