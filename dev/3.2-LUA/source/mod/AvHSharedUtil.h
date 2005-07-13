//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSharedUtil.h $
// $Date: 2002/11/12 22:39:26 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSharedUtil.h,v $
// Revision 1.24  2002/11/12 22:39:26  Flayra
// - Logging changes for Psychostats compatibility
//
// Revision 1.23  2002/10/16 01:07:26  Flayra
// - Added official sizes that HL supports (ugh)
// - Added utility function for drawing range of ghost building, but it's unused
//
// Revision 1.22  2002/09/25 20:51:05  Flayra
// - Allow small items to be built on entities (health can be dropped right on players)
//
// Revision 1.21  2002/08/16 02:47:06  Flayra
// - Fixed bug where not all the entities were iterated through
// - Support for ring-drawing (but not for all entities)
//
// Revision 1.20  2002/08/09 00:51:11  Flayra
// - Cleaned up useless special casing of tracetangible, fixed some problems where buildings could be built on players
//
// Revision 1.19  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.18  2002/07/23 17:26:49  Flayra
// - Siege don't require a nearby turret factory, only add built buildings for range detection, build problems on client (point contents failing on rough surfaces)
//
// Revision 1.17  2002/07/08 17:17:44  Flayra
// - Reworked colors, moved functions into server util
//
// Revision 1.16  2002/07/01 21:46:39  Flayra
// - Added support for generic building ranges, fixed morphing problems
//
// Revision 1.15  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SHAREDUTIL_H
#define AVH_SHAREDUTIL_H

#ifdef AVH_CLIENT
#include "cl_dll/cl_util.h"
#include "cl_dll/util_vector.h"
#include "common/const.h"
#include "engine/progdefs.h"
#include "cl_dll/ev_hldm.h"
#include "common/vector_util.h"
#endif

#ifdef AVH_SERVER
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "common/vector_util.h"
#endif

#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"
#include "pm_shared/pm_movevars.h"
#include "pm_shared/pm_debug.h"
#include "mod/AvHMessage.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMapExtents.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHBaseInfoLocation.h"
#include "mod/AvHBasePlayerWeaponConstants.h"

const int kHitOffsetAmount = 10;
#define kNoBuildTexture		"nobuild"
#define kSeeThroughTexture	"seethrough"

//physent_t* AvHSUGetEntity(int inPhysIndex);
const char* AvHSHUGetClassNameFromUser3(AvHUser3 inUser3);
int AvHSHUGetDrawRangeForMessageID(AvHMessageID inMessageID);
int AvHSHUGetDrawRangeForUser3(AvHUser3 inUser3);
bool AvHSHUGetDrawRingsForUser3(AvHUser3 inUser3, float& outScalar);
bool AvHSHUGetBuildRegions(AvHMessageID inMessageID, EntityListType& outEntities, IntList& outRanges, float& outZAdjustment, bool& outSnapToLocation);
bool AvHSHUGetAreSpecialBuildingRequirementsMet(AvHMessageID inMessageID, vec3_t& inLocation);
bool AvHSHUGetBuildTechClassName(AvHMessageID inMessageID, char*& outClassName);
bool AvHSHUGetResearchTechName(AvHMessageID inMessageID, char*& outResearchTechName);
bool AvHSHUGetCenterPositionForGroup(int inGroupNumber, float* inPlayerOrigin, float* outCenterPosition);
void AvHSHUGetEntities(int inUser3, EntityListType& outEntities);
bool AvHSHUGetEntityLocation(int inEntity, vec3_t& outLocation);
bool AvHSHUGetEntityIUser4(int inEntity, int& outIUser4);
vec3_t AvHSHUGetRealLocation(const vec3_t& inLocation, const vec3_t& inMinBox, const vec3_t& inMaxBox);
void AvHSHUGetFirstNonSolidPoint(float* inStartPos, float* inEndPos, float* outNonSolidPoint);
void AvHSHUGetFirstNonSolidPoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t& outNonSolidPoint);
bool AvHSHUGetIsBuilding(AvHMessageID inMessageID);
bool AvHSHUGetIsBuildTech(AvHMessageID inMessageID);
bool AvHSHUGetIsWeaponFocusable(AvHWeaponID inWeaponID);
bool AvHSHUGetDoesTechCostEnergy(AvHMessageID inMessageID);
bool AvHSHUGetIsCombatModeTech(AvHMessageID inMessageID);
bool AvHSHUUser3ToMessageID(AvHUser3 inUser3, AvHMessageID& outMessageID);
bool AvHSHUMessageIDToUser3(AvHMessageID inMessageID, AvHUser3& outUser3);
bool AvHSHUGetIsResearchTech(AvHMessageID inMessageID);
bool AvHSHUGetIsGroupMessage(AvHMessageID inMessageID);
bool AvHSHUGetBuildTechRange(AvHMessageID inMessageID, float& outRange);
char* AvHSHUGetBuildTechModelName(AvHMessageID inMessageID);
bool AvHSHUGetSizeForPlayerUser3(AvHUser3 inUser3, Vector& outMinSize, Vector& outMaxSize, bool inDucking);
bool AvHSHUGetSizeForTech(AvHMessageID inMessageID, Vector& outMinSize, Vector& outMaxSize, bool inGetSizeToPlace = false);
bool AvHSHUGetSizeForUser3(AvHUser3 inUser3, Vector& outMinSize, Vector& outMaxSize);
float AvHSHUGetTime();
bool AvHSHUGetCanDropItem(vec3_t& inCenter, Vector& inMinSize, Vector& inMaxSize, float inMaxSlopeTangent, int inIgnoredEntityIndex, bool inIgnorePlayers);
bool AvHSHUGetIsVolumeContentNonsolid(vec3_t inCenter, Vector& inMinSize, Vector& inMaxSize, edict_t* inIgnoreEntity, bool inIgnorePlayers);
bool AvHSHUGetIsAreaFree(vec3_t inCenter, Vector& inMinSize, Vector& inMaxSize, edict_t* inIgnoreEntity = NULL, bool inIgnorePlayers = false);
bool AvHSHUGetEntitiesBlocking(Vector& inOrigin, Vector& inMinSize, Vector& inMaxSize, edict_t* inIgnoreEntity = NULL, bool inIgnorePlayers = false);
void AvHSHUGetMinBuildRadiusViolations(AvHMessageID inMessageID, vec3_t& inLocation, EntityListType& outViolations);
bool AvHSHUGetIsSiteValidForBuild(AvHMessageID inMessageID, Vector* inLocation, int inIgnoredEntityIndex = -1);
void AvHSHUMakeViewFriendlyKillerName(string& ioKillerName);
bool AvHSHUTraceAndGetIsSiteValidForBuild(AvHMessageID inMessageID, const Vector& inPointOfView, const Vector& inNormRay, Vector* outLocation = NULL);

