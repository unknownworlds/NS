//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechTree.cpp $
// $Date: 2002/09/23 22:36:08 $
//
//===============================================================================

#include "mod/AvHTechTree.h"

bool AvHSHUGetIsResearchTech(AvHMessageID inMessageID);

AvHTechTree::AvHTechTree(void) { Init(); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHTechTree::AvHTechTree(const AvHTechTree& other)
{
	//because pointers in mNodesByMsg are deleted by
	//destruction, we need to make our own copy on
	//construction
	TechNodeMap::const_iterator current, end = other.mNodesByMsg.end();
	for( current = other.mNodesByMsg.begin(); current != end; ++current )
	{
		mNodesByMsg[current->first] = current->second->clone();
	}
	Init();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHTechTree::~AvHTechTree(void) { Clear(); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::Init(void)
{
#ifdef SERVER
	BALANCE_LISTENER(this); 
	compoundChangeInProgress = false;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::swap(AvHTechTree& other)
{
	TechNodeMap tempMap = mNodesByMsg; mNodesByMsg = other.mNodesByMsg; other.mNodesByMsg = tempMap;
	TechIDMap tempIDMap = mNodesByTech; mNodesByTech = other.mNodesByTech; other.mNodesByTech = tempIDMap;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::InsertNode(const AvHTechNode* inTechNode)
{
	this->RemoveNode(inTechNode->getMessageID()); //remove old node to prevent memory leak
	this->mNodesByMsg[inTechNode->getMessageID()] = inTechNode->clone();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::RemoveNode(const AvHMessageID inMessageID)
{
	AvHTechNode* node = GetNode(inMessageID);
	if( node )
	{
		mNodesByMsg.erase(node->getMessageID());
		//remove from tech map if it's a match before we delete it!
		TechIDMap::iterator item = mNodesByTech.find(node->getTechID());
		if( item != mNodesByTech.end() && item->second == node )
		{ mNodesByTech.erase(item); }
		delete node;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::Clear(void)
{
	TechNodeMap::iterator current, end = mNodesByMsg.end();
	for( current = mNodesByMsg.begin(); current != end; ++current )
	{
		delete current->second;
	}
	mNodesByMsg.clear();
	mNodesByTech.clear();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHTechNode* AvHTechTree::GetNode(const AvHMessageID message)
{
	AvHTechNode* returnVal = NULL;
	TechNodeMap::iterator item = mNodesByMsg.find(message);
	if( item != mNodesByMsg.end() )
	{ returnVal = item->second; }
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const AvHTechNode* AvHTechTree::GetNode(const AvHMessageID message) const
{
	const AvHTechNode* returnVal = NULL;
	TechNodeMap::const_iterator item = mNodesByMsg.find(message);
	if( item != mNodesByMsg.end() )
	{ returnVal = item->second; }
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHTechNode* AvHTechTree::GetNodeByTech(const AvHTechID inTech)
{
	AvHTechNode* returnVal = NULL;
	TechIDMap::iterator item = mNodesByTech.find(inTech);
	if( item != mNodesByTech.end() )
	{ returnVal = item->second; }
	else
	{
		TechNodeMap::iterator current, end = mNodesByMsg.end();
		for( current = mNodesByMsg.begin(); current != end; ++current )
		{
			if( current->second->getTechID() == inTech )
			{
				mNodesByTech[inTech] = current->second;
				returnVal = current->second;
				break;
			}
		}		
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const AvHTechNode* AvHTechTree::GetNodeByTech(const AvHTechID inTech) const
{
	const AvHTechNode* returnVal = NULL;
	TechIDMap::const_iterator item = mNodesByTech.find(inTech);
	if( item != mNodesByTech.end() )
	{ returnVal = item->second; }
	else
	{
		TechNodeMap::const_iterator current, end = mNodesByMsg.end();
		for( current = mNodesByMsg.begin(); current != end; ++current )
		{
			if( current->second->getTechID() == inTech )
			{
				mNodesByTech[inTech] = current->second;
				returnVal = current->second;
				break;
			}
		}		
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int AvHTechTree::GetSize(void) const
{
	return (int)mNodesByTech.size();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetAllowMultiples(const AvHMessageID inMessageID) const
{
	const AvHTechNode* Node = GetNode(inMessageID);
	return (Node != NULL) && Node->getAllowMultiples();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetIsMessageInTechTree(const AvHMessageID inMessageID) const
{
	const AvHTechNode* Node = GetNode(inMessageID);
	return (Node != NULL);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetIsMessageAvailable(const AvHMessageID inMessageID) const
{
	bool returnVal = true;

	const AvHTechNode* Subnode;
	const AvHTechNode* Node = GetNode(inMessageID);

	// : 73
	// HACK to bypass the node checks when issuing one of the CC UI impulses
	// Could probably be reworked prettier by assigning nodes to each of these
	// messages, but this will have to do for now
	if (inMessageID == COMMANDER_SELECTALL || 
		inMessageID == COMMANDER_NEXTAMMO || 
		inMessageID == COMMANDER_NEXTHEALTH || 
		inMessageID == COMMANDER_NEXTIDLE || 
		inMessageID == GROUP_SELECT_1 || 
		inMessageID == GROUP_SELECT_2 || 
		inMessageID == GROUP_SELECT_3 || 
		inMessageID == GROUP_SELECT_4 || 
		inMessageID == GROUP_SELECT_5)
		return true;
	// :
	if( Node == NULL ) //not found
	{ returnVal = false; }
	else if( Node->getIsResearched() && !Node->getAllowMultiples() ) //can only research once?
	{ returnVal = false; }
	else
	{
		AvHTechID prereq = Node->getPrereqTechID1();
		if( prereq != TECH_NULL && ( (Subnode = GetNodeByTech(prereq)) == NULL || !Subnode->getIsResearched() ) )
		{ returnVal = false; }
		else
		{
			prereq = Node->getPrereqTechID2();
			if( prereq != TECH_NULL && ( (Subnode = GetNodeByTech(prereq)) == NULL || !Subnode->getIsResearched() ) )
			{ returnVal = false; }
		}
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetIsMessageAvailableForSelection(const AvHMessageID inMessageID, const EntityListType& inSelection) const
{
	return GetIsMessageAvailable(inMessageID);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetIsTechResearched(AvHTechID inTech) const
{
	const AvHTechNode* Node = GetNodeByTech(inTech);
	return (Node != NULL) && Node->getIsResearched();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::SetFirstNodeWithTechResearchState(const AvHTechID inTech, bool inState)
{
	bool returnVal = false;
	AvHTechNode* Node = GetNodeByTech(inTech);
	if( Node != NULL )
	{
		Node->setResearchState(inState);
		returnVal = true;
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetMessageForTech(const AvHTechID inTech, AvHMessageID& outMessageID) const
{
	bool returnVal = false;
	const AvHTechNode* Node = GetNodeByTech(inTech);
	if( Node != NULL )
	{ 
		outMessageID = Node->getMessageID();
		returnVal = true;
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHMessageID AvHTechTree::GetNextMessageNeededFor(const AvHMessageID inMessageID) const
{
	//perform depth first search for item that hasn't been researched but can be; returns
	//original ID if no match is found... would be better to return boolean false with an
	//output AvHMessageID specified as a parameter.
	AvHMessageID returnVal = inMessageID;
	const AvHTechNode* Node = GetNode(inMessageID);
	bool prereq1_researched = false;

	if( Node != NULL && !Node->getIsResearched() )
	{
		vector<const AvHTechNode*> ParentStack;
		ParentStack.push_back(Node);
		const AvHTechNode* Child;
		AvHTechID prereq;

		while(!ParentStack.empty())
		{
			Node = ParentStack.back();
			ParentStack.pop_back();

			//First child
			prereq1_researched = false;
			prereq = Node->getPrereqTechID1();
			if( prereq == TECH_NULL )
			{ prereq1_researched = true; }
			else
			{
				Child = GetNodeByTech(prereq);
				if( Child != NULL )
				{
					if( Child->getIsResearched() )
					{ prereq1_researched = true; }
					else
					{ ParentStack.push_back(Child); }
				}
			}

			//Second child
			prereq = Node->getPrereqTechID2();
			if( prereq == TECH_NULL && prereq1_researched )
			{
				returnVal = Node->getMessageID();
				break;
			}
			else
			{
				Child = GetNodeByTech(prereq);
				if( Child != NULL )
				{
					if( Child->getIsResearched() )
					{
						if( prereq1_researched )
						{
							returnVal = Node->getMessageID();
							break;
						}
					}
					else
					{ ParentStack.push_back(Child); }
				}
			}
		}
	}

	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::GetResearchNodesDependentOn(AvHTechID inTechID, TechNodeMap& outTechNodes) const
{
	//move up tree from supplied base tech; this won't be a cheap operation - worst case is (tree size)^2
	if(inTechID != TECH_NULL)
	{
		TechNodeMap::const_iterator current, end = mNodesByMsg.end();
		for( current = mNodesByMsg.begin(); current != end; ++current )
		{
			if( !AvHSHUGetIsResearchTech(current->first) || current->second->getTechID() == TECH_NULL )
			{ continue; }

			if( current->second->getPrereqTechID1() == inTechID || current->second->getPrereqTechID2() == inTechID )
			{
				outTechNodes[current->first] = current->second; //don't clone here, caller not responsible for deletion
				GetResearchNodesDependentOn(current->second->getTechID(), outTechNodes); //recurse
			}
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetTechForMessage(const AvHMessageID inMessageID, AvHTechID& outTechID) const
{
	bool returnVal = false;
	const AvHTechNode* Node = GetNode(inMessageID);
	if( Node != NULL )
	{
		outTechID = Node->getTechID();
		returnVal = true;
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetPrequisitesForMessage(const AvHMessageID inMessageID, AvHTechID& outTech1, AvHTechID& outTech2) const
{
	bool returnVal = false;
	const AvHTechNode* Node = GetNode(inMessageID);
	if( Node != NULL )
	{
		outTech1 = Node->getPrereqTechID1();
		outTech2 = Node->getPrereqTechID2();
		returnVal = (outTech1 != TECH_NULL) || (outTech2 != TECH_NULL);
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::GetResearchInfo(AvHMessageID inMessageID, bool& outIsResearchable, int& outCost, float& outTime) const
{
	bool returnVal = false;
	const AvHTechNode* Node = GetNode(inMessageID);
	if( Node != NULL )
	{
		outIsResearchable = Node->getIsResearchable();
		outCost = Node->getCost();
		outTime = Node->getBuildTime();
		returnVal = true;
	}
	else
	{
		outIsResearchable = false;
		outCost = -1;
		outTime = -1;
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::SetIsResearchable(AvHMessageID inMessageID, bool inState)
{
	bool returnVal = false;
	AvHTechNode* Node = GetNode(inMessageID);
	if( Node != NULL )
	{
		Node->setResearchable(inState);
		returnVal = true;
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::SetResearchDone(AvHMessageID inMessageID, bool inState)
{
	bool returnVal = false;
	AvHTechNode* Node = GetNode(inMessageID);
	if( Node != NULL )
	{
		Node->setResearchState(inState);
		returnVal = true;
	}
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::TriggerAddTech(AvHTechID inTechID)
{
	AvHTechNode* Node = GetNodeByTech(inTechID);
	if( Node != NULL )
	{
		Node->setResearchState(true);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::TriggerRemoveTech(AvHTechID inTechID)
{
	AvHTechNode* Node = GetNodeByTech(inTechID);
	if( Node != NULL )
	{
		Node->setResearchState(false);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHTechTree::GetDelta(const AvHTechTree& other, MessageIDListType& delta) const
{

	delta.clear();
	TechNodeMap::const_iterator current = mNodesByMsg.begin(); 
	TechNodeMap::const_iterator end = mNodesByMsg.end();
	TechNodeMap::const_iterator other_current = other.mNodesByMsg.begin();
	TechNodeMap::const_iterator other_end = other.mNodesByMsg.end();
	while( current != end && other_current != other_end )
	{
		if( current->first < other_current->first )
		{
			delta.push_back(current->first);
			++current;
			continue;
		}

		if( current->first > other_current->first )
		{
			delta.push_back(current->first);
			++other_current;
			continue;
		}

		if( *current->second != *other_current->second )
		{
			delta.push_back(current->first);
		}

		++current;
		++other_current;
	}

	while( current != end )
	{
		delta.push_back(current->first);
		++current;
	}

	while( other_current != other_end )
	{
		delta.push_back(other_current->first);
		++other_current;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::operator!=(const AvHTechTree& inTree) const
{
	return !this->operator==(inTree);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHTechTree::operator==(const AvHTechTree& inTree) const
{
	return mNodesByMsg == inTree.mNodesByMsg;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHTechTree& AvHTechTree::operator=(const AvHTechTree& inTree)
{
	AvHTechTree temp(inTree);
	swap(temp);
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef AVH_SERVER
bool AvHTechTree::shouldNotify(const string& name, const BalanceValueType type) const { return true; }

void AvHTechTree::balanceCleared(void) const {}

void AvHTechTree::balanceValueInserted(const string& name, const float value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueInserted(const string& name, const int value) const		
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueInserted(const string& name, const string& value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueRemoved(const string& name, const float old_value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueRemoved(const string& name, const int old_value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueRemoved(const string& name, const string& old_value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueChanged(const string& name, const float old_value, const float new_value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueChanged(const string& name, const int old_value, const int new_value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceValueChanged(const string& name, const string& old_value, const string& default_value) const
{
	if( !compoundChangeInProgress ) 
	{ const_cast<AvHTechTree*>(this)->processBalanceChange(); }
}

void AvHTechTree::balanceStartCompoundChange(void) const 
{ compoundChangeInProgress = true; }

void AvHTechTree::balanceEndCompoundChange(void) const 
{ compoundChangeInProgress = false; }

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "mod/AvHGamerules.h"
void AvHTechTree::processBalanceChange(void)
{
	// Run through our tech nodes and update cost and build time
	TechNodeMap::iterator current, end = mNodesByMsg.end();
	for( current = mNodesByMsg.begin(); current != end; ++current )
	{
		current->second->setBuildTime(GetGameRules()->GetBuildTimeForMessageID(current->first));
		current->second->setCost(GetGameRules()->GetCostForMessageID(current->first));
	}
}

#endif