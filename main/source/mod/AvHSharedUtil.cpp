//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose:
//
// $Workfile: AvHSharedUtil.cpp $
// $Date: 2002/11/12 22:39:25 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSharedUtil.cpp,v $
// Revision 1.33  2002/11/12 22:39:25  Flayra
// - Logging changes for Psychostats compatibility
//
// Revision 1.32  2002/10/24 21:42:32  Flayra
// - Don't give waypoint to selected troops when scanning
//
// Revision 1.31  2002/10/19 21:19:49  Flayra
// - Debugging info for linux
//
// Revision 1.30  2002/10/16 01:07:26  Flayra
// - Added official sizes that HL supports (ugh)
// - Added utility function for drawing range of ghost building, but it's unused
//
// Revision 1.29  2002/10/03 19:07:40  Flayra
// - Hives can never be blocked by func_resources
//
// Revision 1.28  2002/09/25 20:50:58  Flayra
// - Allow small items to be built on entities (health can be dropped right on players)
//
// Revision 1.27  2002/09/23 22:31:40  Flayra
// - Updated team 0 colors so dictation can be read in readyroom
// - Draw range for prototype lab, observatory and sensory chamber
// - Alien building rings
// - Don't allow non-resource buildings built too close to func_resources
// - Added heavy armor and jetpacks
//
// Revision 1.26  2002/09/09 20:06:43  Flayra
// - New observatory artwork
//
// Revision 1.25  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.24  2002/08/16 02:47:06  Flayra
// - Fixed bug where not all the entities were iterated through
// - Support for ring-drawing (but not for all entities)
//
// Revision 1.23  2002/08/09 00:51:11  Flayra
// - Cleaned up useless special casing of tracetangible, fixed some problems where buildings could be built on players
//
// Revision 1.22  2002/08/02 21:50:05  Flayra
// - Made more general, by detecting all entities, not just players
//
// Revision 1.21  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.20  2002/07/23 17:26:49  Flayra
// - Siege don't require a nearby turret factory, only add built buildings for range detection, build problems on client (point contents failing on rough surfaces)
//
// Revision 1.19  2002/07/08 17:17:44  Flayra
// - Reworked colors, moved functions into server util
//
// Revision 1.18  2002/07/01 21:46:39  Flayra
// - Added support for generic building ranges, fixed morphing problems
//
// Revision 1.17  2002/06/25 18:18:00  Flayra
// - Renamed buildings, better is-area-free detection
//
// Revision 1.16  2002/06/03 16:58:13  Flayra
// - Renamed weapons factory and armory, hive is now a buildable
//
// Revision 1.15  2002/05/28 18:13:43  Flayra
// - Changed "entity blocked by whatever" message to be logged for PR/screenshot purposes
//
// Revision 1.14  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================

#ifdef AVH_SERVER
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "types.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#endif

#include "mod/AvHSharedUtil.h"
#include "mod/AvHSelectionHelper.h"
#include "mod/AvHConstants.h"

#ifdef AVH_SERVER
#include "mod/AvHPlayer.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHEntities.h"
#include "mod/AvHWeldable.h"
#include "mod/AvHGamerules.h"
#include "dlls/cfuncwall.h"
//#include "common/com_model.h"
//int NS_PointContents(const hull_t *hull, int num, float p[3]);
#endif

#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"

#ifdef AVH_CLIENT
#include "pm_shared/pm_debug.h"
//extern DebugPointListType				gTriDebugLocations;
//extern DebugPointListType				gSquareDebugLocations;
#endif

#include "util/MathUtil.h"
#include "util/STLUtil.h"

#include "common/vector_util.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"

#include "common/r_efx.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "cl_dll/in_defs.h"
#endif

#include "common/com_model.h"

#include "mod/AvHSpecials.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "dlls/turretconst.h"
#include "mod/AvHMarineWeaponConstants.h"
#include "mod/AvHHulls.h"
#include "mod/AvHAlienEquipmentConstants.h"

#include "mod/CollisionUtil.h"
/*
bool NS_BoxesOverlap(float origin1[3], float size1[3], float origin2[3], float size2[3]);
int  NS_PointContents(const hull_t *hull, int num, float p[3]);
int NS_BoxContents(const hull_t *hull, int num, float mins[3], float maxs[3]);
int  NS_GetValveHull(int inHull);
void NS_TraceLine(const hull_t* hull, float srcPoint[3], float dstPoint[3], trace_t* trace);
*/
#include <time.h>
extern playermove_t* pmove;
vec3_t AvHSHUGetRealLocation(const vec3_t& inLocation, const vec3_t& inMinBox, const vec3_t& inMaxBox);


vec3_t gPMDebugPoint;

int kTeamColors[iNumberOfTeamColors][3] =
{
    { 255, 255, 255 },      // White (default)

//  { 125, 165, 210 },      // Blue (marine 1)
    { 0, 153, 255 },        // Blue (marine 1)

//  { 255, 170, 0 },        // HL orange (alien 1)
    { 255, 160, 0 },        // HL orange (alien 1)

    { 145, 215, 140 },      // Green (marine 2)
    { 200, 90, 70 },        // Red (alien 2)
    { 255, 255, 255 }       // White (spectator)
};

float kFTeamColors[iNumberOfTeamColors][3] =
{
    { kTeamColors[0][0] / 255.0f, kTeamColors[0][1] / 255.0f, kTeamColors[0][2] / 255.0f },
    { kTeamColors[1][0] / 255.0f, kTeamColors[1][1] / 255.0f, kTeamColors[1][2] / 255.0f },
    { kTeamColors[2][0] / 255.0f, kTeamColors[2][1] / 255.0f, kTeamColors[2][2] / 255.0f },
    { kTeamColors[3][0] / 255.0f, kTeamColors[3][1] / 255.0f, kTeamColors[3][2] / 255.0f },
    { kTeamColors[4][0] / 255.0f, kTeamColors[4][1] / 255.0f, kTeamColors[4][2] / 255.0f },
    { kTeamColors[5][0] / 255.0f, kTeamColors[5][1] / 255.0f, kTeamColors[5][2] / 255.0f }
};


// Official allowed sizes
// {0, 0, 0 } { 0, 0, 0 } 0x0x0
// { -16, -16, -18 } { 16, 16, 18 } 32x32x36
// { -16, -16, -36 } { 16, 16, 36 } 32x32x72
// { -32, -32, -32 } { 32, 32, 32 } 64x64x64

//#define		kBuilding1MinSize		Vector(-14.33, -14.84, 0.02)
////#define		kBuilding1MaxSize		Vector(21.61, 14.86, 66.9686)
//#define		kBuilding1MaxSize		Vector(14.33, 14.86, 66.9686)
//
//#define		kBuilding2MinSize		Vector(-25.0, -25.0, 0.02)
//#define		kBuilding2MaxSize		Vector(25.0, 25.0, 66.9686)

#define		kResourceMinSize		Vector(-16.0, -16.0, 0.0)
#define		kResourceMaxSize		Vector(16.0, 16.0, 66.9486)

// Tried 100, 110, still jitters.  Shrink tower down a bit?
#define		kAlienResourceMinSize	Vector(-16.0, -16.0, 0.0)
#define		kAlienResourceMaxSize	Vector(16.0, 16.0, 80.7443)

//physent_t* AvHSUGetEntity(int inPhysIndex)
//{
//	physent_t* theTarget = NULL;
//	int i = 0;
//
//	if(inPhysIndex > 0)
//	{
//		#ifdef AVH_CLIENT
//		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
//
//		// Store off the old count
//		//gEngfuncs.pEventAPI->EV_PushPMStates();
//
//		// Now add in all of the players.
//		gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);
//
//		for (i = 0; i < MAX_PHYSENTS; i++)
//		//for (i = 0; i < pmove->numphysent; i++)
//		{
//			physent_t* thePotentialCandidate = gEngfuncs.pEventAPI->EV_GetPhysent(i);
//			if ( thePotentialCandidate && (thePotentialCandidate->info == inPhysIndex))
//			{
//				theTarget = &pmove->physents[i];
//				break;
//			}
//		}
//
//		//gEngfuncs.pEventAPI->EV_PopPMStates();
//		#endif
//
//		// Check phys entities on server
//		#ifdef AVH_SERVER
//		for (i = 0; i < MAX_PHYSENTS; i++)
//		//for (i = 0; i < pmove->numphysent; i++)
//		{
//			if ( pmove->physents[i].info == inPhysIndex )
//			{
//				theTarget = &pmove->physents[i];
//				break;
//			}
//		}
//
//		//CBaseEntity* theEntityHit = CBaseEntity::Instance(tr.pHit);
//		#endif
//	}
//
//	return theTarget;
//}

const char* AvHSHUGetClassNameFromUser3(AvHUser3 inUser3)
{
    const char* theClassName = "";

    switch(inUser3)
    {
    case AVH_USER3_MARINE_PLAYER:
        theClassName = "soldier";
        break;

    case AVH_USER3_COMMANDER_PLAYER:
        theClassName = "commander";
        break;

    case AVH_USER3_ALIEN_PLAYER1:
        theClassName = "skulk";
        break;

    case AVH_USER3_ALIEN_PLAYER2:
        theClassName = "gorge";
        break;

    case AVH_USER3_ALIEN_PLAYER3:
        theClassName = "lerk";
        break;

    case AVH_USER3_ALIEN_PLAYER4:
        theClassName = "fade";
        break;

    case AVH_USER3_ALIEN_PLAYER5:
        theClassName = "onos";
        break;

    case AVH_USER3_ALIEN_EMBRYO:
        theClassName = "gestate";
        break;
    }

    return theClassName;
}


// Pass in -1 to get all entities with a non-zero iuser3
void AvHSHUGetEntities(int inUser3, EntityListType& outEntities)
{
	#ifdef AVH_SERVER
	FOR_ALL_BASEENTITIES();
	if((theBaseEntity->pev->iuser3 == inUser3) || ((inUser3 == -1) && (theBaseEntity->pev->iuser3 > 0)))
	{
		outEntities.push_back(theBaseEntity->entindex());
	}
	END_FOR_ALL_BASEENTITIES();

	#endif

	#ifdef AVH_CLIENT

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);

	physent_t* theEntity = NULL;
	int theNumEnts = pmove->numphysent;
	for (int i = 0; i < theNumEnts; i++)
	{
		theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(i);
		if((theEntity->iuser3 == inUser3) || ((inUser3 == -1) && (theEntity->iuser3 > 0)))
		{
			outEntities.push_back(i);
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
	#endif
}

bool AvHSHUGetEntityLocation(int inEntity, vec3_t& outLocation)
{
	bool theSuccess = false;

	#ifdef AVH_SERVER
	edict_t* pEdict = g_engfuncs.pfnPEntityOfEntIndex(inEntity);
	if(pEdict && !pEdict->free)
	{
		CBaseEntity* theEntity = CBaseEntity::Instance(pEdict);
		if(theEntity)
		{
			outLocation = AvHSHUGetRealLocation(theEntity->pev->origin, theEntity->pev->mins, theEntity->pev->maxs);
			theSuccess = true;
		}
	}
	#endif

	#ifdef AVH_CLIENT
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);

	physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(inEntity);
	
	gEngfuncs.pEventAPI->EV_PopPMStates();

	if(theEntity)
	{
		outLocation = AvHSHUGetRealLocation(theEntity->origin, theEntity->mins, theEntity->maxs);
		theSuccess = true;
	}
	#endif

	return theSuccess;
}


bool AvHSHUGetEntityIUser3(int inEntity, AvHUser3& outIUser3)
{
	bool theSuccess = false;

	#ifdef AVH_SERVER
	edict_t* pEdict = g_engfuncs.pfnPEntityOfEntIndex(inEntity);
	if(pEdict && !pEdict->free)
	{
		CBaseEntity* theEntity = CBaseEntity::Instance(pEdict);
		if(theEntity)
		{
			outIUser3 = (AvHUser3)theEntity->pev->iuser3;
			theSuccess = true;
		}
	}
#endif

#ifdef AVH_CLIENT
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);

	physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(inEntity);
	if(theEntity)
	{
		outIUser3 = (AvHUser3)theEntity->iuser3;
		theSuccess = true;
	}
	gEngfuncs.pEventAPI->EV_PopPMStates();

#endif
	return theSuccess;
}


bool AvHSHUGetEntityIUser4(int inEntity, int& outIUser4)
{
	bool theSuccess = false;

	#ifdef AVH_SERVER
	edict_t* pEdict = g_engfuncs.pfnPEntityOfEntIndex(inEntity);
	if(pEdict && !pEdict->free)
	{
		CBaseEntity* theEntity = CBaseEntity::Instance(pEdict);
		if(theEntity)
		{
			outIUser4 = theEntity->pev->iuser4;
			theSuccess = true;
		}
	}
#endif

#ifdef AVH_CLIENT
	physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(inEntity);
	if(theEntity)
	{
		outIUser4 = theEntity->iuser4;
		theSuccess = true;
	}
#endif

	return theSuccess;
}


vec3_t AvHSHUGetRealLocation(const vec3_t& inLocation, const vec3_t& inMinBox, const vec3_t& inMaxBox)
{
	vec3_t outLocation;
	VectorCopy(inLocation, outLocation);

	if((outLocation.x == outLocation.y) && (outLocation.y == outLocation.z) && (outLocation.z == 0.0f))
	{
		outLocation.x = (inMinBox.x + inMaxBox.x)/2.0f;
		outLocation.y = (inMinBox.y + inMaxBox.y)/2.0f;
		outLocation.z = (inMinBox.z + inMaxBox.z)/2.0f;
	}
	return outLocation;
}

// Returns range of user3, whatever that means.  Returns -1 for no meaningful range.
int AvHSHUGetDrawRangeForUser3(AvHUser3 inUser3)
{
	int theRange = -1;

	switch(inUser3)
	{
	case AVH_USER3_COMMANDER_STATION:
		theRange = BALANCE_VAR(kCommandStationBuildDistance);
		break;

	case AVH_USER3_FUNC_RESOURCE:
		theRange = BALANCE_VAR(kResourceTowerBuildDistanceTolerance);
		break;

	case AVH_USER3_TURRET_FACTORY:
	case AVH_USER3_ADVANCED_TURRET_FACTORY:
		theRange = BALANCE_VAR(kTurretFactoryBuildDistance);
		break;

	case AVH_USER3_ARMORY:
	case AVH_USER3_ADVANCED_ARMORY:
		theRange = BALANCE_VAR(kArmoryBuildDistance);
		break;

	case AVH_USER3_TURRET:
		theRange = BALANCE_VAR(kTurretRange);
		break;

	case AVH_USER3_SIEGETURRET:
		// TODO: Figure out a way to return minimum range also?
		theRange = BALANCE_VAR(kSiegeTurretRange);
		break;

	case AVH_USER3_PROTOTYPE_LAB:
		theRange = BALANCE_VAR(kArmorDropRange);
		break;

	case AVH_USER3_OBSERVATORY:
		theRange = BALANCE_VAR(kObservatoryXYDetectionRadius);
		break;

	case AVH_USER3_SENSORY_CHAMBER:
		theRange = BALANCE_VAR(kSensoryChamberRange);
		break;
	}

	return theRange;
}

