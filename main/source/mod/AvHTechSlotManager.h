//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechSlotManager.h $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#ifndef TECHSLOTMANAGER_H
#define TECHSLOTMANAGER_H

#include "util/nowarnings.h"
#include "types.h"
#include "build.h"
#include "mod/AvHMessage.h"
#include "mod/AvHSpecials.h"

#ifdef AVH_CLIENT
#include "cl_dll/util_vector.h"
#include "common/const.h"
#include "engine/progdefs.h"
#include "cl_dll/parsemsg.h"
#endif

#ifdef AVH_SERVER
#include "extdll.h"
#include "util.h"
#endif

const int kNumTechSlots = 8;
class AvHTechSlots
{
public:
					AvHTechSlots();
					AvHTechSlots(AvHUser3 inUser3, AvHMessageID inMessageID1 = MESSAGE_NULL, AvHMessageID inMessageID2 = MESSAGE_NULL, AvHMessageID inMessageID3 = MESSAGE_NULL, AvHMessageID inMessageID4 = MESSAGE_NULL, AvHMessageID inMessageID5 = MESSAGE_NULL, AvHMessageID inMessageID6 = MESSAGE_NULL, AvHMessageID inMessageID7 = MESSAGE_NULL, AvHMessageID inMessageID8 = MESSAGE_NULL, bool inReceivesUpgrades = false);

	bool			operator==(const AvHTechSlots& inTechSlot) const;
	bool			operator!=(const AvHTechSlots& inTechSlot) const;
	void			operator=(const AvHTechSlots& inTechSlot);

#ifdef AVH_CLIENT
	int32			ReceiveFromNetworkStream();
#endif
	
#ifdef AVH_SERVER
    void			SendToNetworkStream();
#endif

	AvHUser3		mUser3;
	AvHMessageID	mTechSlots[kNumTechSlots];

#ifdef AVH_SERVER
	bool			mReceivesUpgrades;
#endif
};

typedef vector<AvHTechSlots>		AvHTechSlotListType;

class AvHTechSlotManager
{
public:
	void							AddTechSlots(AvHTechSlots inTechSlots);

	void							Clear();

	const AvHTechSlotListType&		GetTechSlotList() const;
	AvHTechSlotListType&			GetTechSlotList();
	
	bool							GetTechSlotList(AvHUser3 inUser3, AvHTechSlots& outTechSlotList) const;
									
private:							
	AvHTechSlotListType				mTechSlotList;
	
};

#endif