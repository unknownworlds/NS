//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechNodes.cpp $
// $Date: 2002/09/23 22:36:08 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTechNodes.cpp,v $
// Revision 1.9  2002/09/23 22:36:08  Flayra
// - Slight refactoring
//
// Revision 1.8  2002/07/08 17:20:32  Flayra
// - Added hooks to support disallowing actions when a building is "busy"
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHTechNodes.h"

#ifdef AVH_SERVER
#include "extdll.h"
#include "util.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/parsemsg.h"
#endif

// Prototype here instead of include to prevent vec3_t nightmare
bool AvHSHUGetIsResearchTech(AvHMessageID inMessageID);

AvHTechNode::AvHTechNode()
{
	this->mMessageID = MESSAGE_NULL;
	this->mTechID = this->mPrereqID1 = this->mPrereqID2 = TECH_NULL;
	this->mCost = -1;
	this->mBuildTime = 1;
	this->mResearchable = true;
	this->mResearched = false;
	this->mAllowMultiples = false;
}

AvHTechNode::AvHTechNode(AvHMessageID inMessageID, AvHTechID inID, AvHTechID inPrereq1, AvHTechID inPrereq2, int inCost, int inBuildTime, bool inResearched)
{
	this->mMessageID = inMessageID;
	this->mTechID = inID;
	this->mPrereqID1 = inPrereq1;
	this->mPrereqID2 = inPrereq2;
	this->mCost = inCost;
	this->mBuildTime = inBuildTime;
	this->mResearchable = true;
	this->mResearched = inResearched;
	this->mAllowMultiples = false;
}

bool AvHTechNode::GetAllowMultiples() const
{
	return this->mAllowMultiples;
}

AvHMessageID AvHTechNode::GetMessageID() const
{
	return this->mMessageID;
}

AvHTechID AvHTechNode::GetTechID() const
{
	return this->mTechID;
}

AvHTechID AvHTechNode::GetPrereqTechID1() const
{
	return this->mPrereqID1;
}

AvHTechID AvHTechNode::GetPrereqTechID2() const
{
	return this->mPrereqID2;
}

int	AvHTechNode::GetBuildTime() const
{
	return this->mBuildTime;
}

int	AvHTechNode::GetCost() const
{
	return this->mCost;
}

bool AvHTechNode::GetIsResearchable() const
{
	return this->mResearchable;
}

bool AvHTechNode::GetIsResearched() const
{
	return this->mResearched;
}

void AvHTechNode::SetAllowMultiples()
{
	this->mAllowMultiples = true;
}

void AvHTechNode::SetBuildTime(int inBuildTime)
{
	this->mBuildTime = inBuildTime;
}

void AvHTechNode::SetCost(int inCost)
{
	this->mCost = inCost;
}

void AvHTechNode::SetResearchable(bool inState)
{
	this->mResearchable = inState;
}

void AvHTechNode::SetResearchState(bool inState)
{
	this->mResearched = inState;

	if(!this->GetAllowMultiples())
	{
		this->mResearchable = !this->mResearched;
	}
}

#ifdef AVH_SERVER
void AvHTechNode::SendToNetworkStream() const
{
	// Send tech node
	WRITE_BYTE(this->mMessageID);
	
	WRITE_BYTE(this->mTechID);

	// Send prereqs
	WRITE_BYTE(this->mPrereqID1);
	WRITE_BYTE(this->mPrereqID2);
	
	// Send cost
	WRITE_SHORT(this->mCost);

	// Send build time
	WRITE_SHORT(this->mBuildTime);

	// Send researchable, researched, multiples
	WRITE_BYTE(this->mResearchable);
	WRITE_BYTE(this->mResearched);
	WRITE_BYTE(this->mAllowMultiples);
}
#endif						

#ifdef AVH_CLIENT
string AvHTechNode::GetLabel() const
{
	return this->mLabel;
}

void AvHTechNode::SetLabel(const string& inLabel)
{
	this->mLabel = inLabel;
}

string AvHTechNode::GetHelpText() const
{
	return this->mHelpText;
}

