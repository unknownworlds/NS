//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHResearchManager.cpp $
// $Date: 2002/10/24 21:41:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHResearchManager.cpp,v $
// Revision 1.11  2002/10/24 21:41:15  Flayra
// - Attempt to fix progress bars not showing up in release, or when playing online (can't quite figure out when).  Never safe to compare floats anyways.
//
// Revision 1.10  2002/09/23 22:28:33  Flayra
// - Added GetIsResearching method, so automatic armory resupply could be added
//
// Revision 1.9  2002/08/02 21:50:37  Flayra
// - New research system
//
// Revision 1.8  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.7  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.6  2002/07/23 17:20:13  Flayra
// - Added hooks for research starting, for distress beacon sound effects
//
// Revision 1.5  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHResearchManager.h"

#include "mod/AvHAlienWeapons.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHGamerules.h"

AvHResearchNode::AvHResearchNode(AvHMessageID inMessageID, int inEntityIndex)
{
	this->mResearch = inMessageID;
	this->mEntityIndex = inEntityIndex;
	this->mTimeResearchDone = -1;
	this->mTimeResearchStarted = -1;
}

bool AvHResearchNode::GetCanEntityContinueResearch() const
{
	bool theCanContinueResearch = true;

	CBaseEntity* theResearchEntity = AvHSUGetEntityFromIndex(this->mEntityIndex);
	if(!theResearchEntity || !theResearchEntity->IsAlive())
	{
		theCanContinueResearch = false;
	}

	return theCanContinueResearch;
}

int	AvHResearchNode::GetEntityIndex() const
{
	return this->mEntityIndex;
}

AvHMessageID AvHResearchNode::GetResearching() const
{
	return this->mResearch;
}

float AvHResearchNode::GetTimeResearchDone() const
{
	return this->mTimeResearchDone;
}

float AvHResearchNode::GetTimeResearchStarted() const
{
	return this->mTimeResearchStarted;
}

bool AvHResearchNode::UpdateResearch()
{
	bool theResearchDone = false;
	
	AvHMessageID theResearchingTech = this->GetResearching();
	if(theResearchingTech != MESSAGE_NULL)
	{
		CBaseEntity* theResearchEntity = AvHSUGetEntityFromIndex(this->mEntityIndex);
		ASSERT(theResearchEntity);
		
		float theTimeResearchDone = this->GetTimeResearchDone();
		
		// Set time during the first update
		if(theTimeResearchDone < 0)
		{
			this->mTimeResearchStarted = gpGlobals->time;
			theTimeResearchDone = this->mTimeResearchStarted + GetGameRules()->GetBuildTimeForMessageID(theResearchingTech);
			this->mTimeResearchDone = theTimeResearchDone;
			theResearchEntity->pev->iuser2 = (int)this->mResearch;
		}
		
		if((gpGlobals->time >= theTimeResearchDone) || GetGameRules()->GetIsTesting())
		{
			theResearchDone = true;
			//AvHSHUSetBuildResearchState(theResearchEntity->pev->iuser3, theResearchEntity->pev->iuser4, theResearchEntity->pev->fuser1, false, 0.0f);
			//			theResearchEntity->pev->fuser1 = 0.0f;
			//			theResearchEntity->pev->iuser2 = 0;
		}
		else
		{
			float theNormalizedResearchFactor = (gpGlobals->time - this->mTimeResearchStarted)/(this->mTimeResearchDone - this->mTimeResearchStarted);
			theNormalizedResearchFactor = min(max(theNormalizedResearchFactor, 0.0f), 1.0f);

			//theResearchEntity->pev->fuser1 = (kResearchFuser1Base + theNormalizedResearchFactor)*kNormalizationNetworkFactor;
			AvHSHUSetBuildResearchState(theResearchEntity->pev->iuser3, theResearchEntity->pev->iuser4, theResearchEntity->pev->fuser1, false, theNormalizedResearchFactor);
		}
	}
	
	return theResearchDone;
}




// Research manager
void AvHResearchManager::AddTechNode(AvHMessageID inMessageID, AvHTechID inTechID, AvHTechID inPrereq1, AvHTechID inPrereq2, int inPointCost, int inBuildTime, bool inResearched, bool inAllowMultiples)
{
	AvHTechNode theTechNode(inMessageID, inTechID, inPrereq1, inPrereq2, inPointCost, inBuildTime, inResearched);
	if(inAllowMultiples)
	{
		theTechNode.setAllowMultiples();
	}

	this->mTechNodes.InsertNode(&theTechNode);
}