int AvHSHUGetDrawRangeForMessageID(AvHMessageID inMessageID)
{
	AvHUser3 theUser3 = AVH_USER3_NONE;

	switch(inMessageID)
	{
	case BUILD_COMMANDSTATION:
		theUser3 = AVH_USER3_COMMANDER_STATION;
		break;

	case BUILD_TURRET:
		theUser3 = AVH_USER3_TURRET;
		break;

	case BUILD_SIEGE:
		theUser3 = AVH_USER3_SIEGETURRET;
		break;

	case BUILD_OBSERVATORY:
		theUser3 = AVH_USER3_OBSERVATORY;
		break;

	case BUILD_TURRET_FACTORY:
		theUser3 = AVH_USER3_TURRET_FACTORY;
		break;

	case BUILD_PROTOTYPE_LAB:
		theUser3 = AVH_USER3_PROTOTYPE_LAB;
		break;

	case BUILD_ARMORY:
		theUser3 = AVH_USER3_ARMORY;
		break;
	}

	return AvHSHUGetDrawRangeForUser3(theUser3);
}


// Specify whether we draw rings for this entity or not, and draw them at different sizes for aesthetics.  The scalar might go away when the
// artwork all has the correct bounding boxes.
bool AvHSHUGetDrawRingsForUser3(AvHUser3 inUser3, float& outScalar)
{
	bool theDrawRings = false;
	float theScalar = 1.0f;

	switch(inUser3)
	{
	case AVH_USER3_MARINE_PLAYER:
		theScalar = .9f;
		theDrawRings = true;
		break;

	case AVH_USER3_RESTOWER:
		theScalar = 1.6f;
		theDrawRings = true;
		break;

	case AVH_USER3_INFANTRYPORTAL:
		theScalar = 1.3f;
		theDrawRings = true;
		break;

	case AVH_USER3_ARMORY:
	case AVH_USER3_ADVANCED_ARMORY:
		theScalar = 1.35f;
		theDrawRings = true;
		break;

	case AVH_USER3_COMMANDER_STATION:
		theScalar = 1.4f;
		theDrawRings = true;
		break;

	case AVH_USER3_PHASEGATE:
		theScalar = 2.0f;
		theDrawRings = true;
		break;

	case AVH_USER3_OBSERVATORY:
		theScalar = .9f;
		theDrawRings = true;
		break;

	case AVH_USER3_TURRET_FACTORY:
	case AVH_USER3_ADVANCED_TURRET_FACTORY:
		theScalar = 1.6f;
		theDrawRings = true;
		break;

	case AVH_USER3_TURRET:
		theScalar = .7f;
		theDrawRings = true;
		break;

	case AVH_USER3_NUKEPLANT:
		theScalar = 1.5f;
		theDrawRings = true;
		break;

	case AVH_USER3_ARMSLAB:
	case AVH_USER3_PROTOTYPE_LAB:
	case AVH_USER3_CHEMLAB:
	case AVH_USER3_MEDLAB:
	case AVH_USER3_SIEGETURRET:
		theScalar = 1.3f;
		theDrawRings = true;
		break;

	// Alien buildings
	case AVH_USER3_DEFENSE_CHAMBER:
	case AVH_USER3_OFFENSE_CHAMBER:
	case AVH_USER3_MOVEMENT_CHAMBER:
	case AVH_USER3_SENSORY_CHAMBER:
	case AVH_USER3_ALIENRESTOWER:
		theScalar = 1.3f;
		theDrawRings = true;
		break;

	case AVH_USER3_ALIEN_PLAYER1:
		theScalar = .6f;
		theDrawRings = true;
		break;
	case AVH_USER3_ALIEN_PLAYER2:
		theScalar = 1.0f;
		theDrawRings = true;
		break;
	case AVH_USER3_ALIEN_PLAYER3:
		theScalar = 1.0;
		theDrawRings = true;
		break;
	case AVH_USER3_ALIEN_PLAYER4:
		theScalar = 1.3f;
		theDrawRings = true;
		break;
	case AVH_USER3_ALIEN_PLAYER5:
		theScalar = 1.2f;
		theDrawRings = true;
		break;
	}

	if(theDrawRings)
	{
		outScalar = theScalar;
	}

	return theDrawRings;
}

bool AvHSHUGetBuildRegions(AvHMessageID inMessageID, EntityListType& outEntities, IntList& outRanges, float& outZAdjustment, bool& outSnapToLocation)
{
	bool theFoundTech = false;

	typedef vector<AvHUser3> User3ListType;
	User3ListType theUser3s;

	outZAdjustment = 0.0f;
	outSnapToLocation = false;

	switch(inMessageID)
	{
	case BUILD_RESOURCES:
	case ALIEN_BUILD_RESOURCES:
		theUser3s.push_back(AVH_USER3_FUNC_RESOURCE);
		outZAdjustment = (kFuncResourceMaxSize.z - kFuncResourceMinSize.z);
		outSnapToLocation = true;
		break;

	case BUILD_INFANTRYPORTAL:
		theUser3s.push_back(AVH_USER3_COMMANDER_STATION);
		break;

	case BUILD_TURRET:
		theUser3s.push_back(AVH_USER3_TURRET_FACTORY);
		theUser3s.push_back(AVH_USER3_ADVANCED_TURRET_FACTORY);
		break;

	case BUILD_SIEGE:
		theUser3s.push_back(AVH_USER3_ADVANCED_TURRET_FACTORY);
		break;

	case BUILD_MINES:
	case BUILD_WELDER:
	case BUILD_SHOTGUN:
		theUser3s.push_back(AVH_USER3_ARMORY);
		theUser3s.push_back(AVH_USER3_ADVANCED_ARMORY);
		break;

	case BUILD_HMG:
	case BUILD_GRENADE_GUN:
		theUser3s.push_back(AVH_USER3_ADVANCED_ARMORY);
		break;

	case BUILD_HEAVY:
	case BUILD_JETPACK:
		theUser3s.push_back(AVH_USER3_PROTOTYPE_LAB);
		break;
	}

	if(theUser3s.size() > 0)
	{
		for(User3ListType::iterator theUser3Iter = theUser3s.begin(); theUser3Iter != theUser3s.end(); theUser3Iter++)
		{
			EntityListType theEntities;
			AvHSHUGetEntities(*theUser3Iter, theEntities);

			for(EntityListType::iterator theEntityIter = theEntities.begin(); theEntityIter != theEntities.end(); theEntityIter++)
			{
				// Only add buildings that are fully built
				int theIuser4 = 0;
				if(AvHSHUGetEntityIUser4(*theEntityIter, theIuser4))
				{
					if(!GetHasUpgrade(theIuser4, MASK_BUILDABLE))
					{
						outEntities.push_back(*theEntityIter);
						outRanges.push_back(AvHSHUGetDrawRangeForUser3(*theUser3Iter));
					}
				}
			}
		}
		theFoundTech = true;
	}

	return theFoundTech;
}


// tankefugl: 0000291 -- allows listed structures to be dropped on resource towers
bool AvHSHUGetIsDroppableOnRTs(AvHMessageID inMessageID)
{
	switch (inMessageID)
	{
		case BUILD_HEALTH:
		case BUILD_AMMO:
		case BUILD_MINES:
		case BUILD_WELDER:
		case BUILD_SHOTGUN:
		case BUILD_HMG:
		case BUILD_GRENADE_GUN:
		case BUILD_CAT:
		case BUILD_HEAVY:
		case BUILD_JETPACK:
		case BUILD_RESOURCES:
		case ALIEN_BUILD_RESOURCES:
		case ALIEN_BUILD_HIVE:
			return true;
		default:
			return false;
	}
}
// :tankefugl

bool AvHSHUGetIsMarineStructure(AvHMessageID inMessageID)
{

    switch (inMessageID)
    {

    case BUILD_INFANTRYPORTAL:
    case BUILD_RESOURCES:
    case BUILD_TURRET_FACTORY:
    case BUILD_ARMSLAB:
    case BUILD_PROTOTYPE_LAB:
    case BUILD_ARMORY:
    case BUILD_NUKE_PLANT:
    case BUILD_OBSERVATORY:
    case BUILD_PHASEGATE:
    case BUILD_TURRET:
    case BUILD_SIEGE:
    case BUILD_COMMANDSTATION:
        return true;
    default:
        return false;

    }

}

bool AvHSHUGetIsMarineStructure(AvHUser3 inUser3)
{

    switch (inUser3)
    {
    case AVH_USER3_COMMANDER_STATION:
    case AVH_USER3_TURRET_FACTORY:
    case AVH_USER3_ARMORY:
    case AVH_USER3_ADVANCED_ARMORY:
    case AVH_USER3_ARMSLAB:
    case AVH_USER3_PROTOTYPE_LAB:
    case AVH_USER3_OBSERVATORY:
    case AVH_USER3_CHEMLAB:
    case AVH_USER3_MEDLAB:
    case AVH_USER3_NUKEPLANT:
    case AVH_USER3_TURRET:
    case AVH_USER3_SIEGETURRET:
    case AVH_USER3_RESTOWER:
    case AVH_USER3_INFANTRYPORTAL:
    case AVH_USER3_PHASEGATE:
    case AVH_USER3_ADVANCED_TURRET_FACTORY:
        return true;
    default:
        return false;
    }

}

void AvHSHUGetMinBuildRadiusViolations(AvHMessageID inMessageID, vec3_t& inLocation, EntityListType& outViolations)
{

    // Enforce a minimum build radius for marine structures.

    if (AvHSHUGetIsMarineStructure(inMessageID))
    {
        EntityListType theEntities;
        AvHSHUGetEntities(-1, theEntities);

        vec3_t theMinSize, theMaxSize;

        AvHSHUGetSizeForTech(inMessageID, theMinSize, theMaxSize);
        float theMaxRadius1 = max(-min(theMinSize.x, theMinSize.y), max(theMaxSize.x, theMaxSize.y));

        for (EntityListType::iterator theIter = theEntities.begin(); theIter != theEntities.end(); theIter++)
		{

            AvHUser3 theUser3;
            AvHSHUGetEntityIUser3(*theIter, theUser3);

			bool theEntityCouldBlock = AvHSHUGetIsMarineStructure(theUser3);
			if(inMessageID != BUILD_RESOURCES)
			{
				theEntityCouldBlock |= (theUser3 == AVH_USER3_FUNC_RESOURCE);
			}

			bool theEntityIsSolid = false;

#ifdef AVH_SERVER
			edict_t* theEntity = INDEXENT(*theIter);

			if(theEntity)
				theEntityIsSolid = (theEntity->v.solid == SOLID_BBOX);
#endif

#ifdef AVH_CLIENT

			physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(*theIter);

			if(theEntity) 
				theEntityIsSolid = (theEntity->solid == SOLID_BBOX);
				
#endif	
			// joev: 0000291
			// It's possible to place "on" marines if you're offset a little from center. This code and
			// associated changes below and in AvHHudRender.cpp is to enforce a build distance around players
			// in the same way as buildings to prevent this exploit.
			if (theUser3 == AVH_USER3_MARINE_PLAYER)
			{
				theEntityIsSolid = true;
				theEntityCouldBlock = true;
			}

			if (theEntityCouldBlock && theEntityIsSolid)
            {
                AvHSHUGetSizeForUser3(theUser3, theMinSize, theMaxSize);
                float theMaxRadius2 = max(max(theMinSize.x, theMaxSize.x), max(theMinSize.y, theMaxSize.y));

	            vec3_t theLocation;
			    if(AvHSHUGetEntityLocation(*theIter, theLocation))
			    {
			        vec3_t theXYInLocation = inLocation;
                    vec3_t theXYTheLocation = theLocation;

                    theXYInLocation.z = 0;
                    theXYTheLocation.z = 0;

                    float theDistance = VectorDistance((float*)&theXYInLocation, (float*)&theXYTheLocation);
					// joev: 0000291
					// It's possible to place "on" marines if you're offset a little from center. This code and
					// associated changes above and in AvHHudRender.cpp is to enforce a build distance around players
					// in the same way as buildings to prevent this exploit.
 					float theMinMarineBuildDistance;
               		if (theUser3 == AVH_USER3_MARINE_PLAYER) {
						theMinMarineBuildDistance = BALANCE_VAR(kMinMarinePlayerBuildDistance);
					}
					else
					{
						theMinMarineBuildDistance = BALANCE_VAR(kMinMarineBuildDistance);
					}
					// :joev
                    if (theDistance < theMinMarineBuildDistance + theMaxRadius1 + theMaxRadius2)
                    {
                        outViolations.push_back(*theIter);
                    }
                }
            }
        }
    }
}

bool AvHSHUGetAreSpecialBuildingRequirementsMet(AvHMessageID inMessageID, vec3_t& inLocation)
{
	bool theRequirementsMet = false;
	EntityListType theEntities;
	IntList theDistanceRequirements;
	float theZAdjustment = 0.0f;
	bool theSnapToLocation = false;

	if(inMessageID == ALIEN_BUILD_HIVE)
	{
		// Look for inactive hives within radius
		EntityListType theEntities;

		// Look for a unoccupied hive spot within range
		AvHSHUGetEntities(AVH_USER3_HIVE, theEntities);

		for(EntityListType::iterator theIter = theEntities.begin(); theIter != theEntities.end(); theIter++)
		{
			vec3_t theLocation;
			if(AvHSHUGetEntityLocation(*theIter, theLocation))
			{
				// Set z's equal to check 2D distance only
				inLocation.z = theLocation.z;
				float theDistance = VectorDistance((float*)&inLocation, (float*)&theLocation);

				if(theDistance <= kHiveXYDistanceTolerance)
				{
					// Make sure this hive isn't already active
					#ifdef AVH_SERVER
					CBaseEntity* theEntity = AvHSUGetEntityFromIndex(*theIter);
					ASSERT(theEntity);
					if(theEntity->pev->team == 0)
					{
					#endif

						theRequirementsMet = true;
						inLocation = theLocation;

					#ifdef AVH_SERVER
					}
					#endif

					break;
				}
			}
		}
	}
	else if(AvHSHUGetBuildRegions(inMessageID, theEntities, theDistanceRequirements, theZAdjustment, theSnapToLocation))
	{
		ASSERT(theEntities.size() == theDistanceRequirements.size());
		vec3_t theBestLocation;

		int i = 0;
		float theClosestDistance = kMaxMapDimension;
		for(EntityListType::iterator theIter = theEntities.begin(); theIter != theEntities.end(); theIter++, i++)
		{
			vec3_t theLocation;
			if(AvHSHUGetEntityLocation(*theIter, theLocation))
			{
				// Only check xy distance
				vec3_t theXYInLocation = inLocation;
				vec3_t theXYTheLocation = theLocation;
				theXYInLocation.z = 0;
				theXYTheLocation.z = 0;

				float theDistance = VectorDistance((float*)&theXYInLocation, (float*)&theXYTheLocation);

				int theDistanceRequirement = theDistanceRequirements[i];
				if((theDistance <= theDistanceRequirement) || (theDistanceRequirement == -1))
				{
					// Pick the closest one, in case there are multiples in range
					if(theDistance < theClosestDistance)
					{
						theClosestDistance = theDistance;
						VectorCopy(theLocation,theBestLocation);
						theRequirementsMet = true;
					}
				}
			}
		}
		if(theRequirementsMet && theSnapToLocation)
		{
			inLocation = theBestLocation;
			inLocation.z += theZAdjustment;
		}
	}
	else
	{
		theRequirementsMet = true;
	}

    EntityListType theBuildRadiusViolations;
    AvHSHUGetMinBuildRadiusViolations(inMessageID, inLocation, theBuildRadiusViolations);

    if (theBuildRadiusViolations.size() > 0)
    {
        theRequirementsMet = false;
    }

	// Anti-llama/newbie tactic: don't allow non-resource buildings to be placed such that they block access to nozzles
	// Make sure generic building isn't being placed on top of resource nozzles
	// tankefugl: 0000291
	// allow equipment, rts and hives to be dropped around nodes
	if(AvHSHUGetIsDroppableOnRTs(inMessageID) == false)
	{
	// :tankefugl
		// If building is too close to an empty nozzle, don't allow it
		float theResourceBuildingRadius, theTotalMinRadius;
		vec3_t theMinSize, theMaxSize, theMinRadius, theFlattenedInLocation, theLocation;

		theFlattenedInLocation[0] = inLocation[0];
		theFlattenedInLocation[1] = inLocation[1];
		theFlattenedInLocation[2] = 0;

		theResourceBuildingRadius = 60;

		EntityListType theEntities;
		AvHSHUGetEntities(AVH_USER3_FUNC_RESOURCE,theEntities);
		if(AvHSHUGetSizeForTech(inMessageID,theMinSize,theMaxSize))
		{
			EntityListType::iterator end = theEntities.end();
			for(EntityListType::iterator current = theEntities.begin(); current < end; ++current)
			{
				if(AvHSHUGetEntityLocation(*current,theLocation))
				{
					//flatten to 2 dimensions
					theLocation[2] = 0;

					//space = radius of both buildings combined
					theTotalMinRadius = theResourceBuildingRadius;
					theTotalMinRadius += max(-min(theMinSize.x,theMinSize.y),max(theMaxSize.x,theMaxSize.y));

					if(VectorDistance((float*)&theFlattenedInLocation,(float*)&theLocation) < theTotalMinRadius)
					{
						theRequirementsMet = false;
						break;
					}
				}
			}
		}
	}

	return theRequirementsMet;
}

