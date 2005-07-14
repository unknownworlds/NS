//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechNodes.h $
// $Date: 2002/09/23 22:36:08 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTechNodes.h,v $
// Revision 1.7  2002/09/23 22:36:08  Flayra
// - Slight refactoring
//
// Revision 1.6  2002/07/08 17:20:32  Flayra
// - Added hooks to support disallowing actions when a building is "busy"
//
//===============================================================================
#ifndef AVHTECHNODES_H
#define AVHTECHNODES_H

#include "types.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMessage.h"
#include "mod/AvHTechID.h"

class AvHTechNode
{
public:
					AvHTechNode();
					AvHTechNode(AvHMessageID inMessageID, AvHTechID inID, AvHTechID inPrereq1, AvHTechID inPrereq2, int inCost, int inBuildTime, bool inResearched = false);

	bool			GetAllowMultiples() const;
	AvHMessageID	GetMessageID() const;
	AvHTechID		GetTechID() const;
	AvHTechID		GetPrereqTechID1() const;
	AvHTechID		GetPrereqTechID2() const;
	int				GetCost() const;
	int				GetBuildTime() const;

	bool			GetIsResearchable() const;
	bool			GetIsResearched() const;
	void			SetAllowMultiples();
	void			SetBuildTime(int inBuildTime);
	void			SetCost(int inCost);
	void			SetResearchable(bool inState = true);
	void			SetResearchState(bool inState);

	#ifdef AVH_SERVER
	void			SendToNetworkStream() const;
	#endif						
	
	#ifdef AVH_CLIENT
	string			GetLabel() const;
	void			SetLabel(const string& inLabel);
	
	string			GetHelpText() const;
	void			SetHelpText(const string& inHelpText);
	
	int				ReceiveFromNetworkStream();
	#endif

	bool			operator==(const AvHTechNode& inTechNode) const;
	bool			operator!=(const AvHTechNode& inTechNode) const;
	void			operator=(const AvHTechNode& inTechNode);
	
private:
	// Adding a member?  Change the operator== and operator = functions too
	AvHMessageID	mMessageID;
	AvHTechID		mTechID;
	AvHTechID		mPrereqID1;
	AvHTechID		mPrereqID2;
	int				mCost;
	int				mBuildTime;

	bool			mResearchable;
	bool			mResearched;
	bool			mAllowMultiples;

	#ifdef AVH_CLIENT
	string			mLabel;
	string			mHelpText;
	#endif
};

typedef vector<AvHTechNode>		TechNodeListType;

class AvHTechNodes
{
public:
	void					AddTechNode(const AvHTechNode& inTechNode);
	
	#ifdef AVH_PLAYTEST_BUILD
		#ifdef AVH_SERVER
		void				BalanceChanged();
		#endif
	#endif

	void					Clear();
	bool					GetAllowMultiples(AvHMessageID& inMessageID) const;
	bool					GetIsMessageAvailable(AvHMessageID& inMessageID) const;
	bool					GetIsMessageAvailableForSelection(AvHMessageID& inMessageID, EntityListType& inSelection) const;
    bool                    GetIsMessageInTechTree(AvHMessageID& inMessageID) const;
	bool					GetIsTechResearched(AvHTechID inTech) const;
	bool					GetMessageForTech(const AvHTechID inTechID, AvHMessageID& outMessageID) const;
	AvHMessageID			GetNextMessageNeededFor(AvHMessageID inMessageID) const;
	void					GetResearchNodesDependentOn(AvHTechID inTechID, TechNodeListType& outTechNodes) const;
	int						GetNumNodes() const;
	bool					GetTechForMessage(const AvHMessageID inMessageID, AvHTechID& outTechID) const;
	const TechNodeListType& GetTechNodes() const;
	bool					GetPrequisiteForMessage(const AvHMessageID inMessageID, AvHTechID& outTech1, AvHTechID& outTech2) const;
	int						GetNumTechNodes() const;
	bool					GetResearchInfo(AvHMessageID inTech, bool& outIsResearchable, int& outCost, float& outTime) const;
	bool					GetTechNode(int inOffset, AvHTechNode& outTechNode) const;
	bool					GetTechNode(AvHMessageID inMessageID, AvHTechNode& outTechNode) const;
	bool					SetResearchDone(AvHMessageID inTech, bool inState = true);
    bool                    SetIsResearchable(AvHMessageID inMessageID, bool inState = true);
	bool					SetTechNode(int inOffset, const AvHTechNode& inTechNode);
	bool					SetFirstNodeWithTechResearchState(AvHTechID inTech, bool inState = true);

	void					TriggerAddTech(AvHTechID inTechID);
	void					TriggerRemoveTech(AvHTechID inTechID);
	
	bool					operator!=(const AvHTechNodes& inTechNodes) const;
	bool					operator==(const AvHTechNodes& inTechNodes) const;
	void					operator=(const AvHTechNodes& inTechNodes);
	
	//#ifdef AVH_SERVER
	//void					SendToNetworkStream() const;
	//#endif						
	
	//#ifdef AVH_CLIENT
	//int						ReceiveFromNetworkStream();
	//#endif

private:
	TechNodeListType		mTechNodes;
};

#endif