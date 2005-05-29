//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHResearchManager.h $
// $Date: 2002/09/23 22:28:33 $
//
//-------------------------------------------------------------------------------
// $Log: AvHResearchManager.h,v $
// Revision 1.4  2002/09/23 22:28:33  Flayra
// - Added GetIsResearching method, so automatic armory resupply could be added
//
// Revision 1.3  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_RESEARCHMANAGER_H
#define AVH_RESEARCHMANAGER_H

#include "types.h"
#include "mod/AvHMessage.h"
#include "mod/AvHTechNodes.h"

class AvHResearchNode
{
public:
						AvHResearchNode(AvHMessageID inMessageID, int inEntityIndex);

	bool				GetCanEntityContinueResearch() const;
	int					GetEntityIndex() const;
	AvHMessageID		GetResearching() const;
	float				GetTimeResearchDone() const;
	float				GetTimeResearchStarted() const;
	bool				UpdateResearch();

private:
	int					mEntityIndex;
	AvHMessageID		mResearch;
	float				mTimeResearchDone;
	float				mTimeResearchStarted;
};

typedef vector<AvHResearchNode>		ResearchListType;

class AvHResearchManager
{
public:
	void				AddTechNode(AvHMessageID inMessageID, AvHTechID inTechID, AvHTechID inPrereq1, AvHTechID inPrereq2, int inPointCost, int inBuildTime, bool inResearched, bool inAllowMultiples);

	#ifdef AVH_PLAYTEST_BUILD
	void				BalanceChanged();
	#endif

	const AvHTechNodes&	GetTechNodes() const;
	AvHTechNodes&		GetTechNodes();

	bool				GetResearchInfo(AvHMessageID inTech, bool& outIsResearchable, int& outCost, float& outTime) const;

	void				TriggerAddTech(AvHTechID inTechID);
	void				TriggerRemoveTech(AvHTechID inTechID);
	
	void				Reset();

	bool				CancelResearch(int inEntityIndex, float& outResearchPercentage, AvHMessageID& outMessageID);

	bool				GetIsMessageAvailable(AvHMessageID& inMessageID) const;

	TechNodeListType	GetResearchNodesDependentOn(AvHTechID inTechID) const;

	bool				GetIsResearchingTech(AvHMessageID inMessageID) const;

	bool				GetIsResearching(int inEntityIndex) const;
	
	bool				SetResearching(AvHMessageID inMessageID, int inEntityIndex);

	void				SetTeamNumber(AvHTeamNumber inTeamNumber);
		
	void				UpdateResearch();
	
private:
	bool				SetResearchDone(AvHMessageID inTech, int inEntityIndex);

	ResearchListType	mResearchingTech;
	AvHTechNodes		mTechNodes;
	AvHTeamNumber		mTeamNumber;
	
};

#endif