bool AvHSHUGetBuildTechClassName(AvHMessageID inMessageID, char*& outClassName)
{
	bool theSuccess = true;

	switch(inMessageID)
	{
	// Buildings
	case BUILD_RESOURCES:
		outClassName = kwsResourceTower;
		break;

	//case BUILD_REINFORCEMENTS:
	//	outClassName = kwsInfantryPortal;
	//	break;

	case BUILD_INFANTRYPORTAL:
		outClassName = kwsInfantryPortal;
		break;

	case BUILD_COMMANDSTATION:
		outClassName = kwsTeamCommand;
		break;

	case BUILD_TURRET_FACTORY:
		outClassName = kwsTurretFactory;
		break;

	case BUILD_ARMSLAB:
		outClassName = kwsArmsLab;
		break;

	case BUILD_PROTOTYPE_LAB:
		outClassName = kwsPrototypeLab;
		break;

	case BUILD_ARMORY:
		outClassName = kwsArmory;
		break;

	case ARMORY_UPGRADE:
		outClassName = kwsAdvancedArmory;
		break;

	case BUILD_NUKE_PLANT:
		outClassName = kwsNukePlant;
		break;

	case BUILD_OBSERVATORY:
		outClassName = kwsObservatory;
		break;

	case BUILD_SCAN:
		outClassName = kwsScan;
		break;

	case BUILD_PHASEGATE:
		outClassName = kwsPhaseGate;
		break;

	case BUILD_TURRET:
		outClassName = kwsDeployedTurret;
		break;

	case BUILD_SIEGE:
		outClassName = kwsSiegeTurret;
		break;

	// Equipment
	case BUILD_HEALTH:
		outClassName = kwsHealth;
		break;

    case BUILD_CAT:
        outClassName = kwsCatalyst;
        break;

	case BUILD_JETPACK:
		outClassName = kwsJetpack;
		break;

	case BUILD_HEAVY:
		outClassName = kwsHeavyArmor;
		break;

	case BUILD_AMMO:
		outClassName = kwsGenericAmmo;
		break;

	case BUILD_WELDER:
		outClassName = kwsWelder;
		break;

    case BUILD_MINES:
		outClassName = kwsMine;
		break;

	case BUILD_SHOTGUN:
		outClassName = kwsShotGun;
		break;

	case BUILD_HMG:
		outClassName = kwsHeavyMachineGun;
		break;

	case BUILD_NUKE:
		outClassName = kwsNuke;
		break;

	case BUILD_GRENADE_GUN:
		outClassName = kwsGrenadeGun;
		break;

	//case BUILD_MEDKIT:
	//	break;

	case ALIEN_BUILD_RESOURCES:
		outClassName = kwsAlienResourceTower;
		break;

	case ALIEN_BUILD_OFFENSE_CHAMBER:
		outClassName = kwsOffenseChamber;
		break;

	case ALIEN_BUILD_DEFENSE_CHAMBER:
		outClassName = kwsDefenseChamber;
		break;

	case ALIEN_BUILD_SENSORY_CHAMBER:
		outClassName = kwsSensoryChamber;
		break;

	case ALIEN_BUILD_MOVEMENT_CHAMBER:
		outClassName = kwsMovementChamber;
		break;

	case ALIEN_BUILD_HIVE:
		outClassName = kesTeamHive;
		break;

	default:
		theSuccess = false;
		break;
	}

	return theSuccess;
}

bool AvHSHUGetResearchTechName(AvHMessageID inResearchID, char*& outResearchTechName)
{
	bool theSuccess = true;

	switch(inResearchID)
	{
	case RESEARCH_ELECTRICAL:
		outResearchTechName = "research_electrical";
		break;

	case RESEARCH_ARMOR_ONE:
		outResearchTechName = "research_armorl1";
		break;

	case RESEARCH_ARMOR_TWO:
		outResearchTechName = "research_armorl2";
		break;

	case RESEARCH_ARMOR_THREE:
		outResearchTechName = "research_armorl3";
		break;

	case RESEARCH_WEAPONS_ONE:
		outResearchTechName = "research_weaponsl1";
		break;

	case RESEARCH_WEAPONS_TWO:
		outResearchTechName = "research_weaponsl2";
		break;

	case RESEARCH_WEAPONS_THREE:
		outResearchTechName = "research_weaponsl3";
		break;

	case ARMORY_UPGRADE:
		outResearchTechName = "research_advarmory";
		break;

	case TURRET_FACTORY_UPGRADE:
		outResearchTechName = "research_advturretfactory";
		break;

	case RESEARCH_JETPACKS:
		outResearchTechName = "research_jetpacks";
		break;

	case RESEARCH_HEAVYARMOR:
		outResearchTechName = "research_heavyarmor";
		break;

	case RESEARCH_DISTRESSBEACON:
		outResearchTechName = "research_distressbeacon";
		break;

	case RESEARCH_HEALTH:
		outResearchTechName = "research_health";
		break;

    case RESEARCH_CATALYSTS:
        outResearchTechName = "research_catalysts";
        break;

	case MESSAGE_CANCEL:
		outResearchTechName = "research_cancel";
		break;

	case RESEARCH_MOTIONTRACK:
		outResearchTechName = "research_motiontracking";
		break;

	case RESEARCH_PHASETECH:
		outResearchTechName = "research_phasetech";
		break;

    case RESEARCH_GRENADES:
        outResearchTechName = "research_grenades";
        break;

	default:
		theSuccess = false;
		break;
	}

	return theSuccess;
}


bool AvHSHUGetCenterPositionForGroup(int inGroupNumber, float* inPlayerOrigin, float* outCenterPosition)
{
	bool theSuccess = false;
	vec3_t thePosition;
	float theX, theY;

	#ifdef AVH_CLIENT
	theSuccess = gHUD.GetCenterPositionForGroup(inGroupNumber, thePosition);
	theX = thePosition[0];
	theY = thePosition[1];
	#endif

	#ifdef AVH_SERVER

	// Loop through players, find the closest player to inPlayerOrigin, to see which player is being predicted.  Is there a better way?
	AvHPlayer* theClosestPlayer = NULL;
	float theClosestDistance = 10000;

	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		float theDistance = VectorDistance(theEntity->pev->origin, inPlayerOrigin);
		if(theDistance < theClosestDistance)
		{
			theClosestPlayer = theEntity;
			theClosestDistance = theDistance;
		}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

	if(theClosestPlayer)
	{
		theSuccess = theClosestPlayer->GetCenterPositionForGroup(inGroupNumber, theX, theY);
	}
	#endif

	if(theSuccess)
	{
		outCenterPosition[0] = theX;
		outCenterPosition[1] = theY;
	}

	return theSuccess;
}


bool AvHSHUGetIsBuilding(AvHMessageID inMessageID)
{
	bool theIsBuilding = false;

	switch(inMessageID)
	{
		// Buildings
	case BUILD_RESOURCES:
	case BUILD_INFANTRYPORTAL:
	case BUILD_COMMANDSTATION:
	case BUILD_TURRET_FACTORY:
	case BUILD_ARMSLAB:
	case BUILD_PROTOTYPE_LAB:
	case BUILD_ARMORY:
	case BUILD_NUKE_PLANT:
	case BUILD_OBSERVATORY:
	case BUILD_PHASEGATE:
	case BUILD_TURRET:
	case BUILD_SIEGE:

	// Alien buildings
	case ALIEN_BUILD_OFFENSE_CHAMBER:
	case ALIEN_BUILD_DEFENSE_CHAMBER:
	case ALIEN_BUILD_SENSORY_CHAMBER:
	case ALIEN_BUILD_MOVEMENT_CHAMBER:
	case ALIEN_BUILD_HIVE:
		theIsBuilding = true;
		break;
	}

	return theIsBuilding;
}

bool AvHSHUGetIsBuildTech(AvHMessageID inMessageID)
{
	bool theIsBuildTech = false;

	switch(inMessageID)
	{
	// Buildings
	case BUILD_RESOURCES:
	case BUILD_INFANTRYPORTAL:
	case BUILD_COMMANDSTATION:
	case BUILD_TURRET_FACTORY:
	case BUILD_ARMSLAB:
	case BUILD_PROTOTYPE_LAB:
	case BUILD_ARMORY:
	//case UPGRADE_ADVANCED_WEAPON_FACTORY:
	case BUILD_NUKE_PLANT:
	case BUILD_OBSERVATORY:
	case BUILD_SCAN:
	case BUILD_PHASEGATE:
	case BUILD_TURRET:
	case BUILD_SIEGE:
	case BUILD_HEAVY:
	case BUILD_JETPACK:

	// Equipment
	case BUILD_AMMO:
	case BUILD_HEALTH:
    case BUILD_CAT:
	case BUILD_WELDER:
    case BUILD_MINES:
	case BUILD_SHOTGUN:
	case BUILD_HMG:
	case BUILD_NUKE:
	case BUILD_GRENADE_GUN:
	//case BUILD_MEDKIT:
	//case BUILD_STIMPACK:

	// Alien buildings
	case ALIEN_BUILD_OFFENSE_CHAMBER:
	case ALIEN_BUILD_DEFENSE_CHAMBER:
	case ALIEN_BUILD_SENSORY_CHAMBER:
	case ALIEN_BUILD_MOVEMENT_CHAMBER:
		theIsBuildTech = true;
		break;
	}

	return theIsBuildTech;
}

bool AvHSHUGetIsWeaponFocusable(AvHWeaponID inWeaponID)
{
    bool theIsFocusable = false;

    switch(inWeaponID)
    {
    case AVH_WEAPON_BITE:
    case AVH_WEAPON_SPIT:
    case AVH_WEAPON_BITE2:
    case AVH_WEAPON_SWIPE:
    case AVH_WEAPON_CLAWS:
        theIsFocusable = true;
        break;
    }

    return theIsFocusable;
}

bool AvHSHUGetDoesTechCostEnergy(AvHMessageID inMessageID)
{
	bool theTechCostsEnergy = false;

	switch(inMessageID)
	{
	case BUILD_SCAN:
		theTechCostsEnergy = true;
		break;
	}

	return theTechCostsEnergy;
}

bool AvHSHUGetIsCombatModeTech(AvHMessageID inMessageID)
{
	bool theIsCombatModeTech = false;

	switch(inMessageID)
	{
	case BUILD_SHOTGUN:
	case BUILD_GRENADE_GUN:
	case BUILD_HMG:
	case BUILD_WELDER:
	case BUILD_MINES:
	case BUILD_JETPACK:
	case BUILD_HEAVY:
	case BUILD_SCAN:
		theIsCombatModeTech = true;
		break;
	}

	return theIsCombatModeTech;
}

bool AvHSHUGetIsResearchTech(AvHMessageID inMessageID)
{
	bool theIsResearchTech = false;

	switch(inMessageID)
	{
	case RESEARCH_ELECTRICAL:
	case RESEARCH_ARMOR_ONE:
	case RESEARCH_ARMOR_TWO:
	case RESEARCH_ARMOR_THREE:
	case RESEARCH_WEAPONS_ONE:
	case RESEARCH_WEAPONS_TWO:
	case RESEARCH_WEAPONS_THREE:
	case TURRET_FACTORY_UPGRADE:
	case RESEARCH_JETPACKS:
	case RESEARCH_HEAVYARMOR:
	case RESEARCH_DISTRESSBEACON:
	case RESEARCH_HEALTH:
    case RESEARCH_CATALYSTS:
	case MESSAGE_CANCEL:
	case RESEARCH_MOTIONTRACK:
	case RESEARCH_PHASETECH:
    case RESEARCH_GRENADES:

	case RESOURCE_UPGRADE:
	case ARMORY_UPGRADE:
		theIsResearchTech = true;
		break;
	}

	return theIsResearchTech;
}




