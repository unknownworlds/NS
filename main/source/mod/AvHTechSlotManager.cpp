//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechSlotManager.cpp $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#include <stdio.h>
#include "mod/AvHTechSlotManager.h"

AvHTechSlots::AvHTechSlots()
{
	this->mUser3 = AVH_USER3_NONE;
	
	for(int i=0; i < kNumTechSlots; i++)
	{
		this->mTechSlots[i] = MESSAGE_NULL;
	}

#ifdef AVH_SERVER
	this->mReceivesUpgrades = false;
#endif
}

AvHTechSlots::AvHTechSlots(AvHUser3 inUser3, AvHMessageID inMessageID1, AvHMessageID inMessageID2, AvHMessageID inMessageID3, AvHMessageID inMessageID4, AvHMessageID inMessageID5, AvHMessageID inMessageID6, AvHMessageID inMessageID7, AvHMessageID inMessageID8, bool inReceivesUpgrades)
{
	this->mUser3 = inUser3;
	this->mTechSlots[0] = inMessageID1;
	this->mTechSlots[1] = inMessageID2;
	this->mTechSlots[2] = inMessageID3;
	this->mTechSlots[3] = inMessageID4;
	this->mTechSlots[4] = inMessageID5;
	this->mTechSlots[5] = inMessageID6;
	this->mTechSlots[6] = inMessageID7;
	this->mTechSlots[7] = inMessageID8;

#ifdef AVH_SERVER
	this->mReceivesUpgrades = inReceivesUpgrades;
#endif
}

bool AvHTechSlots::operator==(const AvHTechSlots& inTechSlot) const
{
	bool theAreEqual = false;

	if(this->mUser3 == inTechSlot.mUser3)
	{
		if(!memcmp(this->mTechSlots, inTechSlot.mTechSlots, kNumTechSlots*sizeof(AvHMessageID)))
		{
			#ifdef AVH_SERVER
			if(this->mReceivesUpgrades == inTechSlot.mReceivesUpgrades)
			{
			#endif
				theAreEqual = true;
			#ifdef AVH_SERVER
			}
			#endif
		}
	}

	return theAreEqual;
}

bool AvHTechSlots::operator!=(const AvHTechSlots& inTechSlot) const
{
	return !this->operator==(inTechSlot);
}

void AvHTechSlots::operator=(const AvHTechSlots& inTechSlot)
{
	this->mUser3 = inTechSlot.mUser3;
	memcpy(this->mTechSlots, inTechSlot.mTechSlots, kNumTechSlots*sizeof(AvHMessageID));

#ifdef AVH_SERVER
	this->mReceivesUpgrades = inTechSlot.mReceivesUpgrades;
#endif
}

#ifdef AVH_CLIENT
int32 AvHTechSlots::ReceiveFromNetworkStream()
{
	int32 theBytesRead = 0;

	int theUser3 = READ_BYTE();
	this->mUser3 = (AvHUser3)theUser3;
	theBytesRead++;

	for(int i = 0; i < kNumTechSlots; i++)
	{
		this->mTechSlots[i] = (AvHMessageID)READ_BYTE();
		theBytesRead++;
	}

	return theBytesRead;
}
#endif

#ifdef AVH_SERVER
void AvHTechSlots::SendToNetworkStream()
{
	WRITE_BYTE(this->mUser3);

	for(int i=0; i<kNumTechSlots; i++)
	{
		WRITE_BYTE(this->mTechSlots[i]);
	}
}
#endif


void AvHTechSlotManager::AddTechSlots(AvHTechSlots inTechSlots)
{
	// Check to make sure we don't have something with this user3 already
	for(AvHTechSlotListType::iterator theIter = this->mTechSlotList.begin(); theIter != this->mTechSlotList.end(); theIter++)
	{
		if(theIter->mUser3 == inTechSlots.mUser3)
		{
			//ASSERT(false);
			// Remove existing one with this user3
			this->mTechSlotList.erase(theIter);
			break;
		}
	}
	
	this->mTechSlotList.push_back(inTechSlots);
}

void AvHTechSlotManager::Clear()
{
	this->mTechSlotList.clear();
}

const AvHTechSlotListType& AvHTechSlotManager::GetTechSlotList() const
{
	return this->mTechSlotList;
}

AvHTechSlotListType& AvHTechSlotManager::GetTechSlotList()
{
	return this->mTechSlotList;
}

bool AvHTechSlotManager::GetTechSlotList(AvHUser3 inUser3, AvHTechSlots& outTechSlotList) const
{
	bool theSuccess = false;
	
	// Check to make sure we don't have something with this user3 already
	for(AvHTechSlotListType::const_iterator theIter = this->mTechSlotList.begin(); theIter != this->mTechSlotList.end(); theIter++)
	{
		if(theIter->mUser3 == inUser3)
		{
			outTechSlotList = *theIter;
			theSuccess = true;
			break;
		}
	}

	return theSuccess;
}