void AvHTechNode::SetHelpText(const string& inHelpText)
{
	this->mHelpText = inHelpText;
}

int	AvHTechNode::ReceiveFromNetworkStream()
{
	int theBytesRead = 0;

	this->mMessageID = (AvHMessageID)READ_BYTE();
	theBytesRead++;
	
	this->mTechID = (AvHTechID)READ_BYTE();
	theBytesRead++;

	this->mPrereqID1 = (AvHTechID)READ_BYTE();
	theBytesRead++;

	this->mPrereqID2 = (AvHTechID)READ_BYTE();
	theBytesRead++;
	
	this->mCost = READ_SHORT();
	theBytesRead += 2;

	this->mBuildTime = READ_SHORT();
	theBytesRead += 2;
	
	this->mResearchable = READ_BYTE();
	theBytesRead++;

	this->mResearched = READ_BYTE();
	theBytesRead++;

	this->mAllowMultiples = READ_BYTE();
	theBytesRead++;
	
	return theBytesRead;
}
#endif

bool AvHTechNode::operator==(const AvHTechNode& inTechNode) const
{
	bool theIsEqual = false;
	
	if(this->mMessageID == inTechNode.mMessageID)
	{
		if(this->mTechID == inTechNode.mTechID)
		{
			if(this->mPrereqID1 == inTechNode.mPrereqID1)
			{
				if(this->mPrereqID2 == inTechNode.mPrereqID2)
				{
					if(this->mCost == inTechNode.mCost)
					{
						if(this->mBuildTime == inTechNode.mBuildTime)
						{
							if(this->mResearchable == inTechNode.mResearchable)
							{
								if(this->mResearched == inTechNode.mResearched)
								{
									if(this->mAllowMultiples == inTechNode.mAllowMultiples)
									{

#ifdef AVH_CLIENT
										if(this->mHelpText == inTechNode.mHelpText)
										{
											if(this->mLabel == inTechNode.mLabel)
											{
#endif
												theIsEqual = true;

#ifdef AVH_CLIENT
											}
										}
#endif

									}
								}
							}
						}
					}
				}
			}
		}
	}

	return theIsEqual;
}

bool AvHTechNode::operator!=(const AvHTechNode& inTechNode) const
{
	return !this->operator==(inTechNode);
}

void AvHTechNode::operator=(const AvHTechNode& inTechNode)
{
	this->mMessageID = inTechNode.mMessageID;
	this->mTechID = inTechNode.mTechID;
	this->mCost = inTechNode.mCost;
	this->mPrereqID1 = inTechNode.mPrereqID1;
	this->mPrereqID2 = inTechNode.mPrereqID2;
	this->mBuildTime = inTechNode.mBuildTime;
	this->mResearchable = inTechNode.mResearchable;
	this->mResearched = inTechNode.mResearched;
	this->mAllowMultiples = inTechNode.mAllowMultiples;

	#ifdef AVH_CLIENT
	this->mHelpText = inTechNode.mHelpText;
	this->mLabel = inTechNode.mLabel;
#endif
}




void AvHTechNodes::AddTechNode(const AvHTechNode& inTechNode)
{
	this->mTechNodes.push_back(inTechNode);
}

#ifdef AVH_PLAYTEST_BUILD
	#ifdef AVH_SERVER

	#include "mod/AvHGamerules.h"

	void AvHTechNodes::BalanceChanged()
	{
		// Run through our tech nodes and update cost and build time
		for(TechNodeListType::iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
		{
			AvHMessageID theMessageID = theIter->GetMessageID();
			
			int theBuildTime = GetGameRules()->GetBuildTimeForMessageID(theMessageID);
			if(theBuildTime != theIter->GetBuildTime())
			{
				theIter->SetBuildTime(theBuildTime);
			}

			int theCost = GetGameRules()->GetCostForMessageID(theMessageID);
			if(theCost != theIter->GetCost())
			{
				theIter->SetCost(theCost);
			}
		}
	}
	#endif
#endif

void AvHTechNodes::Clear()
{
	this->mTechNodes.clear();
}

bool AvHTechNodes::GetAllowMultiples(AvHMessageID& inMessageID) const
{
	bool theAllowMultiples = false;

	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetMessageID() == inMessageID)
		{
			theAllowMultiples = theIter->GetAllowMultiples();
			break;
		}
	}

	return theAllowMultiples; 
}