//Here's TFC's code that checks whether a player's allowed to build a sentry
//or not.
//I can't remember if there was any good reason why we used
//UTIL_FindEntitiesInSphere()
//instead of UTIL_EntitiesInBox().
//
////=========================================================================
//// Returns 0 if the area around obj is safe to build in
//int CBaseEntity::CheckArea( CBaseEntity *pIgnore )
//{
//	TraceResult tr;
//	Vector vecOrg = pev->origin;
//
//	// Check the origin
//	int iContents = UTIL_PointContents(vecOrg);
//	if ( iContents != CONTENT_EMPTY && iContents != CONTENT_WATER )
//		return CAREA_BLOCKED;
//
//	Vector vecIgnoreOrg = pIgnore->pev->origin;
//	// Get the player's origin irrelevant of crouching
//	if ( pIgnore->pev->flags & FL_DUCKING )
//	{
//		vecIgnoreOrg = vecIgnoreOrg + (VEC_DUCK_HULL_MIN -
//			VEC_HULL_MIN);
//	}
//	// Trace a hull
//	UTIL_TraceHull( vecIgnoreOrg, pev->origin, ignore_monsters,
//		large_hull, edict(), &tr );
//	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
//	if (tr.flFraction != 1 || tr.fAllSolid == 1)
//		return CAREA_BLOCKED;
//
//	// Check for solid entities in the area
//	CBaseEntity *pEnt = NULL;
//	while ( (pEnt = UTIL_FindEntityInSphere( pEnt, pev->origin, 48 )) !=
//		NULL )
//	{
//		// If it's not the engineer, and its a solid entity, fail
//		if (pEnt != pIgnore && pEnt != this && pEnt->pev->solid >
//			SOLID_TRIGGER)
//			return CAREA_BLOCKED;
//	}
//
//	// Cycle through all the Nobuild zones in the map and make sure this
//	isn't in one of them
//		CBaseEntity *pNoBuild = UTIL_FindEntityByClassname( NULL,
//		"func_nobuild" );
//	while ( pNoBuild )
//	{
//		// Check to see if we're building in this zone
//		if ( vecOrg.x >= pNoBuild->pev->mins.x && vecOrg.y >=
//			pNoBuild->pev->mins.y && vecOrg.z >= pNoBuild->pev->mins.z &&
//			vecOrg.x <= pNoBuild->pev->maxs.x &&
//			vecOrg.y <= pNoBuild->pev->maxs.y && vecOrg.z <= pNoBuild->pev->maxs.z )
//			return CAREA_NOBUILD;
//
//		pNoBuild = UTIL_FindEntityByClassname( pNoBuild,
//			"func_nobuild" );
//	}
//
//	// Check below
//	UTIL_TraceLine( vecOrg, vecOrg + Vector(0,0,-64),
//		dont_ignore_monsters, edict(), &tr );
//	if ( tr.flFraction == 1.0 )
//		return CAREA_BLOCKED;
//
//	return CAREA_CLEAR;
//}

//bool AvHSHUGetIsEnoughRoom(const vec3_t& inCenter, const vec3_t& inMin, const vec3_t& inMax)
//{
//	bool theEnoughRoom = false;
//
//	// Do a traceline from center to center + min
//	// Hit nothing?
//		// Do a traceline from center to center + max
//		// Hit nothing?
//			// Success
//			theEnoughRoom = true;
//
//	return theEnoughRoom;
//}

bool AvHSHUGetIsGroupMessage(AvHMessageID inMessageID)
{
	bool theIsGroupMessage = false;

	switch(inMessageID)
	{
	case GROUP_CREATE_1:
	case GROUP_CREATE_2:
	case GROUP_CREATE_3:
	case GROUP_CREATE_4:
	case GROUP_CREATE_5:
	case GROUP_SELECT_1:
	case GROUP_SELECT_2:
	case GROUP_SELECT_3:
	case GROUP_SELECT_4:
	case GROUP_SELECT_5:
		theIsGroupMessage = true;
	}

	return theIsGroupMessage;
}

bool AvHSHUGetSizeForTech(AvHMessageID inMessageID, Vector& outMinSize, Vector& outMaxSize, bool inGetSizeToPlace)
{
	bool theSuccess = false;

	// Onos-sized
	//const int theOnosHeightNeededToSpawn = HULL3_MAXZ - HULL3_MINZ + kRespawnFudgeFactorHeight;
	//const int theOnosWidthNeededToSpawn = HULL3_MAXY - HULL3_MINY + kRespawnFudgeFactorHeight;

	// Marine-sized
	const int theMarineHeightNeededToSpawn = HULL0_MAXZ - HULL0_MINZ + kRespawnFudgeFactorHeight;

	switch(inMessageID)
	{
	case BUILD_INFANTRYPORTAL:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 14.49);

		// Make sure there is room above it for players to spawn
		if(inGetSizeToPlace)
		{
			outMaxSize.z += theMarineHeightNeededToSpawn;
		}

		theSuccess = true;
		break;

	case BUILD_PHASEGATE:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 14.49);

		// Make sure there is remove above it for players to spawn
		if(inGetSizeToPlace)
		{
			//outMinSize.x = outMinSize.y = -theOnosWidthNeededToSpawn;
			//outMaxSize.x = outMaxSize.y = theOnosWidthNeededToSpawn;
			//outMaxSize.z += theOnosHeightNeededToSpawn;
			outMaxSize.z += theMarineHeightNeededToSpawn;
		}

		theSuccess = true;
		break;

	case BUILD_RESOURCES:
		outMinSize = kResourceMinSize;
		outMaxSize = kResourceMaxSize;
		theSuccess = true;
		break;

	case ALIEN_BUILD_RESOURCES:
		outMinSize = kAlienResourceMinSize;
		outMaxSize = kAlienResourceMaxSize;
		theSuccess = true;
		break;

	default:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 66.9486);
		theSuccess = true;
		break;

	case BUILD_TURRET:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 42.0);
		theSuccess = true;
		break;

	case ALIEN_BUILD_OFFENSE_CHAMBER:
	case ALIEN_BUILD_DEFENSE_CHAMBER:
	case ALIEN_BUILD_SENSORY_CHAMBER:
	case ALIEN_BUILD_MOVEMENT_CHAMBER:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 44.0);
		theSuccess = true;
		break;

	case BUILD_COMMANDSTATION:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 70.34);
		theSuccess = true;
		break;

	case BUILD_TURRET_FACTORY:
		outMinSize = Vector(-16, -16, 0);
		//outMaxSize = Vector(16.0, 16.0, 55.68);
		outMaxSize = Vector(16.0, 16.0, 62.1931);
		theSuccess = true;
		break;

	case BUILD_ARMORY:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 62.1931);
		theSuccess = true;
		break;

	case BUILD_PROTOTYPE_LAB:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 67.7443);
		theSuccess = true;
		break;

	case BUILD_OBSERVATORY:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 80.7443);
		theSuccess = true;
		break;

	case BUILD_SIEGE:
		outMinSize = Vector(-16, -16, 0);
		outMaxSize = Vector(16.0, 16.0, 62.1931/*50.6678*/);
		theSuccess = true;
		break;

	case BUILD_HEALTH:
		outMinSize = kHealthMinSize;
		outMaxSize = kHealthMaxSize;
		theSuccess = true;
		break;

    case BUILD_CAT:
		outMinSize = kCatalystMinSize;
		outMaxSize = kCatalystMaxSize;
		theSuccess = true;
		break;

	case BUILD_HEAVY:
		outMinSize = kHeavyMinSize;
		outMaxSize = kHeavyMaxSize;
		theSuccess = true;
		break;

	case BUILD_JETPACK:
		outMinSize = kJetpackMinSize;
		outMaxSize = kJetpackMaxSize;
		theSuccess = true;
		break;

	case BUILD_AMMO:
		outMinSize = kAmmoMinSize;
		outMaxSize = kAmmoMaxSize;
		theSuccess = true;
		break;

	case BUILD_MINES:
		outMinSize = kWeaponMinSize;
		outMaxSize = Vector(16, 16, 40);
		theSuccess = true;
		break;

	case BUILD_SHOTGUN:
	case BUILD_HMG:
	case BUILD_WELDER:
	case BUILD_NUKE:
	case BUILD_GRENADE_GUN:
		outMinSize = kWeaponMinSize;
		outMaxSize = kWeaponMaxSize;
		theSuccess = true;
		break;

	case ALIEN_BUILD_HIVE:
		outMinSize = kHiveMinSize;
		outMaxSize = kHiveMaxSize;
		theSuccess = true;
		break;
	}

	return theSuccess;
}

bool AvHSHUGetSizeForPlayerUser3(AvHUser3 inUser3, Vector& outMinSize, Vector& outMaxSize, bool inDucking)
{
	bool theSuccess = false;

	// Now set size
	switch(inUser3)
	{
	case AVH_USER3_NONE:
	case AVH_USER3_MARINE_PLAYER:
	case AVH_USER3_COMMANDER_PLAYER:
	case AVH_USER3_ALIEN_PLAYER4:
		// Get simple case working first
		if(inDucking)
		{
			outMinSize = HULL1_MIN;
			outMaxSize = HULL1_MAX;
		}
		else
		{
			outMinSize = HULL0_MIN;
			outMaxSize = HULL0_MAX;
		}
		theSuccess = true;
		break;

	case AVH_USER3_ALIEN_PLAYER1:
	case AVH_USER3_ALIEN_PLAYER2:
	case AVH_USER3_ALIEN_PLAYER3:
	case AVH_USER3_ALIEN_EMBRYO:
		outMinSize = HULL1_MIN;
		outMaxSize = HULL1_MAX;
		theSuccess = true;
		break;

	case AVH_USER3_ALIEN_PLAYER5:
		if(inDucking)
		{
			outMinSize = HULL0_MIN;
			outMaxSize = HULL0_MAX;
		}
		else
		{
			outMinSize = HULL3_MIN;
			outMaxSize = HULL3_MAX;
		}
		theSuccess = true;
		break;
	}

	return theSuccess;
}

bool AvHSHUGetSizeForUser3(AvHUser3 inUser3, Vector& outMinSize, Vector& outMaxSize)
{
	bool theSuccess = false;

	// If it's a player, get max size he can be (assuming he's not ducking)
	theSuccess = AvHSHUGetSizeForPlayerUser3(inUser3, outMinSize, outMaxSize, false);
	if(!theSuccess)
	{
		AvHMessageID theMessageID = MESSAGE_NULL;

		// Convert it to a AvHMessageID if possible
		if(AvHSHUUser3ToMessageID(inUser3, theMessageID))
		{
			theSuccess = AvHSHUGetSizeForTech(theMessageID, outMinSize, outMaxSize);
		}
	}

	return theSuccess;
}

bool AvHSHUUser3ToMessageID(AvHUser3 inUser3, AvHMessageID& outMessageID)
{
	AvHMessageID theMessageID = MESSAGE_NULL;
	bool theSuccess = false;

	switch(inUser3)
	{
	case AVH_USER3_FUNC_RESOURCE:
	case AVH_USER3_RESTOWER:
		theMessageID = BUILD_RESOURCES;
		break;

	case AVH_USER3_COMMANDER_STATION:
		theMessageID = BUILD_COMMANDSTATION;
		break;

	case AVH_USER3_TURRET_FACTORY:
	case AVH_USER3_ADVANCED_TURRET_FACTORY:
		theMessageID = BUILD_TURRET_FACTORY;
		break;

	case AVH_USER3_ARMORY:
	case AVH_USER3_ADVANCED_ARMORY:
		theMessageID = BUILD_ARMORY;
		break;

	case AVH_USER3_ARMSLAB:
		theMessageID = BUILD_ARMSLAB;
		break;

	case AVH_USER3_PROTOTYPE_LAB:
		theMessageID = BUILD_PROTOTYPE_LAB;
		break;

	case AVH_USER3_OBSERVATORY:
		theMessageID = BUILD_OBSERVATORY;
		break;

	case AVH_USER3_TURRET:
		theMessageID = BUILD_TURRET;
		break;

	case AVH_USER3_SIEGETURRET:
		theMessageID = BUILD_SIEGE;
		break;

	case AVH_USER3_INFANTRYPORTAL:
		theMessageID = BUILD_INFANTRYPORTAL;
		break;

	case AVH_USER3_PHASEGATE:
		theMessageID = BUILD_PHASEGATE;
		break;

	case AVH_USER3_HEAVY:
		theMessageID = BUILD_HEAVY;
		break;

	case AVH_USER3_JETPACK:
		theMessageID = BUILD_JETPACK;
		break;

	case AVH_USER3_DEFENSE_CHAMBER:
		theMessageID = ALIEN_BUILD_DEFENSE_CHAMBER;
		break;

	case AVH_USER3_MOVEMENT_CHAMBER:
		theMessageID = ALIEN_BUILD_MOVEMENT_CHAMBER;
		break;

	case AVH_USER3_OFFENSE_CHAMBER:
		theMessageID = ALIEN_BUILD_OFFENSE_CHAMBER;
		break;

	case AVH_USER3_SENSORY_CHAMBER:
		theMessageID = ALIEN_BUILD_SENSORY_CHAMBER;
		break;

	case AVH_USER3_ALIENRESTOWER:
		theMessageID = ALIEN_BUILD_RESOURCES;
		break;

	case AVH_USER3_HIVE:
		theMessageID = ALIEN_BUILD_HIVE;
		break;

	case AVH_USER3_ALIEN_PLAYER1:
		theMessageID = ALIEN_LIFEFORM_ONE;
		break;

	case AVH_USER3_ALIEN_PLAYER2:
		theMessageID = ALIEN_LIFEFORM_TWO;
		break;

	case AVH_USER3_ALIEN_PLAYER3:
		theMessageID = ALIEN_LIFEFORM_THREE;
		break;

	case AVH_USER3_ALIEN_PLAYER4:
		theMessageID = ALIEN_LIFEFORM_FOUR;
		break;

	case AVH_USER3_ALIEN_PLAYER5:
		theMessageID = ALIEN_LIFEFORM_FIVE;
		break;
	}

	if(theMessageID != MESSAGE_NULL)
	{
		outMessageID = theMessageID;
		theSuccess = true;
	}

	return theSuccess;
}

bool AvHSHUMessageIDToUser3(AvHMessageID inMessageID, AvHUser3& outUser3)
{
	bool theSuccess = false;

	AvHUser3 theUser3 = AVH_USER3_NONE;

	switch(inMessageID)
	{
	case BUILD_RESOURCES:
		theUser3 = AVH_USER3_RESTOWER;
		break;

	case BUILD_COMMANDSTATION:
		theUser3 = AVH_USER3_COMMANDER_STATION;
		break;

	case BUILD_TURRET_FACTORY:
		theUser3 = AVH_USER3_TURRET_FACTORY;
		break;

	case TURRET_FACTORY_UPGRADE:
		theUser3 = AVH_USER3_ADVANCED_TURRET_FACTORY;
		break;

	case BUILD_ARMORY:
		theUser3 = AVH_USER3_ARMORY;
		break;

	case ARMORY_UPGRADE:
		theUser3 = AVH_USER3_ADVANCED_ARMORY;
		break;

	case BUILD_ARMSLAB:
		theUser3 = AVH_USER3_ARMSLAB;
		break;

	case BUILD_PROTOTYPE_LAB:
		theUser3 = AVH_USER3_PROTOTYPE_LAB;
		break;

	case BUILD_OBSERVATORY:
		theUser3 = AVH_USER3_OBSERVATORY;
		break;

	case BUILD_TURRET:
		theUser3 = AVH_USER3_TURRET;
		break;

	case BUILD_SIEGE:
		theUser3 = AVH_USER3_SIEGETURRET;
		break;

	case BUILD_INFANTRYPORTAL:
		theUser3 = AVH_USER3_INFANTRYPORTAL;
		break;

	case BUILD_PHASEGATE:
		theUser3 = AVH_USER3_PHASEGATE;
		break;

	case BUILD_HEAVY:
		theUser3 = AVH_USER3_HEAVY;
		break;

	case BUILD_JETPACK:
		theUser3 = AVH_USER3_JETPACK;
		break;

	// Menus
	case MENU_BUILD:
		theUser3 = AVH_USER3_MENU_BUILD;
		break;
	case MENU_BUILD_ADVANCED:
		theUser3 = AVH_USER3_MENU_BUILD_ADVANCED;
		break;
	case MENU_ASSIST:
		theUser3 = AVH_USER3_MENU_ASSIST;
		break;
	case MENU_EQUIP:
		theUser3 = AVH_USER3_MENU_EQUIP;
		break;

	// Weapons
	case BUILD_MINES:
		theUser3 = AVH_USER3_MINE;
		break;

	// Lifeforms
	case ALIEN_LIFEFORM_ONE:
		theUser3 = AVH_USER3_ALIEN_PLAYER1;
		break;

	case ALIEN_LIFEFORM_TWO:
		theUser3 = AVH_USER3_ALIEN_PLAYER2;
		break;

	case ALIEN_LIFEFORM_THREE:
		theUser3 = AVH_USER3_ALIEN_PLAYER3;
		break;

	case ALIEN_LIFEFORM_FOUR:
		theUser3 = AVH_USER3_ALIEN_PLAYER4;
		break;

	case ALIEN_LIFEFORM_FIVE:
		theUser3 = AVH_USER3_ALIEN_PLAYER5;
		break;
	}

	if(theUser3 != AVH_USER3_NONE)
	{
		outUser3 = theUser3;
		theSuccess = true;
	}

	return theSuccess;
}


