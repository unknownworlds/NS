//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParticleSystemManager.cpp $
// $Date: 2002/10/28 20:36:06 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleSystemManager.cpp,v $
// Revision 1.14  2002/10/28 20:36:06  Flayra
// - Tweaked particle culling time to work with umbra and spores
//
// Revision 1.13  2002/10/24 21:34:46  Flayra
// - Fixes for particle culling again!
//
// Revision 1.12  2002/07/10 14:44:10  Flayra
// - Visibility fixes (too many PSs drawing, now they expire when not visible for awhile)
//
// Revision 1.11  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHParticleSystemManager.h"

#ifdef AVH_CLIENT
  #include "cl_dll/hud.h"
  #include "cl_dll/cl_util.h"

  // Triangle rendering apis are in gEngfuncs.pTriAPI
  #include "const.h"
  #include "entity_state.h"
  #include "cl_entity.h"
  #include "triangleapi.h"
  #include "particles/papi.h"
  #include "mod/AvHParticleTemplateClient.h"
  
#else
  #include "mod/AvHParticleTemplate.h"
  #include "mod/AvHParticleTemplateServer.h"
  #include "dlls/util.h"
#endif

#include "pm_shared/pm_debug.h"

#ifdef AVH_CLIENT
  AvHParticleTemplateListClient		gParticleTemplateList;
  extern DebugEntityListType		gCubeDebugEntities;
#else
  //AvHParticleTemplateList			gParticleTemplateList;
  AvHParticleTemplateListServer		gParticleTemplateList;
#endif

AvHParticleSystemManager*	AvHParticleSystemManager::sInstance = NULL;
const float	kUpdateIncrement = 0.0f;

AvHParticleSystemManager*
AvHParticleSystemManager::Instance()
{
	if(!sInstance)
	{
		sInstance = new AvHParticleSystemManager();
	}
	return sInstance;
}

void AvHParticleSystemManager::Init()
{
	#ifdef AVH_CLIENT
	this->mStarted = false;
	#else
	this->mStarted = true;
	#endif

	this->mReloadFromTemplates = false;
	
	this->mUpdatedTime = 0.0f;
	this->mTimePassed = 0.0f;
}

AvHParticleSystemManager::AvHParticleSystemManager()
{
	this->Init();
}

void
AvHParticleSystemManager::CreateParticleSystem(uint32 inTemplateIndex, int inParentEntityIndex, ParticleSystemHandle inHandle)
{
	this->mQueuedCreateList.push_back(QueuedParticleSystem(inTemplateIndex, this->GetTime(), inParentEntityIndex, inHandle));
}

void
AvHParticleSystemManager::CreateParticleSystem(uint32 inTemplateIndex, vec3_t inOrigin, vec3_t* inNormal)
{
	QueuedParticleSystem theParticleSystem(inTemplateIndex, this->GetTime(), inOrigin);
	if(inNormal)
	{
		theParticleSystem.SetNormal(*inNormal);
	}
 	this->mQueuedCreateList.push_back(theParticleSystem);
}

bool
AvHParticleSystemManager::CreateParticleSystem(const string& inTemplateName, vec3_t inOrigin, vec3_t* inNormal)
{
	bool theSuccess = false;

	// Lookup template by name
	uint32 theTemplateIndex = 0;

	if(::gParticleTemplateList.GetTemplateIndexWithName(inTemplateName, theTemplateIndex))
	{
		this->CreateParticleSystem(theTemplateIndex, inOrigin, inNormal);
		theSuccess = true;
	}
	
	return theSuccess;
}

void 
AvHParticleSystemManager::CreateParticleSystemIfNotCreated(int inIndex, int inTemplateIndex, int inHandle)
{
	bool theEntityHasParticles = false;
	
	// Look in list of entities, see if we've created one for this entity
	for(ParticleEntityList::iterator theIter = this->mParticleEntities.begin(); theIter != this->mParticleEntities.end(); theIter++)
	{
		if(*theIter == inIndex)
		{
			theEntityHasParticles = true;
			break;
		}
	}
	
	if(!theEntityHasParticles)
	{
		// If not, create one
		this->CreateParticleSystem(inTemplateIndex, inIndex, (ParticleSystemHandle)inHandle);
		
		// Add entity to list
		this->mParticleEntities.push_back(inIndex);
	}
}

