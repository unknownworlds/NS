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

#ifdef AVH_SERVER
#include "extdll.h"
#include "util.h"
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
#include "cl_dll/parsemsg.h"
#include "cl_dll/cl_util.h"
#endif

extern int gmsgUpdateEntityHierarchy;

////////////
// Shared //
////////////
AvHEntityHierarchy::AvHEntityHierarchy()
{
	this->Clear();
}

void AvHEntityHierarchy::Clear()
{
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

			// Don't send ammo, health, weapons, or scans
			bool theIsTransient = ((AvHUser3)(theBaseEntity->pev->iuser3) == AVH_USER3_MARINEITEM) || (theBaseEntity->pev->classname == MAKE_STRING(kwsScan));
		
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
	            float theMinMapX = theMapExtents.GetMinMapX();
	            float theMinMapY = theMapExtents.GetMinMapY();

                mapEntity.mX -= theMinMapX;
                mapEntity.mY -= theMinMapY; 
                
                mEntityList[theEntityIndex] = mapEntity;

            }
        
        }
    }

}

// Returns true when something was sent
bool AvHEntityHierarchy::SendToNetworkStream(AvHEntityHierarchy& inClientHierarchy, entvars_t* inPlayer)
{

	bool theDataSent = false;
			
	// This is calculated from the size of each entity sent (6 bytes), and the max network message size (around 192)
	const int kMaxEntitiesToSend = 30;
	bool theMessageStarted = false;

	// Get iterators for both hierarchies

	MapEntityMap::const_iterator clientIter; 
	MapEntityMap::const_iterator serverIter;
	
	clientIter = inClientHierarchy.mEntityList.begin();
	serverIter = mEntityList.begin();

	int numSubMessagesSent = 0;
	
	while (clientIter != inClientHierarchy.mEntityList.end() && serverIter != mEntityList.end())
	{

		MapEntity	theEntityInfo;
		int			theEntityIndex	= 0;
        bool        deleteFlag      = false;
		bool		sendFlag		= false;

		if (serverIter->first < clientIter->first)
		{
			
			// Send add
			theEntityIndex = serverIter->first;
			theEntityInfo  = serverIter->second;
			sendFlag       = true;
		
			++serverIter;
		
		}
		// If server entity index is greater, then send delete for client index
		else if (serverIter->first > clientIter->first)
		{
		
			theEntityIndex = clientIter->first;
            theEntityInfo  = clientIter->second;
			sendFlag       = true;
            deleteFlag     = true;

			++clientIter;
		
		}
		else	
		{
			if (clientIter->second != serverIter->second)
			{	
			
				// Send update
				theEntityIndex = serverIter->first;
				theEntityInfo  = serverIter->second;
				sendFlag       = true;

			}

			++serverIter;
			++clientIter;
		
		}

		if (sendFlag)
		{

			if (numSubMessagesSent == 0)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgUpdateEntityHierarchy, NULL, inPlayer);
			}

            WriteEntity(theEntityIndex, theEntityInfo, deleteFlag);
            
			++numSubMessagesSent;
			theDataSent = true;

			if (numSubMessagesSent == kMaxEntitiesToSend)
			{
				MESSAGE_END();
				numSubMessagesSent = 0;
			}

		}
	}

    while (serverIter != mEntityList.end())
    {
        
		// Send add
     
		if (numSubMessagesSent == 0)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgUpdateEntityHierarchy, NULL, inPlayer);
		}

        WriteEntity(serverIter->first, serverIter->second, false);

        ++numSubMessagesSent;
		theDataSent = true;

		if (numSubMessagesSent == kMaxEntitiesToSend)
		{
			MESSAGE_END();
			numSubMessagesSent = 0;
		}
        
        ++serverIter;

    }

    while (clientIter != inClientHierarchy.mEntityList.end())
    {
        
		// Send delete.

		EntityInfo	theEntityInfo  = 0; // 0 entity info means delete.
		int			theEntityIndex = clientIter->first;
     
		if (numSubMessagesSent == 0)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgUpdateEntityHierarchy, NULL, inPlayer);
		}

        WriteEntity(clientIter->first, clientIter->second, true);

		++numSubMessagesSent;
		theDataSent = true;

		if (numSubMessagesSent == kMaxEntitiesToSend)
		{
			MESSAGE_END();
			numSubMessagesSent = 0;
		}
        
        ++clientIter;

    }

    if (numSubMessagesSent != 0)
	{
		MESSAGE_END();
	}

	return theDataSent;

}