float AvHSHUGetTime()
{
	float theTime = 0;

	#ifdef AVH_SERVER
	theTime = gpGlobals->time;
	#else
	theTime = gEngfuncs.GetClientTime();
	#endif

	return theTime;
}



// Note, all these models must be precached already
char* AvHSHUGetBuildTechModelName(AvHMessageID inMessageID)
{
	char* theModelName = NULL;

	switch(inMessageID)
	{
	case BUILD_RESOURCES:
		theModelName = kResourceTowerModel;
		break;

	//case BUILD_REINFORCEMENTS:
	//	theModelName = kInfantryPortalModel;
	//	break;

	case BUILD_INFANTRYPORTAL:
		theModelName = kInfantryPortalModel;
		break;

	case BUILD_COMMANDSTATION:
		theModelName = kCommandStationModel;
		break;

	case BUILD_TURRET_FACTORY:
		theModelName = kTurretFactoryModel;
		break;

	case BUILD_ARMSLAB:
		theModelName = kArmsLabModel;
		break;

	case BUILD_PROTOTYPE_LAB:
		theModelName = kPrototypeLabModel;
		break;

	case BUILD_ARMORY:
		theModelName = kArmoryModel;
		break;

	case ARMORY_UPGRADE:
		theModelName = kAdvancedWeaponFactoryModel;
		break;

	case BUILD_OBSERVATORY:
		theModelName = kObservatoryModel;
		break;

	case BUILD_SCAN:
		theModelName = kScanModel;
		break;

	case BUILD_PHASEGATE:
		theModelName = kPhaseGateModel;
		break;

	case BUILD_TURRET:
		theModelName = kDeployedTurretModel;
		break;

	case BUILD_NUKE:
		theModelName = kNukeModel;
		break;

	case BUILD_SIEGE:
		theModelName = kSiegeTurretModel;
		//theModelName = kDeployedTurretModel;
		break;

    case BUILD_CAT:
        theModelName = kCatalystModel;
        break;

	case BUILD_HEALTH:
		theModelName = kHealthModel;
		break;

	case BUILD_HEAVY:
		theModelName = kHeavyModel;
		break;

	case BUILD_JETPACK:
		theModelName = kJetpackModel;
		break;

	case BUILD_AMMO:
		theModelName = kAmmoModel;
		break;

	case BUILD_WELDER:
		theModelName = kWelderWModel;
		break;

	case BUILD_MINES:
		theModelName = kTripmineW2Model;
		break;

	case BUILD_SHOTGUN:
		theModelName = kSGWModel;
		break;

	case BUILD_HMG:
		theModelName = kHMGWModel;
		break;

	case BUILD_GRENADE_GUN:
		theModelName = kGGWModel;
		break;

	// Alien buildings
	case ALIEN_BUILD_HIVE:
		theModelName = kHiveModel;
		break;

	case ALIEN_BUILD_RESOURCES:
		theModelName = kAlienResourceTowerModel;
		break;

	case ALIEN_BUILD_OFFENSE_CHAMBER:
		theModelName = kOffenseChamberModel;
		break;

	case ALIEN_BUILD_DEFENSE_CHAMBER:
		theModelName = kDefenseChamberModel;
		break;

	case ALIEN_BUILD_SENSORY_CHAMBER:
		theModelName = kSensoryChamberModel;
		break;

	case ALIEN_BUILD_MOVEMENT_CHAMBER:
		theModelName = kMovementChamberModel;
		break;
	}

	return theModelName;
}

bool AvHSHUGetBuildTechRange(AvHMessageID inMessageID, float& outRange)
{
	bool theSuccess = false;

	//	switch(inMessageID)
	//	{
	//	case BUILD_CAMERA:
	//		outRange = kResourceTowerSightRange;
	//		theSuccess = true;
	//		break;
	//	case BUILD_PHASEGATE:
	//		break;
	//	case BUILD_TURRET:
	//		outRange = TURRET_RANGE;
	//		theSuccess = true;
	//		break;
	//	case BUILD_SIEGE:
	//		outRange = kSiegeTurretMaxRange;
	//		theSuccess = true;
	//		break;
	//	}

	return theSuccess;
}

bool AvHSHUTraceLineIsAreaFree(Vector& inStart, Vector& inEnd, edict_t* inIgnoreEntity, bool inIgnorePlayers)
{
	bool theAreaIsFree = true;

#ifdef AVH_SERVER
	TraceResult theTR;
	bool theIsDone = false;

	// Do tracelines between the corners, to make sure there's no geometry inside the box
	int theNumIters = 0;

    IGNORE_MONSTERS theIgnoreMonsters = dont_ignore_monsters;

    if (inIgnorePlayers)
    {
        theIgnoreMonsters = ignore_monsters;
    }

	while(!theIsDone)
	{
		UTIL_TraceLine(inStart, inEnd, theIgnoreMonsters, inIgnoreEntity, &theTR);
		if(theTR.flFraction != 1.0f)
		{
			CBaseEntity* theEntity = CBaseEntity::Instance(ENT(theTR.pHit));
			if(theEntity && (theEntity->pev->solid != SOLID_BBOX) && (theEntity->pev->solid != SOLID_SLIDEBOX) && (theEntity->pev->solid != SOLID_BSP) && (inIgnorePlayers != !!theEntity->IsPlayer()) )
			{
				VectorCopy(theTR.vecEndPos, inStart);
			}
			else
			{
				theIsDone = true;
				theAreaIsFree = false;
			}
		}
		else
		{
			theIsDone = true;
		}

		if(theNumIters++ > 50)
		{
			theIsDone = true;
			theAreaIsFree = false;
		}
	}
#endif

//	int theIndex;
//	vec3_t theEndLocation;
//	AvHTeamNumber theTeam;
//	bool thePlayerHit;
//	int theUserThree;
//	int theUserFour;
//
//	if(AvHSHUTraceTangible(inStart, inEnd, theIndex, theEndLocation, theTeam, thePlayerHit, theUserThree, theUserFour))
//	{
//		if(theIndex >= 0)
//		{
//			theAreaIsFree = false;
//		}
//	}

	return theAreaIsFree;
}

float AvHTraceLineAgainstWorld(Vector& vecStart, Vector& vecEnd)
{

#ifdef AVH_SERVER

    TraceResult tr;
    UTIL_TraceLine(vecStart, vecEnd, ignore_monsters, dont_ignore_glass, NULL, &tr);

    return tr.flFraction;

#endif

#ifdef AVH_CLIENT

    // This may not be the most efficient way, but it seems to get the job done.

    float theFraction = 1;

    for (int i = 0; i < kMaxEntities && theFraction > 0; ++i)
    {

        cl_entity_t* theEntity = gEngfuncs.GetEntityByIndex(i);

        if (theEntity != NULL && theEntity->model != NULL && theEntity->model->type == mod_brush)
        {

            vec3_t localStart;
            vec3_t localEnd;

            // Translate the start and end into the model's frame of reference.

            VectorSubtract(vecStart, theEntity->origin, localStart);
            VectorSubtract(vecEnd, theEntity->origin, localEnd);

            // Rotate the start and end into the model's frame of reference.

            if (theEntity->angles[0] ||
                theEntity->angles[1] ||
                theEntity->angles[2])
            {

                vec3_t forward;
                vec3_t right;
                vec3_t up;

                AngleVectors(theEntity->angles, forward, right, up);

                vec3_t temp;

                VectorCopy(localStart, temp);
                localStart[0] =  DotProduct(temp, forward);
                localStart[1] = -DotProduct(temp, right);
                localStart[2] =  DotProduct(temp, up);

                VectorCopy(localEnd, temp);
                localEnd[0] =  DotProduct(temp, forward);
                localEnd[1] = -DotProduct(temp, right);
                localEnd[2] =  DotProduct(temp, up);

            }

            trace_t tr;
            NS_TraceLine(&theEntity->model->hulls[0], localStart, localEnd, &tr);

            if (tr.fraction < theFraction)
            {
                theFraction = tr.fraction;
            }

        }

    }

    return theFraction;

#endif

}


bool AvHSHUGetCanBeBuiltOnPlayers(AvHMessageID inMessageID)
{
	bool theCanBeBuiltOnPlayers = false;

	switch(inMessageID)
	{
		case ALIEN_BUILD_HIVE:
		case BUILD_SCAN:
		case BUILD_AMMO:
		case BUILD_HEALTH:
		case BUILD_CAT:
		case BUILD_MINES:
		case BUILD_WELDER:
		case BUILD_SHOTGUN:
		case BUILD_HMG:
		case BUILD_GRENADE_GUN:
		case BUILD_HEAVY:
		case BUILD_JETPACK:
			theCanBeBuiltOnPlayers = true;
			break;
	}

	return theCanBeBuiltOnPlayers;
}

bool AvHSHUGetIsSiteValidForBuild(AvHMessageID inMessageID, Vector* inLocation, int inIgnoredEntityIndex)
{
	//TODO: - check entity returned by client side commander trace for drop validity before we ever get this far

	bool theSuccess = false;

	// Check that there's enough room for building
	vec3_t theMinSize, theMaxSize;
	if(AvHSHUGetSizeForTech(inMessageID, theMinSize, theMaxSize, true))
	{

		// If it's a build with special placement requirements, check that here (this could modify the location also)
		if(AvHSHUGetAreSpecialBuildingRequirementsMet(inMessageID, *inLocation))
		{

			// Assume mappers place hives correctly (it's such a showstopper if aliens can't create a hive because it's too close to a wall)
			// KGP 4/28/04 - add res towers to hives for the same reason -- if a mapper didn't give room, it's a showstopper.
			bool theIgnorePlayers = AvHSHUGetCanBeBuiltOnPlayers(inMessageID);
			float theSlopeTangent = theIgnorePlayers ? kMaxEquipmentDropSlope : kMaxBuildingDropSlope;
			bool theSkipDropCheck = false;
			switch(inMessageID)
			{
				case BUILD_RESOURCES:
					theSkipDropCheck = true;
					break;
				case ALIEN_BUILD_RESOURCES:
					theSkipDropCheck = true;
					break;
				case ALIEN_BUILD_HIVE:
					theSkipDropCheck = true;
					break;
			}
			if(theSkipDropCheck || AvHSHUGetCanDropItem(*inLocation, theMinSize, theMaxSize, theSlopeTangent, inIgnoredEntityIndex, theIgnorePlayers))
			{
				//TODO: - better check for entity below drop, since it's often off center.
				// Check to make sure building isn't being created on top of another building
				vec3_t thePositionBelowBuild = *inLocation;
				thePositionBelowBuild.z = -kMaxMapDimension;

				int theIndex;
				vec3_t theEndLocation;
				AvHTeamNumber theTeam;
				bool thePlayerHit;
				int theUserThree;
				int theUserFour;

				// Don't allow building on any entities, except allow resource towers on top of func_resources

				//TODO : use full list instead of physents, which isn't accounting for items the commander can't see
				bool theHitSomething = AvHSHUTraceTangible(*inLocation, thePositionBelowBuild, theIndex, theEndLocation, theTeam, thePlayerHit, theUserThree, theUserFour);

				//res building case
				if(inMessageID == BUILD_RESOURCES || inMessageID == ALIEN_BUILD_RESOURCES)
				{
#ifdef AVH_SERVER	//on server, return false if occupied
					FOR_ALL_ENTITIES(kesFuncResource,AvHFuncResource*)
                        if(!theEntity->GetIsOccupied()) // open for use
						{
							if(VectorDistance(theEntity->pev->origin,*inLocation) < 20) //small enough that we don't check the wrong nozzle
							{
								theSuccess = (!theHitSomething || theIndex <= 0 || theUserThree == AVH_USER3_FUNC_RESOURCE);
								break;
							}
						}
					END_FOR_ALL_ENTITIES(kesFuncResource)
#else //on client the occupied function isn't available
					theSuccess = (!theHitSomething || theIndex <= 0 || theUserThree == AVH_USER3_FUNC_RESOURCE);
#endif
				}
				else if ( inMessageID == ALIEN_BUILD_HIVE ) 
				{
					theSuccess = true;
					//theSuccess = ( !theHitSomething || ( *inLocation[2] - theEndLocation[2] > 40.0f ) );
//#ifdef AVH_SERVER
//					ALERT(at_console, "theHitSomething=%d theSuccess=%d\n", theHitSomething, theSuccess);
//					ALERT(at_console, "%f\n", *inLocation[2] - theEndLocation[2]);
//					ALERT(at_console, "{%f, %f, %f} : { %f, %f, %f }\n", 
//						inLocation[0], inLocation[1], inLocation[2], 
//						theEndLocation[0], theEndLocation[1], theEndLocation[2]);
//#endif
				}
				else if(!theHitSomething || (theIgnorePlayers && thePlayerHit) || theIndex <= 0)
				{
					// THEN it's a legal build spot and the building shows up green.
					theSuccess = true;
				}
			}
		}
	}
	return theSuccess;
}

int AvHSHUGetPointContents(vec3_t inPoint)
{
	int thePointContents = 0;

	#ifdef AVH_SERVER
	thePointContents = UTIL_PointContents(inPoint);
	#endif

	#ifdef AVH_CLIENT
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);

	thePointContents = gEngfuncs.PM_PointContents(inPoint, NULL);

	gEngfuncs.pEventAPI->EV_PopPMStates();
	#endif

	return thePointContents;
}

 /*
 * Check to see if the build or item is overlapping the world.
 */