bool AvHTechNodes::GetIsMessageInTechTree(AvHMessageID& inMessageID) const
{
    bool theMessageIsInTechTree = false;

    // Message must be in the tech tree
    for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
    {
        if(theIter->GetMessageID() == inMessageID)
        {
            theMessageIsInTechTree = true;
            break;
        }
    }

    if(!theMessageIsInTechTree)
    {
        int a = 0;
    }

    return theMessageIsInTechTree;
}

bool AvHTechNodes::GetIsMessageAvailable(AvHMessageID& inMessageID) const
{
	bool theMessageIsAvailable = false;

	// Get prereqs for message
    AvHTechID thePrereqOne = TECH_NULL;
	AvHTechID thePrereqTwo = TECH_NULL;

	// Are they both available?
	if(this->GetPrequisiteForMessage(inMessageID, thePrereqOne, thePrereqTwo))
	{
	    if((thePrereqOne == TECH_NULL) || (this->GetIsTechResearched(thePrereqOne)))
	    {
		    if((thePrereqTwo == TECH_NULL) || (this->GetIsTechResearched(thePrereqTwo)))
		    {
			    theMessageIsAvailable = true;
		    }
	    }
	}
	else
	{
	    theMessageIsAvailable = true;
	}

	// Non-multiple research not available if researched
	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
	    if(theIter->GetMessageID() == inMessageID)
	    {
		    if(!theIter->GetAllowMultiples() && theIter->GetIsResearched())
		    {
			    theMessageIsAvailable = false;
			    break;
		    }
	    }
	}

	return theMessageIsAvailable;
}

bool AvHTechNodes::GetIsMessageAvailableForSelection(AvHMessageID& inMessageID, EntityListType& inSelection) const
{
	bool theMessageIsAvailable = this->GetIsMessageAvailable(inMessageID);
	return theMessageIsAvailable;
}

bool AvHTechNodes::GetIsTechResearched(AvHTechID inTech) const
{
	bool theTechIsResearched = false;
	
	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetTechID() == inTech)
		{
			theTechIsResearched = theIter->GetIsResearched();
			break;
		}
	}
	
	return theTechIsResearched;
}

bool AvHTechNodes::SetFirstNodeWithTechResearchState(AvHTechID inTech, bool inState)
{
	bool theSuccess = false;
	
	for(TechNodeListType::iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetTechID() == inTech)
		{
			theIter->SetResearchState(inState);
			theSuccess = true;
			break;
		}
	}
	
	return theSuccess;
}

bool AvHTechNodes::GetMessageForTech(const AvHTechID inTechID, AvHMessageID& outMessageID) const
{
	bool theSuccess = false;
	
	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetTechID() == inTechID)
		{
			outMessageID = theIter->GetMessageID();
			theSuccess = true;
			break;
		}
	}
	
	return theSuccess;
}

AvHMessageID AvHTechNodes::GetNextMessageNeededFor(AvHMessageID inMessageID) const
{
	AvHMessageID theNextMessage = inMessageID;

	// while theNextMessage isn't available for research
	bool theIsDone = false;

	do 
	{
		int theBottleNeckCost = 0;
		float theBottleNeckTime = 0;

		bool theIsAvailable = false;
		this->GetResearchInfo(theNextMessage, theIsAvailable, theBottleNeckCost, theBottleNeckTime);

		AvHTechID theTechID = TECH_NULL;
		if(this->GetTechForMessage(theNextMessage, theTechID))
		{
			// Looking for first tech that isn't researched, but has prereqs researched or NULL
			if(!this->GetIsTechResearched(theTechID))
			{
				do 
				{
					AvHTechID thePrereqID1;
					AvHTechID thePrereqID2;
					if(this->GetPrequisiteForMessage(theNextMessage, thePrereqID1, thePrereqID2))
					{
						// If both prereqs are researched or NULL, then theNextMessage is right
						for(int i = 0; i < 2; i++)
						{
							AvHTechID theCurrentPrereq = ((i == 0) ? thePrereqID1 : thePrereqID2);
							if((theCurrentPrereq == TECH_NULL) || this->GetIsTechResearched(theCurrentPrereq))
							{
								theIsDone = true;
							}
							// else dig deeper
							else
							{
								if(this->GetMessageForTech(theCurrentPrereq, theNextMessage))
								{
									i = 2;
								}
							}
						}
					}
					else
					{
						theIsDone = true;
					}
				}
				while (!theIsDone);
			}
			else
			{
				theIsDone = true;
			}
		}
		else
		{
			theIsDone = true;
		}
	} 
	while(!theIsDone);

	return theNextMessage;
}

