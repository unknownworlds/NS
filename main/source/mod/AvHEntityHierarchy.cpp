//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEntityHierarchy.cpp $
// $Date: 2002/10/16 00:53:31 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEntityHierarchy.cpp,v $
// Revision 1.10  2002/10/16 00:53:31  Flayra
// - Updated with new infinite loop data
//
// Revision 1.9  2002/07/25 16:57:59  flayra
// - Linux changes
//
// Revision 1.8  2002/07/24 21:16:36  Flayra
// - Linux issues
//
// Revision 1.7  2002/07/24 20:05:22  Flayra
// - Linux issues
//
// Revision 1.6  2002/07/24 19:44:05  Flayra
// - Linux issues
//
// Revision 1.5  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.4  2002/07/08 16:56:12  Flayra
// - Workaround to infinite loop encountered during playtest: look into this more
//
// Revision 1.3  2002/06/25 17:55:49  Flayra
// - Quick fix during playtesting...why is this happening?
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHEntityHierarchy.h"
#include "mod/AvHNetworkMessages.h"

#ifdef AVH_SERVER
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHTeam.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHSharedUtil.h"
#include "util/MathUtil.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/chud.h"
#include "cl_dll/cl_util.h"
#endif

////////////
// Shared //
////////////
AvHEntityHierarchy::AvHEntityHierarchy()
{
	this->Clear();
}

void AvHEntityHierarchy::Clear()
{
	mNumMovement=0;
	mNumSensory=0;
	mNumDefence=0;
	this->mEntityList.clear();
}

bool AvHEntityHierarchy::operator!=(const AvHEntityHierarchy& inHierarchy) const
{
	return !this->operator==(inHierarchy);
}

bool AvHEntityHierarchy::operator==(const AvHEntityHierarchy& inHierarchy) const
{
	bool theAreEqual = false;

	// NOTE: This could cause problems if entity hierarchy is stored in a hash or something
	if(this->mEntityList == inHierarchy.mEntityList)
	{
		theAreEqual = true;
	}

	return theAreEqual;
}

// Gets a list of all entity infos, with each one being a muxed entity index and flags
void AvHEntityHierarchy::GetEntityInfoList(MapEntityMap& outList) const
{
    outList = mEntityList;
}

int AvHEntityHierarchy::GetNumSensory() const {
	return this->mNumSensory;
}

int AvHEntityHierarchy::GetNumMovement() const {
	return this->mNumMovement;
}

int AvHEntityHierarchy::GetNumDefense() const {
	return this->mNumDefence;
}

////////////////
// end shared //
////////////////


////////////
// Server //
////////////
#ifdef AVH_SERVER

int GetHotkeyGroupContainingPlayer(AvHPlayer* player)
{

    AvHTeam* theTeam = player->GetTeamPointer();
    if(theTeam)
	{
		for (int i = 0; i < kNumHotkeyGroups; ++i)
		{

			EntityListType theGroup = theTeam->GetGroup(i);
    
			for (int j = 0; j < (signed)theGroup.size(); ++j)
			{
				if (theGroup[j] == player->entindex())
				{
					return i;
				}
			}
		}    
    }

    return -1;

}