void AvHEntityHierarchy::WriteEntity(int theEntityIndex, const MapEntity& theEntityInfo, bool deleteFlag)
{

    short s;
    long  l;

    if (deleteFlag)
    {
        l = 0;
    }
    else
    {

	    ASSERT(theEntityInfo.mUser3 < pow(2, kNumStatusBits));

	    int theX = theEntityInfo.mX / kPositionNetworkConstant;
	    int theY = theEntityInfo.mY / kPositionNetworkConstant;

	    ASSERT(theX < pow(2, kNumPositionCoordinateBits));
	    ASSERT(theY < pow(2, kNumPositionCoordinateBits));

	    int px = theX << (kNumStatusBits + kNumPositionCoordinateBits + kNumTeamBits);
	    int py = theY << (kNumStatusBits + kNumTeamBits);

        ASSERT(theEntityInfo.mTeam < pow(2, kNumTeamBits));

        l = px | py | (theEntityInfo.mTeam << kNumStatusBits) | theEntityInfo.mUser3;

    }

    if (theEntityInfo.mUser3 == AVH_USER3_MARINE_PLAYER ||
	    theEntityInfo.mUser3 == AVH_USER3_COMMANDER_PLAYER  ||
	    theEntityInfo.mUser3 == AVH_USER3_ALIEN_PLAYER1 ||
	    theEntityInfo.mUser3 == AVH_USER3_ALIEN_PLAYER2 ||
	    theEntityInfo.mUser3 == AVH_USER3_ALIEN_PLAYER3 ||
	    theEntityInfo.mUser3 == AVH_USER3_ALIEN_PLAYER4 ||
	    theEntityInfo.mUser3 == AVH_USER3_ALIEN_PLAYER5 ||
	    theEntityInfo.mUser3 == AVH_USER3_ALIEN_EMBRYO)
    {
        
        int i = theEntityIndex;
        ASSERT(i > 0 && i <= 32);

        int a = (WrapFloat(theEntityInfo.mAngle, 0, 360) / 360.0f) * 16;
        ASSERT(a >= 0 && a < 16);

        int h = theEntityInfo.mSquadNumber;
        ASSERT(h >= 0 && h < 8);
        
        s = (h << 10) | (a << 6) | (i << 1) | 1;

    }
    else
    {
        s = (theEntityIndex << 1) | 0;
    }

    WRITE_LONG(l);
    WRITE_SHORT(s);
    

}


#endif
////////////////
// end server //
////////////////

////////////
// Client //
////////////
#ifdef AVH_CLIENT

void AvHEntityHierarchy::ReadEntity(int& outEntityIndex, MapEntity& outEntity, bool& outDeleteFlag)
{

    long  l = READ_LONG();
    short s = READ_SHORT();

    if (l == 0)
    {
        outDeleteFlag = true;
    }
    else
    {
        outDeleteFlag = false;
        
        outEntity.mX = ((l >> (kNumStatusBits + kNumPositionCoordinateBits + kNumTeamBits)) & kPositionCoordinateMask)*kPositionNetworkConstant;
        outEntity.mY = ((l >> (kNumStatusBits + kNumTeamBits)) & kPositionCoordinateMask)*kPositionNetworkConstant;
        outEntity.mUser3 = (AvHUser3)(l & kStatusMask);
        outEntity.mTeam  = (AvHTeamNumber)((l >> kNumStatusBits) & kTeamMask);
    
    }

    if (s & 0x1)
    {
        // Player.
        outEntityIndex = (s >> 1) & 31;
        outEntity.mAngle = ((s >> 6) & 15) * 360.0f / 16.0f;
        outEntity.mSquadNumber = (s >> 10) & 7;
    }
    else
    {
        outEntityIndex = (s >> 1);
        outEntity.mAngle = 0;
        outEntity.mSquadNumber = 0;
    }

}


int	AvHEntityHierarchy::ReceiveFromNetworkStream(int iSize, void *pbuf)
{

	int theBytesRead = 0;
	BEGIN_READ(pbuf, iSize);

	while (theBytesRead < iSize)
	{

		ASSERT(iSize - theBytesRead >= 6);

        int       theEntityIndex;
        MapEntity theEntityInfo;
        bool      deleteFlag;
        
        ReadEntity(theEntityIndex, theEntityInfo, deleteFlag);
		theBytesRead += 6;
        
        if (deleteFlag)
		{
			// Delete the entity.

			MapEntityMap::iterator entityIter = mEntityList.find(theEntityIndex);
			//ASSERT(entityIter != mEntityList.end());
			
			if(entityIter == mEntityList.end())
			{
				char theErrorMessage[512];
				sprintf(theErrorMessage, "ent %d can't be deleted", theEntityIndex);
				CenterPrint(theErrorMessage);
			}
			else
			{
				mEntityList.erase(entityIter);
			}
		
		}
		else
		{
			// Add or update the entity.
			mEntityList[theEntityIndex] = theEntityInfo;
			//mEntityList.insert(MapEntityMap::value_type(theEntityIndex, theEntityInfo));
		}

	}

	return theBytesRead;

}

#endif
////////////////
// end client //
////////////////