bool AvHSHUGetEntityAtRay(const Vector& inPointOfView, const Vector& inNormRay, int& outEntIndex);

const AvHMapExtents& AvHSHUGetMapExtents();

bool AvHSHUTraceTangible(const vec3_t& inStartPos, const vec3_t& inEndPos, int& outIndex, vec3_t& outLocation, AvHTeamNumber& outTeamNumber, bool& outPlayerWasHit, int& outUserThree, int& outUserFour);
bool AvHSHUTraceTangible(const Vector& inPointOfView, const Vector& inNormRay, int* outUserThree = NULL, vec3_t* outLocation = NULL, AvHTeamNumber* outTeamNumber = NULL, bool* outPlayerWasHit = NULL);
bool AvHSHUTraceVerticalTangible(float inX, float inY, float inZ, int& outUserThree, float& outHeight);
bool AvHSHUTraceLineIsAreaFree(Vector& inStart, Vector& inEnd, edict_t* inIgnoreEntity = NULL, bool inIgnorePlayers = false);

float AvHTraceLineAgainstWorld(Vector& vecStart, Vector& vecEnd);

typedef enum
{
	WAYPOINT_SUCCESS,
	WAYPOINT_NOBUILD,
	WAYPOINT_TOOSTEEP,
	WAYPOINT_SEETHROUGH,
	WAYPOINT_CONTENTSFULL,
	WAYPOINT_ENTITYHIT,
} WaypointReturnCode;

#ifdef AVH_CLIENT
void AvHSHUClientGetFirstNonSolidPoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t& outNonSolidPoint);
bool AvHSHUClientTraceWaypoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t* outLocation, WaypointReturnCode* outReturnCode = NULL);
#endif

#ifdef AVH_SERVER
void AvHSHUServerGetFirstNonSolidPoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t& outNonSolidPoint);
bool AvHSHUServerTraceWaypoint(const vec3_t& inStartPos, const vec3_t& inEndPos, vec3_t* outLocation, WaypointReturnCode* outReturnCode);
#endif

typedef enum
{
	WEAPON_SOUND_INVALID,
	WEAPON_SOUND_HUD_ON,
	WEAPON_SOUND_HUD_OFF,
	WEAPON_SOUND_MOVE_SELECT,
	WEAPON_SOUND_SELECT,
	WEAPON_SOUND_DENYSELECT
} WeaponHUDSound;

const char* AvHSHUGetCommonSoundName(bool inIsAlien, WeaponHUDSound inHUDSound);

void AvHSHUGetBuildResearchState(int inUser3, int inUser4, float inFuser1, bool& outIsBuilding, bool& outIsResearching, float& outNormalizedPercentage);
void AvHSHUSetBuildResearchState(int inUser3, int inUser4, float &outFuser1, bool inTrueBuildOrFalseResearch, float inNormalizedPercentage);
void AvHSHUSetEnergyState(int inUser3, float &outFuser1, float inNormalizedPercentage);
string AvHSHUBuildExecutableScriptName(const string& inScriptName, const string& inCurrentMapName);
string AvHSHUGetTimeDateString();

bool AvHSHUGetNameOfLocation(const AvHBaseInfoLocationListType& inLocations, vec3_t inLocation, string& outLocation);
bool AvHSHUGetForceHUDSound(AvHHUDSound inHUDSound);

#endif
