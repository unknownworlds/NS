//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEntityHierarchy.h $
// $Date: 2002/06/25 17:55:49 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEntityHierarchy.h,v $
// Revision 1.3  2002/06/25 17:55:49  Flayra
// - Quick fix during playtesting...why is this happening?
//
//===============================================================================
#ifndef AVH_ENTITYHIERARCHY_H
#define AVH_ENTITYHIERARCHY_H

#include "types.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSpecials.h"

#ifdef AVH_SERVER
#include "util/nowarnings.h"
#include "extdll.h"
#include "dlls/util.h"
#include "cbase.h"
#include "mod/AvHTeam.h"
#endif

// Store flags in lower flag bits, big enough for 16 blip states (used for deciding color or sprite)
const int		kNumStatusBits = 6;
const int		kStatusMask = 0x3F;

const int       kNumTeamBits = 2;
const int       kTeamMask = 0x03;

// Assume flags will fire once, then die out after half a second or a second.
// Only need to send them if they weren't on last update and they are now
// Note that kFriendlyStatus needs to be greater then zero, so an EntityInfo of 0 can be used to send a "delete" message
const int		kFriendlyStatus = 1;
const int		kFriendlyPlayerStatus = 2;
const int		kEnemyStatus = 3;
const int		kEnemyPlayerStatus = 4;
const int		kFriendlyFiringStatus = 5;
const int		kFriendlyEnemySightedStatus = 6;
const int		kFriendlyOverwatchStatus = 7;
const int		kFriendlyRequestPendingStatus = 8;
const int		kFriendlyTakingDamageStatus = 9;
const int		kFriendlyDeadStatus = 10;
const int		kWeldableStatus = 11;
const int		kResourceNodeStatus = 12;
const int		kFriendlyBase = 13;
const int		kUnseenEnemyBase = 14;
const int		kVisibleEnemyBase = 15;
const int		kNumStatusTypes = 15;

// Entity is 9 bits, big enough for 512 entities (this probably doesn't need to be sent, remove?)
//const int		kNumEntityBits = 9;
//const int		kEntityMask = 0x3FE0;

// Position bits
// Need 512x512 resolution, so at least 9 bits for x, 9 bits for y, but the more the better (use top 18 bits)
// Position is encoded between min and max map extents
const int		kNumPositionCoordinateBits = 12;
const int		kNumPositionBits = kNumPositionCoordinateBits*2;
const int		kPositionCoordinateMask  = 0xFFF;

const int		kPositionNetworkConstant = 25;

struct MapEntity
{
    AvHUser3        mUser3;
    AvHTeamNumber   mTeam;
    float           mX;
    float           mY;
    float           mAngle;
    int             mSquadNumber;

    bool operator==(const MapEntity& e) const
    {
        return mUser3       == e.mUser3 &&
               mTeam        == e.mTeam  &&
               mX           == e.mX     &&
               mY           == e.mY     &&
               mAngle       == e.mAngle &&
               mSquadNumber == e.mSquadNumber;
    }

    bool operator!=(const MapEntity& e) const
    {
        return !(*this == e);
    }

};


typedef map<int, MapEntity> MapEntityMap;

// EntityInfo
// 18-bits position, 2 bits unused, 6 bits entity index, 6 bits flags

class AvHEntityHierarchy
{
public:

				AvHEntityHierarchy();
	void		Clear();
				
	#ifdef AVH_SERVER
	void		BuildFromTeam(const AvHTeam* inTeam, BaseEntityListType& inBaseEntityList);
	bool		SendToNetworkStream(AvHEntityHierarchy& inClientHierarchy, entvars_t* inPlayer);
    void        WriteEntity(int theEntityIndex, const MapEntity& theEntityInfo, bool deleteFlag);
	#endif

	#ifdef AVH_CLIENT
	int			ReceiveFromNetworkStream(int iSize, void *pbuf);
    void ReadEntity(int& outEntityIndex, MapEntity& outEntity, bool& outDeleteFlag);	
    #endif

	bool		GetHasBaseLineBeenSent() const;
	void		SetBaseLineSent();

	void		GetEntityInfoList(MapEntityMap& outList) const;
	
	bool		operator!=(const AvHEntityHierarchy& inHierarchy) const;
	bool		operator==(const AvHEntityHierarchy& inHierarchy) const;
	
private:
   

	// The encoded entity info that has been sent to clients

	MapEntityMap    mEntityList;

};

#endif