bool AvHSHUGetIsVolumeContentNonsolid(vec3_t inCenter, Vector& inMinSize, Vector& inMaxSize, edict_t* inIgnoreEntity, bool inIgnorePlayers)
{

	float theMins[3], theMaxs[3];
	VectorAdd(inCenter,inMinSize,theMins);
	VectorAdd(inCenter,inMaxSize,theMaxs);

	bool theIsNonsolid = true; //innocent until proven guilty

	extern playermove_t *pmove;
    if (pmove != NULL && pmove->physents[0].model != NULL)
    {
		const hull_t* theHull = &pmove->physents[0].model->hulls[NS_GetValveHull(2)];
		int theContents = NS_BoxContents(theHull,theHull->firstclipnode,theMins,theMaxs);
		theIsNonsolid = (theContents != CONTENT_SOLID);
	}
    else //fallback if we're just starting a round
    {
        int theContents = AvHSHUGetPointContents(inCenter);
		if(theContents != CONTENT_SOLID)
		{
			//trace between each corner pair once looking for obstructions - 28 total comparisons
			const static int MIN = 0;
			const static int MAX = 1;
			const static int NUM_CORNERS = 8;
			int theIndex;
			Vector theCorners[NUM_CORNERS];

			for(int XPos = MIN; XPos <= MAX; ++XPos)
			{
				for(int YPos = MIN; YPos <= MAX; ++YPos)
				{
					for(int ZPos = MIN; ZPos <= MAX; ++ZPos)
					{
						theIndex = XPos+YPos*2+ZPos*4;
						theCorners[theIndex].x = inCenter.x + ((XPos == MIN) ? inMinSize.x : inMaxSize.x);
						theCorners[theIndex].y = inCenter.y + ((YPos == MIN) ? inMinSize.y : inMaxSize.y);
						theCorners[theIndex].z = inCenter.z + ((ZPos == MIN) ? inMinSize.z : inMaxSize.z);
					}
				}
			}

			for(int startCorner = 0; startCorner < NUM_CORNERS; ++startCorner)
			{
				for(int endCorner = startCorner+1; endCorner < NUM_CORNERS; ++endCorner)
				{
					if(!AvHSHUTraceLineIsAreaFree(theCorners[startCorner],theCorners[endCorner],inIgnoreEntity,inIgnorePlayers))
					{
						theIsNonsolid = false;
						break;
					}
				}
				if(!theIsNonsolid)
				{
					break;
				}
			}
		}
		else
		{
			theIsNonsolid = false;
		}
	}
	return theIsNonsolid;
}

bool AvHSHUGetIsAreaFree(vec3_t inCenter, Vector& inMinSize, Vector& inMaxSize, edict_t* inIgnoreEntity, bool inIgnorePlayers)
{
	bool theAreaIsFree = AvHSHUGetIsVolumeContentNonsolid(inCenter,inMinSize,inMaxSize,inIgnoreEntity,inIgnorePlayers);

	if(theAreaIsFree)
	{
		theAreaIsFree = !AvHSHUGetEntitiesBlocking(inCenter, inMinSize, inMaxSize, inIgnoreEntity, inIgnorePlayers);
	}

	return theAreaIsFree;
}

bool AvHSHUGetEntitiesBlocking(Vector& inOrigin, Vector& inMinSize, Vector& inMaxSize, edict_t* inIgnoreEntity, bool inIgnorePlayers)
{
	bool theEntBlocking = false;

	typedef vector< pair<int, Vector> > PhysEntListType;
	PhysEntListType theEntList;

	const int kSearchRadius = 800;

	// Populate phys ent list
	#ifdef AVH_SERVER
	CBaseEntity* theEntity = NULL;
	while ((theEntity = UTIL_FindEntityInSphere(theEntity, inOrigin, kSearchRadius)) != NULL)
	{

        // If entity is visible non-world object, add it
		if((theEntity->pev->team != TEAM_IND) && (theEntity->pev->solid != SOLID_NOT && theEntity->pev->solid != SOLID_TRIGGER))
		{

            AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);

            if(theEntity->edict() != inIgnoreEntity && !(thePlayer != NULL && inIgnorePlayers))
			{
				theEntList.push_back(make_pair(theEntity->pev->iuser3, theEntity->pev->origin));
			}
		}
	}
	#endif

	#ifdef AVH_CLIENT
	// Haven't implemented this, so depend on it not being passed in
	ASSERT(inIgnoreEntity == NULL);

	for(int i = 0; i < pmove->numphysent; i++)
	{
		physent_t* thePhysEnt = pmove->physents + i;
		if((thePhysEnt->team != TEAM_IND) && (thePhysEnt->solid != SOLID_NOT && thePhysEnt->solid != SOLID_TRIGGER))
		{
			theEntList.push_back(make_pair(thePhysEnt->iuser3, thePhysEnt->origin));
		}
	}
	#endif

	Vector theAbsMax;
	VectorAdd(inOrigin, inMaxSize, theAbsMax);

	Vector theAbsMin;
	VectorAdd(inOrigin, inMinSize, theAbsMin);

	Vector theOrigin1;
	VectorAdd(theAbsMax, theAbsMin, theOrigin1);
	theOrigin1 = theOrigin1*.5f;

	Vector theSize1;
	VectorSubtract(theAbsMax, theAbsMin, theSize1);
	theSize1 = theSize1*.5f;

	// Do collision on list, making sure none are too close to inOrigin
	for(PhysEntListType::iterator theIter = theEntList.begin(); theIter != theEntList.end(); theIter++)
	{
		// Get size for tech
		Vector theMinSize, theMaxSize;
		if(AvHSHUGetSizeForUser3(AvHUser3(theIter->first), theMinSize, theMaxSize))
		{
			Vector theEntOrigin = theIter->second;

			Vector theAbsMax;
			VectorAdd(theEntOrigin, theMaxSize, theAbsMax);

			Vector theAbsMin;
			VectorAdd(theEntOrigin, inMinSize, theAbsMin);

			Vector theOrigin2;
			VectorAdd(theAbsMax, theAbsMin, theOrigin2);
			theOrigin2 = theOrigin2*.5f;

			Vector theSize2;
			VectorSubtract(theAbsMax, theAbsMin, theSize2);
			theSize2 = theSize2*.5f;

			// Do simple box collision here
			if(NS_BoxesOverlap((float*)theOrigin1, (float*)theSize1, (float*)theOrigin2, (float*)theSize2))
			{
				theEntBlocking = true;
				break;
			}
		}
	}

	return theEntBlocking;
}



void AvHSHUMakeViewFriendlyKillerName(string& ioKillerName)
{
	string theOutputName = ioKillerName;
	int theStrLen = (int)ioKillerName.length();

	if(!strncmp(ioKillerName.c_str(), "weapon_", 7))
	{
		theOutputName = ioKillerName.substr(7);
	}
	else if(!strncmp(ioKillerName.c_str(), "monster_", 8))
	{
		theOutputName = ioKillerName.substr(8);
	}
	else if(!strncmp(ioKillerName.c_str(), "func_", 5))
	{
		theOutputName = ioKillerName.substr(5);
	}

	ioKillerName = theOutputName;
}

bool AvHSHUGetCanDropItem(vec3_t& ioCenter, Vector& inMinSize, Vector& inMaxSize, float inMaxSlopeTangent, int inIgnoreIndex, bool inIgnorePlayers)
{

	float theMaxXWidth = max(-inMinSize[0],inMaxSize[0]);
	float theMaxYWidth = max(-inMinSize[1],inMaxSize[1]);
	float theRadius = sqrt(theMaxXWidth*theMaxXWidth + theMaxYWidth * theMaxYWidth);
	float theHeight = inMaxSize[2] - inMinSize[2];
	//adjust origin to be base
	float theOrigin[3] = { ioCenter[0], ioCenter[1], ioCenter[2] + inMinSize[2] };

	CollisionChecker Checker(pmove,kHLPointHullIndex,CollisionChecker::HULL_TYPE_ALL,inIgnorePlayers,CollisionChecker::IGNORE_NONE,inIgnoreIndex);
	bool theCanDropItem = (Checker.GetContentsInCylinder(theOrigin,theRadius,theHeight) != CONTENTS_SOLID);


	if(!theCanDropItem) //can't place it -- can we drop it?
	{

		float theMaxDropHeight = theRadius * inMaxSlopeTangent;

		float theDropOrigin[3] = { theOrigin[0], theOrigin[1], theOrigin[2] + theMaxDropHeight };
		theCanDropItem = (Checker.GetContentsInCylinder(theDropOrigin,theRadius,theHeight) != CONTENTS_SOLID);

		if(theCanDropItem) //can drop it -- get as low to ground as possible for drop
		{

			float theBestDropHeight = theMaxDropHeight;
			float theShiftFraction = 1.0f;
			for(float theShiftAdjust = 0.5f; theShiftAdjust > 0.05f; theShiftAdjust /= 2)
			{
				theShiftFraction += theShiftAdjust * (theCanDropItem ? -1 : 1); //try lower if last was a success
				theDropOrigin[2] = theMaxDropHeight;
				theDropOrigin[2] *= theShiftFraction;
				theDropOrigin[2] += theOrigin[2];

				theCanDropItem = (Checker.GetContentsInCylinder(theDropOrigin,theRadius,theHeight) != CONTENTS_SOLID);
				if(theCanDropItem)
				{
					theBestDropHeight = theShiftFraction;
					theBestDropHeight *= theMaxDropHeight;
				}
			}

			//adjust center to best position
			ioCenter[2] += theBestDropHeight;
			theCanDropItem = true;
		}
	}

	return theCanDropItem;
}

bool AvHSHUTraceAndGetIsSiteValidForBuild(AvHMessageID inMessageID, const Vector& inPointOfView, const Vector& inNormRay, Vector* outLocation)
{
	bool theSuccess = false;

	// TODO: Check if area is within the mapextents

	int theUser3;
	bool theTraceSuccess = AvHSHUTraceTangible(inPointOfView, inNormRay, &theUser3, outLocation);

	// tankefugl: 0000291
	// ignore trace for scans (removed due to cost being paid when drop failed)
	//if (inMessageID == BUILD_SCAN)
	//{
	//	theSuccess = true;
	//}
    //else
	// :tankefugl
	if(theTraceSuccess)
	{
		// tankefugl: 0000291
		if((inMessageID == BUILD_SCAN) || (AvHSHUGetIsSiteValidForBuild(inMessageID, outLocation)))
		// :tankefugl
		{
			theSuccess = true;
		}

	}
	return theSuccess;
}

#ifdef AVH_CLIENT
bool AvHSHUGetEntityAtRay(const Vector& inPointOfView, const Vector& inNormRay, int& outEntIndex)
{
	bool theSuccess = false;

	// Offset starting position a little so we don't select ourselves
	Vector theStartPosition;
	VectorMA(inPointOfView, kSelectionStartRange, inNormRay, theStartPosition);

	Vector theEndPos;
	VectorMA(inPointOfView, kSelectionEndRange, inNormRay, theEndPos);

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	int theNumPlayers = gEngfuncs.GetMaxClients();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( -1 );

	bool theDone = false;
	int theEntityIndex = -1;

	vec3_t theNewStartPos;
	AvHSHUGetFirstNonSolidPoint(theStartPosition, theEndPos, theNewStartPos);
	VectorCopy(theNewStartPos, theStartPosition);

	do
	{
		pmtrace_t tr;
		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( theStartPosition, theEndPos, PM_NORMAL, theEntityIndex, &tr );

		physent_t *pEntity = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
		theEntityIndex = gEngfuncs.pEventAPI->EV_IndexFromTrace( &tr );

		if(pEntity)
		{
			int theUser3 = pEntity->iuser3;
			if(theEntityIndex > 0)
			{
				outEntIndex = theEntityIndex;
				theSuccess = true;
				theDone = true;
			}
		}

		if((tr.fraction >= (1.0f - kFloatTolerance)) || (tr.fraction < kFloatTolerance))
		{
			theDone = true;
		}
		else
		{
			VectorCopy(tr.endpos, theStartPosition);
		}

	} while(!theDone);

	gEngfuncs.pEventAPI->EV_PopPMStates();

	return theSuccess;
}
#endif

#ifdef AVH_SERVER
bool AvHSHUGetEntityAtRay(const Vector& inPointOfView, const Vector& inNormRay, int& outEntIndex)
{
	TraceResult		tr;
	Vector			theStartPos;
	Vector			theEndPos;
	bool			theSuccess = false;
	bool			theDone = false;

	VectorMA(inPointOfView, kSelectionStartRange, inNormRay, theStartPos);
	VectorMA(inPointOfView, kSelectionEndRange, inNormRay, theEndPos);

	CBaseEntity* theEntityHit = NULL;
	edict_t* theEdictToIgnore = NULL;

	vec3_t theNewStartPos;
	AvHSHUGetFirstNonSolidPoint(theStartPos, theEndPos, theNewStartPos);
	VectorCopy(theNewStartPos, theStartPos);

	do
	{
		UTIL_TraceLine(theStartPos, theEndPos, dont_ignore_monsters, ignore_glass, theEdictToIgnore, &tr);

		theEntityHit = CBaseEntity::Instance(tr.pHit);
		if(theEntityHit)
		{
			if(theEntityHit->entindex() > 0)
			{
				outEntIndex = theEntityHit->entindex();
				theSuccess = true;
				theDone = true;
			}
		}


		if((tr.flFraction > (1.0f - kFloatTolerance)) || (tr.flFraction < kFloatTolerance))
		{
			theDone = true;
		}
		else
		{
			if(theEntityHit)
			{
				theEdictToIgnore = ENT(theEntityHit->pev);
			}
			VectorCopy(tr.vecEndPos, theStartPos);
		}
	} while(!theDone);

	return theSuccess;
}
#endif

const AvHMapExtents& AvHSHUGetMapExtents()
{
	#ifdef AVH_CLIENT
	const AvHMapExtents& theMapExtents = gHUD.GetMapExtents();
	#endif

	#ifdef AVH_SERVER
	const AvHMapExtents& theMapExtents = GetGameRules()->GetMapExtents();
	#endif

	return theMapExtents;
}


#ifdef AVH_CLIENT
bool AvHSUClientTraceTangible(const vec3_t& inStartPos, const vec3_t& inEndPos, int& outIndex, vec3_t& outLocation, AvHTeamNumber& outTeamNumber, bool& outPlayerWasHit, int& outUserThree, int& outUserFour)
{
	physent_t*	theEntity = NULL;
	vec3_t		theStartPos;
	vec3_t		theEndPos;
	int			theFoundEntity = -1;
	bool		theSuccess = false;
	bool		theDone = false;

	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inEndPos, theEndPos);

	vec3_t theNormal;
	VectorSubtract(inEndPos, inStartPos, theNormal);
	VectorNormalize(theNormal);

	outIndex = -1;

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	int theNumPlayers = gEngfuncs.GetMaxClients();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( -1 );

	vec3_t theNewStartPos;
	AvHSHUGetFirstNonSolidPoint(theStartPos, theEndPos, theNewStartPos);
	VectorCopy(theNewStartPos, theStartPos);

	do
	{
		pmtrace_t tr;
		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( theStartPos, theEndPos, PM_NORMAL/*PM_GLASS_IGNORE*/, theFoundEntity, &tr );

		physent_t *pEntity = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
		theFoundEntity = gEngfuncs.pEventAPI->EV_IndexFromTrace( &tr );

		if(pEntity)
		{
			if(pEntity->iuser3 != AVH_USER3_NONE)
			{
				VectorCopy(tr.endpos, outLocation);
				outIndex = theFoundEntity;
				theEntity = pEntity;
				outUserThree = pEntity->iuser3;
				outUserFour = pEntity->iuser4;
				outTeamNumber = (AvHTeamNumber)(pEntity->team);

				if(pEntity->player)
				{
					outPlayerWasHit = true;
				}

				theSuccess = true;
				theDone = true;
			}
		}

		if(tr.fraction >= (1.0f - kFloatTolerance))
		{
			theDone = true;
		}
		else
		{
			vec3_t theDiff = theNormal*kHitOffsetAmount;
			float theLength = sqrt(theDiff.x*theDiff.x + theDiff.y*theDiff.y + theDiff.z*theDiff.z);
			if(theLength > kFloatTolerance)
			{
				VectorAdd(tr.endpos, theDiff, theStartPos);
			}
			else
			{
				theDone = true;
			}
			// Offset a bit so we don't hit again
			//VectorMA(tr.endpos, kHitOffsetAmount, theNormal, theStartPos);
		}
	} while(!theDone);

	gEngfuncs.pEventAPI->EV_PopPMStates();

	// If we didn't hit any special targets, see if it's a valid area to build or be ordered to
	if(!theSuccess)
	{
		WaypointReturnCode theReturnCode = WAYPOINT_SUCCESS;
		theSuccess = AvHSHUClientTraceWaypoint(inStartPos, inEndPos, &outLocation, &theReturnCode);
		bool theWaypointTooSteep = (theReturnCode == WAYPOINT_TOOSTEEP);

		if(theSuccess || theWaypointTooSteep)
		{
			outUserThree = AVH_USER3_WAYPOINT;
			outPlayerWasHit = false;
			outIndex = -1;
			outTeamNumber = TEAM_IND;
			theSuccess = true;
		}

		// Treat too steep as success, but mark it as nobuild
		if(theWaypointTooSteep)
		{
			outUserThree = AVH_USER3_NOBUILD;
		}
	}

	return theSuccess;
}
#endif