void AvHEntityHierarchy::BuildFromTeam(const AvHTeam* inTeam, BaseEntityListType& inBaseEntityList)
{

	this->Clear();
	int mc=0, sc=0, dc=0;

  	if (inTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE ||
        inTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
	{
		
		// Loop through all entities in the world
		for(BaseEntityListType::iterator theIter = inBaseEntityList.begin(); theIter != inBaseEntityList.end(); theIter++)
		{			
            CBaseEntity* theBaseEntity = *theIter;

			int  theEntityIndex = theBaseEntity->entindex();
			bool theEntityIsVisible = (theBaseEntity->pev->team == (int)(inTeam->GetTeamNumber())) ||
                                       GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_VIS_SIGHTED);
			bool theEntityIsDetected = GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_VIS_DETECTED);
			bool theEntityIsUnderAttack = GetGameRules()->GetIsEntityUnderAttack(theEntityIndex);
			if ( theEntityIsUnderAttack )
			{
				int a=0;
			}
			// Don't send ammo, health, weapons, or scans
			bool theIsTransient = ((AvHUser3)(theBaseEntity->pev->iuser3) == AVH_USER3_MARINEITEM) || (theBaseEntity->pev->classname == MAKE_STRING(kwsScan));
		

			
			if ( inTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN && theBaseEntity->pev->team == (int)(inTeam->GetTeamNumber()) ) {
				AvHBuildable *theBuildable=dynamic_cast<AvHBuildable *>(theBaseEntity);
				if ( theBuildable && theBuildable->GetHasBeenBuilt() ) {
					if ( theBaseEntity->pev->iuser3 == AVH_USER3_MOVEMENT_CHAMBER ) {
						mc++;
					}
					if ( theBaseEntity->pev->iuser3 == AVH_USER3_SENSORY_CHAMBER ) { 
						sc++;
					}
					if ( theBaseEntity->pev->iuser3 == AVH_USER3_DEFENSE_CHAMBER ) {
						dc++;
					}
				}
			}
			
			MapEntity mapEntity;

            mapEntity.mX     = theBaseEntity->pev->origin.x;
            mapEntity.mY     = theBaseEntity->pev->origin.y;
            mapEntity.mUser3 = (AvHUser3)(theBaseEntity->pev->iuser3);

			// Don't draw detected blips as their real selves
			if(!theEntityIsVisible && theEntityIsDetected)
				mapEntity.mUser3 = AVH_USER3_UNKNOWN;

            mapEntity.mAngle = theBaseEntity->pev->angles[1];
            mapEntity.mTeam  = (AvHTeamNumber)(theBaseEntity->pev->team);
            mapEntity.mSquadNumber = 0;
			mapEntity.mUnderAttack = theEntityIsUnderAttack ? 1 : 0;

            bool sendEntity = false;

            if (mapEntity.mUser3 == AVH_USER3_HIVE)
            {
                if (!theEntityIsVisible)
                {
                    mapEntity.mTeam = TEAM_IND;
                }
                sendEntity = true;
            }
            else if (mapEntity.mUser3 == AVH_USER3_WELD)
            {
				vec3_t theEntityOrigin = AvHSHUGetRealLocation(theBaseEntity->pev->origin, theBaseEntity->pev->mins, theBaseEntity->pev->maxs);
                mapEntity.mX = theEntityOrigin.x;
                mapEntity.mY = theEntityOrigin.y;
                sendEntity = true;
            }
            else if (mapEntity.mUser3 == AVH_USER3_FUNC_RESOURCE)
            {
                sendEntity = true;
            }
            else if ((theEntityIsVisible || theEntityIsDetected) && !(theBaseEntity->pev->effects & EF_NODRAW) && !theIsTransient)
            {
                AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
                
                if (thePlayer)
                {
                    ASSERT(theEntityIndex > 0);
                    ASSERT(theEntityIndex <= 32);
                    mapEntity.mSquadNumber = GetHotkeyGroupContainingPlayer(thePlayer) + 1;

                    if ((thePlayer->GetPlayMode() == PLAYMODE_PLAYING) && !thePlayer->GetIsSpectator())
                    {

                        sendEntity = true;

                        // If the player has the heavy armor upgrade switch the
                        // user3 to something that will let us reconstruct that later.
 
                        if (thePlayer->pev->iuser3 == AVH_USER3_MARINE_PLAYER && 
                            GetHasUpgrade(thePlayer->pev->iuser4, MASK_UPGRADE_13))
                        {
                            mapEntity.mUser3 = AVH_USER3_HEAVY;
                        }
                       
                    }

                }
                else
                {
                    if (mapEntity.mUser3 != AVH_USER3_HEAVY)
                    {
                        sendEntity = true;
                    }
                }
                
            }

            if (sendEntity)
            {

	            const AvHMapExtents& theMapExtents = GetGameRules()->GetMapExtents();
//				commented this out here, commented out corresponding shift in AvHOverviewMap::Draw at line 771
//				float theMinMapX = theMapExtents.GetMinMapX();
//				float theMinMapY = theMapExtents.GetMinMapY();

//				mapEntity.mX -= theMinMapX;
//				mapEntity.mY -= theMinMapY; 
                
                mEntityList[theEntityIndex] = mapEntity;

            }
        }
		if ( inTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN ) {
			sc=min(3, sc);
			dc=min(3,dc);
			mc=min(3,mc);
			if ( this->mNumSensory != sc || this->mNumDefence != dc || this->mNumMovement != mc ) {
				this->mNumSensory=sc;
				this->mNumDefence=dc;
				this->mNumMovement=mc;
			}
		}
    }
}

