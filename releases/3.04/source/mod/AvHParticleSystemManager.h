//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParticleSystemManager.h $
// $Date: 2002/05/23 02:33:20 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleSystemManager.h,v $
// Revision 1.10  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_PSMANAGER_H
#define AVH_PSMANAGER_H

#include "util/nowarnings.h"
#include "types.h"
#include "mod/AvHParticleSystem.h"

class AvHParticleSystemManager;

class QueuedParticleSystem
{
public:
	friend class AvHParticleSystemManager;

	QueuedParticleSystem(uint32 inTemplateIndex, float inTime, int inParentEntityIndex, ParticleSystemHandle inHandle) 
	{
		this->Init();
		this->mTemplateIndex = inTemplateIndex;
		this->mTimeQueued = inTime;
		this->mParentEntityIndex = inParentEntityIndex;
		this->mHandle = inHandle;
		this->mHasNormal = false;
	}

	QueuedParticleSystem(uint32 inTemplateIndex, float inTime, vec3_t inOrigin)
	{ 
		this->Init();
		this->mTemplateIndex = inTemplateIndex;
		this->mTimeQueued = inTime;
		this->mOrigin[0] = inOrigin[0];
		this->mOrigin[1] = inOrigin[1];
		this->mOrigin[2] = inOrigin[2];
		this->mHasNormal = false;
	}

	void SetNormal(const vec3_t& inNormal)
	{
		this->mHasNormal = true;
		this->mNormal.x = inNormal.x;
		this->mNormal.y = inNormal.y;
		this->mNormal.z = inNormal.z;
	}

	int GetParentEntityIndex() const
	{
		return this->mParentEntityIndex;
	}
	
private:
	void Init()
	{
		this->mTemplateIndex = 0;
		this->mHandle = 0;
		this->mParentEntityIndex = -1;
		memset(this->mOrigin, 0, sizeof(this->mOrigin));
		this->mTimeQueued = 0;
	}

	uint32					mTemplateIndex;
	int						mParentEntityIndex;
	ParticleSystemHandle	mHandle;
	vec3_t					mOrigin;
	float					mTimeQueued;
	bool					mHasNormal;
	vec3_t					mNormal;
};

class AvHParticleSystemManager
{
public:

	void								CreateParticleSystem(uint32 inTemplateIndex, int inParentEntityIndex, ParticleSystemHandle inHandle = 0);

	void								CreateParticleSystem(uint32 inTemplateIndex, vec3_t inOrigin, vec3_t* inNormal = NULL);

	bool								CreateParticleSystem(const string& inTemplateName, vec3_t inOrigin, vec3_t* inNormal = NULL);

	void								CreateParticleSystemIfNotCreated(int inIndex, int inTemplateIndex, int inHandle);

	void								DestroyParticleSystem(ParticleSystemHandle inHandle);

	void								DestroyParticleSystemIfNotDestroyed(int inIndex, ParticleSystemHandle inHandle);

	void								Draw(const pVector &inView);

	AvHParticleSystem*					GetParticleSystem(ParticleSystemHandle inHandle);

	int									GetNumberParticleSystems();

#ifdef AVH_CLIENT
	int									GetNumberVisibleParticleSystems();
#endif

	static AvHParticleSystemManager*	Instance();

	void								MarkParticleSystemForDeletion(int inIndex, ParticleSystemHandle inHandle);

	void								ReloadFromTemplates();

	void								Reset();

	bool								SetParticleSystemCustomData(uint16 inCustomData, ParticleSystemHandle inHandle);
	
#ifdef AVH_CLIENT
	int									GetNumVisibleParticleSystems() const;
	void								SetParticleSystemGenerationEntityExtents(vec3_t& inMin, vec3_t& inMax, ParticleSystemHandle inHandle);
	void								SetParticleSystemPosition(vec3_t& inPosition, ParticleSystemHandle inHandle);
	void								SetParticleSystemVisibility(bool inVisibility, ParticleSystemHandle inHandle);
#endif
	
	void								Start();
	
	void								Update(double inTime);
	
private:
	
	
	AvHParticleSystemManager();
	
	void								Init();
	
	bool								InternalCreateParticleSystem(const QueuedParticleSystem& inParticleSystem);
	
	bool								InternalDestroyParticleSystem(ParticleSystemHandle inHandle);
	
	float								GetTime(void) const;
	
	void								ProcessQueuedList();
	
	void								QueueParticleSystem(const QueuedParticleSystem& inParticleSystem);
	
	static AvHParticleSystemManager*	sInstance;
	
	typedef vector<AvHParticleSystem>	ParticleSystemList;
	ParticleSystemList					mParticleSystemList;
	
	bool								mStarted;
	bool								mReloadFromTemplates;
	
	float								mTimePassed;
	float								mUpdatedTime;
	
	typedef vector<QueuedParticleSystem>	QueuedCreateList;
	QueuedCreateList						mQueuedCreateList;
	
	typedef vector<ParticleSystemHandle>	QueuedDestroyList;
	QueuedDestroyList						mQueuedDestroyList;
	
	typedef vector<int>					ParticleEntityList;
	ParticleEntityList					mParticleEntities;
	
};

#endif