void AvHTechNodes::GetResearchNodesDependentOn(AvHTechID inTechID, TechNodeListType& outTechNodes) const
{
	if(inTechID != TECH_NULL)
	{
		// Add all techs that directly depend on this one
		for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
		{
            AvHMessageID theMessageID = theIter->GetMessageID();

			if(AvHSHUGetIsResearchTech(theMessageID) && ((theIter->GetPrereqTechID1() == inTechID) || (theIter->GetPrereqTechID2() == inTechID)))
			{
				AvHTechID theDependentID = theIter->GetTechID();
				if(theDependentID != TECH_NULL)
				{
				    if(std::find(outTechNodes.begin(), outTechNodes.end(), *theIter) == outTechNodes.end())
				    {
					    // Add node
					    outTechNodes.push_back(*theIter);

					    // Add research nodes that are recursively dependent on this one
					    this->GetResearchNodesDependentOn(theDependentID, outTechNodes);
                    }
				}
			}
		}
	}
}

int	AvHTechNodes::GetNumNodes() const
{
	return this->mTechNodes.size();
}

bool AvHTechNodes::GetTechForMessage(const AvHMessageID inMessageID, AvHTechID& outTechID) const
{
	bool theSuccess = false;
	
	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetMessageID() == inMessageID)
		{
			outTechID = theIter->GetTechID();
			theSuccess = true;
			break;
		}
	}
	
	return theSuccess;
}

const TechNodeListType& AvHTechNodes::GetTechNodes() const
{
	return this->mTechNodes;
}

bool AvHTechNodes::GetPrequisiteForMessage(const AvHMessageID inMessageID, AvHTechID& outTech1, AvHTechID& outTech2) const
{
	bool theTechHasPrereq = false;
	
	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetMessageID() == inMessageID)
		{
			AvHTechID thePrereqTechID1 = theIter->GetPrereqTechID1();
			AvHTechID thePrereqTechID2 = theIter->GetPrereqTechID2();
			if((thePrereqTechID1 != TECH_NULL) || (thePrereqTechID2 != TECH_NULL))
			{
				outTech1 = thePrereqTechID1;
				outTech2 = thePrereqTechID2;
				theTechHasPrereq = true;
			}
			break;
		}
	}
	
	return theTechHasPrereq;
}

int	AvHTechNodes::GetNumTechNodes() const
{
	return this->mTechNodes.size();
}

bool AvHTechNodes::GetResearchInfo(AvHMessageID inTech, bool& outIsResearchable, int& outCost, float& outTime) const
{
	bool theFoundIt = false;
	
	outIsResearchable = false;
	outCost = -1;
	outTime = -1;

	// Run through list, remembering previous node
	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		// If node found, make sure previous node is empty or researched
		//if(theIter->GetTechID() == inTech)
		if(theIter->GetMessageID() == inTech)
		{
			// Fill in info
			theFoundIt = true;

			outCost = theIter->GetCost();
			outTime = (float)theIter->GetBuildTime();
			outIsResearchable = theIter->GetIsResearchable();
			
			//			AvHTechID thePrereq = theIter->GetPrereqTechID();
			//			if((thePrereq == TECH_NULL) || (this->GetIsTechResearched(thePrereq)))
			//			{
			//				outIsResearchable = true;
			//			}
			break;
		}
	}
	
	return theFoundIt;
}