#ifdef AVH_SERVER
bool AvHSUServerTraceTangible(const vec3_t& inStartPos, const vec3_t& inEndPos, int& outIndex, vec3_t& outLocation, AvHTeamNumber& outTeamNumber, bool& outPlayerWasHit, int& outUserThree, int& outUserFour)
{
	bool theSuccess = false;
	bool theDone = false;
	edict_t* theEdictToIgnore = NULL;
	vec3_t theStartPos;
	vec3_t theEndPos;

	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inEndPos, theEndPos);

	vec3_t theNormal;
	VectorSubtract(inEndPos, inStartPos, theNormal);
	VectorNormalize(theNormal);

//	vec3_t theNewStartPos;
//	AvHSHUGetFirstNonSolidPoint(theStartPos, theEndPos, theNewStartPos);
//	VectorCopy(theNewStartPos, theStartPos);

	do
	{
		TraceResult tr;
		UTIL_TraceLine(theStartPos, theEndPos, dont_ignore_monsters, theEdictToIgnore, &tr);
		CBaseEntity* theEntityHit = NULL;

		// Return the entity in special way
		if(tr.flFraction < 1 && tr.pHit)
		{
			theEntityHit = CBaseEntity::Instance(tr.pHit);
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntityHit);
			AvHWeldable* theWeldable = dynamic_cast<AvHWeldable*>(theEntityHit);

			AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(theEntityHit);

			if(AvHSUGetIsDebugging())
			{
				// Create an entity where the trace hit
				Vector theAngles(0, 0, 0);

				CBaseEntity* pEnt = CBaseEntity::Create(kwsDebugEntity, tr.vecEndPos, theAngles);
				ASSERT(pEnt);
				pEnt->pev->movetype = MOVETYPE_FLY;
				pEnt->pev->solid = SOLID_NOT;
			}

			if(theEntityHit && (theEntityHit->pev->iuser3 != AVH_USER3_NONE))
			{
				// Don't hit seethroughs
				if(theEntityHit->pev->iuser3 != AVH_USER3_ALPHA)
				{
					outIndex = ENTINDEX(tr.pHit);
					VectorCopy(tr.vecEndPos, outLocation);
					outTeamNumber = (AvHTeamNumber)theEntityHit->pev->team;
					outUserThree = theEntityHit->pev->iuser3;
					outUserFour = theEntityHit->pev->iuser4;

					if(thePlayer)
					{
						outPlayerWasHit = true;
					}

					theSuccess = true;
					theDone = true;
				}
			}

			theEdictToIgnore = theEntityHit->edict();
		}

		if((tr.flFraction >= (1.0f - kFloatTolerance)) || (tr.flFraction < kFloatTolerance))
		{
			theDone = true;
		}
		else
		{
			vec3_t theDiff = theNormal*kHitOffsetAmount;
			float theLength = sqrt(theDiff.x*theDiff.x + theDiff.y*theDiff.y + theDiff.z*theDiff.z);
			if(theLength > kFloatTolerance)
			{
				VectorAdd(theStartPos, theDiff, theStartPos);
			}
			else
			{
				theDone = true;
			}
			// Offset a bit so we don't hit again
			//VectorMA(tr.vecEndPos, kHitOffsetAmount, theNormal, theStartPos);
		}
	} while(!theDone);

	if(!theSuccess)
	{
		WaypointReturnCode theReturnCode = WAYPOINT_SUCCESS;
		theSuccess = AvHSHUServerTraceWaypoint(inStartPos, inEndPos, &outLocation, &theReturnCode);
		bool theWaypointTooSteep = (theReturnCode == WAYPOINT_TOOSTEEP);
		if(theSuccess || theWaypointTooSteep)
		{
			outUserThree = AVH_USER3_WAYPOINT;
			outPlayerWasHit = false;
			outIndex = -1;
			outTeamNumber = TEAM_IND;
			theSuccess = true;
		}
		// Treat too steep as success, but mark it as nobuild
		if(theWaypointTooSteep)
		{
			outUserThree = AVH_USER3_NOBUILD;
		}
	}

	return theSuccess;
}
#endif



#ifdef AVH_CLIENT
bool AvHSHUClientTraceWaypoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t* outLocation, WaypointReturnCode* outReturnCode)
{
	vec3_t		theStartPos;
	vec3_t		theEndPos;
	int			theFoundEntity = -1;
	int			theEntityToIgnore = -1;
	bool		theDone = false;
	bool		theLegalToBuild = false;

	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inEndPos, theEndPos);

	vec3_t theNormal;
	VectorSubtract(inEndPos, inStartPos, theNormal);
	VectorNormalize(theNormal);

	int theNumPlayers = gEngfuncs.GetMaxClients();
	pmtrace_t tr;

	//DebugPoint theDebugPoint(theStartPos[0], theStartPos[1], theStartPos[2]);
	//gSquareDebugLocations.push_back(theDebugPoint);

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);

	vec3_t theNewStartPos;
	AvHSHUGetFirstNonSolidPoint(theStartPos, theEndPos, theNewStartPos);
	VectorCopy(theNewStartPos, theStartPos);

	do
	{
		gEngfuncs.pEventAPI->EV_PlayerTrace( theStartPos, theEndPos, PM_NORMAL/*PM_GLASS_IGNORE*/, theEntityToIgnore, &tr );

		physent_t* pEntity = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent );
		theFoundEntity = gEngfuncs.pEventAPI->EV_IndexFromTrace( &tr );

		// Trace until we hit a worldbrush or non-seethrough
		if(!pEntity || (pEntity->iuser3 != AVH_USER3_ALPHA))
		{
			// If entity is a "no waypoint" entity we can't build here and we're done
			if(pEntity && (pEntity->iuser3 == AVH_USER3_NOBUILD))
			{
				if(outReturnCode)
				{
					*outReturnCode = WAYPOINT_NOBUILD;
				}
				theDone = true;
			}
			else
			{
				// else if texture is NOBUILD, we're done
				const char* theTextureHitCStr = gEngfuncs.pEventAPI->EV_TraceTexture(theFoundEntity, tr.endpos, theEndPos);
				if(theTextureHitCStr && (LowercaseString(theTextureHitCStr) == LowercaseString(kNoBuildTexture)))
				{
					if(outReturnCode)
					{
						*outReturnCode = WAYPOINT_NOBUILD;
					}
					theDone = true;
				}
				else if(theTextureHitCStr && (LowercaseString(theTextureHitCStr) == LowercaseString(kSeeThroughTexture)))
				{
					// Not valid, but allow it to pass through
					if(outReturnCode)
					{
						*outReturnCode = WAYPOINT_SEETHROUGH;
					}
				}
				else
				{
					// Trace texture sometimes seems to miss entities that the start point lies within.  Don't count
					// the trace texture if it found the texture of the next entity below it
						// else if surface is more flat than vertical
					if(tr.plane.normal[2] >= 0.7f)
					{
						// and if surface isn't under water, lava, in the sky, etc.
						int thePointContents = gEngfuncs.PM_PointContents(tr.endpos, NULL);
						if(thePointContents == CONTENTS_EMPTY)
						{
							// and if there's enough room to build

							// we can build here, and we're done
							theLegalToBuild = true;
							theDone = true;

							if(outReturnCode)
							{
								*outReturnCode = WAYPOINT_SUCCESS;
							}
						}
						else
						{
							if(outReturnCode)
							{
								*outReturnCode = WAYPOINT_CONTENTSFULL;
								theDone = true;
							}
						}
					}
					else
					{
						if(outReturnCode)
						{
							if(theTextureHitCStr)
							{
								*outReturnCode = WAYPOINT_TOOSTEEP;
							}
							else
							{
								*outReturnCode = WAYPOINT_ENTITYHIT;
							}
							theDone = true;
						}
					}
				}
			}
		}

		if(theFoundEntity != 0)
		{
			theEntityToIgnore = theFoundEntity;
		}

		if(((tr.fraction >= (1.0f - kFloatTolerance)) || (tr.fraction == 0.0f)) /*&& !tr.startsolid && !tr.allsolid*/)
		{
			theDone = true;
		}
		else
		{
			vec3_t theDiff = theNormal*kHitOffsetAmount;
			float theLength = sqrt(theDiff.x*theDiff.x + theDiff.y*theDiff.y + theDiff.z*theDiff.z);
			if(theLength > kFloatTolerance)
			{
				VectorAdd(tr.endpos, theDiff, theStartPos);
			}
			else
			{
				theDone = true;
			}
			// Offset a bit so we don't hit again
			//VectorMA(tr.endpos, kHitOffsetAmount, theNormal, theStartPos);
		}
	} while(!theDone);

	gEngfuncs.pEventAPI->EV_PopPMStates();

	// Always set end location to show where invalid position is
	*outLocation = tr.endpos;

	return theLegalToBuild;
}
#endif

#ifdef AVH_CLIENT
void AvHSHUClientGetFirstNonSolidPoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t& outNonSolidPoint)
{
	vec3_t theStartPos;
	vec3_t theEndPos;
	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inStartPos, outNonSolidPoint);
	VectorCopy(inEndPos, theEndPos);

	pmtrace_t tr;
	gEngfuncs.pEventAPI->EV_PlayerTrace( theStartPos, theEndPos, PM_NORMAL, -1, &tr );

	// Put this workaround in because a bug in EV_PlayerTrace means that when it starts solid, tr.fraction isn't returned (but it is in UTIL_TraceLine)
	if((tr.startsolid) && (tr.fraction == 0.0f))
	{
		int			theFoundEntity = -1;
		bool		theDone = false;

		vec3_t theStartToEnd;
		VectorSubtract(inEndPos, inStartPos, theStartToEnd);

		gEngfuncs.pEventAPI->EV_SetTraceHull(2);

		float theIncrement = 10.0f/theStartToEnd.Length();
		float theT = 0.0f;
		int theNumIterations = 0;

		do
		{
			theStartPos = inStartPos + theT*theStartToEnd;
			gEngfuncs.pEventAPI->EV_PlayerTrace( theStartPos, theEndPos, PM_WORLD_ONLY, -1, &tr );

			theNumIterations++;

			// If start point is solid, bisect area and move start point 1/2 between current start and current end
			if(tr.startsolid)
			{
				theT += theIncrement;
			}
			// else if start point isn't solid, bisect area and move start point back towards original start point
			else
			{
				theDone = true;
			}
		} while(!theDone && (theNumIterations < 200));

		// Always set end location to show where invalid position is
		if(!theDone)
		{
			outNonSolidPoint = inStartPos;
		}
		else
		{
			outNonSolidPoint = theStartPos;
		}
	}
}
#endif

#ifdef AVH_SERVER
void AvHSHUServerGetFirstNonSolidPoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t& outNonSolidPoint)
{
	vec3_t theStartPos;
	vec3_t theEndPos;
	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inStartPos, outNonSolidPoint);
	VectorCopy(inEndPos, theEndPos);

	TraceResult tr;
	UTIL_TraceLine(theStartPos, theEndPos, ignore_monsters, NULL, &tr);

	bool theDone = false;

	// Put this workaround in because a bug in EV_PlayerTrace means that when it starts solid, tr.fraction isn't returned (but it is in UTIL_TraceLine)
	if((tr.fStartSolid) || (tr.flFraction == 0.0f))
	{
		int			theFoundEntity = -1;

		vec3_t theStartToEnd;
		VectorSubtract(inEndPos, inStartPos, theStartToEnd);

		float theIncrement = 10.0f/theStartToEnd.Length();
		float theT = 0.0f;
		int theNumIterations = 0;
		int thePointContents = 0;

		do
		{
			theStartPos = inStartPos + theT*theStartToEnd;
			thePointContents = UTIL_PointContents(theStartPos);

			theNumIterations++;

			// If start point is solid, bisect area and move start point 1/2 between current start and current end
			if((thePointContents == CONTENTS_SOLID) || (thePointContents == CONTENTS_SKY))
			{
				theT += theIncrement;
			}
			// else if start point isn't solid, bisect area and move start point back towards original start point
			else
			{
				theDone = true;
			}
		} while(!theDone && (theNumIterations < 200));

		// Always set end location to show where invalid position is
		outNonSolidPoint = theStartPos;
	}
	else
	{
		VectorCopy(tr.vecEndPos, outNonSolidPoint);
		theDone = true;
	}

	if(!theDone)
	{
		// When we don't hit anything, return start point
		VectorCopy(inStartPos, outNonSolidPoint);
	}

	VectorCopy(outNonSolidPoint, gPMDebugPoint);
}
#endif

void AvHSHUGetFirstNonSolidPoint(float* inStartPos, float* inEndPos, float* outNonSolidPoint)
{
	vec3_t theStartPos;
	vec3_t theEndPos;

	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inEndPos, theEndPos);

	vec3_t theNonSolidPoint;
	AvHSHUGetFirstNonSolidPoint(theStartPos, theEndPos, theNonSolidPoint);

	VectorCopy(theNonSolidPoint, outNonSolidPoint);
}

void AvHSHUGetFirstNonSolidPoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t& outNonSolidPoint)
{
	#ifdef AVH_CLIENT
	AvHSHUClientGetFirstNonSolidPoint(inStartPos, inEndPos, outNonSolidPoint);

	//extern DebugPointListType gSquareDebugLocations;
	//DebugPoint theDebugPoint(outNonSolidPoint[0], outNonSolidPoint[1], outNonSolidPoint[2]);
	//gSquareDebugLocations.push_back(theDebugPoint);
	#endif

	#ifdef AVH_SERVER
	AvHSHUServerGetFirstNonSolidPoint(inStartPos, inEndPos, outNonSolidPoint);
	#endif
}