void 
AvHParticleSystemManager::DestroyParticleSystemIfNotDestroyed(int inIndex, ParticleSystemHandle inHandle)
{
	// Look in list of entities, see if we've created one for this entity
	for(ParticleEntityList::iterator theIter = this->mParticleEntities.begin(); theIter != this->mParticleEntities.end(); theIter++)
	{
		if(*theIter == inIndex)
		{
			// If so, destroy this particle system.  Note that this particle system could have already
			// expired, that's ok.  The list of particle entities just needs to be kept up to date.
			this->DestroyParticleSystem(inHandle);
			
			// Remove entity from list
			this->mParticleEntities.erase(theIter);
			
			break;
		}
	}
}

void
AvHParticleSystemManager::DestroyParticleSystem(ParticleSystemHandle inHandle)
{
	InternalDestroyParticleSystem(inHandle);
}

void
AvHParticleSystemManager::Draw(const pVector& inView)
{
	if(this->mStarted)
	{
		ParticleSystemList::iterator theIterator;
		for(theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
		{
			theIterator->Draw(inView);
		}
	}
}

AvHParticleSystem*
AvHParticleSystemManager::GetParticleSystem(ParticleSystemHandle inHandle)
{
	AvHParticleSystem* theParticleSystem = NULL;

	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			theParticleSystem = &*theIterator;
			break;
		}
	}

	return theParticleSystem;
}

int
AvHParticleSystemManager::GetNumberParticleSystems()
{
	return this->mParticleSystemList.size();
}

#ifdef AVH_CLIENT
int	AvHParticleSystemManager::GetNumberVisibleParticleSystems()
{
 	int theNumVisibleSystems = 0;

	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetIsVisible())
		{
			theNumVisibleSystems++;
		}
	}
	return theNumVisibleSystems;
}
#endif

float
AvHParticleSystemManager::GetTime(void) const
{
	float theTime = 0;
	
#ifdef AVH_CLIENT
	theTime = gEngfuncs.GetClientTime();
#else
	theTime = gpGlobals->time;
#endif
	
	return theTime;
}

bool 
AvHParticleSystemManager::InternalDestroyParticleSystem(ParticleSystemHandle inHandle)
{
	bool theDestroyed = false;
	
	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			// Tell it to kill its particles and clean up
			theIterator->Kill();

			// Remove it from the list
			theIterator = this->mParticleSystemList.erase(theIterator);

			// We're successful and done
			theDestroyed = true;
			break;
		}
	}

	return theDestroyed;
}

bool
AvHParticleSystemManager::InternalCreateParticleSystem(const QueuedParticleSystem& inParticleSystem)
{
	bool theSuccess = false;
	
	// add new particle system to list
	AvHParticleTemplate* theTemplate = ::gParticleTemplateList.GetTemplateAtIndex(inParticleSystem.mTemplateIndex);
	if(theTemplate)
	{
		AvHParticleSystem theNewParticleSystem(theTemplate, inParticleSystem.mTemplateIndex);
		
		// set it's template handle
		int theHandle = inParticleSystem.mHandle;
		if(theHandle)
		{
			theNewParticleSystem.SetHandle(theHandle);
		}
		else
		{
			// TODO: Check to be sure it has a lifetime, because it won't be able to be deleted
		}
		
		// Bind it to an entity if it was specified
		//if(inParticleSystem.mEntity)
		//{
		//	theNewParticleSystem.SetEntity(inParticleSystem.mEntity);
		//}
		
		theNewParticleSystem.SetPosition(inParticleSystem.mOrigin);
		
		// Set time created
		theNewParticleSystem.SetTimeCreated(this->GetTime());

		// Set normal if specified
		if(inParticleSystem.mHasNormal)
		{
			theNewParticleSystem.SetNormal(inParticleSystem.mNormal);
		}
		
		this->mParticleSystemList.push_back(theNewParticleSystem);

		#ifdef AVH_CLIENT
		//int theGenerationEntityIndex;
		//if(theNewParticleSystem.GetGenerationEntity(theGenerationEntityIndex))
		//{
		//	gCubeDebugEntities.push_back(theGenerationEntityIndex);
		//}
		//else
		//{
			int theParentEntityIndex = inParticleSystem.GetParentEntityIndex();
			if(theParentEntityIndex > 0)
			{
				gCubeDebugEntities.push_back(theParentEntityIndex);
			}
		//}
		#endif
		
		theSuccess = true;
	}
	
	return theSuccess;
}