bool AvHResearchManager::GetResearchInfo(AvHMessageID inTech, bool& outIsResearchable, int& outCost, float& outTime) const
{
	bool theFoundIt = false;
	
	// Check each tech nodes
	if(this->mTechNodes.GetResearchInfo(inTech, outIsResearchable, outCost, outTime))
	{
		theFoundIt = true;
	}
	
	return theFoundIt;
}

const AvHTechTree&	AvHResearchManager::GetTechNodes() const
{
	return this->mTechNodes;
}

AvHTechTree& AvHResearchManager::GetTechNodes()
{
	return this->mTechNodes;
}

void AvHResearchManager::TriggerAddTech(AvHTechID inTechID)
{
	this->mTechNodes.TriggerAddTech(inTechID);
}

void AvHResearchManager::TriggerRemoveTech(AvHTechID inTechID)
{
	this->mTechNodes.TriggerRemoveTech(inTechID);
}

void AvHResearchManager::SetTeamNumber(AvHTeamNumber inTeamNumber)
{
	this->mTeamNumber = inTeamNumber;
}

bool AvHResearchManager::SetResearchDone(AvHMessageID inTech, int inEntityIndex)
{
	bool theFoundIt = false;
	
	if(this->mTechNodes.SetResearchDone(inTech))
	{
		edict_t* theEdict = g_engfuncs.pfnPEntityOfEntIndex(inEntityIndex);
		ASSERT(!theEdict->free);
		CBaseEntity* theEntity = CBaseEntity::Instance(theEdict);
		ASSERT(theEntity);

		// Potentially inform all entities and team of upgrade
		GetGameRules()->ProcessTeamUpgrade(inTech, this->mTeamNumber, inEntityIndex, true);
		
		// Hook research complete
		AvHSUResearchComplete(theEntity, inTech);

		// No longer researching
		//theEntity->pev->fuser1 = kResearchFuser1Base*kNormalizationNetworkFactor;
		AvHSHUSetBuildResearchState(theEntity->pev->iuser3, theEntity->pev->iuser4, theEntity->pev->fuser1, true, 0.0f);
		
		// Tell entity that it's no longer researching
		AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theEntity);
		if(theBuildable)
		{
			theBuildable->SetResearching(false);
		}
		
		// Send message indicating research is done
		GetGameRules()->TriggerAlert(this->mTeamNumber, ALERT_RESEARCH_COMPLETE, inEntityIndex);
		
		theFoundIt = true;
	}
	
	return theFoundIt;
}

void AvHResearchManager::Reset()
{
	// Clear out everything
	this->mResearchingTech.clear();
	this->mTechNodes.Clear();
}

bool AvHResearchManager::CancelResearch(int inEntityIndex, float& outResearchPercentage, AvHMessageID& outMessageID)
{
	bool theSuccess = false;
	
	// Search through researching tech and make sure we're researching this
	for(ResearchListType::iterator theIter = this->mResearchingTech.begin(); theIter != this->mResearchingTech.end(); theIter++)
	{
		if(theIter->GetEntityIndex() == inEntityIndex)
		{
			float theEndTime = theIter->GetTimeResearchDone();
			float theStartTime = theIter->GetTimeResearchStarted();
			outResearchPercentage = (gpGlobals->time - theStartTime)/(theEndTime - theStartTime);
			
			outMessageID = theIter->GetResearching();
			
			this->mResearchingTech.erase(theIter);
			
			// Look up entity and reset research progress
			CBaseEntity* theResearchEntity = AvHSUGetEntityFromIndex(inEntityIndex);
			ASSERT(theResearchEntity);
			//theResearchEntity->pev->fuser1 = kResearchFuser1Base*kNormalizationNetworkFactor;
			AvHSHUSetBuildResearchState(theResearchEntity->pev->iuser3, theResearchEntity->pev->iuser4, theResearchEntity->pev->fuser1, false, 0.0f);
			
			AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theResearchEntity);
			if(theBuildable)
			{
				theBuildable->SetResearching(false);
			}			
			
			theSuccess = true;
			break;
		}
	}

	return theSuccess;
}

