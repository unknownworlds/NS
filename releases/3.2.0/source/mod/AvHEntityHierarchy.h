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
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "mod/AvHTeam.h"
#endif

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

class MapEntity
{
public:
	MapEntity(void) : mUser3(AVH_USER3_NONE), mTeam(TEAM_IND), mX(0.0f), mY(0.0f), mAngle(0.0f), mSquadNumber(0), mUnderAttack(0) {}

    AvHUser3        mUser3;
    AvHTeamNumber   mTeam;
    float           mX;
    float           mY;
    float           mAngle;
    int             mSquadNumber;
	int				mUnderAttack;

    bool operator==(const MapEntity& e) const
    {
        return mUser3       == e.mUser3 &&
               mTeam        == e.mTeam  &&
               mX           == e.mX     &&
               mY           == e.mY     &&
               mAngle       == e.mAngle &&
			   mUnderAttack == e.mUnderAttack &&
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
	bool		SendToNetworkStream(AvHEntityHierarchy& inClientHierarchy, entvars_t* inPlayer, bool spectating);
	void		BuildFromTeam(const AvHTeam* inTeam, BaseEntityListType& inBaseEntityList);
	void		BuildForSpec(BaseEntityListType& inBaseEntityList);
	#endif

	bool		GetHasBaseLineBeenSent() const;
	void		SetBaseLineSent();

	bool		InsertEntity( const int inIndex, const MapEntity& inEntity );
	bool		DeleteEntity( const int inIndex );

	void		GetEntityInfoList(MapEntityMap& outList) const;
	
	bool		operator!=(const AvHEntityHierarchy& inHierarchy) const;
	bool		operator==(const AvHEntityHierarchy& inHierarchy) const;
	
	int			GetNumSensory() const;
	int			GetNumDefense() const;
	int			GetNumMovement() const;
private:
   

	// The encoded entity info that has been sent to clients

	MapEntityMap    mEntityList;
	int mNumMovement;
	int mNumSensory;
	int mNumDefence;
};

#endif