void AvHEntityHierarchy::BuildForSpec(BaseEntityListType& inBaseEntityList)
{

	this->Clear();

	// Loop through all entities in the world
	for(BaseEntityListType::iterator theIter = inBaseEntityList.begin(); theIter != inBaseEntityList.end(); theIter++)
	{			
        CBaseEntity* theBaseEntity = *theIter;

		int  theEntityIndex = theBaseEntity->entindex();
		bool theEntityIsVisible = false;
		if ( theBaseEntity->pev->team == TEAM_ONE || theBaseEntity->pev->team == TEAM_TWO || theBaseEntity->pev->team == TEAM_THREE || theBaseEntity->pev->team == TEAM_FOUR )
			theEntityIsVisible=true;
		bool theEntityIsUnderAttack = GetGameRules()->GetIsEntityUnderAttack(theEntityIndex);
		// Don't send ammo, health, weapons, or scans
		bool theIsTransient = ((AvHUser3)(theBaseEntity->pev->iuser3) == AVH_USER3_MARINEITEM) || (theBaseEntity->pev->classname == MAKE_STRING(kwsScan));
	
		MapEntity mapEntity;

        mapEntity.mX     = theBaseEntity->pev->origin.x;
        mapEntity.mY     = theBaseEntity->pev->origin.y;
        mapEntity.mUser3 = (AvHUser3)(theBaseEntity->pev->iuser3);

        mapEntity.mAngle = theBaseEntity->pev->angles[1];
        mapEntity.mTeam  = (AvHTeamNumber)(theBaseEntity->pev->team);
        mapEntity.mSquadNumber = 0;
		mapEntity.mUnderAttack = theEntityIsUnderAttack ? 1 : 0;

        bool sendEntity = false;

        if (mapEntity.mUser3 == AVH_USER3_HIVE)
        {
            if (!theEntityIsVisible)
            {
                mapEntity.mTeam = TEAM_IND;
            }
            sendEntity = true;
        }
        else if (mapEntity.mUser3 == AVH_USER3_WELD)
        {
			vec3_t theEntityOrigin = AvHSHUGetRealLocation(theBaseEntity->pev->origin, theBaseEntity->pev->mins, theBaseEntity->pev->maxs);
            mapEntity.mX = theEntityOrigin.x;
            mapEntity.mY = theEntityOrigin.y;
            sendEntity = true;
        }
        else if (mapEntity.mUser3 == AVH_USER3_FUNC_RESOURCE)
        {
            sendEntity = true;
        }
        else if (theEntityIsVisible && !(theBaseEntity->pev->effects & EF_NODRAW) && !theIsTransient)
        {
            AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
            
            if (thePlayer)
            {
                ASSERT(theEntityIndex > 0);
                ASSERT(theEntityIndex <= 32);
                mapEntity.mSquadNumber = GetHotkeyGroupContainingPlayer(thePlayer) + 1;

                if ((thePlayer->GetPlayMode() == PLAYMODE_PLAYING) && !thePlayer->GetIsSpectator())
                {

                    sendEntity = true;

                    // If the player has the heavy armor upgrade switch the
                    // user3 to something that will let us reconstruct that later.

                    if (thePlayer->pev->iuser3 == AVH_USER3_MARINE_PLAYER && 
                        GetHasUpgrade(thePlayer->pev->iuser4, MASK_UPGRADE_13))
                    {
                        mapEntity.mUser3 = AVH_USER3_HEAVY;
                    }
                    
                }

            }
            else
            {
                if (mapEntity.mUser3 != AVH_USER3_HEAVY)
                {
                    sendEntity = true;
                }
            }
            
        }

        if (sendEntity)
        {
	        //const AvHMapExtents& theMapExtents = GetGameRules()->GetMapExtents();
//				commented this out here, commented out corresponding shift in AvHOverviewMap::Draw at line 771
//				float theMinMapX = theMapExtents.GetMinMapX();
//				float theMinMapY = theMapExtents.GetMinMapY();

//				mapEntity.mX -= theMinMapX;
//				mapEntity.mY -= theMinMapY; 
			
            mEntityList[theEntityIndex] = mapEntity;
			// debug the entity hierarchy message size
//			if ( mapEntity.mUser3 == AVH_USER3_COMMANDER_STATION ) {
//				for (int  i=0; i<200; i++ ) {
//					mEntityList[100+i] = mapEntity;
//				}
//			}
        }
    }
}