bool AvHResearchManager::SetResearching(AvHMessageID inMessageID, int inEntityIndex)
{
	bool theCouldStart = true;
	
	// Search through researching tech and make sure this entity isn't already researching something
	for(ResearchListType::iterator theIter = this->mResearchingTech.begin(); theIter != this->mResearchingTech.end(); theIter++)
	{
		if(theIter->GetEntityIndex() == inEntityIndex)
		{
			theCouldStart = false;
			break;
		}
	}
	
	if(theCouldStart)
	{
		bool theIsResearchable = false;
		int theCost;
		float theTime;
		// joev: 0000199
		// Look up entity and check that research is valid for this entity.
		CBaseEntity* theResearchEntity = AvHSUGetEntityFromIndex(inEntityIndex);
		ASSERT(theResearchEntity);
			
		if (!AvHSUGetIsResearchApplicable(theResearchEntity,inMessageID)) {
			theCouldStart = false;
		}
		// :joev

		if(!this->GetResearchInfo(inMessageID, theIsResearchable, theCost, theTime) || !theIsResearchable)
		{
			theCouldStart = false;
		}
	}
	
	if(theCouldStart)
	{
		this->mResearchingTech.push_back(AvHResearchNode(inMessageID, inEntityIndex));
		
		// Tell entity that it's researching
		edict_t* theEdict = g_engfuncs.pfnPEntityOfEntIndex(inEntityIndex);
		ASSERT(!theEdict->free);
		CBaseEntity* theEntity = CBaseEntity::Instance(theEdict);
		ASSERT(theEntity);
		
		AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theEntity);
		if(theBuildable)
		{
			theBuildable->SetResearching(true);
			AvHSUResearchStarted(theEntity, inMessageID);
		}
	}
	
	return theCouldStart;
}

bool AvHResearchManager::GetIsMessageAvailable(AvHMessageID& inMessageID) const
{
	bool theIsAvailable = false;

	if(this->mTechNodes.GetIsMessageAvailable(inMessageID))
	{
		// Make sure it's not a one time research that we're currently researching
		if(this->GetIsResearchingTech(inMessageID) && !this->mTechNodes.GetAllowMultiples(inMessageID))
		{
			theIsAvailable = false;
		}
		else
		{
			theIsAvailable = true;
		}
	}

	return theIsAvailable;
}

TechNodeMap AvHResearchManager::GetResearchNodesDependentOn(AvHTechID inTechID) const
{
	TechNodeMap theTechNodes;

	this->mTechNodes.GetResearchNodesDependentOn(inTechID, theTechNodes);
	
	return theTechNodes;
}

bool AvHResearchManager::GetIsResearchingTech(AvHMessageID inMessageID) const
{
	bool theIsResearchingTech = false;

	for(ResearchListType::const_iterator theIter = this->mResearchingTech.begin(); theIter != this->mResearchingTech.end(); theIter++)
	{
		if(theIter->GetResearching() == inMessageID)
		{
			theIsResearchingTech = true;
			break;
		}
	}

	return theIsResearchingTech;
}


bool AvHResearchManager::GetIsResearching(int inEntityIndex) const
{
	bool theIsResearching = false;

	// Run through every item in the list and update research, marking any done 
	for(ResearchListType::const_iterator theIter = this->mResearchingTech.begin(); theIter != this->mResearchingTech.end(); theIter++)
	{
		if(theIter->GetEntityIndex() == inEntityIndex)
		{
			theIsResearching = true;
			break;
		}
	}

	return theIsResearching;
}

void AvHResearchManager::UpdateResearch()
{
	// Run through every item in the list and update research, marking any done 
	for(ResearchListType::iterator theIter = this->mResearchingTech.begin(); theIter != this->mResearchingTech.end(); )
	{
		if(theIter->GetCanEntityContinueResearch())
		{
			bool theHighTechCheat = GetGameRules()->GetIsCheatEnabled(kcHighTech);
			if(theIter->UpdateResearch() || theHighTechCheat)
			{
				AvHMessageID theResearchingTech = theIter->GetResearching();
				int theEntityIndex = theIter->GetEntityIndex();

				this->SetResearchDone(theResearchingTech, theEntityIndex);

				theIter = this->mResearchingTech.erase(theIter);
			}
			else
			{
				theIter++;
			}
		}
		else
		{
			theIter = this->mResearchingTech.erase(theIter);
		}
	}
}
