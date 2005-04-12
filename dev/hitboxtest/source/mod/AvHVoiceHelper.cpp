//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHVoiceHelper.cpp $
// $Date: 2002/11/15 23:32:34 $
//
//-------------------------------------------------------------------------------
// $Log: AvHVoiceHelper.cpp,v $
// Revision 1.9  2002/11/15 23:32:34  Flayra
// - Voice changes to make tourny mode and observation smoother
//
// Revision 1.8  2002/09/09 20:09:49  Flayra
// - Tried to fix bug where players can sometimes hear players on the opposite team after they die
// - Removed old NSTR preprocessor directives
//
// Revision 1.7  2002/07/24 18:55:53  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.6  2002/07/10 14:46:09  Flayra
// - Removed unnecessary code, hopefully fixing bug #274
//
// Revision 1.5  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHVoiceHelper.h"
#include "mod/AvHGamerules.h"
#include "pm_shared/pm_shared.h"

AvHVoiceHelper gVoiceHelper;

bool AvHVoiceHelper::CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker)
{
	AvHPlayer* theListener = dynamic_cast<AvHPlayer*>(pListener);
	ASSERT(theListener);

	AvHPlayer* theTalker = dynamic_cast<AvHPlayer*>(pTalker);
	ASSERT(theTalker);

	// Don't allow talking during countdown, to allow players to record demos, and for dramatic effect
//	if(GetGameRules()->GetCountdownStarted() && !GetGameRules()->GetGameStarted())
//	{
//		return false;
//	}

	// If commander is talking and has crouch key held, only selected players can hear him
	if(theTalker->GetIsInTopDownMode())
	{
		if(theTalker->GetTeam() == theListener->GetTeam())
		{
			bool theCrouchKeyHeld = (theTalker->pev->button & IN_DUCK);
			if(theCrouchKeyHeld)
			{
				if(theTalker->GetIsSelected(theListener->entindex()))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	// Players on a team can only hear other players on their team
	if((theListener->GetTeam() == theTalker->GetTeam()) && (theListener->GetTeam() != TEAM_IND))
	{
		return true;
	}
	
	// Players in the ready room can only hear other players in the ready room
	if((theListener->GetPlayMode() == PLAYMODE_READYROOM) && (theTalker->GetPlayMode() == PLAYMODE_READYROOM))
	{
		return true;
	}

	// Spectators can always hear other spectators
	if(theListener->GetPlayMode() == PLAYMODE_OBSERVER)
	{
		if(theTalker->GetPlayMode() == PLAYMODE_OBSERVER)
		{
			return true;
		}
		
		// If spectator is spectating first-person in casual mode, he can hear other members of this team
		if((theListener->pev->iuser1 == OBS_IN_EYE) && !GetGameRules()->GetIsTournamentMode())
		{
			if(theListener->GetTeam(true) == theTalker->GetTeam())
			{
				return true;
			}
		}
	}

	return false;
}


