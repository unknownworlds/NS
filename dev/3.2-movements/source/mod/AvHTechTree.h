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
// $Log: AvHTechTree.h,v $
// Revision 1.7  2002/09/23 22:36:08  Flayra
// - Slight refactoring
//
// Revision 1.6  2002/07/08 17:20:32  Flayra
// - Added hooks to support disallowing actions when a building is "busy"
//
//===============================================================================
#ifndef AVHTECHTREE_H
#define AVHTECHTREE_H

#include "mod/AvHTechNode.h"
#include "mod/AvHConstants.h"
#include "util/Balance.h"

typedef vector<AvHMessageID> MessageIDListType;	//used in GetDelta, remove when refactoring ChangeListener
typedef map<AvHMessageID,AvHTechNode*> TechNodeMap;
typedef map<AvHTechID,AvHTechNode*> TechIDMap;

class AvHTechTree : public BalanceChangeListener
{
public:
	AvHTechTree(void);
	AvHTechTree(const AvHTechTree& other);
	~AvHTechTree(void);

	void				Clear(void);	
	void				Init(void);
	void				InsertNode(const AvHTechNode* node);	//makes an internal copy of the node
	void				RemoveNode(const AvHMessageID message);	//removes internal copy of the node by message ID if it exists
	AvHTechNode*		GetNode(const AvHMessageID message);
	const AvHTechNode*	GetNode(const AvHMessageID message) const;
	AvHTechNode*		GetNodeByTech(const AvHTechID tech);
	const AvHTechNode*	GetNodeByTech(const AvHTechID tech) const;
	const int			GetSize(void) const;

	bool				GetAllowMultiples(const AvHMessageID inMessageID) const;
	bool				GetIsMessageAvailable(const AvHMessageID inMessageID) const;
	bool				GetIsMessageAvailableForSelection(const AvHMessageID inMessageID, const EntityListType& inSelection) const;
    bool				GetIsMessageInTechTree(const AvHMessageID inMessageID) const;
	bool				GetIsTechResearched(const AvHTechID inTech) const;
	bool				GetMessageForTech(const AvHTechID inTechID, AvHMessageID& outMessageID) const;
	AvHMessageID		GetNextMessageNeededFor(const AvHMessageID inMessageID) const;
	void				GetResearchNodesDependentOn(AvHTechID inTechID, TechNodeMap& outTechNodes) const;
	bool				GetTechForMessage(const AvHMessageID inMessageID, AvHTechID& outTechID) const;
	bool				GetPrequisitesForMessage(const AvHMessageID inMessageID, AvHTechID& outTech1, AvHTechID& outTech2) const;
	bool				GetResearchInfo(const AvHMessageID inTech, bool& outIsResearchable, int& outCost, float& outTime) const;
	bool				SetResearchDone(const AvHMessageID inTech, bool inState = true);
    bool                SetIsResearchable(const AvHMessageID inMessageID, bool inState = true);
	bool				SetFirstNodeWithTechResearchState(const AvHTechID inTech, bool inState = true);

	void				TriggerAddTech(const AvHTechID inTechID);
	void				TriggerRemoveTech(const AvHTechID inTechID);
	
	void				GetDelta(const AvHTechTree& other, MessageIDListType& delta) const;
	bool				operator!=(const AvHTechTree& other) const;
	bool				operator==(const AvHTechTree& other) const;
	AvHTechTree&		operator=(const AvHTechTree& other);
	void				swap(AvHTechTree& other);
	
private:
	TechNodeMap			mNodesByMsg;
	mutable TechIDMap	mNodesByTech;	//cache for techIDs that we've already looked up

#ifdef AVH_SERVER
	//listener implementation - server only, client uses defaults
public:
	bool shouldNotify(const std::string& name, const BalanceValueType type) const;
	void balanceCleared(void) const;
	void balanceValueInserted(const std::string& name, const float value) const;
	void balanceValueInserted(const std::string& name, const int value) const;
	void balanceValueInserted(const std::string& name, const std::string& value) const;
	void balanceValueChanged(const std::string& name, const float old_value, const float new_value) const;
	void balanceValueChanged(const std::string& name, const int old_value, const int new_value) const;
	void balanceValueChanged(const std::string& name, const std::string& old_value, const std::string& default_value) const;
	void balanceValueRemoved(const std::string& name, const float old_value) const;
	void balanceValueRemoved(const std::string& name, const int old_value) const;
	void balanceValueRemoved(const std::string& name, const std::string& old_value) const;
	void balanceStartCompoundChange(void) const;
	void balanceEndCompoundChange(void) const;
	void processBalanceChange(void);	//update all nodes to reflect new balance - doesn't attempt to verify that actual change is required

private:
	mutable bool compoundChangeInProgress;
#endif
};

#endif