void
AvHParticleSystemManager::ProcessQueuedList()
{
	// Process pending creates
	for(QueuedCreateList::iterator theCreateIter = this->mQueuedCreateList.begin(); theCreateIter != this->mQueuedCreateList.end(); /* no increment */)
	{
		// Run through the list, creating each queued particle system.  If a system couldn't be created, we probably
		// don't have that particle template yet, so just skip it for now and try to create it next frame.

		// Add code that detects how long a queued particle system has been in?
		const float kTimeOut = 20.0f;
		float theCurrentTime = this->GetTime();
		
		bool theCreateTimedOut = theCurrentTime - theCreateIter->mTimeQueued > kTimeOut;
		if(theCreateTimedOut)
		{
			//ASSERT(!theCreateTimedOut);
			theCreateIter = this->mQueuedCreateList.erase(theCreateIter);
		}
		else
		{
			bool theCreated = this->InternalCreateParticleSystem(*theCreateIter);
			if(theCreated)
			{
				theCreateIter = this->mQueuedCreateList.erase(theCreateIter);
			}
			else
			{
				theCreateIter++;
			}
		}
	}
	
	// Process pending destroys
	for(QueuedDestroyList::iterator theDestroyIter = this->mQueuedDestroyList.begin(); theDestroyIter != this->mQueuedDestroyList.end(); /* no increment */)
	{
		ParticleSystemHandle theHandle = *theDestroyIter;

		// Try to delete it.  If it hasn't been created yet, keep the pending destruction around
		bool theDestroyed = this->InternalDestroyParticleSystem(theHandle);
		if(theDestroyed)
		{
			theDestroyIter = this->mQueuedDestroyList.erase(theDestroyIter);
		}
		else
		{
			theDestroyIter++;
		}
	}
}

void
AvHParticleSystemManager::QueueParticleSystem(const QueuedParticleSystem& inParticleSystem)
{
	this->mQueuedCreateList.push_back(inParticleSystem);
}

void
AvHParticleSystemManager::ReloadFromTemplates()
{
	this->mReloadFromTemplates = true;
}

void
AvHParticleSystemManager::Reset()
{
	// Clear out alive particle systems 
	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		theIterator->Kill();
	}
	this->mParticleSystemList.clear();
	
	// Clear out queued particle systems
	this->mQueuedCreateList.clear();
	this->mQueuedDestroyList.clear();
	
	// Clear entities we're tracking
	this->mParticleEntities.clear();
	
	// Init list again so it's just like we just built a new one
	this->Init();
}

bool AvHParticleSystemManager::SetParticleSystemCustomData(uint16 inCustomData, ParticleSystemHandle inHandle)
{
	bool theSuccess = false;

	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			theIterator->SetCustomData(inCustomData);
			theSuccess = true;
			break;
		}
	}

	return theSuccess;
}

void
AvHParticleSystemManager::MarkParticleSystemForDeletion(int inIndex, ParticleSystemHandle inHandle)
{
	for(ParticleEntityList::iterator theIter = this->mParticleEntities.begin(); theIter != this->mParticleEntities.end(); theIter++)
	{
		if(*theIter == inIndex)
		{
			// Remove it from the list
			theIter = this->mParticleEntities.erase(theIter);
			break;
		}
	}

	// Mark it for deletion
	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			theIterator->SetIsMarkedForDeletion();
			break;
		}
	}
}