#ifdef AVH_SERVER
bool AvHSHUServerTraceWaypoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t* outLocation, WaypointReturnCode* outReturnCode)
{
	bool theLegalToBuild = false;
	bool theDone = false;
	edict_t* theEdictToIgnore = NULL;
	TraceResult tr;
	vec3_t theStartPos;
	vec3_t theEndPos;

	VectorCopy(inStartPos, theStartPos);
	VectorCopy(inEndPos, theEndPos);

	vec3_t theNormal;
	VectorSubtract(inEndPos, inStartPos, theNormal);
	VectorNormalize(theNormal);

	vec3_t theNewStartPos;
	AvHSHUGetFirstNonSolidPoint(theStartPos, theEndPos, theNewStartPos);
	VectorCopy(theNewStartPos, theStartPos);

	do
	{
		UTIL_TraceLine(theStartPos, theEndPos, dont_ignore_monsters, theEdictToIgnore, &tr);

		// Trace until we hit a worldbrush or non-seethrough
		CBaseEntity* theEntityHit = CBaseEntity::Instance(tr.pHit);
		CWorld* theWorld = dynamic_cast<CWorld*>(theEntityHit);
		bool theHitSeethrough = (theEntityHit->pev->iuser3 == AVH_USER3_ALPHA);
		if(!theHitSeethrough)
		{
			// If entity is a "no waypoint" entity we can't build here and we're done
			if(theEntityHit && (theEntityHit->pev->iuser3 == AVH_USER3_NOBUILD))
			{
				if(outReturnCode)
				{
					*outReturnCode = WAYPOINT_NOBUILD;
				}
				theDone = true;
			}
			else
			{
				edict_t* theEdict = ENT(theEntityHit->pev);
				const char* theTextureHitCStr = TRACE_TEXTURE(theEdict, tr.vecEndPos, theEndPos);
				if(theTextureHitCStr && (LowercaseString(theTextureHitCStr) == LowercaseString(kNoBuildTexture)))
				{
					if(outReturnCode)
					{
						*outReturnCode = WAYPOINT_NOBUILD;
					}
					theDone = true;
				}
				else if(theTextureHitCStr && (LowercaseString(theTextureHitCStr) == LowercaseString(kSeeThroughTexture)))
				{
					// Do nothing, but allow it to pass through
					if(outReturnCode)
					{
						*outReturnCode = WAYPOINT_SEETHROUGH;
					}
				}
				else
				{
					// else if surface is more flat than vertical
					if(tr.vecPlaneNormal.z >= .7f)
					{
						// and if surface isn't under water, lava, sky
						int thePointContents = UTIL_PointContents(tr.vecEndPos);
						if(thePointContents == CONTENTS_EMPTY)
						{
							// and if there's enough room to build
							if(outReturnCode)
							{
								*outReturnCode = WAYPOINT_SUCCESS;
							}

							// we can build here, and we're done
							theLegalToBuild = true;
							theDone = true;
						}
						else
						{
							if(outReturnCode)
							{
								*outReturnCode = WAYPOINT_CONTENTSFULL;
							}
						}
					}
					else
					{
						if(theTextureHitCStr)
						{
							*outReturnCode = WAYPOINT_TOOSTEEP;
						}
						else
						{
							*outReturnCode = WAYPOINT_ENTITYHIT;
						}
						theDone = true;
					}
				}
			}

			if(!theWorld)
			{
				theEdictToIgnore = theEntityHit->edict();
			}

			if(((tr.flFraction >= (1.0f - kFloatTolerance)) || (tr.flFraction < kFloatTolerance)) /*&& !tr.fStartSolid && !tr.fAllSolid*/)
			{
				theDone = true;
			}
			else
			{
				vec3_t theDiff = theNormal*kHitOffsetAmount;
				float theLength = sqrt(theDiff.x*theDiff.x + theDiff.y*theDiff.y + theDiff.z*theDiff.z);
				if(theLength > kFloatTolerance)
				{
					//VectorAdd(tr.vecEndPos, theDiff, theStartPos);
					VectorAdd(theStartPos, theDiff, theStartPos);
				}
				else
				{
					theDone = true;
				}
				// Offset a bit so we don't hit again
				//VectorMA(tr.vecEndPos, kHitOffsetAmount, theNormal, theStartPos);
			}
		}
		else
		{
			theEdictToIgnore = theEntityHit->edict();
			if(tr.vecEndPos == theStartPos)
			{
				vec3_t theDiff = theNormal*kHitOffsetAmount;
				float theLength = sqrt(theDiff.x*theDiff.x + theDiff.y*theDiff.y + theDiff.z*theDiff.z);
				if(theLength > kFloatTolerance)
				{
					//VectorAdd(tr.vecEndPos, theDiff, theStartPos);
					VectorAdd(theStartPos, theDiff, theStartPos);
				}
			}
			else
			{
				VectorCopy(tr.vecEndPos, theStartPos);
			}
//			vec3_t theEntityPosition = AvHSHUGetRealLocation(theEntityHit->pev->origin, theEntityHit->pev->mins, theEntityHit->pev->maxs);
//			//VectorCopy(theEntityPosition, theStartPos);
//			theStartPos[2] = theEntityPosition[2];
		}
	} while(!theDone);

	// Always set end location to show where invalid position is
	*outLocation = tr.vecEndPos;

	return theLegalToBuild;
}
#endif

bool AvHSHUTraceTangible(const vec3_t& inStartPos, const vec3_t& inEndPos, int& outIndex, vec3_t& outLocation, AvHTeamNumber& outTeamNumber, bool& outPlayerWasHit, int& outUserThree, int& outUserFour)
{
	bool theSuccess = false;

	outPlayerWasHit = false;
	outUserThree = 0;
	outUserFour = 0;

	// On client, set up players for prediction and do a PM_TraceLine
	#ifdef AVH_CLIENT
	theSuccess = AvHSUClientTraceTangible(inStartPos, inEndPos, outIndex, outLocation, outTeamNumber, outPlayerWasHit, outUserThree, outUserFour);
	#endif

	// On the server, do a UTIL_TraceLine
	#ifdef AVH_SERVER
	theSuccess = AvHSUServerTraceTangible(inStartPos, inEndPos, outIndex, outLocation, outTeamNumber, outPlayerWasHit, outUserThree, outUserFour);
	#endif

	return theSuccess;
}

bool AvHSHUTraceTangible(const Vector& inPointOfView, const Vector& inNormRay, int* outUserThree, vec3_t* outLocation, AvHTeamNumber* outTeamNumber, bool* outPlayerWasHit)
{
	bool theSuccess = false;
	vec3_t theTraceStart;
	vec3_t theTraceEnd;
	vec3_t theFoundLocation;
	int theFoundIndex = -1;
	int theUserThree = 0;
	int theUserFour = 0;
	AvHTeamNumber theTeamOfThingHit;
	bool thePlayerHit = false;

	// Offset a little so we don't hit the commander
	VectorMA(inPointOfView, kSelectionStartRange, inNormRay, theTraceStart);
	VectorMA(inPointOfView, kSelectionEndRange, inNormRay, theTraceEnd);

	if(!outUserThree)
		outUserThree = &theUserThree;

	if(!outLocation)
		outLocation = &theFoundLocation;

	if(!outTeamNumber)
		outTeamNumber = &theTeamOfThingHit;

	if(!outPlayerWasHit)
		outPlayerWasHit = &thePlayerHit;

	theSuccess = AvHSHUTraceTangible(theTraceStart, theTraceEnd, theFoundIndex, *outLocation, *outTeamNumber, *outPlayerWasHit, *outUserThree, theUserFour);

	return theSuccess;
}

bool AvHSHUTraceVerticalTangible(float inX, float inY, float inZ, int& outUserThree, float& outHeight)
{
	bool theSuccess = false;

	// Create a start pos above
	vec3_t theStartPos(inX, inY, inZ - 100.0f);
	//vec3_t theEndPos(inX, inY, inZ - 800.0f/*-4096.0f*/);
	vec3_t theEndPos(inX, inY, (float)kWorldMinimumZ);

//	#ifdef AVH_CLIENT
//	extern DebugPointListType				gTriDebugLocations;
//	bool theDrawDebug = true;//(rand() % 100 == 0);
//
//	if(theDrawDebug)
//	{
//		DebugPoint thePoint;
//		thePoint.x = theStartPos.x;
//		thePoint.y = theStartPos.y;
//		thePoint.z = theStartPos.z;
//		gTriDebugLocations.push_back(thePoint);
//
//		thePoint.z = theEndPos.z;
//		gTriDebugLocations.push_back(thePoint);
//	}
//
//	#endif

	int theIndex;
	vec3_t theEndLocation;
	AvHTeamNumber theTeam;
	bool thePlayerHit;
	int theUserFour;

	theSuccess = AvHSHUTraceTangible(theStartPos, theEndPos, theIndex, theEndLocation, theTeam, thePlayerHit, outUserThree, theUserFour);
	if(theSuccess)
	{
		outHeight = theEndLocation.z;
		ASSERT(outHeight <= inZ);
	}

	return theSuccess;
}

const char* AvHSHUGetCommonSoundName(bool inIsAlien, WeaponHUDSound inHUDSound)
{
	const char* theSoundName = NULL;

	switch(inHUDSound)
	{
	case WEAPON_SOUND_HUD_ON:
		theSoundName = inIsAlien ? "common/wpn_hudon-a.wav" : "common/wpn_hudon.wav";
		break;
	case WEAPON_SOUND_HUD_OFF:
		theSoundName = inIsAlien ? "common/wpn_hudoff-a.wav" : "common/wpn_hudoff.wav";
		break;
	case WEAPON_SOUND_MOVE_SELECT:
		theSoundName = inIsAlien ? "common/wpn_moveselect-a.wav" : "common/wpn_moveselect.wav";
		break;
	case WEAPON_SOUND_SELECT:
		theSoundName = inIsAlien ? "common/wpn_select-a.wav" : "common/wpn_select.wav";
		break;
	case WEAPON_SOUND_DENYSELECT:
		theSoundName = inIsAlien ? "common/wpn_denyselect-a.wav" : "common/wpn_denyselect.wav";
		break;
	}

	return theSoundName;
}


// Values for fuser1: 0-1 means it's building.  2-3 means it's researching (see kResearchFuser1Base), 3-4 is "energy" (returns both building and researching)
const float	kResearchFuser1Base = 2.0f;
const float	kEnergyFuser1Base = 3.0f;

void AvHSHUGetBuildResearchState(int inUser3, int inUser4, float inFuser1, bool& outIsBuilding, bool& outIsResearching, float& outNormalizedPercentage)
{
	outIsBuilding = outIsResearching = false;

	float theStatus = (inFuser1/kNormalizationNetworkFactor);

	if((GetHasUpgrade(inUser4, MASK_BUILDABLE) || (inUser3 == AVH_USER3_WELD)) && !GetHasUpgrade(inUser4, MASK_RECYCLING))
	{
		if((theStatus >= 0.0f) && (theStatus <= 1.0f))
		{
			outNormalizedPercentage = theStatus;
			outIsBuilding = true;
		}
		if(inUser3 == AVH_USER3_WELD)
		{
			if(outNormalizedPercentage == -1)
			{
				outIsBuilding = outIsResearching = false;
			}
		}
	}
	else
	{
		if((theStatus > kResearchFuser1Base) && (theStatus <= (1.0f + kResearchFuser1Base)))
		{
			outNormalizedPercentage = theStatus - kResearchFuser1Base;
			outIsResearching = true;
		}
		else if((theStatus > kEnergyFuser1Base) && (theStatus <= (1.0f + kEnergyFuser1Base)))
		{
			// Set "energy"
			outNormalizedPercentage = theStatus - kEnergyFuser1Base;
			outIsBuilding = outIsResearching = true;
		}
		else
		{
			outNormalizedPercentage = 1.0f;
		}
	}
}

void AvHSHUSetEnergyState(int inUser3, float &outFuser1, float inNormalizedPercentage)
{
	outFuser1 = (kEnergyFuser1Base + inNormalizedPercentage)*kNormalizationNetworkFactor;
}

void AvHSHUSetBuildResearchState(int inUser3, int inUser4, float &outFuser1, bool inTrueBuildOrFalseResearch, float inNormalizedPercentage)
{
	//ASSERT(GetHasUpgrade(inUser4, MASK_BUILDABLE) /*|| GetHasUpgrade(inUser4, MASK_SELECTABLE)*/ || (inUser3 == AVH_USER3_WELD));

	if(inNormalizedPercentage != -1)
	{
		inNormalizedPercentage = min(inNormalizedPercentage, 1.0f);
		inNormalizedPercentage = max(inNormalizedPercentage, 0.0f);
	}

	// Build
	if(inTrueBuildOrFalseResearch)
	{
		//ASSERT(GetHasUpgrade(inUser4, MASK_BUILDABLE) || (inUser3 == AVH_USER3_WELD));

		outFuser1 = inNormalizedPercentage*kNormalizationNetworkFactor;
	}
	// Research
	else
	{
		outFuser1 = (kResearchFuser1Base + inNormalizedPercentage)*kNormalizationNetworkFactor;
	}
}


string AvHSHUBuildExecutableScriptName(const string& inScriptName, const string& inCurrentMapName)
{
	string theExecutableScriptName = string(getModDirectory()) + string("/") + kScriptsDirectory + string("/") + inCurrentMapName + string("/") + inScriptName;

	return theExecutableScriptName;
}

string AvHSHUGetTimeDateString()
{
	string theTimeDateString("<date unknown>");

	time_t theTimeThingie = time ((time_t *) NULL);
	struct tm* theTime = localtime(&theTimeThingie);

	if(theTime)
	{
		// 04/22/03, 16:59:12
		char theBuffer[512];
		sprintf(theBuffer, "%.2d/%.2d/%.2d, %.2d:%.2d:%.2d", (theTime->tm_mon + 1), theTime->tm_mday, (theTime->tm_year - 100), theTime->tm_hour, theTime->tm_min, theTime->tm_sec);
		theTimeDateString = theBuffer;
	}

	return theTimeDateString;
}

bool AvHSHUGetNameOfLocation(const AvHBaseInfoLocationListType& inLocations, vec3_t inLocation, string& outLocation)
{
	bool theSuccess = false;

	// Look at our current position, and see if we lie within of the map locations
	for(AvHBaseInfoLocationListType::const_iterator theIter = inLocations.begin(); theIter != inLocations.end(); theIter++)
	{
		if(theIter->GetIsPointInRegion(inLocation))
		{
			outLocation = theIter->GetLocationName();
			theSuccess = true;
			break;
		}
	}

	return theSuccess;
}

// Also check AvHSUGetAlwaysPlayAlert to make sure some alerts are always triggered
bool AvHSHUGetForceHUDSound(AvHHUDSound inHUDSound)
{
    bool theForceSound = false;

    switch(inHUDSound)
    {
    case HUD_SOUND_ALIEN_HIVE_COMPLETE:
    case HUD_SOUND_ALIEN_HIVE_DYING:
    case HUD_SOUND_ALIEN_HIVE_ATTACK:
    case HUD_SOUND_MARINE_CCUNDERATTACK:
    case HUD_SOUND_SQUAD1:
    case HUD_SOUND_SQUAD2:
    case HUD_SOUND_SQUAD3:
    case HUD_SOUND_SQUAD4:
    case HUD_SOUND_SQUAD5:
        theForceSound = true;
        break;
    }

    return theForceSound;
}
