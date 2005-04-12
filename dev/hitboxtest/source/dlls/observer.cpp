//=========== (C) Copyright 1996-2002, Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Functionality for the observer chase camera
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "pm_shared.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHGamerules.h"

const float kNextTargetInterval = .2f;

// Find the next client in the game for this player to spectate
bool CBasePlayer::Observer_FindNextPlayer(bool inReverse)
{
	// MOD AUTHORS: Modify the logic of this function if you want to restrict the observer to watching
	//				only a subset of the players. e.g. Make it check the target's team.
	bool theSuccess = false;
	int	iStart;
	if ( m_hObserverTarget )
		iStart = ENTINDEX( m_hObserverTarget->edict() );
	else
		iStart = ENTINDEX( edict() );
	int	iCurrent = iStart;
	m_hObserverTarget = NULL;

	if(this->entindex() == 1)
	{
		int a = 0;
	}

	int iDir = inReverse ? -1 : 1; 
	AvHPlayer* theThisAvHPlayer = dynamic_cast<AvHPlayer*>(this);

	do
	{
		iCurrent += iDir;

		// Loop through the clients
		if (iCurrent > gpGlobals->maxClients)
			iCurrent = 1;
		if (iCurrent < 1)
			iCurrent = gpGlobals->maxClients;

		CBaseEntity *pEnt = UTIL_PlayerByIndex( iCurrent );
		if ( !pEnt )
			continue;
		if ( pEnt == this )
			continue;
		// Don't spec observers or invisible players
		if ( ((CBasePlayer*)pEnt)->IsObserver() || (pEnt->pev->effects & EF_NODRAW))
			continue;

		// MOD AUTHORS: Add checks on target here.
		AvHPlayer* thePotentialTargetPlayer = dynamic_cast<AvHPlayer*>(pEnt);
		if(theThisAvHPlayer && thePotentialTargetPlayer)
		{
			if(!thePotentialTargetPlayer->GetIsRelevant())
			{
				continue;
			}

			// Don't allow spectating of other team when we could come back in
			if(((theThisAvHPlayer->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT) || (theThisAvHPlayer->GetPlayMode() == PLAYMODE_REINFORCING)) && (thePotentialTargetPlayer->pev->team != this->pev->team))
			{
				continue;
			}

			if(theThisAvHPlayer->GetPlayMode() != PLAYMODE_OBSERVER)
			{
				if(GetGameRules()->GetIsTournamentMode() && (thePotentialTargetPlayer->pev->team != this->pev->team) && (this->pev->team != TEAM_IND))
				{
					continue;
				}
			}
			
			// Don't allow spectating of players in top down mode
			if(thePotentialTargetPlayer->GetIsInTopDownMode())
			{
				continue;
			}
			
			// Don't allow spectating opposite teams in tournament mode
			//if(GetGameRules()->GetIsTournamentMode())
			//{
			//}

			m_hObserverTarget = pEnt;
		}
		break;

	} while ( iCurrent != iStart );

	// Did we find a target?
	if ( m_hObserverTarget )
	{
		// Store the target in pev so the physics DLL can get to it
		if (pev->iuser1 != OBS_ROAMING)
			pev->iuser2 = ENTINDEX( m_hObserverTarget->edict() );
		// Move to the target
		UTIL_SetOrigin( pev, m_hObserverTarget->pev->origin );

		//ALERT( at_console, "Now Tracking %s\n", STRING( m_hObserverTarget->pev->classname ) );
		m_flNextObserverInput = gpGlobals->time + kNextTargetInterval;

		theSuccess = true;
	}
	else
	{
		//ALERT( at_console, "No observer targets.\n" );
	}
	return theSuccess;
}

void CBasePlayer::Observer_SpectatePlayer(int index)
{

    CBaseEntity* pEnt = UTIL_PlayerByIndex(index);
    
    AvHPlayer* thePotentialTargetPlayer = dynamic_cast<AvHPlayer*>(pEnt);
    AvHPlayer* theThisAvHPlayer = dynamic_cast<AvHPlayer*>(this);

    if (theThisAvHPlayer == NULL || thePotentialTargetPlayer == NULL)
    {
        return;
    }
    
	if(!thePotentialTargetPlayer->GetIsRelevant())
	{
		return;
	}

	// Don't allow spectating of other team when we could come back in
	if(((theThisAvHPlayer->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT) || (theThisAvHPlayer->GetPlayMode() == PLAYMODE_REINFORCING)) && (thePotentialTargetPlayer->pev->team != this->pev->team))
	{
		return;
	}

	if(theThisAvHPlayer->GetPlayMode() != PLAYMODE_OBSERVER)
	{
		if(GetGameRules()->GetIsTournamentMode() && (thePotentialTargetPlayer->pev->team != this->pev->team) && (this->pev->team != TEAM_IND))
		{
			return;
		}
	}
	
	// Don't allow spectating of players in top down mode
	if(thePotentialTargetPlayer->GetIsInTopDownMode())
	{
		return;
	}
	
	m_hObserverTarget = pEnt;
    pev->iuser2 = index;

}