bool AvHTechNodes::GetTechNode(int inOffset, AvHTechNode& outTechNode) const
{
	bool theFoundTechNode = false;
	
	if(inOffset < (signed)this->mTechNodes.size())
	{
		outTechNode = this->mTechNodes[inOffset];
		theFoundTechNode = true;
	}
	
	return theFoundTechNode;
}

bool AvHTechNodes::GetTechNode(AvHMessageID inMessageID, AvHTechNode& outTechNode) const
{
	bool theSuccess = false;

	for(TechNodeListType::const_iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetMessageID() == inMessageID)
		{
			outTechNode = *theIter;
			theSuccess = true;
			break;
		}
	}

	return theSuccess;
}

bool AvHTechNodes::SetIsResearchable(AvHMessageID inMessageID, bool inState)
{
    bool theSuccess = false;

    for(TechNodeListType::iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
    {
        if(theIter->GetMessageID() == inMessageID)
        {
            theIter->SetResearchable(inState);
            theSuccess = true;
            break;
        }
    }

    return theSuccess;
}

bool AvHTechNodes::SetResearchDone(AvHMessageID inMessageID, bool inState)
{
	bool theSuccess = false;
	
	for(TechNodeListType::iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetMessageID() == inMessageID)
		{
			theIter->SetResearchState(inState);
			theSuccess = true;
			break;
		}
	}
	
	return theSuccess;
}


bool AvHTechNodes::SetTechNode(int inOffset, const AvHTechNode& inTechNode)
{
	bool theSuccess = false;
	
	if(inOffset < (signed)this->mTechNodes.size())
	{
		this->mTechNodes[inOffset] = inTechNode;
		theSuccess = true;
	}
	else if(inOffset == (signed)this->mTechNodes.size())
	{
		this->mTechNodes.push_back(inTechNode);
		theSuccess = true;
	}
	else
	{
		ASSERT(false);
	}
	
	return theSuccess;
}

void AvHTechNodes::TriggerAddTech(AvHTechID inTechID)
{
	for(TechNodeListType::iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetTechID() == inTechID)
		{
			theIter->SetResearchState(true);
		}
	}
}

void AvHTechNodes::TriggerRemoveTech(AvHTechID inTechID)
{
	for(TechNodeListType::iterator theIter = this->mTechNodes.begin(); theIter != this->mTechNodes.end(); theIter++)
	{
		if(theIter->GetTechID() == inTechID)
		{
			theIter->SetResearchState(false);
		}
	}
}

bool AvHTechNodes::operator!=(const AvHTechNodes& inTechNodes) const
{
	return !this->operator==(inTechNodes);
}

bool AvHTechNodes::operator==(const AvHTechNodes& inTechNodes) const
{
	bool theAreEqual = false;
	
	if(this->mTechNodes == inTechNodes.mTechNodes)
	{
		theAreEqual = true;
	}
	
	return theAreEqual;
}

void AvHTechNodes::operator=(const AvHTechNodes& inTechNodes)
{
	this->mTechNodes = inTechNodes.mTechNodes;
}

//#ifdef AVH_SERVER
//void AvHTechNodes::SendToNetworkStream() const
//{
//	// send num nodes
//	int theNumNodes = this->mTechNodes.size();
//	WRITE_SHORT(theNumNodes);
//	
//	// for each one
//	for(TechNodeListType::const_iterator theIterator = this->mTechNodes.begin(); theIterator != this->mTechNodes.end(); theIterator++)
//	{
//		theIterator->SendToNetworkStream();
//	}
//}
//#endif						
//
//#ifdef AVH_CLIENT
//int	AvHTechNodes::ReceiveFromNetworkStream()
//{
//	int theBytesRead = 0;
//
//	this->Clear();
//
//	int theNumNodes = READ_SHORT();
//	theBytesRead += 2;
//
//	for(int i = 0; i < theNumNodes; i++)
//	{
//		AvHTechNode theTechNode;
//		theBytesRead += theTechNode.ReceiveFromNetworkStream();
//		this->mTechNodes.push_back(theTechNode);
//	}
//	
//	return theBytesRead;
//}
//#endif
	






