// Returns true when something was sent
bool AvHEntityHierarchy::SendToNetworkStream(AvHEntityHierarchy& inClientHierarchy, entvars_t* inPlayer, bool spectating)
{
	// Get iterators for both hierarchies

	MapEntityMap::const_iterator clientIter = inClientHierarchy.mEntityList.begin();
	MapEntityMap::const_iterator clientEnd = inClientHierarchy.mEntityList.end();
	MapEntityMap::iterator serverIter = mEntityList.begin();
	MapEntityMap::const_iterator serverEnd = mEntityList.end();

	// Create maps for changes
	MapEntityMap NewItems; //to be added or changed
	EntityListType OldItems; //to be deleted
	//TODO : replace OldItems with vector<int>

	int entityCount=0;
	// HACK
	// If we update more than kMaxSpecEntity then we need to clean out the remaining ones so they get sent next time.
	const int kMaxSpecEntity=100;
	while (clientIter != clientEnd && serverIter != serverEnd)
	{
		if ( entityCount < kMaxSpecEntity ) {
			if (serverIter->first < clientIter->first)
			{
				NewItems.insert( *serverIter );
				entityCount++;
				++serverIter;
				continue;
			}
			if (serverIter->first > clientIter->first)
			{
				OldItems.push_back( clientIter->first );		
				entityCount++;
				++clientIter;
				continue;
			}
			if (clientIter->second != serverIter->second ) 
			{	
				entityCount++;
				NewItems.insert( *serverIter );			
			}
			++serverIter;
			++clientIter;
		}
		else {
			mEntityList.erase(serverIter++);
		}
	}

	while(serverIter != serverEnd)
	{
		if ( entityCount < kMaxSpecEntity ) {
			entityCount++;
			NewItems.insert( *serverIter );
	        ++serverIter;
		}
		else {
			mEntityList.erase(serverIter++);
		}
    }

	while(clientIter != clientEnd)
	{
		if ( entityCount < kMaxSpecEntity ) {
			entityCount++;
			OldItems.push_back( clientIter->first );		
		}
		++clientIter;
	}

	ASSERT(entityCount < kMaxSpecEntity +1);
	NetMsg_UpdateEntityHierarchy( inPlayer, NewItems, OldItems, spectating );
	return (!NewItems.empty() || !OldItems.empty());
}

#endif
////////////////
// end server //
////////////////

bool AvHEntityHierarchy::InsertEntity( const int inIndex, const MapEntity &inEntity )
{
	this->mEntityList[inIndex] = inEntity;
	return true;
}

bool AvHEntityHierarchy::DeleteEntity( const int inIndex )
{
	MapEntityMap::iterator loc = this->mEntityList.find(inIndex);
	if( loc == this->mEntityList.end() )
	{ return false; }
	this->mEntityList.erase(loc);
	return true;
}