// Handle buttons in observer mode
void CBasePlayer::Observer_HandleButtons()
{

    // Removed by mmcguire.
    // This is all handled through the client side UI.

    /*
	// Slow down mouse clicks
	if ( m_flNextObserverInput > gpGlobals->time )
		return;

	// Only do this if spectating
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this);
	ASSERT(thePlayer);
	bool theIsObserving = (thePlayer->GetPlayMode() == PLAYMODE_OBSERVER);
		
	// Jump changes from modes: Chase to Roaming
	if ( m_afButtonPressed & IN_JUMP )
	{
		if ( pev->iuser1 == OBS_CHASE_LOCKED )
		{
			Observer_SetMode( OBS_CHASE_FREE );
		}
		else if ( pev->iuser1 == OBS_CHASE_FREE )
		{
			if(theIsObserving)
			{
				Observer_SetMode(OBS_ROAMING);
			}
			else
			{
				Observer_SetMode(OBS_IN_EYE);
			}
		}
		else if ( pev->iuser1 == OBS_ROAMING )
		{
			Observer_SetMode( OBS_IN_EYE );
		}
		else if ( pev->iuser1 == OBS_IN_EYE )
		{
			if(theIsObserving)
			{
				Observer_SetMode(OBS_MAP_FREE);
			}
			else
			{
				Observer_SetMode(OBS_CHASE_LOCKED);
			}
		}
		else if ( pev->iuser1 == OBS_MAP_FREE )
		{
			Observer_SetMode( OBS_MAP_CHASE );
		}
		else
		{
			Observer_SetMode( OBS_CHASE_FREE );	// don't use OBS_CHASE_LOCKED anymore
		}

		m_flNextObserverInput = gpGlobals->time + kNextTargetInterval;
	}

	// Attack moves to the next player
	if ( (m_afButtonPressed & IN_ATTACK) || ((m_afButtonPressed & IN_MOVERIGHT) && (pev->iuser1 != OBS_ROAMING)) )
	{
		if(Observer_FindNextPlayer( false ))
		{
			m_flNextObserverInput = gpGlobals->time + kNextTargetInterval;
		}
		else
		{
			Observer_SetMode( OBS_CHASE_FREE );
		}
	}

	// Attack2 moves to the prev player
	if ( (m_afButtonPressed & IN_ATTACK2) || ((m_afButtonPressed & IN_MOVELEFT) && (pev->iuser1 != OBS_ROAMING)) )
	{
		if(Observer_FindNextPlayer( true ))
		{
			m_flNextObserverInput = gpGlobals->time + kNextTargetInterval;
		}
		else
		{
			Observer_SetMode( OBS_CHASE_FREE );
		}
	}
    */

}

// Attempt to change the observer mode
void CBasePlayer::Observer_SetMode( int iMode )
{
	// Limit some modes if we're not observing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this);
	ASSERT(thePlayer);

    bool theIsObserving = (thePlayer->GetPlayMode() == PLAYMODE_OBSERVER);
    
    if(!theIsObserving)
	{
		if((iMode == OBS_ROAMING) /*|| (iMode == OBS_MAP_FREE) || (iMode == OBS_MAP_CHASE)*/)
		{
			return;
		}
	}

	// Just abort if we're changing to the mode we're already in
	if ( iMode == pev->iuser1 )
		return;

    // Removed by mmcguire.
    
	// is valid mode ?
	if ( iMode < OBS_CHASE_LOCKED || iMode > OBS_IN_EYE )
		iMode = OBS_IN_EYE; // now it is

    // if we are not roaming, we need a valid target to track
	if ( (iMode != OBS_ROAMING) && (m_hObserverTarget == NULL) )
	{
		Observer_FindNextPlayer();

		// if we didn't find a valid target switch to roaming
		if (m_hObserverTarget == NULL)
		{
			iMode = OBS_ROAMING;
		}
	}

	// set spectator mode
	pev->iuser1 = iMode;

	// set target if not roaming
	if (iMode == OBS_ROAMING)
		pev->iuser2 = 0;
	else
		pev->iuser2 = ENTINDEX( m_hObserverTarget->edict() );

	// Make sure our target is valid (go backward then forward)
	Observer_FindNextPlayer(true);
	Observer_FindNextPlayer(false);
}