#ifdef AVH_CLIENT
int	AvHParticleSystemManager::GetNumVisibleParticleSystems() const
{
	int theNumVisible = 0;
	
	for(ParticleSystemList::const_iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetIsVisible())
		{
			theNumVisible++;
		}
	}
	
	return theNumVisible;
}

void
AvHParticleSystemManager::SetParticleSystemGenerationEntityExtents(vec3_t& inMin, vec3_t& inMax, ParticleSystemHandle inHandle)
{
	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			theIterator->SetGenerationEntityExtents(inMin, inMax);
			break;
		}
	}
}

void
AvHParticleSystemManager::SetParticleSystemPosition(vec3_t& inPos, ParticleSystemHandle inHandle)
{
	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			theIterator->SetPosition(inPos);
			break;
		}
	}
}

void
AvHParticleSystemManager::SetParticleSystemVisibility(bool inVisibility, ParticleSystemHandle inHandle)
{
	for(ParticleSystemList::iterator theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); theIterator++)
	{
		if(theIterator->GetHandle() == inHandle)
		{
			theIterator->SetIsVisible(inVisibility, this->GetTime());
			break;
		}
	}
}
#endif

void
AvHParticleSystemManager::Start()
{
	if(!this->mStarted)
	{
		this->mStarted = true;
		this->mUpdatedTime = this->mTimePassed = 0.0f;
	}
}

void
AvHParticleSystemManager::Update(double inTime)
{
	if(this->mStarted)
	{
		this->ProcessQueuedList();

		ASSERT(inTime >= 0.0f);

		this->mTimePassed += (float)inTime;
		if(this->mTimePassed - this->mUpdatedTime > kUpdateIncrement)
		{
			float theTimeToUpdate = this->mTimePassed - this->mUpdatedTime;

			// Run through list, updating each one
			ParticleSystemList::iterator theIterator;
			for(theIterator = this->mParticleSystemList.begin(); theIterator != this->mParticleSystemList.end(); /* notice: no increment */ )
			{
				if(this->mReloadFromTemplates)
				{
					AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(theIterator->GetTemplateIndex());
					ASSERT(theTemplate);
					theIterator->LoadFromTemplate(theTemplate);
				}

				// Always update particle systems
				theIterator->Update(theTimeToUpdate);

				// If the particle system has lived long enough, mark it for deletion
				float theCurrentTime = this->GetTime();
				float theParticleSystemLifetime = theIterator->GetParticleSystemLifetime();
				if(!theIterator->GetIsMarkedForDeletion())
				{
					if((theParticleSystemLifetime != -1) && theIterator->GetHasGeneratedParticles())
					{
						if(theCurrentTime > (theIterator->GetTimeCreated() + theParticleSystemLifetime))
						{
							theIterator->SetIsMarkedForDeletion();
						}
					}
				}

				#ifdef AVH_CLIENT
				// Expire visibility of particle systems.  The server sends down particle entities as they're updated, but we need
				// a way to expire them.
				if(theIterator->GetIsVisible())
				{
					const float kExpireTime = 6.0f;
					if((theIterator->GetLastTimeVisibilityLastSetTrue() == -1) || (this->mTimePassed > (theIterator->GetLastTimeVisibilityLastSetTrue() + kExpireTime)))
					{
						theIterator->SetIsVisible(false, inTime);
					}
				}
				#endif

				// If a particle system is marked for deletion and it has no more particles, delete it
				if(theIterator->GetIsMarkedForDeletion() && (theIterator->GetNumberOfParticles() == 0) )
				{
					theIterator->Kill();
					theIterator = this->mParticleSystemList.erase(theIterator);
				}
				else
				{
					theIterator++;
				}
			}

			this->mUpdatedTime = this->mTimePassed;
		}
	}
	this->mReloadFromTemplates = false;
}