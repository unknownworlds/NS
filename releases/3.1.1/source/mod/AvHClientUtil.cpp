//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHClientUtil.cpp $
// $Date: 2002/07/24 18:45:41 $
//
//-------------------------------------------------------------------------------
// $Log: AvHClientUtil.cpp,v $
// Revision 1.5  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.4  2002/07/23 16:59:41  Flayra
// - AvHCUWorldToScreen now returns true if the position is in front of player
//
// Revision 1.3  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHClientUtil.h"
#include "mod/AvHSpecials.h"
#include "common/vector_util.h"
#include "common/com_model.h"
#include "cl_dll/studio_util.h"
#include "cl_dll/r_studioint.h"
#include "engine/studio.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "common/cl_entity.h"
#include "common/triangleapi.h"
#include "cl_dll/vgui_TeamFortressViewport.h"
#include "util/STLUtil.h"

extern engine_studio_api_t	IEngineStudio;

int AvHCUGetIconHeightForPlayer(AvHUser3 theUser3)
{
	int theIconHeight = 45;
	
	switch(theUser3)
	{
	case AVH_USER3_ALIEN_EMBRYO:
	case AVH_USER3_ALIEN_PLAYER1:
	case AVH_USER3_ALIEN_PLAYER2:
	case AVH_USER3_ALIEN_PLAYER3:
		theIconHeight = 25;
		break;
		
	case AVH_USER3_ALIEN_PLAYER4:
		theIconHeight = 60;
		break;
	case AVH_USER3_ALIEN_PLAYER5:
		theIconHeight = 80;
		break;
	}
	
	return theIconHeight;
}

void AvHCUGetViewAngles(cl_entity_t* inEntity, float* outViewAngles)
{
	VectorCopy(inEntity->curstate.angles, outViewAngles);
	
	if(GetHasUpgrade(inEntity->curstate.iuser4, MASK_BUILDABLE))
	{
		if(inEntity->curstate.iuser3 == AVH_USER3_TURRET)
		{
			studiohdr_t* m_pStudioHeader = (studiohdr_t *)IEngineStudio.Mod_Extradata(inEntity->model);
			mstudiobonecontroller_t* pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pStudioHeader + m_pStudioHeader->bonecontrollerindex);
			
			// Get attachment 0 for angles
			//int theAngle = pbone->bonecontroller[0];
			mstudiobone_t* pbone = (mstudiobone_t *)((byte *)m_pStudioHeader + m_pStudioHeader->boneindex);
			pbone = pbone + pbonecontroller->bone;
			int theAngle = (int)(pbone->value[0]);
			if(theAngle != -1)
			{
				outViewAngles[1] = (float)theAngle;
			}
		}
	}
}

void AvHCUGetViewOrigin(cl_entity_t* inEntity, float* outViewOrigin)
{
	VectorCopy(inEntity->origin, outViewOrigin);
	
	if(GetHasUpgrade(inEntity->curstate.iuser4, MASK_BUILDABLE))
	{
		if(inEntity->curstate.iuser3 == AVH_USER3_TURRET)
		{
		}
	}
}

bool AvHCUWorldToScreen(float* inWorldCoords, float* outScreenCoords)
{
	bool theIsOnScreen = false;
	
	ASSERT(inWorldCoords);
	ASSERT(outScreenCoords);
	
	if(!gEngfuncs.pTriAPI->WorldToScreen(inWorldCoords, outScreenCoords))
	{
		outScreenCoords[0] = XPROJECT(outScreenCoords[0]);
		outScreenCoords[1] = YPROJECT(outScreenCoords[1]);
		outScreenCoords[2] = 0.0f;
		
		theIsOnScreen = true;
	}
	
	return theIsOnScreen;
}

bool AvHCUGetIsEntityInPVSAndVisible(int inEntityIndex)
{
	bool theReturnCode = false;

	cl_entity_t* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	cl_entity_s* theClient = gEngfuncs.GetEntityByIndex(inEntityIndex);
	if(theLocalPlayer && theClient)
	{
		// Don't show an icon if the player is not in our PVS.
		if(theClient->curstate.messagenum >= theLocalPlayer->curstate.messagenum)
		{
			// Don't show an icon for dead or spectating players (ie: invisible entities).
			if(!(theClient->curstate.effects & EF_NODRAW))
			{
				theReturnCode = true;
			}
		}
	}
	
	return theReturnCode;
}

void AvHCUTrimExtraneousLocationText(string& ioTranslatedString)
{
	// Trim extra whitespace, carriage-returns and newlines
	TrimString(ioTranslatedString);

	// Now trim off the "Hive location" before hand, everything up to the -
	string theChoppedTranslatedLocationName;
	int theStartChopIndex = (int)ioTranslatedString.find_first_of("-") + 1;
	if((theStartChopIndex < 0) || (theStartChopIndex >= (int)(ioTranslatedString.length())))
	{
		theStartChopIndex = 0;
	}

	int theLength = (int)ioTranslatedString.length() - theStartChopIndex;
	if(theLength > 0)
	{
		theChoppedTranslatedLocationName = ioTranslatedString.substr(theStartChopIndex, theLength);
	}

	ioTranslatedString = theChoppedTranslatedLocationName;

	// Remove any more spaces
	TrimString(ioTranslatedString);
}