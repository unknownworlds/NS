//  Copyright (c) 1999, Valve LLC. All rights reserved.
//  
//  This product contains software technology licensed from Id 
//  Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
//  All Rights Reserved.
//
//  Use, distribution, and modification of this source code and/or resulting
//  object code is restricted to non-commercial enhancements to products from
//  Valve LLC.  All other use, distribution, or modification is prohibited
//  without written permission from Valve LLC.
//
//
// $Workfile: pm_shared.cpp $
// $Date: 2002/10/24 21:20:28 $
//
//-------------------------------------------------------------------------------
// $Log: pm_shared.cpp,v $
// Revision 1.39  2002/10/24 21:20:28  Flayra
// - Reworked jetpack effect
// - Extended spectating distance
// - Only play blink effect once
// - Update alien energy in shared code
//
// Revision 1.38  2002/10/18 22:16:51  Flayra
// - Level5 crouching fix
// - Skulks can't use ladders
//
// Revision 1.37  2002/10/16 20:50:28  Flayra
// - Updated onos footsteps with new sound
//
// Revision 1.36  2002/10/16 00:42:55  Flayra
// - Removed blink fail event
//
// Revision 1.35  2002/10/03 18:30:31  Flayra
// - Moved alien energy to fuser3
// - Flying/ladder fix (lerks can't use them)
// - When players are frozen, don't allow them to press buttons either
//
// Revision 1.34  2002/09/25 20:41:45  Flayra
// - Removed marine strafing slowdown
//
// Revision 1.33  2002/09/23 22:05:58  Flayra
// - Added code for rotation skulk model (removed it though)
// - Removed inverse lighting for skulks
// - Started to add speed abuse code, but realized the physics are inprecise and going over maxspeed is normal
//
// Revision 1.32  2002/09/09 19:45:30  Flayra
// - Blink fixes, removed level 5 custom step size
//
// Revision 1.31  2002/08/16 02:28:03  Flayra
// - Removed overwatch code
// - Webs now prevent jumping and jetpacking
//
// Revision 1.30  2002/08/09 01:12:16  Flayra
// - Turned on wall-climbing by default
// - Tweaked jetpack physics
// - Only update predicted player origin when runfuncs is true (fixes bug with flickering ghost unit in CM)
//
// Revision 1.29  2002/08/02 21:45:46  Flayra
// - Fixed jetpack energy problem, moved wall-running back to duck
//
// Revision 1.28  2002/07/25 17:28:37  Flayra
// - More linux changes
//
// Revision 1.27  2002/07/23 16:51:49  Flayra
// - MrBlonde's fix for player shaking (not sure how well it works)
//
// Revision 1.26  2002/07/10 14:37:18  Flayra
// - Removed large differing step-sizes (bug #255)
//
// Revision 1.25  2002/07/08 16:23:28  Flayra
// - Added debug code to fix solidity problems, fixed commander/hera-landing-pad bug, added document header
//
//===============================================================================
#include <assert.h>
#include "common/mathlib.h"
#include "common/const.h"
#include "common/usercmd.h"
#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_movevars.h"
#include "pm_debug.h"
#include <stdio.h>  // NULL
#include <math.h>   // sqrt
#include <string.h> // strcpy
#include <stdlib.h> // atoi
#include <ctype.h>  // isspace
#include "mod/AvHSpecials.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHMessage.h"
#include "util/MathUtil.h"
#include "util/Quat.h"
#include "util/Mat3.h"
#include "common/event_flags.h"
#include "mod/AvHSoundConstants.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHHulls.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHMapExtents.h"
#include "mod/AvHSharedMovementInfo.h"
#include "util/Balance.h"



#include "common/const.h"
#include "common/com_model.h"
#include "common/mathlib.h"

#include "mod/CollisionUtil.h"
#include "engine/studio.h"


//#ifdef AVH_SERVER
#include "engine/edict.h"
#include "engine/eiface.h"
#include "dlls/enginecallback.h"
//#endif

#ifdef AVH_SERVER
void AvHSUAddDebugPoint(float inX, float inY, float inZ);
#endif

void AvHSHUGetFirstNonSolidPoint(float* inStartPos, float* inEndPos, float* outNonSolidPoint);
bool AvHSHUGetCenterPositionForGroup(int inGroupNumber, float* inPlayerOrigin, float* outCenterPosition);

#ifdef AVH_CLIENT
////#include "cl_dll/cl_dll.h"
////#include "cl_dll/hud.h"
//#include "cl_dll/cl_util.h"
//#include "cl_dll/cl_dll.h"
#include "cl_dll/eventscripts.h"
#endif

float PM_GetDesiredTopDownCameraHeight(qboolean& outFoundEntity);
qboolean PM_CanWalljump();
qboolean PM_CanFlap();
void PM_Overwatch();
bool PM_TopDown();
void PM_Jump(void);
void PM_PreventMegaBunnyJumping(bool inIgnoreZ);
bool GetIsEntityAPlayer(int inPhysIndex);

#ifdef AVH_CLIENT
#include "pm_shared/pm_debug.h"
#include "..\common\hltv.h"
void PM_DebugLocations();
typedef vector< DebugPoint >        PositionListType;
DebugPointListType                  gTriDebugLocations;
DebugPointListType                  gSquareDebugLocations;
DebugEntityListType                 gCubeDebugEntities;
#endif

//extern int gWallJumpEventID;
//extern int gFlightEventID;

extern int gJetpackEventID;
extern int gBlinkEffectSuccessEventID;

#define Vector vec3_t
#include "mod/AvHSelectionHelper.h"

int gHeightLevel = 0;

const float kMarineBackpedalSpeedScalar = .4f;
const float kMarineSidestepSpeedScalar = 1.0f;

//physent_t* AvHSUGetEntity(int inPhysIndex);

const AvHMapExtents& GetMapExtents();

#ifdef AVH_CLIENT
// Spectator Mode
int     iJumpSpectator;
float   vJumpOrigin[3];
float   vJumpAngles[3];
//float gTopDownHeight = 0;

float   gTopDownViewOrigin[3];
float   gTopDownViewAngles[3];
//int       iHasNewViewOrigin = 0;
//int       iHasNewViewAngles = 0;
#endif

static int pm_shared_initialized = 0;

//void InterpolateAngles( float *start, float *end, float *output, float frac );
    
#pragma warning( disable : 4305 )


playermove_t *pmove = NULL;

extern "C"
{
    Vector  gPredictedPlayerOrigin;
    Vector  gPredictedPlayerVOfs;
}

#ifdef AVH_CLIENT
float gOverwatchTargetRange;
#endif


///////////////////////////////
// Begin Max's Code
///////////////////////////////     

const float kSkulkRotationRate      = (float)(2.5 * M_PI);  // Radians per second.
const float kSkulkRotationLookAhead = 75;

// Distance around the skulk to look each in direction for wallsticking.
const vec3_t kWallstickingDistanceCheck = { 5, 5, 5 };

#ifdef AVH_CLIENT
extern vec3_t gPlayerAngles;
extern vec3_t gTargetPlayerAngles;
extern vec3_t gWorldViewAngles;
#endif

///////////////////////////////
// End Max's Code
///////////////////////////////     

// Ducking time
#define TIME_TO_DUCK    (0.4 * 1000)
//#define VEC_DUCK_HULL_MIN -18
//#define VEC_DUCK_HULL_MAX 18
//#define VEC_DUCK_VIEW     12
#define PM_DEAD_VIEWHEIGHT  -8
#define MAX_CLIMB_SPEED 120
#define STUCK_MOVEUP 1
#define STUCK_MOVEDOWN -1
//#define VEC_HULL_MIN      -36
//#define VEC_HULL_MAX      36
//#define VEC_VIEW          28
#define STOP_EPSILON    0.1

#define CTEXTURESMAX        512         // max number of textures loaded
#define CBTEXTURENAMEMAX    13          // only load first n chars of name

#define CHAR_TEX_CONCRETE   'C'         // texture types
#define CHAR_TEX_METAL      'M'
#define CHAR_TEX_DIRT       'D'
#define CHAR_TEX_VENT       'V'
#define CHAR_TEX_GRATE      'G'
#define CHAR_TEX_TILE       'T'
#define CHAR_TEX_SLOSH      'S'
#define CHAR_TEX_WOOD       'W'
#define CHAR_TEX_COMPUTER   'P'
#define CHAR_TEX_GLASS      'Y'
#define CHAR_TEX_FLESH      'F'

#define STEP_CONCRETE   0       // default step sound
#define STEP_METAL      1       // metal floor
#define STEP_DIRT       2       // dirt, sand, rock
#define STEP_VENT       3       // ventillation duct
#define STEP_GRATE      4       // metal grating
#define STEP_TILE       5       // floor tiles
#define STEP_SLOSH      6       // shallow liquid puddle
#define STEP_WADE       7       // wading in liquid
#define STEP_LADDER     8       // climbing ladder

#define PLAYER_FATAL_FALL_SPEED     1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED  580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED       (float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED     200
#define PLAYER_FALL_PENALTY_THRESHHOLD (float)200 // If player lands with this much speed or more, slow them down as penalty for jumping
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

#define PLAYER_LONGJUMP_SPEED 350 // how fast we longjump

const float kAlienEnergyFlap = .025f;

// double to float warning
#pragma warning(disable : 4244)
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
// up / down
#define PITCH   0
// left / right
#define YAW     1
// fall over
#define ROLL    2 

#define MAX_CLIENTS 32

#define CONTENTS_CURRENT_0      -9
#define CONTENTS_CURRENT_90     -10
#define CONTENTS_CURRENT_180    -11
#define CONTENTS_CURRENT_270    -12
#define CONTENTS_CURRENT_UP     -13
#define CONTENTS_CURRENT_DOWN   -14

#define CONTENTS_TRANSLUCENT    -15

static vec3_t rgv3tStuckTable[54];
static int rgStuckLast[MAX_CLIENTS][2];

// Texture names
static int gcTextures = 0;
static char grgszTextureName[CTEXTURESMAX][CBTEXTURENAMEMAX];   
static char grgchTextureType[CTEXTURESMAX];

int     g_onladder[MAX_CLIENTS];
bool    gIsJetpacking[MAX_CLIENTS];


// Borrowed from Quake1.

static  hull_t      box_hull;
static  dclipnode_t box_clipnodes[6];
static  mplane_t    box_planes[6];


bool PM_GetIsBlinking()
{
    if (pmove->cmd.weaponselect == 255 || pmove->flags & FL_FAKECLIENT)
    {
        return (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER4) && (pmove->cmd.impulse == ALIEN_ABILITY_BLINK);
    }
    return false;
}

bool PM_GetIsLeaping()
{
    if (pmove->cmd.weaponselect == 255 || pmove->flags & FL_FAKECLIENT)
    {
        return (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER1) && (pmove->cmd.impulse == ALIEN_ABILITY_LEAP);
    }
    return false;
}

bool PM_GetIsCharging()
{
    if (pmove->cmd.weaponselect == 255 || pmove->flags & FL_FAKECLIENT)
    {
        return (pmove->cmd.impulse == ALIEN_ABILITY_CHARGE);
    }
    return false;
}


/*
===================
PM_InitBoxHull

Set up the planes and clipnodes so that the six floats of a bounding box
can just be stored out and get a proper hull_t structure.
===================
*/
void PM_InitBoxHull (void)
{
    int     i;
    int     side;

    box_hull.clipnodes = box_clipnodes;
    box_hull.planes = box_planes;
    box_hull.firstclipnode = 0;
    box_hull.lastclipnode = 5;

    for (i=0 ; i<6 ; i++)
    {
        box_clipnodes[i].planenum = i;
        
        side = i&1;
        
        box_clipnodes[i].children[side] = CONTENTS_EMPTY;
        if (i != 5)
            box_clipnodes[i].children[side^1] = i + 1;
        else
            box_clipnodes[i].children[side^1] = CONTENTS_SOLID;

    }
    
}


/*
===================
PM_HullForBox

To keep everything totally uniform, bounding boxes are turned into small
BSP trees instead of being compared directly.
===================
*/
hull_t* PM_HullForBox (vec3_t mins, vec3_t maxs)
{

    for (int i=0 ; i<6 ; i++)
    {
        
        box_planes[i].type = i>>1;
        
        box_planes[i].normal[0] = 0;
        box_planes[i].normal[1] = 0;
        box_planes[i].normal[2] = 0;
        box_planes[i].normal[i>>1] = 1;
    
    }

    box_planes[0].dist = maxs[0];
    box_planes[1].dist = mins[0];
    box_planes[2].dist = maxs[1];
    box_planes[3].dist = mins[1];
    box_planes[4].dist = maxs[2];
    box_planes[5].dist = mins[2];

    return &box_hull;
}


void NS_DrawBoundingBox(const vec3_t mins,
                        const vec3_t maxs,
                        const vec3_t inEntityOrigin,
                        const vec3_t inEntityAngles)
{

#ifdef AVH_SERVER

    void PM_DrawRectangle(vec3_t tl, vec3_t bl, vec3_t tr, vec3_t br, int pcolor, float life);
    extern int PM_boxpnt[6][4];

    int pcolor = 132;
    float plife = 0.1;

	vec3_t tmp;
	vec3_t		p[8];
	float gap = 0;
    int j;

	for (j = 0; j < 8; j++)
	{
		tmp[0] = (j & 1) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = (j & 2) ? mins[1] - gap : maxs[1] + gap;
		tmp[2] = (j & 4) ? mins[2] - gap : maxs[2] + gap;

		VectorCopy(tmp, p[j]);
	}

	// If the bbox should be rotated, do that
	vec3_t	forward, right, up;

	AngleVectorsTranspose(inEntityAngles, forward, right, up);
	for (j = 0; j < 8; j++)
	{
		VectorCopy(p[j], tmp);
		p[j][0] = DotProduct  ( tmp, forward );
		p[j][1] = DotProduct ( tmp, right );
		p[j][2] = DotProduct  ( tmp, up );
	}

	// Offset by entity origin, if any.
	for (j = 0; j < 8; j++)
		VectorAdd(p[j], inEntityOrigin, p[j]);

	for (j = 0; j < 6; j++)
	{
		PM_DrawRectangle(
			p[PM_boxpnt[j][1]],
			p[PM_boxpnt[j][0]],
			p[PM_boxpnt[j][2]],
			p[PM_boxpnt[j][3]],
			pcolor, plife);
	}

#endif

}


void NS_DrawBoundingBox(const OBBox& inBox)
{

    vec3_t theAngles;
    VectorsToAngles(inBox.mAxis[1], inBox.mAxis[0], inBox.mAxis[2], theAngles);

    vec3_t theMin;
    VectorScale(inBox.mExtents, -1, theMin);

    vec3_t theMax;
    VectorCopy(inBox.mExtents, theMax);

    NS_DrawBoundingBox(theMin, theMax, inBox.mOrigin, theAngles);

}



/**
 * Computes the hull for an oriented bounding box.
 */
hull_t* NS_HullForBox(vec3_t inEntityMins, vec3_t inEntityMaxs,
    vec3_t inEntityAngles, vec3_t inPlayerExtents)
{

    float thePlayerRadius = max(inPlayerExtents[0], inPlayerExtents[1]);

    // Players "feel" too large so artificially shrink them relative to the
    // structures.
    thePlayerRadius *= 0.8; 
    
    vec3_t axis[3];
    AngleVectors(inEntityAngles, axis[0], axis[1], axis[2]);

    // For some reason the y-axis is negated so flip it.
    
    axis[1][0] = -axis[1][0];
    axis[1][1] = -axis[1][1];
    axis[1][2] = -axis[1][2];

    for (int i = 0; i < 3; ++i)
    {
        
        int p1 = i * 2;
        int p2 = p1 + 1;
        
        vec3_t p;

        // Compute the offset the planes to account for the size of the player.
 
        /*
        float offset = fabs(inPlayerExtents[0] * axis[i][0]) +
                       fabs(inPlayerExtents[1] * axis[i][1]) +
                       fabs(inPlayerExtents[2] * axis[i][2]);
        */

        // Use a cylindrical player instead of a square one.
        float offset = thePlayerRadius;

        // Positive plane.

        VectorCopy(axis[i], box_planes[p1].normal);
        VectorScale(box_planes[p1].normal, inEntityMaxs[i], p);

        box_planes[p1].dist = DotProduct(p, box_planes[p1].normal) + offset;
        box_planes[p1].type = 3; // Not axis aligned.

        // Negative plane.

        VectorCopy(axis[i], box_planes[p2].normal);
        VectorScale(box_planes[p2].normal, inEntityMins[i], p);

        box_planes[p2].dist = DotProduct(p, box_planes[p2].normal) - offset;
        box_planes[p2].type = 3; // Not axis aligned.

    }

    return &box_hull;

}


bool NS_GetCollisionSizeForUser3(AvHUser3 inUser3, vec3_t outMinSize, vec3_t outMaxSize)
{

    switch (inUser3)
    {
    
    case AVH_USER3_COMMANDER_STATION:
        outMinSize[0] = -45;
        outMinSize[1] = -16;
        outMinSize[2] =  0;
        outMaxSize[0] =  45;
        outMaxSize[1] =  16;
        outMaxSize[2] =  70.34;
        return true;

    case AVH_USER3_INFANTRYPORTAL:
        outMinSize[0] = -25;
        outMinSize[1] = -25;
        outMinSize[2] =  0;
        outMaxSize[0] =  25;
        outMaxSize[1] =  25;
        outMaxSize[2] =  5;
        return true;
	
    case AVH_USER3_PHASEGATE:
        outMinSize[0] = -55;
        outMinSize[1] = -40;
        outMinSize[2] =  0;
        outMaxSize[0] =  40;
        outMaxSize[1] =  50;
        outMaxSize[2] =  14.49;
        return true;

    case AVH_USER3_ARMORY:
    case AVH_USER3_ADVANCED_ARMORY:
        outMinSize[0] = -20;
        outMinSize[1] = -20;
        outMinSize[2] =  0;
        outMaxSize[0] =  20;
        outMaxSize[1] =  20;
        outMaxSize[2] =  62.1931;
        return true;

    case AVH_USER3_ARMSLAB:
        outMinSize[0] = -30;
        outMinSize[1] = -20;
        outMinSize[2] =  0;
        outMaxSize[0] =  30;
        outMaxSize[1] =  20;
        outMaxSize[2] =  60;
        return true;

    case AVH_USER3_PROTOTYPE_LAB:
        outMinSize[0] = -16;
        outMinSize[1] = -16;
        outMinSize[2] =  0;
        outMaxSize[0] =  16;
        outMaxSize[1] =  16;
        outMaxSize[2] =  67.7443;
        return true;

    case AVH_USER3_OBSERVATORY:
        outMinSize[0] = -16;
        outMinSize[1] = -16;
        outMinSize[2] =  0;
        outMaxSize[0] =  16;
        outMaxSize[1] =  16;
        outMaxSize[2] =  80.7443;
        return true;

        /*
    case AVH_USER3_RESTOWER:
        outMinSize[0] = -35;
        outMinSize[1] = -35;
        outMinSize[2] =  0;
        outMaxSize[0] =  35;
        outMaxSize[1] =  35;
        outMaxSize[2] =  60;
        return true;
        */

    case AVH_USER3_TURRET_FACTORY:
    case AVH_USER3_ADVANCED_TURRET_FACTORY:
	    outMinSize[0] = -40;
        outMinSize[1] = -25;
        outMinSize[2] =  0;
        outMaxSize[0] =  40;
        outMaxSize[1] =  25;
        outMaxSize[2] =  50;
        return true;

    case AVH_USER3_TURRET:
	    outMinSize[0] = -16;
        outMinSize[1] = -16;
        outMinSize[2] =  0;
        outMaxSize[0] =  16;
        outMaxSize[1] =  16;
        outMaxSize[2] =  42;
		return true;
	
    case AVH_USER3_SIEGETURRET:
	    outMinSize[0] = -16;
        outMinSize[1] = -16;
        outMinSize[2] =  0;
        outMaxSize[0] =  16;
        outMaxSize[1] =  16;
        outMaxSize[2] =  62.1931;
		return true;

    /*
    case AVH_USER3_ALIENRESTOWER:
        outMinSize[0] = -30;
        outMinSize[1] = -35;
        outMinSize[2] =  0;
        outMaxSize[0] =  30;
        outMaxSize[1] =  75;
        outMaxSize[2] = 100;
        return true;

	case AVH_USER3_OFFENSE_CHAMBER:
	case AVH_USER3_DEFENSE_CHAMBER:
	case AVH_USER3_SENSORY_CHAMBER:
	case AVH_USER3_MOVEMENT_CHAMBER:
	    outMinSize[0] = -16;
        outMinSize[1] = -16;
        outMinSize[2] =  0;
        outMaxSize[0] =  16;
        outMaxSize[1] =  16;
        outMaxSize[2] =  44;
		return true;

    case AVH_USER3_HIVE:
	    outMinSize[0] = -50;
        outMinSize[1] = -80;
        outMinSize[2] = -145;
        outMaxSize[0] =  50;
        outMaxSize[1] =  80;
        outMaxSize[2] =  50;
        return true;
    */

    }
    
    return false;

}

void NS_GetClosestPlaneNormal(vec3_t inDirection, mplane_t inPlanes[], int inNumPlanes, vec3_t outNormal)
{

    VectorCopy(inDirection, outNormal);
    float best = 0;

    for (int i = 0; i < inNumPlanes; ++i)
    {
    
        float d = fabs(DotProduct(inDirection, inPlanes[i].normal));

        if (d >= best)
        {
            VectorCopy(inPlanes[i].normal, outNormal);
            best = d;
        }
    
    }

}

void NS_TraceLine(float* start, float* end, int hull, int traceFlags, int ignore_pe, bool ignorePlayers, trace_t& result)
{

    memset(&result, 0, sizeof(trace_t));
    
    result.fraction = 1;
    result.ent      = NULL;
    
    VectorCopy(end, result.endpos);

    int hullNumber = NS_GetValveHull(hull);

    for (int i = 0; i < pmove->numphysent; ++i) //Elven - why is this ++i? could this be an off by 1 error?
    {
        if (i != ignore_pe && pmove->physents[i].solid != SOLID_NOT && (!ignorePlayers || !pmove->physents[i].player))
        {

            hull_t* hull = NULL;
            bool    ignoreRotation = false;
            
            if (pmove->physents[i].model != NULL)
            {
                hull = &pmove->physents[i].model->hulls[hullNumber];
            }
            else if (!(traceFlags & PM_WORLD_ONLY))
            {

                vec3_t min;
                vec3_t max;

                VectorAdd(pmove->physents[i].mins, pmove->player_mins[pmove->usehull], min);
                VectorAdd(pmove->physents[i].maxs, pmove->player_maxs[pmove->usehull], max);

                vec3_t offset;
                VectorAdd(pmove->physents[i].maxs, pmove->physents[i].mins, offset);

                vec3_t center;
                VectorMA(pmove->physents[i].origin, 0.5, offset, center);

                vec3_t theEntityMins;
                vec3_t theEntityMaxs;

                if (NS_GetCollisionSizeForUser3((AvHUser3)pmove->physents[i].iuser3,
                        theEntityMins, theEntityMaxs))
                {

                    vec3_t thePlayerExtents;
                    VectorSubtract(pmove->player_maxs[hullNumber], pmove->player_mins[hullNumber], thePlayerExtents);
                    VectorScale(thePlayerExtents, 0.5, thePlayerExtents);

                    hull = NS_HullForBox(theEntityMins,
                                         theEntityMaxs,
                                         pmove->physents[i].angles,
                                         thePlayerExtents);

                    // We've already taken the rotation of the entity into account.
                    ignoreRotation = true;                    
                
                }
                else
                {

                    vec3_t min;
                    vec3_t max;

                    VectorAdd(pmove->physents[i].mins, pmove->player_mins[hullNumber], min);
                    VectorAdd(pmove->physents[i].maxs, pmove->player_maxs[hullNumber], max);

                    hull = PM_HullForBox(min, max);
                
                }

            }

            if (hull != NULL)
            {
                
                vec3_t localStart;
                vec3_t localEnd;

                // Translate the start and end into the model's frame of reference.

                VectorSubtract(start, pmove->physents[i].origin, localStart);
                VectorSubtract(end, pmove->physents[i].origin, localEnd);

                // Rotate the start and end into the model's frame of reference.

                bool rotated;

                if (pmove->physents[i].angles[0] ||
                    pmove->physents[i].angles[1] ||
                    pmove->physents[i].angles[2])
                {
                    rotated = !ignoreRotation;
                }
                else
                {
                    rotated = false;
                }

                vec3_t forward;
                vec3_t right;
                vec3_t up;

                if (rotated)
                {

                    AngleVectors(pmove->physents[i].angles, forward, right, up);

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

                trace_t trace;
                NS_TraceLine(hull, localStart, localEnd, &trace);

                if (trace.fraction < result.fraction)
                {
                    memcpy(&result, &trace, sizeof(trace_t));

                    if (rotated)
                    {
                        
                        vec3_t invAngles;

                        VectorScale(pmove->physents[i].angles, -1, invAngles);
                        AngleVectors(invAngles, forward, right, up);

                        vec3_t temp;
                        VectorCopy(result.plane.normal, temp);

                        result.plane.normal[0] =  DotProduct(temp, forward);
                        result.plane.normal[1] = -DotProduct(temp, right);
                        result.plane.normal[2] =  DotProduct(temp, up);

                        // TODO result.plane.dst needs to be updated.

                    }
                    
                }

            }

        }
    }

    // Compute the end position of the trace.

    result.endpos[0] = start[0] + result.fraction * (end[0] - start[0]);
    result.endpos[1] = start[1] + result.fraction * (end[1] - start[1]);
    result.endpos[2] = start[2] + result.fraction * (end[2] - start[2]);

}

pmtrace_t NS_PlayerTrace(playermove_t* pmove, float* start, float* end, int traceFlags, int ignore_pe)
{

    //return pmove->PM_PlayerTrace(start, end, traceFlags, ignore_pe);

    pmtrace_t result = { 0 };
    
    result.fraction = 1;
    result.ent      = -1;
    
    VectorCopy(end, result.endpos);

    int hullNumber = NS_GetValveHull(pmove->usehull);

	
    for (int i = 0; i < pmove->numphysent; ++i)  //wtf is this again? elven.
    {
        if (i != ignore_pe && pmove->physents[i].solid != SOLID_NOT)
        {

            hull_t* hull = NULL;
            bool    ignoreRotation = false;

            vec3_t awayDirection;
        
            if (pmove->physents[i].model != NULL)
            {
                hull = &pmove->physents[i].model->hulls[hullNumber];
            }
            else if (!(traceFlags & PM_WORLD_ONLY))
            {

                
                /*
                OBBox theBox[50];
                int theNumBoxes;
                NS_GetHitBoxesForEntity(pmove->physents[i].info, 50, theBox, theNumBoxes, pmove->time);

                for (int z = 0; z < theNumBoxes; ++z)
                {
                    NS_DrawBoundingBox(theBox[z]);
                }
                */    

                vec3_t min;
                vec3_t max;

                VectorAdd(pmove->physents[i].mins, pmove->player_mins[pmove->usehull], min);
                VectorAdd(pmove->physents[i].maxs, pmove->player_maxs[pmove->usehull], max);

                vec3_t offset;
                VectorAdd(pmove->physents[i].maxs, pmove->physents[i].mins, offset);

                vec3_t center;
                VectorMA(pmove->physents[i].origin, 0.5, offset, center);

                // Check if the player is moving away from the object. If they
                // are, don't check for collision. This makes it so that objects
                // can still move apart even if they are intersecting.

                vec3_t direction;
                VectorSubtract(end, start, direction);
                
                VectorSubtract(start, center, awayDirection);
                VectorNormalize(awayDirection);
                
                vec3_t theEntityMins;
                vec3_t theEntityMaxs;

                if (NS_GetCollisionSizeForUser3((AvHUser3)pmove->physents[i].iuser3,
                        theEntityMins, theEntityMaxs))
                {

                    vec3_t thePlayerExtents;
                    VectorSubtract(pmove->player_maxs[pmove->usehull], pmove->player_mins[pmove->usehull], thePlayerExtents);
                    VectorScale(thePlayerExtents, 0.5, thePlayerExtents);

                    hull = NS_HullForBox(theEntityMins,
                                         theEntityMaxs,
                                         pmove->physents[i].angles,
                                         thePlayerExtents);

                    /*
                    NS_DrawBoundingBox(theEntityMins,
                                       theEntityMaxs,
                                       pmove->physents[i].origin,
                                       pmove->physents[i].angles);
                    */
                    

                    // We've already taken the rotation of the entity into account.
                    ignoreRotation = true;                    
                
                }
                else
                {

                    vec3_t min;
                    vec3_t max;

                    VectorAdd(pmove->physents[i].mins, pmove->player_mins[pmove->usehull], min);
                    VectorAdd(pmove->physents[i].maxs, pmove->player_maxs[pmove->usehull], max);

                    hull = PM_HullForBox(min, max);
                
                }

                vec3_t localStart;
                VectorSubtract(start, pmove->physents[i].origin, localStart);

                if (NS_PointContents(hull, 0, localStart) == CONTENTS_SOLID)
                {
                    // If the player is stuck but trying to move away, let them.
                    if (DotProduct(direction, awayDirection) >= 0)
                    {
                        hull = NULL;
                    }
                }

            }

            if (hull != NULL)
            {
                
                vec3_t localStart;
                vec3_t localEnd;

                // Translate the start and end into the model's frame of reference.

                VectorSubtract(start, pmove->physents[i].origin, localStart);
                VectorSubtract(end, pmove->physents[i].origin, localEnd);

                // Rotate the start and end into the model's frame of reference.

                bool rotated;

                if (pmove->physents[i].model == NULL)
                {
                    rotated = false;
                }
                else
                {
                    if (pmove->physents[i].angles[0] ||
                        pmove->physents[i].angles[1] ||
                        pmove->physents[i].angles[2])
                    {
                        rotated = !ignoreRotation;
                    }
                    else
                    {
                        rotated = false;
                    }
                }

                vec3_t forward;
                vec3_t right;
                vec3_t up;

                if (rotated)
                {

                    AngleVectors(pmove->physents[i].angles, forward, right, up);

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

                trace_t trace;
                NS_TraceLine(hull, localStart, localEnd, &trace);

                if (trace.fraction < result.fraction)
                {

                    result.allsolid   = trace.allsolid;
                    result.startsolid = trace.startsolid;
                    result.inopen     = trace.inopen;
                    result.inwater    = trace.inwater;
                    result.fraction   = trace.fraction;
                    result.ent        = i;
                    result.hitgroup   = trace.hitgroup;

                    if (pmove->physents[i].model == NULL && result.allsolid)
                    {

                        // We don't have a normal, so use the plane normal
                        // closest to the away direction.

                        NS_GetClosestPlaneNormal(awayDirection, box_planes, 6, result.plane.normal);
                        result.plane.dist = 0;
                        
                        //VectorCopy(awayDirection, result.plane.normal);

                        result.allsolid = false;
                    
                    }
                    else
                    {
                        VectorCopy(trace.plane.normal, result.plane.normal);
                        result.plane.dist = trace.plane.dist;
                    }

                    if (rotated)
                    {
                        
                        vec3_t invAngles;

                        VectorScale(pmove->physents[i].angles, -1, invAngles);
                        AngleVectors(invAngles, forward, right, up);

                        vec3_t temp;
                        VectorCopy(result.plane.normal, temp);

                        result.plane.normal[0] =  DotProduct(temp, forward);
                        result.plane.normal[1] = -DotProduct(temp, right);
                        result.plane.normal[2] =  DotProduct(temp, up);

                        // TODO result.plane.dst needs to be updated.

                    }
                    
                }

            }

        }
    }

    // Compute the end position of the trace.

    result.endpos[0] = start[0] + result.fraction * (end[0] - start[0]);
    result.endpos[1] = start[1] + result.fraction * (end[1] - start[1]);
    result.endpos[2] = start[2] + result.fraction * (end[2] - start[2]);

    // Compute the delta velocity

    if (result.fraction < 1)
    {

        float s = DotProduct(result.plane.normal, pmove->velocity);

        vec3_t antiVelocity;
        VectorScale(result.plane.normal, s, antiVelocity);

        VectorSubtract(pmove->velocity, antiVelocity, result.deltavelocity);
    
    }

    return result;

}

int NS_TestPlayerPosition(playermove_t* pmove, float* origin, pmtrace_t* trace)
{

    //return pmove->PM_TestPlayerPosition(origin, trace);

    pmtrace_t tempTrace = NS_PlayerTrace(pmove, origin, origin, PM_WORLD_ONLY, -1); 
    
    if (trace != NULL)
    {
        memcpy(trace, &tempTrace, sizeof(pmtrace_t));
    }

    return tempTrace.ent;
    
}



//#ifdef AVH_CLIENT
void PM_DebugLocations(int theRandomNumber)
{
#ifdef AVH_CLIENT
    gTriDebugLocations.clear();
#endif
    
    //int theNumEnts = pmove->numphysent;
    int theNumEnts = pmove->numvisent;
    
    //if(pmove->server)
    //{
    //  pmove->Con_Printf("DEBUGLOCATIONS (server, %d ents):\n", theNumEnts);
    //}
    //else
    //{
    //  pmove->Con_Printf("DEBUGLOCATIONS (client, %d ents):\n", theNumEnts);
    //}
    
    physent_t* theEntity = NULL;
    for (int i = 0; i < theNumEnts; i++)
    {
        //theEntity = pmove->physents + i;;
        theEntity = pmove->visents + i;;
        if(theEntity)// && (theEntity->info >= 1) && (theEntity->info <= 16))
        {
            vec3_t theEntityOrigin;
            VectorCopy(theEntity->origin, theEntityOrigin);
            
            //if(pmove->iuser3 == AVH_USER3_FUNC_RESOURCE)
            //{
            //  pmove->Con_Printf("  - entnumber(%d)  iuser3 (%d)  %d origin(%f, %f, %f)  mins(%f, %f, %f)  maxs(%f %f %f)\n", theEntity->info, pmove->iuser3, theEntityOrigin[0], theEntityOrigin[1], theEntityOrigin[2], theEntity->mins[0], theEntity->mins[1], theEntity->mins[2], theEntity->maxs[0], theEntity->maxs[1], theEntity->maxs[2]);
            //}
            //pmove->Con_Printf("mins(%f, %f, %f)  maxs(%f %f %f)\n", i, theEntityOrigin[0], theEntityOrigin[1], theEntityOrigin[2]);
            
#ifdef AVH_CLIENT
            DebugPoint thePoint;
            thePoint.x = theEntityOrigin[0];
            thePoint.y = theEntityOrigin[1];
            thePoint.z = theEntityOrigin[2];

            gTriDebugLocations.push_back(thePoint);
#endif

#ifdef AVH_SERVER
            AvHSUAddDebugPoint(theEntityOrigin[0], theEntityOrigin[1], theEntityOrigin[2]);
#endif
            //          float theOriginX = (theEntity->maxs.x + theEntity->mins.x)/2.0f;
            //          float theOriginY = (theEntity->maxs.y + theEntity->mins.y)/2.0f;
            //          float theOriginZ = (theEntity->maxs.z + theEntity->mins.z)/2.0f;
            //DrawCircleOnGroundAtPoint(theEntityOrigin, 4, 0, 12, 1, 1, 0, .5f);
        }
    }
    pmove->Con_Printf("\n");
    
    //  if(theRandomNumber < 10)
    //  {
    //      pmove->Con_Printf("Using hull: %d\n", pmove->usehull);
    //  }
}
//#endif

void PM_GetWishVelocity(vec3_t& outWishVelocity)
{
    VectorNormalize(pmove->forward);
    VectorNormalize(pmove->right);
    VectorNormalize(pmove->up);

    float fmove = pmove->cmd.forwardmove;
    float smove = pmove->cmd.sidemove;
    float umove = pmove->cmd.upmove;
    
    for(int i=0 ; i<3 ; i++)
    {
        outWishVelocity[i] = pmove->forward[i]*fmove + pmove->right[i]*smove + pmove->up[i]*umove;
    }
}

bool NS_CheckOffsetFromOrigin(float inX, float inY, float inZ, vec3_t& outNormal)
{

    ///////////////////////////////
    // Begin Max's Code
    ///////////////////////////////     

    bool thePointIsSolid = false;

    const float kScalar = 1.3f;

    vec3_t thePoint;

    thePoint[0] = pmove->origin[0] + inX*kScalar;
    thePoint[1] = pmove->origin[1] + inY*kScalar;
    thePoint[2] = pmove->origin[2] + inZ*kScalar;

    // Trace against the world geometry.

    trace_t trace;
    NS_TraceLine(pmove->origin, thePoint, 1, PM_WORLD_ONLY, -1, true, trace);

    if (trace.fraction < 1)
    {
        
        // Weight the normal based on how close the intersection is.

        vec3_t normal;
        
        VectorScale(trace.plane.normal, 1.0f / (trace.fraction + 0.1f), normal);
        VectorAdd(normal, outNormal, outNormal);

        thePointIsSolid = true;

        #ifdef AVH_SERVER
        //PM_ParticleLine(pmove->origin, thePoint, 63, 0.1, 5.0);
        #endif      

    }
    else
    {
        #ifdef AVH_SERVER
        //PM_ParticleLine(pmove->origin, thePoint, 0, 0.1, 5.0);
        #endif      
    }

    // If there was no intersection with the world, trace the entities.
    
    // We prioritize world tracing over entity tracing so that small entities (like
    // IPs, resource nozzles and switches) don't cause changes in the skulk's orientation.
    
    if (!thePointIsSolid)
    {

        const int pointSizeHull = 2;

        trace_t trace;
        NS_TraceLine(pmove->origin, thePoint, pointSizeHull, 0, -1, true, trace);
        
        /*
        struct pmtrace_s* theTrace = pmove->PM_TraceLine(pmove->origin, thePoint,
            PM_TRACELINE_ANYVISIBLE, pointSizeHull, pmove->player_index);
            */
    
        //if (theTrace && (theTrace->fraction < 1) && !GetIsEntityAPlayer(theTrace->ent))
        if (trace.fraction < 1)
        {
        
            // Add the normal so that we compute an average normal over all tests.
            // Weight the normal based on how close the intersection is.

            vec3_t normal;
        
            //VectorScale(theTrace->plane.normal, 1.0f / (theTrace->fraction + 0.1f), normal);
            VectorScale(trace.plane.normal, 1.0f / (trace.fraction + 0.1f), normal);
            VectorAdd(normal, outNormal, outNormal);

            thePointIsSolid = true;

            #ifdef AVH_SERVER
            //PM_ParticleLine(pmove->origin, thePoint, 79, 0.1, 5.0);
            #endif
        
        }
    
    }

    return thePointIsSolid;

    ///////////////////////////////
    // End Max's Code
    ///////////////////////////////     

}

void NS_GetWallstickingAngles(const vec3_t surfaceNormal, const vec3_t currentAngles, vec3_t angles)
{

    vec3_t forward;
    vec3_t right;
    vec3_t up;

    VectorCopy(surfaceNormal, up);
    AngleVectors(currentAngles, forward, NULL, NULL);

    CrossProduct(forward, up, right);
    CrossProduct(up, right, forward);

    VectorNormalize(forward);
    VectorNormalize(right);
    VectorNormalize(up);

    VectorsToAngles(forward, right, up, angles);

}

void NS_FixWallstickingAngles(vec3_t angles)
{

    const float minValue = -1000;
    const float maxValue =  1000;

    if (!(angles[0] > minValue && angles[0] < maxValue) ||
        !(angles[1] > minValue && angles[1] < maxValue) ||
        !(angles[2] > minValue && angles[2] < maxValue))
    {
        
        // The angles have become invalid, so reset them.

        angles[0] = 0;
        angles[1] = 0;
        angles[2] = 0;

    }

}

void PM_ParticleAxes(vec3_t origin, vec3_t angles)
{


    vec3_t forward;
    vec3_t right;
    vec3_t up;

    AngleVectors(angles, forward, right, up);

    VectorScale(forward, 100, forward);
    VectorScale(right, 100, right);
    VectorScale(up, 100, up);

    VectorAdd(origin, forward, forward);
    VectorAdd(origin, right, right);
    VectorAdd(origin, up, up);

    PM_ParticleLine(origin, forward, 63, 0.1, 2.0);
    PM_ParticleLine(origin, right, 79, 0.1, 2.0);
    PM_ParticleLine(origin, up, 15, 0.1, 2.0);

}


void NS_UpdateWallsticking()
{

    SetUpgradeMask(&pmove->iuser4, MASK_WALLSTICKING, false);

    //pmove->effects &= ~EF_INVLIGHT;

    if (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER1)
    {
        if(!(pmove->cmd.buttons & IN_DUCK)) //&& ((pmove->onground != -1) || (pmove->numtouch > 0)))
        {
            
            vec3_t theMinPoint;
            vec3_t theMaxPoint;
            
            VectorScale(kWallstickingDistanceCheck, -1, theMinPoint);
            VectorCopy(kWallstickingDistanceCheck, theMaxPoint);
            
            // Trace corners of hull to see if any are touching a solid and
            // compute an average surface normal.
            
            vec3_t theSurfaceNormal = { 0, 0, 0 };
            
            bool wallsticking = false;

            wallsticking |= NS_CheckOffsetFromOrigin(theMinPoint[0], theMinPoint[1], theMinPoint[2], theSurfaceNormal);
            wallsticking |= NS_CheckOffsetFromOrigin(theMinPoint[0], theMaxPoint[1], theMinPoint[2], theSurfaceNormal);
            wallsticking |= NS_CheckOffsetFromOrigin(theMaxPoint[0], theMinPoint[1], theMinPoint[2], theSurfaceNormal);
            wallsticking |= NS_CheckOffsetFromOrigin(theMaxPoint[0], theMaxPoint[1], theMinPoint[2], theSurfaceNormal);

            wallsticking |= NS_CheckOffsetFromOrigin(theMinPoint[0], theMinPoint[1], theMaxPoint[2], theSurfaceNormal);
            wallsticking |= NS_CheckOffsetFromOrigin(theMinPoint[0], theMaxPoint[1], theMaxPoint[2], theSurfaceNormal);
            wallsticking |= NS_CheckOffsetFromOrigin(theMaxPoint[0], theMinPoint[1], theMaxPoint[2], theSurfaceNormal);
            wallsticking |= NS_CheckOffsetFromOrigin(theMaxPoint[0], theMaxPoint[1], theMaxPoint[2], theSurfaceNormal);

            VectorNormalize(theSurfaceNormal);

            if (wallsticking)
            {

                // Set wall sticking to true.
                SetUpgradeMask(&pmove->iuser4, MASK_WALLSTICKING);

                vec3_t angles;
                         
                #ifdef AVH_SERVER

                vec3_t worldViewAngles;
                
                Mat3 objectMatrix(pmove->vuser1);
                Mat3 viewMatrix(pmove->angles);

                (objectMatrix * viewMatrix).GetEulerAngles(worldViewAngles);                

                NS_GetWallstickingAngles(theSurfaceNormal, pmove->angles, angles);   
                VectorCopy(angles, pmove->vuser2);

                #endif

                #ifdef AVH_CLIENT
                NS_GetWallstickingAngles(theSurfaceNormal, gWorldViewAngles, angles);
                VectorCopy(angles, gTargetPlayerAngles);
                #endif

            }
            else
            {
                
                // This seems like a good idea, but it doesn't work too well in
                // practice (maybe the constant just need to be tweaked).
            
                /*
                // If the Skulk is not wallsticking, then rotate to align with the
                // surface he's moving towards (if there's one nearby).

                vec3_t traceEnd;
                vec3_t traceDir;

                VectorCopy(pmove->velocity, traceDir);
                VectorNormalize(traceDir);

                VectorScale(traceDir, kSkulkRotationLookAhead, traceDir);
                VectorAdd(traceDir, pmove->origin, traceEnd);

                pmtrace_t trace = NS_PlayerTrace(pmove, pmove->origin, traceEnd, PM_NORMAL, -1 );

                if (trace.fraction < 1)
                {

                    vec3_t angles;
                    
                    #ifdef AVH_SERVER
                    NS_GetWallstickingAngles(trace.plane.normal, pmove->vuser1, angles);
                    VectorCopy(angles, pmove->vuser2);
                    #endif

                    #ifdef AVH_CLIENT
                    NS_GetWallstickingAngles(trace.plane.normal, gWorldViewAngles, angles);
                    VectorCopy(angles, gTargetPlayerAngles);
                    #endif                    
                }
                */

                // When the player isn't wall sticking, just revert to the normal
                // angles.

                #ifdef AVH_SERVER
                VectorCopy(pmove->angles, pmove->vuser2);
                #endif

                #ifdef AVH_CLIENT
                VectorCopy(pmove->angles, gTargetPlayerAngles);
                #endif                      

            }

            // These buttons have changed this frame
            int theButtonsChanged = (pmove->oldbuttons ^ pmove->cmd.buttons);

            // The changed ones still down are "pressed"
            //int theButtonPressed =  theButtonsChanged & pmove->cmd.buttons;
            
            // If we're now wallsticking and we JUST pressed our duck button, slow our velocity
            if(GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) && (theButtonsChanged & IN_DUCK))
            {
                //pmove->Con_Printf("Setting vel to 0\n");
                VectorScale(pmove->velocity, .999f, pmove->velocity);               
                //pmove->velocity[0] = pmove->velocity[1] = pmove->velocity[2] = 0.0f;

                // Trigger footstep immediately
                pmove->flTimeStepSound = 0.0f;
            }
        }
        else
        {
            #ifdef AVH_SERVER
            //VectorCopy(pmove->angles, pmove->vuser2);
            
            vec3_t up = { 0, 0, 1 };
            NS_GetWallstickingAngles(up, pmove->angles, pmove->vuser2);
            #endif
        }

    }

}


int NS_GetStepsize(int iuser3)
{
    // TODO: Move this into a server variable?
    const int kDefaultStepsize = 18;
    int theStepSize = kDefaultStepsize;

    //if((pmove->iuser3 == AVH_USER3_ALIEN_PLAYER1) || (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER2))
    //{
    //  theStepSize = kDefaultStepsize;//.66f*kDefaultStepsize;
    //}
    //else if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER4)
    //{
    //  theStepSize = 1.3f*kDefaultStepsize;
    //}
    //else if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER5)
    //{
    //  theStepSize = 1.6*kDefaultStepsize;
    //}
    //if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER5)
    //{
    //  theStepSize = 1.3f*kDefaultStepsize;
    //}
    
    return theStepSize;
}


void PM_SwapTextures( int i, int j )
{
    char chTemp;
    char szTemp[ CBTEXTURENAMEMAX ];

    strcpy( szTemp, grgszTextureName[ i ] );
    chTemp = grgchTextureType[ i ];
    
    strcpy( grgszTextureName[ i ], grgszTextureName[ j ] );
    grgchTextureType[ i ] = grgchTextureType[ j ];

    strcpy( grgszTextureName[ j ], szTemp );
    grgchTextureType[ j ] = chTemp;
}

void PM_SortTextures( void )
{
    // Bubble sort, yuck, but this only occurs at startup and it's only 512 elements...
    //
    int i, j;

    for ( i = 0 ; i < gcTextures; i++ )
    {
        for ( j = i + 1; j < gcTextures; j++ )
        {
            if ( stricmp( grgszTextureName[ i ], grgszTextureName[ j ] ) > 0 )
            {
                // Swap
                //
                PM_SwapTextures( i, j );
            }
        }
    }
}

void PM_InitTextureTypes()
{
    char buffer[512];
    int i, j;
    byte *pMemFile;
    int fileSize, filePos;
    static qboolean bTextureTypeInit = false;

    if ( bTextureTypeInit )
        return;

    memset(&(grgszTextureName[0][0]), 0, CTEXTURESMAX * CBTEXTURENAMEMAX);
    memset(grgchTextureType, 0, CTEXTURESMAX);

    gcTextures = 0;
    memset(buffer, 0, 512);

    fileSize = pmove->COM_FileSize( "sound/materials.txt" );
    pMemFile = pmove->COM_LoadFile( "sound/materials.txt", 5, NULL );
    if ( !pMemFile )
        return;

    filePos = 0;
    // for each line in the file...
    while ( pmove->memfgets( pMemFile, fileSize, &filePos, buffer, 511 ) != NULL && (gcTextures < CTEXTURESMAX) )
    {
        // skip whitespace
        i = 0;
        while(buffer[i] && isspace(buffer[i]))
            i++;
        
        if (!buffer[i])
            continue;

        // skip comment lines
        if (buffer[i] == '/' || !isalpha(buffer[i]))
            continue;

        // get texture type
        grgchTextureType[gcTextures] = toupper(buffer[i++]);

        // skip whitespace
        while(buffer[i] && isspace(buffer[i]))
            i++;
        
        if (!buffer[i])
            continue;

        // get sentence name
        j = i;
        while (buffer[j] && !isspace(buffer[j]))
            j++;

        if (!buffer[j])
            continue;

        // null-terminate name and save in sentences array
        j = min (j, CBTEXTURENAMEMAX-1+i);
        buffer[j] = 0;
        strcpy(&(grgszTextureName[gcTextures++][0]), &(buffer[i]));
    }

    // Must use engine to free since we are in a .dll
    pmove->COM_FreeFile ( pMemFile );

    PM_SortTextures();

    bTextureTypeInit = true;
}

char PM_FindTextureType( char *name )
{
    int left, right, pivot;
    int val;

    assert( pm_shared_initialized );

    left = 0;
    right = gcTextures - 1;

    while ( left <= right )
    {
        pivot = ( left + right ) / 2;

        val = strnicmp( name, grgszTextureName[ pivot ], CBTEXTURENAMEMAX-1 );
        if ( val == 0 )
        {
            return grgchTextureType[ pivot ];
        }
        else if ( val > 0 )
        {
            left = pivot + 1;
        }
        else if ( val < 0 )
        {
            right = pivot - 1;
        }
    }
    
    return CHAR_TEX_CONCRETE;
}

float PM_GetDesiredTopDownCameraHeight(qboolean& outFoundEntity)
{
//  vec3_t      theTraceStart;
//  vec3_t      theTraceEnd;
//  vec3_t      theForward;
//  pmtrace_t*  theTrace = NULL;
//  physent_t*  theTarget = NULL;
//  float       theHeight = 320;//180;
//  qboolean    theDone = false;
//  int         theEntityHit = -1;
//
//  // Start tracing at player, but at the highest point in the map
//  VectorCopy(pmove->origin, theTraceStart);
//  theTraceStart[2] = 750;
//
//  VectorCopy(pmove->origin, theTraceEnd);
//  theTraceEnd[2] = -500;
//  
//  //AngleVectors(pmove->angles, theForward, NULL, NULL);
//  //VectorNormalize(theForward);
//  theForward[0] = theForward[1] = 0;
//  theForward[2] = -1;
//  
//  //VectorMA(pmove->origin, theHeight, theForward, theTraceEnd);
//
//  do
//  {
//      //struct pmtrace_s *(*PM_TraceLine)( float *start, float *end, int flags, int usehull, int ignore_pe );
//      theTrace = pmove->PM_TraceLine(theTraceStart, theTraceEnd, PM_TRACELINE_ANYVISIBLE, 2,  theEntityHit);
//      
//      // Find out if there are any view height entities at our x,y
//      theEntityHit = theTrace->ent;
//      if(theEntityHit > 0)
//      {
//          //theTarget = AvHSUGetEntity(theEntityHit);
//          for (int i = 0; i < pmove->numphysent; i++)
//          {
//              if ( pmove->physents[i].info == theEntityHit)
//              {
//                  theTarget = pmove->physents + i;
//              }
//          }
//      }
//
//      // If so, pick the nearest one and set our height to it
//      if(theTarget && (theTarget->iuser3 == AVH_USER3_VIEWHEIGHT))
//      {
//          // Use it's center for the height
//          theHeight = (theTarget->maxs[2] + theTarget->mins[2])/2.0f;
//
//          outFoundEntity = true;
//
//          // We're done
//          theDone = true;
//      }
//      //if(!theTarget)
//      //{
//      //  theDone = true;
//      //}
//
//      // Do the trace again but starting at this point
//      VectorCopy(theTrace->endpos, theTraceStart);
////        if(theTrace->startsolid)
////        {
////            VectorMA(theTrace->endpos, 10, theForward, theTraceStart);
////        }
//  }
//  // While the end point is not a view entity and not below
//  //while(theTrace->fraction > kFloatTolerance && !theDone);
//  while((theTrace->fraction != 1.0f) && !theDone /*&& (!theTrace->startsolid || theTrace->fraction != 0)*/);
//  
//  // Return the result
//  return theHeight;
    return 640;
}

bool NS_GetIsPlayerAlien(string& outExtension, float* outVolume = NULL)
{
    int theUser3 = pmove->iuser3;

    // Default alien extension
    bool theIsAlien1 = (theUser3 == AVH_USER3_ALIEN_PLAYER1);
    bool theIsAlien2 = (theUser3 == AVH_USER3_ALIEN_PLAYER2);
    bool theIsAlien3 = (theUser3 == AVH_USER3_ALIEN_PLAYER3);
    bool theIsAlien4 = (theUser3 == AVH_USER3_ALIEN_PLAYER4);
    bool theIsAlien5 = (theUser3 == AVH_USER3_ALIEN_PLAYER5);
    
    if(theIsAlien1)
    {
        outExtension = kAlien1FootstepExtension;
    }
    else if(theIsAlien5)
    {
        outExtension = kAlien5FootstepExtension;
        if(outVolume)
        {
            *outVolume = .3f;
        }
    }
    else if(theIsAlien2 || theIsAlien3 || theIsAlien4)
    {
        outExtension = kAlienFootstepExtension;
    }

    return theIsAlien1 || theIsAlien2 || theIsAlien3 || theIsAlien4 || theIsAlien5;
}

void NS_PlayStepSound(int inMaterialType, int inSoundNumber, float inVolume)
{
    // Is this an alien step sound?
    string theExtension;
    bool theIsAlien = NS_GetIsPlayerAlien(theExtension, &inVolume);

    // If we have heavy armor
    if((pmove->iuser3 == AVH_USER3_MARINE_PLAYER) && GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_13))
    {
        theExtension = kHeavyFootstepExtension;

        // Can't be too quiet with heavy armor
        inVolume = max(inVolume, .5f);
    }

    // Build base name
    string theBaseName;

    // Switch material type
    switch(inMaterialType)
    {
    case STEP_CONCRETE:
        theBaseName = kConcreteBaseName;
        break;

    case STEP_METAL:
        theBaseName = kMetalBaseName;
        break;
        
    case STEP_DIRT:
        theBaseName = kDirtBaseName;
        break;
        
    case STEP_VENT:
        theBaseName = kDuctBaseName;
        break;
        
    case STEP_GRATE:
        theBaseName = kGrateBaseName;
        break;
        
    case STEP_TILE:
        theBaseName = kTileBaseName;
        break;
        
    case STEP_SLOSH:
        theBaseName = kSloshBaseName;
        break;
        
    case STEP_WADE:
        theBaseName = kWadeBaseName;
        break;
        
    case STEP_LADDER:
        theBaseName = kLadderBaseName;
        break;
    }

    // Map random number to indices Valve uses
    int theFinalNumber;
    switch(inSoundNumber)
    {
    case 0:
        theFinalNumber = 1;
        break;
    case 1:
        theFinalNumber = 3;
        break;
    case 2:
        theFinalNumber = 2;
        break;
    case 3:
        theFinalNumber = 4;
        break;
    case 4:
        theFinalNumber = 5;
        break;
    }

    // If player is walking, don't play sound
    float theWalkFactor = .5f;//AvHMUGetWalkSpeedFactor((AvHUser3)pmove->iuser3);

    int theCurrentSpeed = Length(pmove->velocity);//sqrt( pmove->cmd.forwardmove * pmove->cmd.forwardmove ) +   ( pmove->cmd.sidemove * pmove->cmd.sidemove ) + ( pmove->cmd.upmove * pmove->cmd.upmove );
    int theMaxSpeed = pmove->clientmaxspeed;
    theMaxSpeed = pmove->maxspeed;
    float theSpeedFraction = theCurrentSpeed/(float)theMaxSpeed;

    bool thePlayingFootstep = true;
    if(theSpeedFraction > theWalkFactor)
    {
        // Construct full name using base name and sound number
        char theFinalName[128];
        sprintf(theFinalName, "%s%s%d", kFootstepDirectory, theBaseName.c_str(), theFinalNumber);
        
        //if(theIsAlien && (inMaterialType != STEP_WADE))
        float theNorm = ATTN_NORM;
        if((theExtension != "") && (inMaterialType == STEP_CONCRETE))
        {
            //theNorm = ATTN_IDLE;
            strcat(theFinalName, theExtension.c_str());
        }
        strcat(theFinalName, ".wav");
        
        // If alien has silencio upgrade, mute footstep volume
        float theSilenceUpgradeFactor = 0.0f;

        if(theIsAlien)
        {
            int theSilenceUpgradeLevel = AvHGetAlienUpgradeLevel(pmove->iuser4, MASK_UPGRADE_6);
            theSilenceUpgradeFactor = theSilenceUpgradeLevel/3.0f;
        }
        
        inVolume = inVolume - inVolume*theSilenceUpgradeFactor;
        
        // Play it at the specified volume
        pmove->PM_PlaySound(CHAN_BODY, theFinalName, inVolume, theNorm, 0, PITCH_NORM);
    }
    else
    {
//      pmove->Con_DPrintf("Skipping footstep sound\n");
        thePlayingFootstep = false;
    }

//  pmove->Con_DPrintf("current speed: %d, max speed: %d, fraction %f, played footstep: %d\n", theCurrentSpeed, theMaxSpeed, theSpeedFraction, thePlayingFootstep);
}

void PM_PlayStepSound( int step, float fvol )
{
    static int iSkipStep = 0;
    int irand;
    vec3_t hvel;

    pmove->iStepLeft = !pmove->iStepLeft;

    if ( !pmove->runfuncs )
    {
        return;
    }
    
    irand = pmove->RandomLong(0,1) + ( pmove->iStepLeft * 2 );

    // Don't play footsteps when ducked
    // FIXME mp_footsteps needs to be a movevar
    if((pmove->multiplayer && !pmove->movevars->footsteps) || (pmove->flags & FL_DUCKING))
    {
        return;
    }

    VectorCopy( pmove->velocity, hvel );
    hvel[2] = 0.0;

    //if ( pmove->multiplayer && ( !g_onladder && Length( hvel ) <= 220 ) ) (use CBasePlayer::GetMaxWalkSpeed somehow if ever uncommenting this)
    //  return;

    // irand - 0,1 for right foot, 2,3 for left foot
    // used to alternate left and right foot
    // FIXME, move to player state

    switch (step)
    {
    default:
    case STEP_CONCRETE:
    case STEP_METAL:
    case STEP_DIRT:
    case STEP_VENT:
    case STEP_SLOSH:
    case STEP_GRATE:
    case STEP_WADE:
    case STEP_LADDER:
        NS_PlayStepSound(step, irand, fvol);
        break;

    case STEP_TILE:
        if ( !pmove->RandomLong(0,4) )
            irand = 4;
        NS_PlayStepSound(step, irand, fvol);
        break;
    }

//  switch (step)
//  {
//  default:
//  case STEP_CONCRETE:
//      switch (irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_step1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_step3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_step2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_step4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      }
//      break;
//  case STEP_METAL:
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_metal1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_metal3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_metal2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_metal4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      }
//      break;
//  case STEP_DIRT:
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_dirt1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_dirt3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_dirt2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_dirt4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      }
//      break;
//  case STEP_VENT:
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_duct1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_duct3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_duct2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_duct4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      }
//      break;
//  case STEP_GRATE:
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_grate1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_grate3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_grate2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_grate4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      }
//      break;
//  case STEP_TILE:
//      if ( !pmove->RandomLong(0,4) )
//          irand = 4;
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_tile1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_tile3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_tile2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_tile4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 4: pmove->PM_PlaySound( CHAN_BODY, "player/pl_tile5.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      }
//      break;
//  case STEP_SLOSH:
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_slosh1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_slosh3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_slosh2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_slosh4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );   break;
//      }
//      break;
//  case STEP_WADE:
//      if ( iSkipStep == 0 )
//      {
//          iSkipStep++;
//          break;
//      }
//
//      if ( iSkipStep++ == 3 )
//      {
//          iSkipStep = 0;
//      }
//
//      switch (irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );    break;
//      }
//      break;
//  case STEP_LADDER:
//      switch(irand)
//      {
//      // right foot
//      case 0: pmove->PM_PlaySound( CHAN_BODY, "player/pl_ladder1.wav", fvol, ATTN_NORM, 0, PITCH_NORM );  break;
//      case 1: pmove->PM_PlaySound( CHAN_BODY, "player/pl_ladder3.wav", fvol, ATTN_NORM, 0, PITCH_NORM );  break;
//      // left foot
//      case 2: pmove->PM_PlaySound( CHAN_BODY, "player/pl_ladder2.wav", fvol, ATTN_NORM, 0, PITCH_NORM );  break;
//      case 3: pmove->PM_PlaySound( CHAN_BODY, "player/pl_ladder4.wav", fvol, ATTN_NORM, 0, PITCH_NORM );  break;
//      }
//      break;
//  }
}   

int PM_MapTextureTypeStepType(char chTextureType)
{
    int theTextureType = 0;

    switch (chTextureType)
    {
        default:
        case CHAR_TEX_CONCRETE:
            theTextureType = STEP_CONCRETE;
            break;

        case CHAR_TEX_METAL:
            theTextureType = STEP_METAL;
            break;

        case CHAR_TEX_DIRT:
            theTextureType = STEP_DIRT; 
            break;

        case CHAR_TEX_VENT:
            theTextureType = STEP_VENT;
            break;

        case CHAR_TEX_GRATE:
            theTextureType = STEP_GRATE;
            break;

        case CHAR_TEX_TILE:
            theTextureType = STEP_TILE;
            break;

        case CHAR_TEX_SLOSH:
            theTextureType = STEP_SLOSH;
            break;
    }
    
    return theTextureType;
}

/*
====================
PM_CatagorizeTextureType

Determine texture info for the texture we are standing on.
====================
*/
void PM_CatagorizeTextureType( void )
{
    vec3_t start, end;
    const char *pTextureName;

    VectorCopy( pmove->origin, start );
    VectorCopy( pmove->origin, end );

    // Straight down
    end[2] -= 64;

    // Fill in default values, just in case.
    pmove->sztexturename[0] = '\0';
    pmove->chtexturetype = CHAR_TEX_CONCRETE;

    pTextureName = pmove->PM_TraceTexture( pmove->onground, start, end );
    if ( !pTextureName )
        return;

    // strip leading '-0' or '+0~' or '{' or '!'
    if (*pTextureName == '-' || *pTextureName == '+')
        pTextureName += 2;

    if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
        pTextureName++;
    // '}}'
    
    strcpy( pmove->sztexturename, pTextureName);
    pmove->sztexturename[ CBTEXTURENAMEMAX - 1 ] = 0;
        
    // get texture type
    pmove->chtexturetype = PM_FindTextureType( pmove->sztexturename );  
}

void PM_GetSpeeds(float& outVelWalk, float& outVelRun)
{
}

float PM_SetStepInterval()
{
    //int theCurrentSpeed = Length(pmove->velocity);
    int theCurrentSpeed =   ( pmove->cmd.forwardmove * pmove->cmd.forwardmove ) +
                            ( pmove->cmd.sidemove * pmove->cmd.sidemove ) +
                            ( pmove->cmd.upmove * pmove->cmd.upmove );
    theCurrentSpeed = sqrt( (double)theCurrentSpeed );

    int theMaxSpeed = kMaxGroundPlayerSpeed;//pmove->clientmaxspeed;

//  int kFastestFootstepInterval = BALANCE_VAR(kFootstepFastInterval);
//  int kSlowestFootstepInterval = BALANCE_VAR(kFootstepSlowInterval);
//
//  if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER5)
//  {
//      kFastestFootstepInterval = 200;
//      kSlowestFootstepInterval = 800;
//  }

    // Play louder as we get closer to max speed
    float theFraction = (float)theCurrentSpeed/theMaxSpeed;

//  int theCurrentInterval = kSlowestFootstepInterval - (kSlowestFootstepInterval - kFastestFootstepInterval)*theFraction;

    int theScalar = 800;

    switch(pmove->iuser3)
    {
    case AVH_USER3_MARINE_PLAYER:
        if(GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_13))
        {
            theScalar = BALANCE_VAR(kFootstepHeavyScalar);
        }
        else
        {
            theScalar = BALANCE_VAR(kFootstepMarineScalar);
        }
        break;
    case AVH_USER3_ALIEN_PLAYER1:
        theScalar = BALANCE_VAR(kFootstepSkulkScalar);
        break;
    case AVH_USER3_ALIEN_PLAYER2:
        theScalar = BALANCE_VAR(kFootstepGorgeScalar);
        break;
    case AVH_USER3_ALIEN_PLAYER3:
        theScalar = BALANCE_VAR(kFootstepLerkScalar);
        break;
    case AVH_USER3_ALIEN_PLAYER4:
        theScalar = BALANCE_VAR(kFootstepFadeScalar);
        break;
    case AVH_USER3_ALIEN_PLAYER5:
        theScalar = BALANCE_VAR(kFootstepOnosScalar);
        break;
    }

    int theCurrentInterval = max(((float)theScalar/theCurrentSpeed)*100, 200);

    pmove->flTimeStepSound = theCurrentInterval;

    return theFraction;
}

void PM_SetHulls( void )
{
    pmove->player_mins[0][0] = HULL0_MINX;
    pmove->player_mins[0][1] = HULL0_MINY;
    pmove->player_mins[0][2] = HULL0_MINZ;

    pmove->player_maxs[0][0] = HULL0_MAXX;
    pmove->player_maxs[0][1] = HULL0_MAXY;
    pmove->player_maxs[0][2] = HULL0_MAXZ;

    pmove->player_mins[1][0] = HULL1_MINX;
    pmove->player_mins[1][1] = HULL1_MINY;
    pmove->player_mins[1][2] = HULL1_MINZ;
    
    pmove->player_maxs[1][0] = HULL1_MAXX;
    pmove->player_maxs[1][1] = HULL1_MAXY;
    pmove->player_maxs[1][2] = HULL1_MAXZ;

    pmove->player_mins[2][0] = HULL2_MINX;
    pmove->player_mins[2][1] = HULL2_MINY;
    pmove->player_mins[2][2] = HULL2_MINZ;
    
    pmove->player_maxs[2][0] = HULL2_MAXX;
    pmove->player_maxs[2][1] = HULL2_MAXY;
    pmove->player_maxs[2][2] = HULL2_MAXZ;

    pmove->player_mins[3][0] = HULL3_MINX;
    pmove->player_mins[3][1] = HULL3_MINY;
    pmove->player_mins[3][2] = HULL3_MINZ;
    
    pmove->player_maxs[3][0] = HULL3_MAXX;
    pmove->player_maxs[3][1] = HULL3_MAXY;
    pmove->player_maxs[3][2] = HULL3_MAXZ;


    // Work around for a problem in the engine where it always uses the default HL
    // hull sizes for the clip_mins and clip_maxs. 

    for (int i = 0; i < pmove->numphysent; ++i)
    {

        // HACK: for some reason the player field isn't correct on the clients.
        // This is either a problem with HL or with NS's propagation of entities.

        if (pmove->physents[i].info >= 1 && pmove->physents[i].info <= 32)
        {
            pmove->physents[i].player = 1;
        }
        else
        {
            pmove->physents[i].player = 0;
        }
        
        model_s* model = pmove->physents[i].model;
        
        if (model)
        {
            
            model->hulls[0].clip_mins[0] = HULL2_MINX;
            model->hulls[0].clip_mins[1] = HULL2_MINY;
            model->hulls[0].clip_mins[2] = HULL2_MINZ;
            
            model->hulls[0].clip_maxs[0] = HULL2_MAXX;
            model->hulls[0].clip_maxs[1] = HULL2_MAXY;
            model->hulls[0].clip_maxs[2] = HULL2_MAXZ;

            model->hulls[1].clip_mins[0] = HULL0_MINX;
            model->hulls[1].clip_mins[1] = HULL0_MINY;
            model->hulls[1].clip_mins[2] = HULL0_MINZ;
            
            model->hulls[1].clip_maxs[0] = HULL0_MAXX;
            model->hulls[1].clip_maxs[1] = HULL0_MAXY;
            model->hulls[1].clip_maxs[2] = HULL0_MAXZ;
        
            model->hulls[2].clip_mins[0] = HULL3_MINX;
            model->hulls[2].clip_mins[1] = HULL3_MINY;
            model->hulls[2].clip_mins[2] = HULL3_MINZ;
            
            model->hulls[2].clip_maxs[0] = HULL3_MAXX;
            model->hulls[2].clip_maxs[1] = HULL3_MAXY;
            model->hulls[2].clip_maxs[2] = HULL3_MAXZ;
            
            model->hulls[3].clip_mins[0] = HULL1_MINX;
            model->hulls[3].clip_mins[1] = HULL1_MINY;
            model->hulls[3].clip_mins[2] = HULL1_MINZ;
            
            model->hulls[3].clip_maxs[0] = HULL1_MAXX;
            model->hulls[3].clip_maxs[1] = HULL1_MAXY;
            model->hulls[3].clip_maxs[2] = HULL1_MAXZ;
        
        }
    
    }

}

void PM_UpdateStepSound( void )
{
    if(!GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        int fWalking;
        float fvol;
        vec3_t knee;
        vec3_t feet;
        vec3_t center;
        float height;
        float speed;
        float velrun;
        float velwalk;
//      float flduck;
        int fLadder;
        int step;
        
        if ( pmove->flTimeStepSound > 0 )
            return;
        
        if ( pmove->flags & FL_FROZEN )
            return;
        
        PM_CatagorizeTextureType();
        
        speed = Length( pmove->velocity );
        
        // determine if we are on a ladder
        fLadder = ( pmove->movetype == MOVETYPE_FLY );// IsOnLadder();
        
        // UNDONE: need defined numbers for run, walk, crouch, crouch run velocities!!!!    
        if ( !GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) && (( pmove->flags & FL_DUCKING) || fLadder) )
        {
            velwalk = 60;       // These constants should be based on cl_movespeedkey * cl_forwardspeed somehow
            velrun = 80;        // UNDONE: Move walking to server
//          flduck = 100;
        }
        else
        {
            velwalk = 120;
            velrun = 210;
//          flduck = 0;
        }
        
        // If we're on a ladder or on the ground, and we're moving fast enough,
        //  play step sound.  Also, if pmove->flTimeStepSound is zero, get the new
        //  sound right away - we just started moving in new level.
        if ( (fLadder || GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) || ( pmove->onground != -1 ) ) && ( Length( pmove->velocity ) > 0.0 ))
        {
            fWalking = speed < velrun;      
            
            VectorCopy( pmove->origin, center );
            VectorCopy( pmove->origin, knee );
            VectorCopy( pmove->origin, feet );
            
            height = pmove->player_maxs[ pmove->usehull ][ 2 ] - pmove->player_mins[ pmove->usehull ][ 2 ];
            
            knee[2] = pmove->origin[2] - 0.3 * height;
            feet[2] = pmove->origin[2] - 0.5 * height;
            
            // find out what we're stepping in or on...
            if (fLadder)
            {
                step = STEP_LADDER;
                fvol = 0.35;
                pmove->flTimeStepSound = 350;
            }
            else if ( pmove->PM_PointContents ( knee, NULL ) == CONTENTS_WATER )
            {
                step = STEP_WADE;
                fvol = 0.65;
                pmove->flTimeStepSound = 600;
            }
            else if ( pmove->PM_PointContents ( feet, NULL ) == CONTENTS_WATER )
            {
                step = STEP_SLOSH;
                fvol = fWalking ? 0.2 : 0.5;
                pmove->flTimeStepSound = fWalking ? 400 : 300;      
            }
            else
            {
                // find texture under player, if different from current texture, 
                // get material type
                step = PM_MapTextureTypeStepType( pmove->chtexturetype );
                float theFraction = PM_SetStepInterval();
                
                switch ( pmove->chtexturetype )
                {
                default:
                case CHAR_TEX_CONCRETE:                     
                case CHAR_TEX_METAL:    
                case CHAR_TEX_GRATE:
                case CHAR_TEX_TILE: 
                case CHAR_TEX_SLOSH:
                    //fvol = .2f + .3f*theFraction;
                    fvol = .7f*theFraction;
                    break;
                    
                case CHAR_TEX_DIRT: 
                    fvol = .75f*theFraction;
                    //fvol = .25f + .3f*theFraction;
                    //fvol = fWalking ? 0.25 : 0.55;
                    //pmove->flTimeStepSound = fWalking ? 400 : 300;
                    break;
                    
                case CHAR_TEX_VENT: 
                    fvol = .3f + .5f*theFraction;
                    //fvol = .5f + .3f*theFraction;
                    //fvol = fWalking ? 0.4 : 0.7;
                    //pmove->flTimeStepSound = fWalking ? 400 : 300;
                    break;
                }
            }
            
//          pmove->flTimeStepSound += flduck; // slower step time if ducking
            
            // play the sound
            // 35% volume if ducking
            if (!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) && (pmove->flags & FL_DUCKING))
            {
                fvol *= 0.35;
            }
            
            PM_PlayStepSound( step, fvol );
        }
    }
}

/*
================
PM_AddToTouched

Add's the trace result to touch list, if contact is not already in list.
================
*/
qboolean PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity)
{
    int i;

    for (i = 0; i < pmove->numtouch; i++)
    {
        if (pmove->touchindex[i].ent == tr.ent)
            break;
    }
    if (i != pmove->numtouch)  // Already in list.
        return false;

    VectorCopy( impactvelocity, tr.deltavelocity );

    if (pmove->numtouch >= MAX_PHYSENTS)
        pmove->Con_DPrintf("Too many entities were touched!\n");

    pmove->touchindex[pmove->numtouch++] = tr;
    return true;
}

/*
================
PM_CheckVelocity

See if the player has a bogus velocity value.
================
*/
void PM_CheckVelocity ()
{
    int     i;

//
// bound velocity
//
    for (i=0 ; i<3 ; i++)
    {
        // See if it's bogus.
        if (IS_NAN(pmove->velocity[i]))
        {
            pmove->Con_Printf ("PM  Got a NaN velocity %i\n", i);
            pmove->velocity[i] = 0;
        }
        if (IS_NAN(pmove->origin[i]))
        {
            pmove->Con_Printf ("PM  Got a NaN origin on %i\n", i);
            pmove->origin[i] = 0;
        }

        // Bound it.
        if (pmove->velocity[i] > pmove->movevars->maxvelocity) 
        {
            pmove->Con_DPrintf ("PM  Got a velocity too high on %i\n", i);
            pmove->velocity[i] = pmove->movevars->maxvelocity;
        }
        else if (pmove->velocity[i] < -pmove->movevars->maxvelocity)
        {
            pmove->Con_DPrintf ("PM  Got a velocity too low on %i\n", i);
            pmove->velocity[i] = -pmove->movevars->maxvelocity;
        }

    }
}

/*
==================
PM_ClipVelocity

Slide off of the impacting object
returns the blocked flags:
0x01 == floor
0x02 == step / wall
==================
*/
int PM_ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
    float   backoff;
    float   change;
    float angle;
    int     i, blocked;
    
    angle = normal[ 2 ];

    blocked = 0x00;            // Assume unblocked.
    if (angle > 0)      // If the plane that is blocking us has a positive z component, then assume it's a floor.
        blocked |= 0x01;        // 
    if (!angle)         // If the plane has no Z, it is vertical (wall/step)
        blocked |= 0x02;        // 
    
    // Determine how far along plane to slide based on incoming direction.
    // Scale by overbounce factor.
    backoff = DotProduct (in, normal) * overbounce;

    for (i=0 ; i<3 ; i++)
    {
        change = normal[i]*backoff;
        out[i] = in[i] - change;
        // If out velocity is too small, zero it out.
        if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
            out[i] = 0;
    }
    
    // Return blocking flags.
    return blocked;
}

float PM_GetHorizontalSpeed()
{
    return sqrtf(pmove->velocity[0] * pmove->velocity[0] +
                 pmove->velocity[1] * pmove->velocity[1]);
}

void PM_AddCorrectGravity()
{
    float   ent_gravity;

    // TODO: Put back in alien glide when alien upgrades worked out
//  if(GetHasUpgrade(pmove->iuser4, ALIEN_ABILITY_1))
//  {
//      pmove->gravity = .2f;
//  }
/*  else */
    if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3)
    {
        /*
        // Glide down
        if((pmove->cmd.buttons & IN_JUMP) && (pmove->velocity[2] <= 0.0f))
        {
            pmove->gravity = .10f;
        }
        else
        {
            pmove->gravity = .55f;
        }
        */

        float theMinGravity = 0.10f;
        float theMaxGravity = 0.75f;
        float theGravity = theMaxGravity;

        if (pmove->cmd.buttons & IN_JUMP)
        {
        
            float theSpeed = PM_GetHorizontalSpeed();
            float theLift  = (theSpeed / 300) * (pmove->forward[2] + 0.5) / 1.5;

            if (theLift < 0)
            {
                theLift = 0;
            }
            
            theGravity = theMinGravity + (1 - theLift) * (theMaxGravity - theMinGravity);
            
        }

        pmove->gravity = max(min(theGravity, theMaxGravity), theMinGravity);
    }
    else
    {
        pmove->gravity = 1.0f;
    }

    // If they're in the air and they have jump held down, and they have the gliding type,
    // apply less gravity to them
//  if ( pmove->cmd.buttons & IN_JUMP )
//  {
//      if(pmove->iuser3 == AVH_USER3_GLIDE)
//      {
//          // Only glide on the way down
//          if(pmove->velocity[2] < 0)
//          {
//              if(pmove->onground == -1)
//              {
//                  // Glide and drag baybee
//                  pmove->gravity = .15f;
//                  pmove->velocity[0] /= 1.01f;
//                  pmove->velocity[1] /= 1.01f;
//              }
//          }
//      }
//  }

    if ( pmove->waterjumptime )
        return;

    if (pmove->gravity)
        ent_gravity = pmove->gravity;
    else
        ent_gravity = 1.0;

    if(GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) || GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        pmove->gravity = ent_gravity = 0.0f;
    }

    // Add gravity so they'll be in the correct position during movement
    // yes, this 0.5 looks wrong, but it's not.  
    pmove->velocity[2] -= (ent_gravity * pmove->movevars->gravity * 0.5 * pmove->frametime );
    pmove->velocity[2] += pmove->basevelocity[2] * pmove->frametime;
    pmove->basevelocity[2] = 0;

    PM_CheckVelocity();
}


void PM_FixupGravityVelocity ()
{
    float   ent_gravity;

    if ( pmove->waterjumptime )
        return;

    if (pmove->gravity)
        ent_gravity = pmove->gravity;
    else
        ent_gravity = 1.0;

    if(GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) || GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        pmove->gravity = ent_gravity = 0.0f;
    }

    // Get the correct velocity for the end of the dt 
    pmove->velocity[2] -= (ent_gravity * pmove->movevars->gravity * pmove->frametime * 0.5 );

    PM_CheckVelocity();
}

/*
============
PM_FlyMove

The basic solid body movement clip that slides along multiple planes
============
*/
int PM_FlyMove (void)
{
    int         bumpcount, numbumps;
    vec3_t      dir;
    float       d;
    int         numplanes;
    vec3_t      planes[MAX_CLIP_PLANES];
    vec3_t      primal_velocity, original_velocity;
    vec3_t      new_velocity;
    int         i, j;
    pmtrace_t   trace;
    vec3_t      end;
    float       time_left, allFraction;
    int         blocked;
        
    numbumps  = 4;           // Bump up to four times
    
    blocked   = 0;           // Assume not blocked
    numplanes = 0;           //  and not sliding along any planes
    VectorCopy (pmove->velocity, original_velocity);  // Store original velocity
    VectorCopy (pmove->velocity, primal_velocity);
    
    allFraction = 0;
    time_left = pmove->frametime;   // Total time for this movement operation.

    // CGC - Removed zeroing of velocity so commander can float around easily outside of world 1/11/02
    bool theIsInTopDown = (pmove->iuser4 & MASK_TOPDOWN);

    for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
    {
        if (!pmove->velocity[0] && !pmove->velocity[1] && !pmove->velocity[2])
            break;

        // Assume we can move all the way from the current origin to the
        //  end point.
        for (i=0 ; i<3 ; i++)
            end[i] = pmove->origin[i] + time_left * pmove->velocity[i];

        // See if we can make it from origin to end point.

        trace = NS_PlayerTrace (pmove, pmove->origin, end, PM_NORMAL, -1 );

        if(theIsInTopDown)
        {
            // Don't collide with anything when we're commander
            VectorCopy(end, trace.endpos);
        }

        allFraction += trace.fraction;
        // If we started in a solid object, or we were in solid space
        //  the whole way, zero out our velocity and return that we
        //  are blocked by floor and wall.
        if (trace.allsolid && !theIsInTopDown)
        {
            // entity is trapped in another solid
            VectorCopy (vec3_origin, pmove->velocity);
            //Con_DPrintf("Trapped 4\n");
            return 4;
        }

        // If we moved some portion of the total distance, then
        //  copy the end position into the pmove->origin and 
        //  zero the plane counter.
        if (trace.fraction > 0 || theIsInTopDown)
        {   // actually covered some distance
            VectorCopy (trace.endpos, pmove->origin);
            VectorCopy (pmove->velocity, original_velocity);
            numplanes = 0;
        }

        // If we covered the entire distance, we are done
        //  and can return.
        if (trace.fraction == 1 || theIsInTopDown)
             break;     // moved the entire distance

        //if (!trace.ent)
        //  Sys_Error ("PM_PlayerTrace: !trace.ent");

        // Save entity that blocked us (since fraction was < 1.0)
        //  for contact
        // Add it if it's not already in the list!!!
        PM_AddToTouched(trace, pmove->velocity);

        // If the plane we hit has a high z component in the normal, then
        //  it's probably a floor
        if (trace.plane.normal[2] > 0.7)
        {
            blocked |= 1;       // floor
        }
        // If the plane has a zero z component in the normal, then it's a 
        //  step or wall
        if (!trace.plane.normal[2])
        {
            blocked |= 2;       // step / wall
            //Con_DPrintf("Blocked by %i\n", trace.ent);
        }

        // Reduce amount of pmove->frametime left by total time left * fraction
        //  that we covered.
        time_left -= time_left * trace.fraction;
        
        // Did we run out of planes to clip against?
        if (numplanes >= MAX_CLIP_PLANES)
        {   // this shouldn't really happen
            //  Stop our movement if so.
            VectorCopy (vec3_origin, pmove->velocity);
            //Con_DPrintf("Too many planes 4\n");

            break;
        }

        // Set up next clipping plane
        VectorCopy (trace.plane.normal, planes[numplanes]);
        numplanes++;
//

// modify original_velocity so it parallels all of the clip planes
//
        if ( pmove->movetype == MOVETYPE_WALK &&
            ((pmove->onground == -1) || (pmove->friction != 1)) )   // relfect player velocity
        {
            for ( i = 0; i < numplanes; i++ )
            {
                if ( planes[i][2] > 0.7  )
                {// floor or slope
                    PM_ClipVelocity( original_velocity, planes[i], new_velocity, 1 );
                    VectorCopy( new_velocity, original_velocity );
                }
                else                                                            
                    PM_ClipVelocity( original_velocity, planes[i], new_velocity, 1.0 + pmove->movevars->bounce * (1-pmove->friction) );
            }

            VectorCopy( new_velocity, pmove->velocity );
            VectorCopy( new_velocity, original_velocity );
        }
        else
        {
            for (i=0 ; i<numplanes ; i++)
            {
                PM_ClipVelocity (
                    original_velocity,
                    planes[i],
                    pmove->velocity,
                    1);
                for (j=0 ; j<numplanes ; j++)
                    if (j != i)
                    {
                        // Are we now moving against this plane?
                        if (DotProduct (pmove->velocity, planes[j]) < 0)
                            break;  // not ok
                    }
                if (j == numplanes)  // Didn't have to clip, so we're ok
                    break;
            }
            
            // Did we go all the way through plane set
            if (i != numplanes)
            {   // go along this plane
                // pmove->velocity is set in clipping call, no need to set again.
                ;  
            }
            else
            {   // go along the crease
                if (numplanes != 2)
                {
                    //Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
                    VectorCopy (vec3_origin, pmove->velocity);
                    //Con_DPrintf("Trapped 4\n");

                    break;
                }
                CrossProduct (planes[0], planes[1], dir);
                d = DotProduct (dir, pmove->velocity);
                VectorScale (dir, d, pmove->velocity );
            }

    //
    // if original velocity is against the original velocity, stop dead
    // to avoid tiny occilations in sloping corners
    //
            if (DotProduct (pmove->velocity, primal_velocity) <= 0)
            {
                //Con_DPrintf("Back\n");
                VectorCopy (vec3_origin, pmove->velocity);
                break;
            }
        }
    }

    if ( allFraction == 0 )
    {
        if(!GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
        {
            VectorCopy (vec3_origin, pmove->velocity);
        }
        //Con_DPrintf( "Don't stick\n" );
    }

    return blocked;
}

/*
==============
PM_Accelerate
==============
*/
void PM_Accelerate (vec3_t wishdir, float wishspeed, float accel)
{
    int         i;
    float       addspeed, accelspeed, currentspeed;

    // Dead player's don't accelerate
    if (pmove->dead)
        return;

    // If waterjumping, don't accelerate
    if (pmove->waterjumptime)
        return;

    // See if we are changing direction a bit
    currentspeed = DotProduct (pmove->velocity, wishdir);

    // Reduce wishspeed by the amount of veer.
    addspeed = wishspeed - currentspeed;

    // If not going to add any speed, done.
    if (addspeed <= 0)
        return;

    // Determine amount of accleration.
    accelspeed = accel * pmove->frametime * wishspeed * pmove->friction;
    
    // Cap at addspeed
    if (accelspeed > addspeed)
        accelspeed = addspeed;
    
    // Adjust velocity.
    for (i=0 ; i<3 ; i++)
    {
        pmove->velocity[i] += accelspeed * wishdir[i];  
    }
}

/*
=====================
PM_WalkMove

Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
======================
*/
void PM_WalkMove ()
{
    int         clip;
    int         oldonground;
    int i;

    vec3_t      wishvel;
    float       spd;
    float       fmove, smove, umove;
    vec3_t      wishdir;
    float       wishspeed;

    vec3_t dest, start;
    vec3_t original, originalvel;
    vec3_t down, downvel;
    float downdist, updist;

    pmtrace_t trace;
    
    // Copy movement amounts
    fmove = pmove->cmd.forwardmove;
    smove = pmove->cmd.sidemove;
    // Added by mmcguire.
    umove = pmove->cmd.upmove;

    float theVelocityLength = Length(pmove->velocity);
    //pmove->Con_Printf("Max speed: %f, velocity: %f\n", pmove->maxspeed, theVelocityLength);

    // Marines move slower when moving backwards or sideways
    if(pmove->iuser3 == AVH_USER3_MARINE_PLAYER)
    {
        if(fmove < 0)
        {
            fmove *= kMarineBackpedalSpeedScalar;
        }

        smove *= kMarineSidestepSpeedScalar;
    }

    if(GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
    {

        vec3_t theForwardView, theRightView, theUpView;

        VectorCopy(pmove->forward, theForwardView);
        VectorCopy(pmove->up, theUpView);

        VectorNormalize(theForwardView);
        VectorNormalize(theUpView);
        
        CrossProduct(theForwardView, theUpView, theRightView);
        
        // Changed by mmcguire. Added up movement.
        for (i=0 ; i<3 ; i++)
        {
            wishvel[i] = theForwardView[i]*fmove + theRightView[i]*smove + theUpView[i]*umove;
        }
    
    }
    else
    {
        // Zero out z components of movement vectors
        pmove->forward[2] = 0;
        pmove->right[2]   = 0;
        
        VectorNormalize (pmove->forward);  // Normalize remainder of vectors.
        VectorNormalize (pmove->right);    // 

        //pmove->Con_Printf("PM_WalkMove()->fmove: %f\n", fmove);

        if (PM_GetIsCharging())
        {
            SetUpgradeMask(&pmove->iuser4, MASK_ALIEN_MOVEMENT, true);

            // Modify fmove?
            fmove = 500;
        }
        
        for (i=0 ; i<2 ; i++)       // Determine x and y parts of velocity
            wishvel[i] = pmove->forward[i]*fmove + pmove->right[i]*smove;

        wishvel[2] = 0;             // Zero out z part of velocity
    }
    
    VectorCopy (wishvel, wishdir);   // Determine maginitude of speed of move
    wishspeed = VectorNormalize(wishdir);

//
// Clamp to server defined max speed
//
    if (wishspeed > pmove->maxspeed)
    {
        VectorScale (wishvel, pmove->maxspeed/wishspeed, wishvel);
        wishspeed = pmove->maxspeed;
    }

    // Set pmove velocity
    if(!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
        pmove->velocity[2] = 0;

    PM_Accelerate (wishdir, wishspeed, pmove->movevars->accelerate);

    if(!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
        pmove->velocity[2] = 0;

    // Add in any base velocity to the current velocity.
    VectorAdd (pmove->velocity, pmove->basevelocity, pmove->velocity );

    spd = Length( pmove->velocity );

    if (spd < 1.0f)
    {
        VectorClear( pmove->velocity );
        return;
    }

    // If we are not moving, do nothing
    //if (!pmove->velocity[0] && !pmove->velocity[1] && !pmove->velocity[2])
    //  return;

    oldonground = pmove->onground;

// first try just moving to the destination 
    dest[0] = pmove->origin[0] + pmove->velocity[0]*pmove->frametime;
    dest[1] = pmove->origin[1] + pmove->velocity[1]*pmove->frametime;   

    // Wall-sticking change
    //dest[2] = pmove->origin[2];
    dest[2] = pmove->origin[2] + pmove->velocity[2]*pmove->frametime;

    // first try moving directly to the next spot
    VectorCopy (dest, start);

        trace = NS_PlayerTrace (pmove, pmove->origin, dest, PM_NORMAL, -1 );    

    
    // If we made it all the way, then copy trace end
    //  as new player position.
    if (trace.fraction == 1)
    {
        VectorCopy (trace.endpos, pmove->origin);
        return;
    }

    if(!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
    {
        if (oldonground == -1 &&   // Don't walk up stairs if not on ground.
            pmove->waterlevel  == 0)
            return;
    }
    
    if (pmove->waterjumptime)         // If we are jumping out of water, don't do anything more.
        return;

    // Try sliding forward both on ground and up 16 pixels
    //  take the move that goes farthest
    VectorCopy (pmove->origin, original);       // Save out original pos &
    VectorCopy (pmove->velocity, originalvel);  //  velocity.

    // Slide move
    clip = PM_FlyMove ();

    // Copy the results out
    VectorCopy (pmove->origin  , down);
    VectorCopy (pmove->velocity, downvel);

    // Reset original values.
    VectorCopy (original, pmove->origin);

    VectorCopy (originalvel, pmove->velocity);

    // Start out up one stair height
    VectorCopy (pmove->origin, dest);
    //dest[2] += pmove->movevars->stepsize;
    dest[2] += NS_GetStepsize(pmove->iuser3);
    
    trace = NS_PlayerTrace (pmove, pmove->origin, dest, PM_NORMAL, -1 );    

    // If we started okay and made it part of the way at least,
    //  copy the results to the movement start position and then
    //  run another move try.
    if (!trace.startsolid && !trace.allsolid)
    {
        VectorCopy (trace.endpos, pmove->origin);
    }

// slide move the rest of the way.
    clip = PM_FlyMove ();

// Now try going back down from the end point
//  press down the stepheight
    VectorCopy (pmove->origin, dest);
    //dest[2] -= pmove->movevars->stepsize;
    dest[2] -= NS_GetStepsize(pmove->iuser3);
    
    trace = NS_PlayerTrace (pmove, pmove->origin, dest, PM_NORMAL, -1 );

    // If we are not on the ground any more then
    //  use the original movement attempt
    if ( trace.plane.normal[2] < 0.7)
        goto usedown;
    // If the trace ended up in empty space, copy the end
    //  over to the origin.
    if (!trace.startsolid && !trace.allsolid)
    {
        VectorCopy (trace.endpos, pmove->origin);
    }
    // Copy this origion to up.
    VectorCopy (pmove->origin, pmove->up);

    // decide which one went farther
    downdist = (down[0] - original[0])*(down[0] - original[0])
             + (down[1] - original[1])*(down[1] - original[1]);
    updist   = (pmove->up[0]   - original[0])*(pmove->up[0]   - original[0])
             + (pmove->up[1]   - original[1])*(pmove->up[1]   - original[1]);

    if (downdist > updist)
    {
usedown:
        VectorCopy (down   , pmove->origin);
        VectorCopy (downvel, pmove->velocity);
    } else // copy z value from slide move
        pmove->velocity[2] = downvel[2];
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
void PM_Friction (void)
{
    float   *vel;
    float   speed, newspeed, control;
    float   friction;
    float   drop;
    vec3_t newvel;
    
    // If we are in water jump cycle, don't apply friction
    if (pmove->waterjumptime)
        return;

    if (PM_GetIsBlinking())
        return;

    // Get velocity
    vel = pmove->velocity;
    
    // Calculate speed
    speed = sqrt(vel[0]*vel[0] +vel[1]*vel[1] + vel[2]*vel[2]);
    
    // If too slow, return
    if (speed < 0.1f)
    {
        return;
    }

    drop = 0;

// apply ground friction
    if ((pmove->onground != -1) || (GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING)))  // On an entity that is the ground
    {
        vec3_t start, stop;
        pmtrace_t trace;

        start[0] = stop[0] = pmove->origin[0] + vel[0]/speed*16;
        start[1] = stop[1] = pmove->origin[1] + vel[1]/speed*16;
        start[2] = pmove->origin[2] + pmove->player_mins[pmove->usehull][2];
        stop[2] = start[2] - 34;

        trace = NS_PlayerTrace (pmove, start, stop, PM_NORMAL, -1 );

        if (trace.fraction == 1.0)
            friction = pmove->movevars->friction*pmove->movevars->edgefriction;
        else
            friction = pmove->movevars->friction;
        
        // Grab friction value.
        //friction = pmove->movevars->friction;      

        friction *= pmove->friction;  // player friction?

        // Bleed off some speed, but if we have less than the bleed
        //  threshhold, bleed the theshold amount.
        control = (speed < pmove->movevars->stopspeed) ?
            pmove->movevars->stopspeed : speed;
        // Add the amount to t'he drop amount.
        drop += control*friction*pmove->frametime;
    }

// apply water friction
//  if (pmove->waterlevel)
//      drop += speed * pmove->movevars->waterfriction * waterlevel * pmove->frametime;

// scale the velocity
    newspeed = speed - drop;
    if (newspeed < 0)
        newspeed = 0;

    // Determine proportion of old speed we are using.
    newspeed /= speed;

    // Adjust velocity according to proportion.
    newvel[0] = vel[0] * newspeed;
    newvel[1] = vel[1] * newspeed;
    newvel[2] = vel[2] * newspeed;

    VectorCopy( newvel, pmove->velocity );
}

void PM_AirAccelerate (vec3_t wishdir, float wishspeed, float accel)
{
    int         i;
    float       addspeed, accelspeed, currentspeed, wishspd = wishspeed;
        
    if (pmove->dead)
        return;
    if (pmove->waterjumptime)
        return;
    if(GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
        return;

    // Cap speed
    //wishspd = VectorNormalize (pmove->wishveloc);
    
    if (wishspd > 30)
        wishspd = 30;
    // Determine veer amount
    currentspeed = DotProduct (pmove->velocity, wishdir);
    // See how much to add
    addspeed = wishspd - currentspeed;
    // If not adding any, done.
    if (addspeed <= 0)
        return;
    // Determine acceleration speed after acceleration

    accelspeed = accel * wishspeed * pmove->frametime * pmove->friction;
    // Cap it
    if (accelspeed > addspeed)
        accelspeed = addspeed;
    
    // Adjust pmove vel.
    for (i=0 ; i<3 ; i++)
    {
        pmove->velocity[i] += accelspeed*wishdir[i];    
    }
}

/*
===================
PM_WaterMove

===================
*/
void PM_WaterMove (void)
{
    int     i;
    vec3_t  wishvel;
    float   wishspeed;
    vec3_t  wishdir;
    vec3_t  start, dest;
    vec3_t  temp;
    pmtrace_t   trace;

    float speed, newspeed, addspeed, accelspeed;

//
// user intentions
//
    for (i=0 ; i<3 ; i++)
        wishvel[i] = pmove->forward[i]*pmove->cmd.forwardmove + pmove->right[i]*pmove->cmd.sidemove;

    // Sinking after no other movement occurs
    if (!pmove->cmd.forwardmove && !pmove->cmd.sidemove && !pmove->cmd.upmove)
        wishvel[2] -= 60;       // drift towards bottom
    else  // Go straight up by upmove amount.
        wishvel[2] += pmove->cmd.upmove;

    // Copy it over and determine speed
    VectorCopy (wishvel, wishdir);
    wishspeed = VectorNormalize(wishdir);

    // Cap speed.
    if (wishspeed > pmove->maxspeed)
    {
        VectorScale (wishvel, pmove->maxspeed/wishspeed, wishvel);
        wishspeed = pmove->maxspeed;
    }
    // Slow us down a bit.
    wishspeed *= 0.8;

    VectorAdd (pmove->velocity, pmove->basevelocity, pmove->velocity);
// Water friction
    VectorCopy(pmove->velocity, temp);
    speed = VectorNormalize(temp);
    if (speed)
    {
        newspeed = speed - pmove->frametime * speed * pmove->movevars->friction * pmove->friction;

        if (newspeed < 0)
            newspeed = 0;
        VectorScale (pmove->velocity, newspeed/speed, pmove->velocity);
    }
    else
        newspeed = 0;

//
// water acceleration
//
    if ( wishspeed < 0.1f )
    {
        return;
    }

    addspeed = wishspeed - newspeed;
    if (addspeed > 0)
    {

        VectorNormalize(wishvel);
        accelspeed = pmove->movevars->accelerate * wishspeed * pmove->frametime * pmove->friction;
        if (accelspeed > addspeed)
            accelspeed = addspeed;

        for (i = 0; i < 3; i++)
            pmove->velocity[i] += accelspeed * wishvel[i];
    }

// Now move
// assume it is a stair or a slope, so press down from stepheight above
    VectorMA (pmove->origin, pmove->frametime, pmove->velocity, dest);
    VectorCopy (dest, start);

    //start[2] += pmove->movevars->stepsize + 1;
    start[2] += NS_GetStepsize(pmove->iuser3) + 1;

    trace = NS_PlayerTrace (pmove, start, dest, PM_NORMAL, -1 );
    
    if (!trace.startsolid && !trace.allsolid)   // FIXME: check steep slope?
    {   // walked up the step, so just keep result and exit
        VectorCopy (trace.endpos, pmove->origin);
        return;
    }
    
    // Try moving straight along out normal path.
    PM_FlyMove ();
}


/*
===================
PM_AirMove

===================
*/
void PM_AirMove (void)
{
    int         i;
    vec3_t      wishvel;
    float       fmove, smove;
    vec3_t      wishdir;
    float       wishspeed;

    // Copy movement amounts
    fmove = pmove->cmd.forwardmove;
    smove = pmove->cmd.sidemove;
    
    // Zero out z components of movement vectors
    pmove->forward[2] = 0;
    pmove->right[2]   = 0;
    // Renormalize
    VectorNormalize (pmove->forward);
    VectorNormalize (pmove->right);

    // Determine x and y parts of velocity
    for (i=0 ; i<2 ; i++)       
    {
        wishvel[i] = pmove->forward[i]*fmove + pmove->right[i]*smove;
    }
    // Zero out z part of velocity
    wishvel[2] = 0;             

     // Determine maginitude of speed of move
    VectorCopy (wishvel, wishdir);  
    wishspeed = VectorNormalize(wishdir);

    // Clamp to server defined max speed
    if (wishspeed > pmove->maxspeed)
    {
        VectorScale (wishvel, pmove->maxspeed/wishspeed, wishvel);
        wishspeed = pmove->maxspeed;
    }

    PM_PreventMegaBunnyJumping(true);
    
	float theAirAccelerate = gIsJetpacking[pmove->player_index] ? pmove->movevars->airaccelerate*4 : pmove->movevars->airaccelerate;
    if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3)
    {
        theAirAccelerate = pmove->movevars->airaccelerate/22.0f;
    }
        
    PM_AirAccelerate (wishdir, wishspeed, theAirAccelerate);

    // Add in any base velocity to the current velocity.
    VectorAdd (pmove->velocity, pmove->basevelocity, pmove->velocity );

    //pmove->Con_Printf("airmove wish vel: %f %f %f\n", wishvel[0], wishvel[1], wishvel[2]);
    //pmove->Con_Printf("airmove vel: %f %f %f\n", pmove->velocity[0], pmove->velocity[1], pmove->velocity[2]);

    PM_FlyMove ();
}

qboolean PM_InWater( void )
{
    return ( pmove->waterlevel > 1 );
}

/*
=============
PM_CheckWater

Sets pmove->waterlevel and pmove->watertype values.
=============
*/
qboolean PM_CheckWater ()
{
    vec3_t  point;
    int     cont;
    int     truecont;
    float     height;
    float       heightover2;

    // Pick a spot just above the players feet.
    point[0] = pmove->origin[0] + (pmove->player_mins[pmove->usehull][0] + pmove->player_maxs[pmove->usehull][0]) * 0.5;
    point[1] = pmove->origin[1] + (pmove->player_mins[pmove->usehull][1] + pmove->player_maxs[pmove->usehull][1]) * 0.5;
    point[2] = pmove->origin[2] + pmove->player_mins[pmove->usehull][2] + 1;
    
    // Assume that we are not in water at all.
    pmove->waterlevel = 0;
    pmove->watertype = CONTENTS_EMPTY;

    // Not sure why this is happening, but the commander on hera, above the droppad is returning CONTENTS_WATER
    // The commander can never be under water
    if(!GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        // Grab point contents.
        cont = pmove->PM_PointContents (point, &truecont );
        // Are we under water? (not solid and not empty?)
        if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT )
        {
            // Set water type
            pmove->watertype = cont;
        
            // We are at least at level one
            pmove->waterlevel = 1;
        
            height = (pmove->player_mins[pmove->usehull][2] + pmove->player_maxs[pmove->usehull][2]);
            heightover2 = height * 0.5;
        
            // Now check a point that is at the player hull midpoint.
            point[2] = pmove->origin[2] + heightover2;
            cont = pmove->PM_PointContents (point, NULL );
            // If that point is also under water...
            if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT )
            {
                // Set a higher water level.
                pmove->waterlevel = 2;
        
                // Now check the eye position.  (view_ofs is relative to the origin)
                point[2] = pmove->origin[2] + pmove->view_ofs[2];
        
                cont = pmove->PM_PointContents (point, NULL );
                if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT ) 
                    pmove->waterlevel = 3;  // In over our eyes
            }
        
            // Adjust velocity based on water current, if any.
            if ( ( truecont <= CONTENTS_CURRENT_0 ) &&
                 ( truecont >= CONTENTS_CURRENT_DOWN ) )
            {
                // The deeper we are, the stronger the current.
                static vec3_t current_table[] =
                {
                    {1, 0, 0}, {0, 1, 0}, {-1, 0, 0},
                    {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
                };
        
                VectorMA (pmove->basevelocity, 50.0*pmove->waterlevel, current_table[CONTENTS_CURRENT_0 - truecont], pmove->basevelocity);
            }
        }
    }

    return pmove->waterlevel > 1;
}

/*
=============
PM_CategorizePosition
=============
*/
void PM_CategorizePosition (void)
{
    vec3_t      point;
    pmtrace_t       tr;

// if the player hull point one unit down is solid, the player
// is on ground

// see if standing on something solid   

    // Doing this before we move may introduce a potential latency in water detection, but
    // doing it after can get us stuck on the bottom in water if the amount we move up
    // is less than the 1 pixel 'threshold' we're about to snap to. Also, we'll call
    // this several times per frame, so we really need to avoid sticking to the bottom of
    // water on each call, and the converse case will correct itself if called twice.
    PM_CheckWater();

    point[0] = pmove->origin[0];
    point[1] = pmove->origin[1];
    point[2] = pmove->origin[2] - 2;

    if (pmove->velocity[2] > 180)   // Shooting up really fast.  Definitely not on ground.
    {
        pmove->onground = -1;
    }
    else
    {
        // Try and move down.
        tr = NS_PlayerTrace (pmove, pmove->origin, point, PM_NORMAL, -1 );

//      // If we hit a steep plane, we are not on ground
//      if ( tr.plane.normal[2] < 0.7)
//          pmove->onground = -1;   // too steep
//      else
//          pmove->onground = tr.ent;  // Otherwise, point to index of ent under us.

        // Change from Mr. Blonde, to fix weird problems when standing on buildings or resource nodes
        if(tr.ent > 0 && !pmove->physents[tr.ent].model)
        {
            pmove->onground = tr.ent; // Standing on a point entity
        }
        else if ( tr.plane.normal[2] < 0.7 )
        {
            pmove->onground = -1;   // too steep
        }
        else
        {
            pmove->onground = tr.ent;  // Otherwise, point to index of ent under us.
        }

        // If we are on something...
        if (pmove->onground != -1)
        {
            // Then we are not in water jump sequence
            pmove->waterjumptime = 0;
            // If we could make the move, drop us down that 1 pixel
            if (pmove->waterlevel < 2 && !tr.startsolid && !tr.allsolid)
                VectorCopy (tr.endpos, pmove->origin);
        }

        // Standing on an entity other than the world
        if (tr.ent > 0)   // So signal that we are touching something.
        {
            PM_AddToTouched(tr, pmove->velocity);
        }
    }
}

/*
=================
PM_GetRandomStuckOffsets

When a player is stuck, it's costly to try and unstick them
Grab a test offset for the player based on a passed in index
=================
*/
int PM_GetRandomStuckOffsets(int nIndex, int server, vec3_t offset)
{
 // Last time we did a full
    int idx;
    idx = rgStuckLast[nIndex][server]++;

    VectorCopy(rgv3tStuckTable[idx % 54], offset);

    return (idx % 54);
}

void PM_ResetStuckOffsets(int nIndex, int server)
{
    rgStuckLast[nIndex][server] = 0;
}

/*
=================
NudgePosition

If pmove->origin is in a solid position,
try nudging slightly on all axis to
allow for the cut precision of the net coordinates
=================
*/
#define PM_CHECKSTUCK_MINTIME 0.05  // Don't check again too quickly.

int PM_CheckStuck (void)
{
    // Can't be stuck when you're a commander, allows commander to fly around outside the world.  Do the same for observers/spectators?
    if(GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        return 0;
    }

    vec3_t  base;
    vec3_t  offset;
    vec3_t  test;
    int     hitent;
    int     idx;
    float   fTime;
    int i;
    pmtrace_t traceresult;



//  int theEntNumber = 1;
//  
//  vec3_t theMins;
//  VectorCopy(pmove->physents[theEntNumber].mins, theMins);
//  
//  vec3_t theMaxs;
//  VectorCopy(pmove->physents[theEntNumber].maxs, theMaxs);
//  
//  //char* theName = pmove->physents[hitent].model
//  //int theModType = pmove->PM_GetModelType( pmove->physents[hitent].model );
//  
//  int theSolidity = pmove->physents[theEntNumber].solid;
//
//  vec3_t theAngles;
//  VectorCopy(pmove->physents[theEntNumber].angles, theAngles);
//
//  vec3_t theOrigin;
//  VectorCopy(pmove->physents[theEntNumber].origin, theOrigin);
//  
//  if(pmove->server)
//  {
//      pmove->Con_DPrintf("Server: ent: %d, solidity: %d  mins(%f, %f, %f), maxs(%f, %f, %f), origin(%f, %f, %f), angles(%f, %f, %f)\n", theEntNumber, theSolidity, theMins[0], theMins[1], theMins[2], theMaxs[0], theMaxs[1], theMaxs[2], theOrigin[0], theOrigin[1], theOrigin[2], theAngles[0], theAngles[1], theAngles[2]);
//  }
//  else
//  {
//      pmove->Con_DPrintf("Client: ent: %d, solidity: %d  mins(%f, %f, %f), maxs(%f, %f, %f), origin(%f, %f, %f), angles(%f, %f, %f)\n", theEntNumber, theSolidity, theMins[0], theMins[1], theMins[2], theMaxs[0], theMaxs[1], theMaxs[2], theOrigin[0], theOrigin[1], theOrigin[2], theAngles[0], theAngles[1], theAngles[2]);
//  }
    




    static float rgStuckCheckTime[MAX_CLIENTS][2]; // Last time we did a full

    // If position is okay, exit
    //hitent = pmove->PM_TestPlayerPosition (pmove->origin, &traceresult );
    hitent = NS_TestPlayerPosition(pmove, pmove->origin, &traceresult);

    if (hitent == -1 )
    {
        PM_ResetStuckOffsets( pmove->player_index, pmove->server );
        return 0;
    }

    VectorCopy (pmove->origin, base);

    // 
    // Deal with precision error in network.
    // 
    if (!pmove->server)
    {
        // World or BSP model
        if ( ( hitent == 0 ) ||
             ( pmove->physents[hitent].model != NULL ) )
        {
            int nReps = 0;
            PM_ResetStuckOffsets( pmove->player_index, pmove->server );
            do 
            {
                i = PM_GetRandomStuckOffsets(pmove->player_index, pmove->server, offset);

                VectorAdd(base, offset, test);
                //if (pmove->PM_TestPlayerPosition (test, &traceresult ) == -1)
                if (NS_TestPlayerPosition(pmove, test, &traceresult ) == -1)
                {
                    PM_ResetStuckOffsets( pmove->player_index, pmove->server );
        
                    VectorCopy ( test, pmove->origin );
                    return 0;
                }
                nReps++;
            } while (nReps < 54);
        }
    }

    // Only an issue on the client.

    if (pmove->server)
        idx = 0;
    else
        idx = 1;

    fTime = pmove->Sys_FloatTime();
    // Too soon?
    if (rgStuckCheckTime[pmove->player_index][idx] >= 
        ( fTime - PM_CHECKSTUCK_MINTIME ) )
    {
        return 1;
    }
    rgStuckCheckTime[pmove->player_index][idx] = fTime;

    pmove->PM_StuckTouch( hitent, &traceresult );

    i = PM_GetRandomStuckOffsets(pmove->player_index, pmove->server, offset);

    VectorAdd(base, offset, test);
    
    if ( ( hitent = NS_TestPlayerPosition (pmove, test, NULL ) ) == -1 )
    //if ( ( hitent = pmove->PM_TestPlayerPosition ( test, NULL ) ) == -1 )
    {
        //pmove->Con_DPrintf("Nudged\n");

        PM_ResetStuckOffsets( pmove->player_index, pmove->server );

        if (i >= 27)
            VectorCopy ( test, pmove->origin );

        return 0;
    }

    // If player is flailing while stuck in another player ( should never happen ), then see
    //  if we can't "unstick" them forceably.
    if ( pmove->cmd.buttons & ( IN_JUMP | IN_DUCK | IN_ATTACK ) && ( pmove->physents[ hitent ].player != 0 ) )
    {
        float x, y, z;
        float xystep = 8.0;
        float zstep = 18.0;
        float xyminmax = xystep;
        float zminmax = 4 * zstep;
        
        for ( z = 0; z <= zminmax; z += zstep )
        {
            for ( x = -xyminmax; x <= xyminmax; x += xystep )
            {
                for ( y = -xyminmax; y <= xyminmax; y += xystep )
                {
                    VectorCopy( base, test );
                    test[0] += x;
                    test[1] += y;
                    test[2] += z;

                    //if ( pmove->PM_TestPlayerPosition ( test, NULL ) == -1 )
                    if (NS_TestPlayerPosition (pmove, test, NULL ) == -1 )
                    {
                        VectorCopy( test, pmove->origin );
                        return 0;
                    }
                }
            }
        }
    }

    //VectorCopy (base, pmove->origin);

    return 1;
}

/*
===============
PM_SpectatorMove
===============
*/
void PM_SpectatorMove (void)
{
    float   speed, drop, friction, control, newspeed;
    //float   accel;
    float   currentspeed, addspeed, accelspeed;
    int         i;
    vec3_t      wishvel;
    float       fmove, smove;
    vec3_t      wishdir;
    float       wishspeed;
    // this routine keeps track of the spectators psoition
    // there a two different main move types : track player or moce freely (OBS_ROAMING)
    // doesn't need excate track position, only to generate PVS, so just copy
    // targets position and real view position is calculated on client (saves server CPU)
    
    if ( pmove->iuser1 == OBS_ROAMING)
    {

#ifdef AVH_CLIENT
        // jump only in roaming mode
        if ( iJumpSpectator )
        {
            VectorCopy( vJumpOrigin, pmove->origin );
            VectorCopy( vJumpAngles, pmove->angles );
            VectorCopy( vec3_origin, pmove->velocity );
            iJumpSpectator  = 0;
            return;
        }
        #endif
        // Move around in normal spectator method

        speed = Length (pmove->velocity);
        if (speed < 1)
        {
            VectorCopy (vec3_origin, pmove->velocity)
        }
        else
        {
            drop = 0;

            friction = pmove->movevars->friction*1.5;   // extra friction
            control = speed < pmove->movevars->stopspeed ? pmove->movevars->stopspeed : speed;
            drop += control*friction*pmove->frametime;

            // scale the velocity
            newspeed = speed - drop;
            if (newspeed < 0)
                newspeed = 0;
            newspeed /= speed;

            VectorScale (pmove->velocity, newspeed, pmove->velocity);
        }

        // accelerate
        fmove = pmove->cmd.forwardmove;
        smove = pmove->cmd.sidemove;
        
        VectorNormalize (pmove->forward);
        VectorNormalize (pmove->right);

        for (i=0 ; i<3 ; i++)
        {
            wishvel[i] = pmove->forward[i]*fmove + pmove->right[i]*smove;
        }
        wishvel[2] += pmove->cmd.upmove;

        VectorCopy (wishvel, wishdir);
        wishspeed = VectorNormalize(wishdir);
        
        //
        // clamp to server defined max speed
        //
        if (wishspeed > pmove->movevars->spectatormaxspeed)
        {
            VectorScale (wishvel, pmove->movevars->spectatormaxspeed/wishspeed, wishvel);
            wishspeed = pmove->movevars->spectatormaxspeed;
        }
        
        currentspeed = DotProduct(pmove->velocity, wishdir);
        addspeed = wishspeed - currentspeed;
        if (addspeed <= 0)
            return;
        
        accelspeed = pmove->movevars->accelerate*pmove->frametime*wishspeed;
        if (accelspeed > addspeed)
            accelspeed = addspeed;
        
        for (i=0 ; i<3 ; i++)
            pmove->velocity[i] += accelspeed*wishdir[i];    
        
        // move
        VectorMA (pmove->origin, pmove->frametime, pmove->velocity, pmove->origin);
    }
    else
    {
        // all other modes just track some kind of target, so spectator PVS = target PVS
        
        int target;
        
        // no valid target ?
        if ( pmove->iuser2 <= 0)
            return;
        
        // Find the client this player's targeting
        for (target = 0; target < pmove->numphysent; target++)
        {
            if ( pmove->physents[target].info == pmove->iuser2 )
                break;
        }
        
        if (target == pmove->numphysent)
            return;
        
        // use targets position as own origin for PVS
        VectorCopy( pmove->physents[target].angles, pmove->angles );
        VectorCopy( pmove->physents[target].origin, pmove->origin );
        
        // no velocity
        VectorCopy( vec3_origin, pmove->velocity );
    }
}

/*
==================
PM_SplineFraction

Use for ease-in, ease-out style interpolation (accel/decel)
Used by ducking code.
==================
*/
float PM_SplineFraction( float value, float scale )
{
    float valueSquared;

    value = scale * value;
    valueSquared = value * value;

    // Nice little ease-in, ease-out spline-like curve
    return 3 * valueSquared - 2 * valueSquared * value;
}

void PM_FixPlayerCrouchStuck( int direction )
{
    int     hitent;
    int i;
    vec3_t test;

    hitent = pmove->PM_TestPlayerPosition ( pmove->origin, NULL );
    if (hitent == -1 )
        return;
    
    VectorCopy( pmove->origin, test );  
    for ( i = 0; i < 36; i++ )
    {
        pmove->origin[2] += direction;
        
        hitent = NS_TestPlayerPosition (pmove, pmove->origin, NULL );
        //hitent = pmove->PM_TestPlayerPosition ( pmove->origin, NULL );
        if (hitent == -1 )
            return;
    }

    VectorCopy( test, pmove->origin ); // Failed
}

void PM_UnDuck( void )
{
    if(AvHMUGetCanDuck(pmove->iuser3))
    {

        pmtrace_t trace;
        vec3_t newOrigin;
        
        VectorCopy( pmove->origin, newOrigin );
        
        if ( pmove->onground != -1 )
        {
            int theStandingHull = AvHMUGetHull(false, pmove->iuser3);
            int theCrouchingHull = AvHMUGetHull(true, pmove->iuser3);

            newOrigin[2] += ( pmove->player_mins[theCrouchingHull][2] - pmove->player_mins[theStandingHull][2] );
        }

        trace = NS_PlayerTrace(pmove, newOrigin, newOrigin, PM_NORMAL, -1 );        
        
        if ( !trace.startsolid )
        {
            //pmove->usehull = 0;
            pmove->usehull = AvHMUGetHull(false, pmove->iuser3);
            
            // Oh, no, changing hulls stuck us into something, try unsticking downward first.
            
            trace = NS_PlayerTrace(pmove, newOrigin, newOrigin, PM_NORMAL, -1  );

            if ( trace.startsolid )
            {
                // Trace again for onos, but move us up first.  Trying to get him unstick by moving down will never work for him.
                if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER5)
                {
                    newOrigin[2] += 30;
                }

                trace = NS_PlayerTrace(pmove, newOrigin, newOrigin, PM_NORMAL, -1  );

                if ( trace.startsolid )
                {
                    
                    // See if we are stuck?  If so, stay ducked with the duck hull until we have a clear spot
                    //pmove->Con_Printf( "unstick got stuck\n" );
                    
                    //pmove->usehull = 1;
                    pmove->usehull = AvHMUGetHull(true, pmove->iuser3);
                    //pmove->Con_Printf("Using hull: %d\n", pmove->usehull);
                    return;
                }
            }
            //pmove->Con_Printf("Using hull: %d\n", pmove->usehull);
            
            pmove->flags &= ~FL_DUCKING;
            pmove->bInDuck  = false;
            //pmove->view_ofs[2] = VEC_VIEW;
            float theViewHeight = kStandingViewHeightPercentage*pmove->player_maxs[pmove->usehull][2];
            pmove->view_ofs[2] = theViewHeight;
            pmove->flDuckTime = 0;
            
            VectorCopy( newOrigin, pmove->origin );
            
            // Recatagorize position since ducking can change origin
            PM_CategorizePosition();
        }
    }
}

void PM_Duck( void )
{
    if(AvHMUGetCanDuck(pmove->iuser3))
    {
        
        int buttonsChanged  = ( pmove->oldbuttons ^ pmove->cmd.buttons );   // These buttons have changed this frame
        int nButtonPressed  =  buttonsChanged & pmove->cmd.buttons;     // The changed ones still down are "pressed"
        
        int duckchange      = buttonsChanged & IN_DUCK ? 1 : 0;
        int duckpressed     = nButtonPressed & IN_DUCK ? 1 : 0;
        
        if ( pmove->cmd.buttons & IN_DUCK )
        {
            pmove->oldbuttons |= IN_DUCK;
        }
        else
        {
            pmove->oldbuttons &= ~IN_DUCK;
        }
        
        // Prevent ducking if the iuser3 variable is set
        //if ( pmove->iuser3 || pmove->dead )
        //{
        //  // Try to unduck
        //  if ( pmove->flags & FL_DUCKING )
        //  {
        //      PM_UnDuck();
        //  }
        //  return;
        //}
        
        if((pmove->flags & FL_DUCKING) && (!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING)))
        {
            pmove->cmd.forwardmove *= 0.333;
            pmove->cmd.sidemove    *= 0.333;
            pmove->cmd.upmove      *= 0.333;
        }
        
        if ( ( pmove->cmd.buttons & IN_DUCK ) || ( pmove->bInDuck ) || ( pmove->flags & FL_DUCKING ) )
        {
            if ( pmove->cmd.buttons & IN_DUCK )
            {
                if ( (nButtonPressed & IN_DUCK ) && !( pmove->flags & FL_DUCKING ) )
                {
                    // Use 1 second so super long jump will work
                    pmove->flDuckTime = TIME_TO_DUCK;
                    pmove->bInDuck    = true;
                }
                
                float duckFraction = 1 - ((float)pmove->flDuckTime / TIME_TO_DUCK);
                
                if ( pmove->bInDuck )
                {

                    int theStandingHull = AvHMUGetHull(false, pmove->iuser3);
                    int theCrouchingHull = AvHMUGetHull(true, pmove->iuser3);
                    
                    // Finish ducking immediately if duck time is over or not on ground
                    if ( (pmove->flDuckTime == 0) || (pmove->onground == -1 ) )
                    {

                        pmove->usehull = theCrouchingHull;
                        pmove->view_ofs[2] = kDuckingViewHeightPercentage*pmove->player_maxs[pmove->usehull][2];

                        pmove->flags |= FL_DUCKING;
                        pmove->bInDuck = false;
                        
                        // HACKHACK - Fudge for collision bug - no time to fix this properly
                        if ( pmove->onground != -1 )
                        {
                            pmove->origin[2] -= ( pmove->player_mins[theCrouchingHull][2] - pmove->player_mins[theStandingHull][2] );
                            // See if we are stuck?
                            PM_FixPlayerCrouchStuck( STUCK_MOVEUP );
                            
                            // Recatagorize position since ducking can change origin
                            PM_CategorizePosition();
                        }

                    }
                    else
                    {
                        float theDuckingZ = kDuckingViewHeightPercentage*(pmove->player_maxs[theCrouchingHull][2]) +( pmove->player_mins[theStandingHull][2] - pmove->player_mins[theCrouchingHull][2] );
                        float theStandingZ = kStandingViewHeightPercentage*(pmove->player_maxs[theStandingHull][2]);

                        // Calc parametric time
                        float duckSplineFraction = PM_SplineFraction( duckFraction, 1);
                        pmove->view_ofs[2] = theStandingZ + duckSplineFraction * (theDuckingZ - theStandingZ);
                        
                    }
                }
            }
            else
            {
                // Try to unduck
                PM_UnDuck();
            }
        }
    }
}

/*
bool NS_PositionFree(float inX, float inY, float inZ)
{
    bool thePositionFree = false;
    pmtrace_t traceresult;
    vec3_t thePosition;
    thePosition[0] = inX;
    thePosition[1] = inY;
    thePosition[2] = inZ;
    int hitent = pmove->PM_TestPlayerPosition(thePosition, &traceresult);
    if(hitent == -1)
    {
        thePositionFree = true;
    }
    return thePositionFree;
}

bool NS_PositionFreeForPlayer(vec3_t& inPosition)
{
    bool thePositionFree = false;

    // Test center point
    if(NS_PositionFree(inPosition[0], inPosition[1], inPosition[2]))
    {
        // Test 8 corners of hull
        vec3_t theMinHull;
        VectorCopy(pmove->player_mins[pmove->usehull], theMinHull);

        vec3_t theMaxHull;
        VectorCopy(pmove->player_maxs[pmove->usehull], theMaxHull);

        // Don't test hull points exactly (not exactly sure if PM_TestPlayerPosition() takes into account hull or not)
        float theFudgeFactor = .5f;
        float theMinX = inPosition[0] + theMinHull[0]*theFudgeFactor;
        float theMinY = inPosition[1] + theMinHull[1]*theFudgeFactor;
        float theMinZ = inPosition[2] + theMinHull[2]*theFudgeFactor;

        float theMaxX = inPosition[0] + theMaxHull[0]*theFudgeFactor;
        float theMaxY = inPosition[1] + theMaxHull[1]*theFudgeFactor;
        float theMaxZ = inPosition[2] + theMaxHull[2]*theFudgeFactor;

        if(NS_PositionFree(theMinX, theMinY, theMinZ))
        {
            if(NS_PositionFree(theMinX, theMinY, theMaxZ))
            {
                if(NS_PositionFree(theMinX, theMaxY, theMinZ))
                {
                    if(NS_PositionFree(theMinX, theMaxY, theMaxZ))
                    {
                        if(NS_PositionFree(theMaxX, theMinY, theMinZ))
                        {
                            if(NS_PositionFree(theMaxX, theMinY, theMaxZ))
                            {
                                if(NS_PositionFree(theMaxX, theMaxY, theMinZ))
                                {
                                    if(NS_PositionFree(theMaxX, theMaxY, theMaxZ))
                                    {
                                        thePositionFree = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return thePositionFree;
}
*/


void PM_AlienAbilities()
{
    // Give some energy back if we're an alien and not evolving
    string theExt;
    if(NS_GetIsPlayerAlien(theExt))
    {
        float theTimePassed = pmove->cmd.msec*0.001f;
        AvHMUUpdateAlienEnergy(theTimePassed, pmove->iuser3, pmove->iuser4, pmove->fuser3);

        // Stop charging when we're out of energy
        if(GetHasUpgrade(pmove->iuser4, MASK_ALIEN_MOVEMENT))
        {
            if(pmove->fuser3 <= 0.0f)
            {
                SetUpgradeMask(&pmove->iuser4, MASK_ALIEN_MOVEMENT, false);
            }
        }
    }

    //#endif

    if (PM_GetIsLeaping() || PM_GetIsBlinking())
    {
        float theScalar = 500;
		float theEnergyCost = 0;

        if (PM_GetIsBlinking())
        {
            theScalar = 225;
			AvHMUGetEnergyCost(AVH_WEAPON_BLINK, theEnergyCost);
        }
		else
		{
			AvHMUGetEnergyCost(AVH_ABILITY_LEAP, theEnergyCost);
		}

		if(AvHMUHasEnoughAlienEnergy(pmove->fuser3, theEnergyCost))
		{
			AvHMUDeductAlienEnergy(pmove->fuser3, theEnergyCost);
		}
		else
		{
			//voogru: I'd like to kill them or do something evil here (since this can only happen if they try the exploit), but nah.
			return;
		}

		vec3_t forward, right, up;
		AngleVectors(pmove->angles, forward, right, up);

		PM_Jump();

        vec3_t theAbilityVelocity;
        //VectorScale(pmove->forward, theScalar, theAbilityVelocity);
        VectorScale(forward, theScalar, theAbilityVelocity);
        
        vec3_t theFinalVelocity;
        VectorAdd(pmove->velocity, theAbilityVelocity, theFinalVelocity);
        
        VectorCopy(theFinalVelocity, pmove->velocity);

            //pmove->oldbuttons |= IN_JUMP; // don't jump again until released
            
            
            //      if(pmove->runfuncs)
            //      {
            //          pmove->PM_PlaySound(CHAN_WEAPON, kLeapSound, 1.0f, ATTN_NORM, 0, 94 + pmove->RandomLong(0, 0xf));
            //      }
            //pmove->velocity[2] += 300;
        //}
    }

//      else if((pmove->cmd.impulse == ALIEN_ABILITY_BLINK) && (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER4))
//      {
//
//            vec3_t theBlinkStart;
//          VectorCopy(pmove->origin, theBlinkStart);
//          theBlinkStart[2] += pmove->view_ofs[2];
//
//          vec3_t theBlinkDirection;
//          VectorCopy(pmove->forward, theBlinkDirection);
//
//          vec3_t theBlinkEnd;
//          VectorMA(pmove->origin, kMaxMapDimension, theBlinkDirection, theBlinkEnd);
//
//          // Do traceline through glass, until we hit something (this uses the current player hull, so it's like it's actually flying forward)
//          int theTraceFlags = PM_TRACELINE_ANYVISIBLE;
//          int theHull = -1;//pmove->usehull;
//          int theIgnoreEntity = -1;
//          pmtrace_t* theTrace = pmove->PM_TraceLine(theBlinkStart, theBlinkEnd, theTraceFlags, theHull, theIgnoreEntity);
//          ASSERT(theTrace);
//
//          // While position isn't free, bring distance back a little
//          vec3_t theFreeEndPoint;
//          VectorCopy(theTrace->endpos, theFreeEndPoint);
//
//          // Subtract view height again
//          theFreeEndPoint[2] -= pmove->view_ofs[2];
//
//          int theNumIterations = 0;
//          bool theSuccess = false;
//          while(!theSuccess && (theNumIterations < 5))
//          {
//              if(NS_PositionFreeForPlayer(theFreeEndPoint))
//              {
//                  // If position is still in front of us
//                  vec3_t theFreeEndPointDirection;
//                  VectorSubtract(theFreeEndPoint, theBlinkStart, theFreeEndPointDirection);
//                  if(DotProduct(theBlinkDirection, theFreeEndPointDirection) > 0)
//                  {
//                      // Save position so client can use it to draw
//                      theSuccess = true;
//                  }
//              }
//              else
//              {
//                  vec3_t theBackwardsIncrement;
//                  VectorMA(vec3_origin, -50, theBlinkDirection, theBackwardsIncrement);
//                      
//                  VectorAdd(theFreeEndPoint, theBackwardsIncrement, theFreeEndPoint);
//              }
//
//              theNumIterations++;
//          }
//
//          // If position is okay, exit
//          if(theSuccess)
//          {
//              // If so, set our new location to it
//              VectorCopy(theFreeEndPoint, pmove->origin);
//
//              if(pmove->runfuncs)
//              {
//                  pmove->PM_PlaybackEventFull(0, pmove->player_index, gBlinkEffectSuccessEventID, 0, (float *)theBlinkStart, (float *)theFreeEndPoint, 0.0, 0.0, 0, 0, 0, 0 );
//              }
//          }
//          //else
//          //{
//          //  // Play a "blink failed" event
//          //  pmove->PM_PlaybackEventFull(0, pmove->player_index, gBlinkEffectFailEventID, 0, (float *)pmove->origin, (float *)pmove->origin, 0.0, 0.0, 0, 0, 0, 0 );
//          //}
//
//      }
    
}

void PM_LadderMove( physent_t *pLadder )
{
    vec3_t      ladderCenter;
    trace_t     trace;
    qboolean    onFloor;
    vec3_t      floor;
    vec3_t      modelmins, modelmaxs;

    if ( pmove->movetype == MOVETYPE_NOCLIP )
        return;

    pmove->PM_GetModelBounds( pLadder->model, modelmins, modelmaxs );

    VectorAdd( modelmins, modelmaxs, ladderCenter );
    VectorScale( ladderCenter, 0.5, ladderCenter );

    pmove->movetype = MOVETYPE_FLY;

    // On ladder, convert movement to be relative to the ladder

    VectorCopy( pmove->origin, floor );
    floor[2] += pmove->player_mins[pmove->usehull][2] - 1;

    if ( pmove->PM_PointContents( floor, NULL ) == CONTENTS_SOLID )
        onFloor = true;
    else
        onFloor = false;

    pmove->gravity = 0;
    pmove->PM_TraceModel( pLadder, pmove->origin, ladderCenter, &trace );
    if ( trace.fraction != 1.0 )
    {
        float forward = 0, right = 0;
        vec3_t vpn, v_right;

        AngleVectors( pmove->angles, vpn, v_right, NULL );
        if ( pmove->cmd.buttons & IN_BACK )
            forward -= MAX_CLIMB_SPEED;
        if ( pmove->cmd.buttons & IN_FORWARD )
            forward += MAX_CLIMB_SPEED;
        if ( pmove->cmd.buttons & IN_MOVELEFT )
            right -= MAX_CLIMB_SPEED;
        if ( pmove->cmd.buttons & IN_MOVERIGHT )
            right += MAX_CLIMB_SPEED;

        if ( PM_GetIsBlinking() )
        {
            pmove->movetype = MOVETYPE_WALK;
            VectorScale( trace.plane.normal, 270, pmove->velocity );
        }
        else if ( pmove->cmd.buttons & IN_JUMP )
        {
            pmove->movetype = MOVETYPE_WALK;
            VectorScale( trace.plane.normal, 270, pmove->velocity );
        }
        else
        {
            if ( forward != 0 || right != 0 )
            {
                vec3_t velocity, perp, cross, lateral, tmp;
                float normal;

                //ALERT(at_console, "pev %.2f %.2f %.2f - ",
                //  pev->velocity.x, pev->velocity.y, pev->velocity.z);
                // Calculate player's intended velocity
                //Vector velocity = (forward * gpGlobals->v_forward) + (right * gpGlobals->v_right);
                VectorScale( vpn, forward, velocity );
                VectorMA( velocity, right, v_right, velocity );

                
                // Perpendicular in the ladder plane
    //                  Vector perp = CrossProduct( Vector(0,0,1), trace.vecPlaneNormal );
    //                  perp = perp.Normalize();
                VectorClear( tmp );
                tmp[2] = 1;
                CrossProduct( tmp, trace.plane.normal, perp );
                VectorNormalize( perp );


                // decompose velocity into ladder plane
                normal = DotProduct( velocity, trace.plane.normal );
                // This is the velocity into the face of the ladder
                VectorScale( trace.plane.normal, normal, cross );


                // This is the player's additional velocity
                VectorSubtract( velocity, cross, lateral );

                // This turns the velocity into the face of the ladder into velocity that
                // is roughly vertically perpendicular to the face of the ladder.
                // NOTE: It IS possible to face up and move down or face down and move up
                // because the velocity is a sum of the directional velocity and the converted
                // velocity through the face of the ladder -- by design.
                CrossProduct( trace.plane.normal, perp, tmp );
                VectorMA( lateral, -normal, tmp, pmove->velocity );
                if ( onFloor && normal > 0 )    // On ground moving away from the ladder
                {
                    VectorMA( pmove->velocity, MAX_CLIMB_SPEED, trace.plane.normal, pmove->velocity );
                }
                //pev->velocity = lateral - (CrossProduct( trace.vecPlaneNormal, perp ) * normal);
            }
            else
            {
                VectorClear( pmove->velocity );
            }
        }
    }
}

physent_t *PM_Ladder( void )
{
    int         i;
    physent_t   *pe;
    hull_t      *hull;
    int         num;
    vec3_t      test;

    for ( i = 0; i < pmove->nummoveent; i++ )
    {
        pe = &pmove->moveents[i];
        
        if ( pe->model && (modtype_t)pmove->PM_GetModelType( pe->model ) == mod_brush && pe->skin == CONTENTS_LADDER )
        {
    
            hull = (hull_t *)pmove->PM_HullForBsp( pe, test );
            num = hull->firstclipnode;
    
            // Offset the test point appropriately for this hull.
            VectorSubtract ( pmove->origin, test, test);
    
            // Test the player's hull for intersection with this model
            if ( pmove->PM_HullPointContents (hull, num, test) == CONTENTS_EMPTY)
                continue;
            
            return pe;
        }
    }

    return NULL;
}



void PM_WaterJump (void)
{
    if ( pmove->waterjumptime > 10000 )
    {
        pmove->waterjumptime = 10000;
    }

    if ( !pmove->waterjumptime )
        return;

    pmove->waterjumptime -= pmove->cmd.msec;
    if ( pmove->waterjumptime < 0 ||
         !pmove->waterlevel )
    {
        pmove->waterjumptime = 0;
        pmove->flags &= ~FL_WATERJUMP;
    }

    pmove->velocity[0] = pmove->movedir[0];
    pmove->velocity[1] = pmove->movedir[1];
}

/*
============
PM_AddGravity

============
*/
void PM_AddGravity ()
{
    float   ent_gravity;

    if (pmove->gravity)
        ent_gravity = pmove->gravity;
    else
        ent_gravity = 1.0;

    // Add gravity incorrectly
    pmove->velocity[2] -= (ent_gravity * pmove->movevars->gravity * pmove->frametime );
    pmove->velocity[2] += pmove->basevelocity[2] * pmove->frametime;
    pmove->basevelocity[2] = 0;
    PM_CheckVelocity();
}
/*
============
PM_PushEntity

Does not change the entities velocity at all
============
*/
pmtrace_t PM_PushEntity (vec3_t push)
{
    pmtrace_t   trace;
    vec3_t  end;
        
    VectorAdd (pmove->origin, push, end);

    trace = NS_PlayerTrace (pmove, pmove->origin, end, PM_NORMAL, -1 );
    
    VectorCopy (trace.endpos, pmove->origin);

    // So we can run impact function afterwards.
    if (trace.fraction < 1.0 &&
        !trace.allsolid)
    {
        PM_AddToTouched(trace, pmove->velocity);
    }

    return trace;
}   

/*
============
PM_Physics_Toss()

Dead player flying through air., e.g.
============
*/
void PM_Physics_Toss()
{
    pmtrace_t trace;
    vec3_t  move;
    float   backoff;

    PM_CheckWater();

    if (pmove->velocity[2] > 0)
        pmove->onground = -1;

    // If on ground and not moving, return.
    if ( pmove->onground != -1 )
    {
        if (VectorCompare(pmove->basevelocity, vec3_origin) &&
            VectorCompare(pmove->velocity, vec3_origin))
            return;
    }

    PM_CheckVelocity ();

// add gravity
    if ( pmove->movetype != MOVETYPE_FLY &&
         pmove->movetype != MOVETYPE_BOUNCEMISSILE &&
         pmove->movetype != MOVETYPE_FLYMISSILE )
        PM_AddGravity ();

// move origin
    // Base velocity is not properly accounted for since this entity will move again after the bounce without
    // taking it into account
    VectorAdd (pmove->velocity, pmove->basevelocity, pmove->velocity);
    
    PM_CheckVelocity();
    VectorScale (pmove->velocity, pmove->frametime, move);
    VectorSubtract (pmove->velocity, pmove->basevelocity, pmove->velocity);

    trace = PM_PushEntity (move);   // Should this clear basevelocity

    PM_CheckVelocity();

    if (trace.allsolid)
    {   
        // entity is trapped in another solid
        pmove->onground = trace.ent;
        VectorCopy (vec3_origin, pmove->velocity);
        return;
    }
    
    if (trace.fraction == 1)
    {
        PM_CheckWater();
        return;
    }


    if (pmove->movetype == MOVETYPE_BOUNCE)
        backoff = 2.0 - pmove->friction;
    else if (pmove->movetype == MOVETYPE_BOUNCEMISSILE)
        backoff = 2.0;
    else
        backoff = 1;

    PM_ClipVelocity (pmove->velocity, trace.plane.normal, pmove->velocity, backoff);

    // stop if on ground
    if (trace.plane.normal[2] > 0.7)
    {       
        float vel;
        vec3_t base;

        VectorClear( base );
        if (pmove->velocity[2] < pmove->movevars->gravity * pmove->frametime)
        {
            // we're rolling on the ground, add static friction.
            pmove->onground = trace.ent;
            pmove->velocity[2] = 0;
        }

        vel = DotProduct( pmove->velocity, pmove->velocity );

        // Con_DPrintf("%f %f: %.0f %.0f %.0f\n", vel, trace.fraction, ent->velocity[0], ent->velocity[1], ent->velocity[2] );

        if (vel < (30 * 30) || (pmove->movetype != MOVETYPE_BOUNCE && pmove->movetype != MOVETYPE_BOUNCEMISSILE))
        {
            pmove->onground = trace.ent;
            VectorCopy (vec3_origin, pmove->velocity);
        }
        else
        {
            VectorScale (pmove->velocity, (1.0 - trace.fraction) * pmove->frametime * 0.9, move);
            trace = PM_PushEntity (move);
        }
        VectorSubtract( pmove->velocity, base, pmove->velocity )
    }
    
// check for in water
    PM_CheckWater();
}

/*
====================
PM_NoClip

====================
*/
void PM_NoClip()
{
    int         i;
    vec3_t      wishvel;
    float       fmove, smove;
//  float       currentspeed, addspeed, accelspeed;

    // Copy movement amounts
    fmove = pmove->cmd.forwardmove;
    smove = pmove->cmd.sidemove;
    
    VectorNormalize ( pmove->forward ); 
    VectorNormalize ( pmove->right );

    for (i=0 ; i<3 ; i++)       // Determine x and y parts of velocity
    {
        wishvel[i] = pmove->forward[i]*fmove + pmove->right[i]*smove;
    }
    wishvel[2] += pmove->cmd.upmove;
    
    VectorMA (pmove->origin, pmove->frametime, wishvel, pmove->origin);
    
    // Zero out the velocity so that we don't accumulate a huge downward velocity from
    //  gravity, etc.
    VectorClear( pmove->velocity );
    
}

void PM_PlaybackEvent(int inEventID)
{
    int theFlags = 0;

    //#if defined( AVH_SERVER )
    theFlags = FEV_NOTHOST;
    //#endif
    
    vec3_t theZeroVector;
    theZeroVector[0] = theZeroVector[1] = theZeroVector[2] = 0.0f;
    
    // Lame way to get the local player index that playback event full needs...there must be another way?
    //vec3_t theTraceEnd;
    //VectorMA(pmove->origin, 100, pmove->forward, theTraceEnd);
    //pmtrace_t theTrace = pmove->PM_PlayerTrace(pmove->origin, theTraceEnd, PM_TRACELINE_PHYSENTSONLY, -1);
    //theTrace = pmove->PM_TraceLine(pmove->origin, theTraceEnd, PM_TRACELINE_PHYSENTSONLY, 2,  -1);
    
    int thisPredictedPlayer = pmove->player_index;/*theTrace.ent;*/
    pmove->PM_PlaybackEventFull(theFlags, thisPredictedPlayer, inEventID, 0, (float *)theZeroVector, (float *)theZeroVector, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
}

// Only allow bunny jumping up to 1.1x server / player maxspeed setting
#define BUNNYJUMP_MAX_SPEED_FACTOR 1.7f

//-----------------------------------------------------------------------------
// Purpose: Corrects bunny jumping ( where player initiates a bunny jump before other
//  movement logic runs, thus making onground == -1 thus making PM_Friction get skipped and
//  running PM_AirMove, which doesn't crop velocity to maxspeed like the ground / other
//  movement logic does.
//-----------------------------------------------------------------------------
void PM_PreventMegaBunnyJumping(bool inAir)
{
    // Current player speed
    float spd;
    // If we have to crop, apply this cropping fraction to velocity
    float fraction;
    // Speed at which bunny jumping is limited
    float maxscaledspeed;
    
    maxscaledspeed = BUNNYJUMP_MAX_SPEED_FACTOR * pmove->maxspeed;
    if(inAir)
    {
        // Allow flyers, leapers, and JPers to go faster in the air, but still capped
        maxscaledspeed = BALANCE_VAR(kAirspeedMultiplier)*pmove->maxspeed;
    }
    
    // Don't divide by zero
    if ( maxscaledspeed <= 0.0f )
        return;

    vec3_t theVelVector;
    VectorCopy(pmove->velocity, theVelVector);

    if(inAir)
    {
        theVelVector[2] = 0.0f;
    }

    spd = Length( theVelVector );
    
    if ( spd <= maxscaledspeed )
        return;
    
    // Returns the modifier for the velocity
    fraction = maxscaledspeed/spd;
    
    float theCurrentSpeed = Length(pmove->velocity);

    VectorScale( pmove->velocity, fraction, pmove->velocity ); //Crop it down!.

//#ifdef AVH_CLIENT
//  if(pmove->runfuncs)
//  {
//      pmove->Con_Printf("Preventing speed exploits (max speed: %f, current speed: %f, adjusted speed: %f\n", pmove->maxspeed, theCurrentSpeed, Length(pmove->velocity));
//  }
//#endif

    // Trigger footstep immediately to prevent silent bunny-hopping
    //pmove->flTimeStepSound = 0.0f;
}

/*
=============
PM_Jump
=============
*/
void PM_Jump (void)
{
    int i;
    qboolean tfc = false;
    
    qboolean cansuperjump = false;
    
    if (pmove->dead || GetHasUpgrade(pmove->iuser4, MASK_ENSNARED))
    {
        pmove->oldbuttons |= IN_JUMP ;  // don't jump again until released
        return;
    }
    
    tfc = atoi( pmove->PM_Info_ValueForKey( pmove->physinfo, "tfc" ) ) == 1 ? true : false;
    
    // Spy that's feigning death cannot jump
    if ( tfc && 
        ( pmove->deadflag == ( DEAD_DISCARDBODY + 1 ) ) )
    {
        return;
    }
    
    // See if we are waterjumping.  If so, decrement count and return.
    if ( pmove->waterjumptime )
    {
        pmove->waterjumptime -= pmove->cmd.msec;
        if (pmove->waterjumptime < 0)
        {
            pmove->waterjumptime = 0;
        }
        return;
    }
    
    // If we are in the water most of the way...
    if (pmove->waterlevel >= 2)
    {   // swimming, not jumping
        pmove->onground = -1;
        
        if (pmove->watertype == CONTENTS_WATER)    // We move up a certain amount
            pmove->velocity[2] = 100;
        else if (pmove->watertype == CONTENTS_SLIME)
            pmove->velocity[2] = 80;
        else  // LAVA
            pmove->velocity[2] = 50;
        
        // play swiming sound
        if ( pmove->flSwimTime <= 0 )
        {
            // If alien has silencio upgrade, mute footstep volume
            int theSilenceUpgradeLevel = AvHGetAlienUpgradeLevel(pmove->iuser4, MASK_UPGRADE_6);
            float theVolumeScalar = 1.0f - theSilenceUpgradeLevel/3.0f;
            
            // Don't play sound again for 1 second
            pmove->flSwimTime = 1000;
            switch ( pmove->RandomLong( 0, 3 ) )
            { 
            case 0:
                pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade1.wav", theVolumeScalar, ATTN_NORM, 0, PITCH_NORM );
                break;
            case 1:
                pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade2.wav", theVolumeScalar, ATTN_NORM, 0, PITCH_NORM );
                break;
            case 2:
                pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade3.wav", theVolumeScalar, ATTN_NORM, 0, PITCH_NORM );
                break;
            case 3:
                pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade4.wav", theVolumeScalar, ATTN_NORM, 0, PITCH_NORM );
                break;
            }
        }
        
        return;
    }
    
    // For wall jumping, remove bit above and replace with this (from coding forums)
    
    
    if(PM_CanFlap())
    {
        //          //pmove->punchangle[0] = -2;
        //          //float theXComp = (fabs(pmove->velocity[0]) > fabs(pmove->forward[0]) ? pmove->velocity[0] : pmove->forward[0]);
        //          //float theYComp = (fabs(pmove->velocity[1]) > fabs(pmove->forward[1]) ? pmove->velocity[1] : pmove->forward[1]);
        //          float theXComp = pmove->forward[0]*(pmove->cmd.forwardmove/pmove->clientmaxspeed) + pmove->right[0]*(pmove->cmd.sidemove/pmove->clientmaxspeed);
        //          float theYComp = pmove->forward[1]*(pmove->cmd.forwardmove/pmove->clientmaxspeed) + pmove->right[1]*(pmove->cmd.sidemove/pmove->clientmaxspeed);
        //
        //          pmove->velocity[0] += theXComp * 8;
        //          pmove->velocity[1] += theYComp * 8;
        //          pmove->velocity[2] += 35;

        AvHMUDeductAlienEnergy(pmove->fuser3, kAlienEnergyFlap);
        
        // Added by mmcguire.
        // Move the lerk in the direction has is facing.

        vec3_t theFlapVelocity;
        
        float theThrust;
        float theLift;
        
        if (pmove->cmd.forwardmove != 0)
        {

            if (pmove->cmd.forwardmove > 0)
            {

                theThrust = pmove->cmd.forwardmove * kWingThrustForwardScalar;
                theLift = 200 * (pmove->forward[2] + 0.5) / 1.5;
            
                if (theLift < 0)
                {
                    theLift = 0;
                }

            }
            else
            {
				// tankefugl: 0000522 reverse lerk flight
                theThrust = pmove->cmd.forwardmove * kWingThrustBackwardScalar;
                theLift = 200 * (pmove->forward[2] + 0.5) / 1.5;

				if (theLift < 0)
                {
                    theLift = 0;
                }
                //theThrust = -pmove->cmd.forwardmove * kWingThrustBackwardScalar;
                //theLift = 200;
				// :tankefugl
            }

        }
        else
        {
            theLift = 300;
            theThrust = 0;
        }

        VectorScale(pmove->forward, theThrust, theFlapVelocity);
        theFlapVelocity[2] += theLift;

		int theSpeedUpgradeLevel = 0;
        if(GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_4))
        {
            theSpeedUpgradeLevel = 1;

            if(GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_12))
            {
                theSpeedUpgradeLevel = 2;
            }
            else if(GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_13))
            {
                theSpeedUpgradeLevel = 3;
            }
        }
		int theAdjustment=theSpeedUpgradeLevel * BALANCE_VAR(kAlienCelerityBonus);
		float theAscendMax=BALANCE_VAR(kLerkBaseAscendSpeedMax) + theAdjustment;
		static float maxVelocity=0;
		maxVelocity=max(maxVelocity, pmove->velocity[2]); 
		if ( pmove->velocity[2] > theAscendMax ) {
			theFlapVelocity[2]=0;
		}
		// cap diving too
		if ( -pmove->velocity[2] > theAscendMax*1.3 ) {
			theFlapVelocity[2]=0;
		}

        vec3_t theNewVelocity;
        VectorAdd(pmove->velocity, theFlapVelocity, theNewVelocity);

        VectorCopy(theNewVelocity, pmove->velocity);
        /*

        // Old Lerk flight model.

        vec3_t theWishVelocity;
        PM_GetWishVelocity(theWishVelocity);
        
        float theWishXPercent = theWishVelocity[0]/pmove->clientmaxspeed; 
        float theWishYPercent = theWishVelocity[1]/pmove->clientmaxspeed;
        float theWishZPercent = max(0.0f, 1.0f - fabs(theWishXPercent) - fabs(theWishYPercent));
        pmove->velocity[0] += theWishXPercent*kWingFlapLateralScalar;
        pmove->velocity[1] += theWishYPercent*kWingFlapLateralScalar;
        pmove->velocity[2] += theWishZPercent*pmove->clientmaxspeed;

        */

        if(pmove->runfuncs)
        {
//          #ifdef AVH_CLIENT
//          int theLong = pmove->RandomLong(0, 20);
//          pmove->Con_Printf("Flap %d\n", theLong);
//          #endif
            
            // Pick a random sound to play
            int theSoundIndex = pmove->RandomLong(0, 2);
            char* theSoundToPlay = NULL;
            switch(theSoundIndex)
            {
            case 0:
                theSoundToPlay = kWingFlapSound1;
                break;
            case 1:
                theSoundToPlay = kWingFlapSound2;
                break;
            case 2:
                theSoundToPlay = kWingFlapSound3;
                break;
            }
            
            // If alien has silencio upgrade, mute footstep volume
            int theSilenceUpgradeLevel = AvHGetAlienUpgradeLevel(pmove->iuser4, MASK_UPGRADE_6);
            const float theBaseVolume = .5f;
            float theVolumeScalar = theBaseVolume - (theSilenceUpgradeLevel/(float)3)*theBaseVolume;
            theVolumeScalar = min(max(theVolumeScalar, 0.0f), 1.0f);
            
            pmove->PM_PlaySound(CHAN_BODY, theSoundToPlay, theVolumeScalar, ATTN_NORM, 0, PITCH_NORM);
            
            //pmove->PM_PlaySound(channel, , 1.0f, ATTN_NORM, flags, pitch);
            //gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, thePlayer->origin, CHAN_AUTO, kEndCloakSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
            
            //PM_PlaybackEvent(gFlightEventID);
        }
        //      else if(PM_CanWalljump())
//      {
//          pmove->punchangle[0] = -2;
//          pmove->velocity[2] += (pmove->forward[2] * 300);
//          
//          //pmove->velocity[2] = sqrt(2 * 800 * 45.0);
//
//          PM_PlaybackEvent(gWallJumpEventID);
//      }

        pmove->oldbuttons |= IN_JUMP; // don't jump again until released
        return; // in air, so no; effect
    }
    else
    {
    
        // Added by mmcguire.
        // Lerk gliding.

        if (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3 && pmove->onground == -1)
        {
                
            // Compute the velocity not in the direction we're facing.

            float theGlideAmount = PM_GetHorizontalSpeed() / 1000;
            
            if (theGlideAmount > 0.2)
            {
                theGlideAmount = 0.2;
            }


            float speed = Length(pmove->velocity);
            float projectedSpeed = DotProduct(pmove->velocity, pmove->forward);

			// tankefugl: 0000522 reverse lerk flight
			if (projectedSpeed < 0)
				speed *= -1;
			// :tankefugl
			vec3_t forwardVelocity;
            VectorScale(pmove->forward, speed, forwardVelocity);

            vec3_t glideVelocity;
            VectorSubtract(pmove->velocity, forwardVelocity, glideVelocity);
            VectorScale(glideVelocity, theGlideAmount, glideVelocity);

            VectorSubtract(pmove->velocity, glideVelocity, pmove->velocity);
            
            return;

        }

    
    }

    // No more effect
    if ( pmove->onground == -1 )
    {
        // Flag that we jumped.
        // HACK HACK HACK
        // Remove this when the game .dll no longer does physics code!!!!
        pmove->oldbuttons |= IN_JUMP;   // don't jump again until released
        return;     // in air, so no effect
    }


//	string theAlienExtension;
//	bool theIsAlien = NS_GetIsPlayerAlien(theAlienExtension);
//	if ( pmove->oldbuttons & IN_JUMP && (pmove->velocity[0] == 0 || !theIsAlien  || pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3) )
		//return;     // don't pogo stick

	if ( pmove->oldbuttons & IN_JUMP )
		return;     // don't pogo stick

	// In the air now.
    pmove->onground = -1;

    PM_PreventMegaBunnyJumping(false);

    if ( tfc )
    {
        pmove->PM_PlaySound( CHAN_BODY, "player/plyrjmp8.wav", 0.5, ATTN_NORM, 0, PITCH_NORM );
    }
    else
    {
        PM_PlayStepSound( PM_MapTextureTypeStepType( pmove->chtexturetype ), 1.0 );
    }
    
    // See if user can super long jump?
    cansuperjump = atoi( pmove->PM_Info_ValueForKey( pmove->physinfo, "slj" ) ) == 1 ? true : false;
    
    // Acclerate upward
    // If we are ducking...
    if ( ( pmove->bInDuck ) || ( pmove->flags & FL_DUCKING ) )
    {
        // Adjust for super long jump module
        // UNDONE -- note this should be based on forward angles, not current velocity.
        if ( cansuperjump &&
            ( pmove->cmd.buttons & IN_DUCK ) &&
            ( pmove->flDuckTime > 0 ) &&
            Length( pmove->velocity ) > 50 )
        {
            pmove->punchangle[0] = -5;
            
            for (i =0; i < 2; i++)
            {
                pmove->velocity[i] = pmove->forward[i] * PLAYER_LONGJUMP_SPEED * 1.6;
            }
            
            pmove->velocity[2] = sqrt(2 * 800 * 56.0);
        }
        else
        {
            pmove->velocity[2] = sqrt(2 * 800 * 45.0);
        }
    }
    else
    {
        pmove->velocity[2] = sqrt(2 * 800 * 45.0);
    }
    
    // Decay it for simulation
    PM_FixupGravityVelocity();
    
    // Flag that we jumped.
    pmove->oldbuttons |= IN_JUMP;   // don't jump again until released
}

/*
=============
PM_CheckWaterJump
=============
*/
#define WJ_HEIGHT 8
void PM_CheckWaterJump (void)
{
    vec3_t  vecStart, vecEnd;
    vec3_t  flatforward;
    vec3_t  flatvelocity;
    float curspeed;
    pmtrace_t tr;
    int     savehull;

    // Already water jumping.
    if ( pmove->waterjumptime )
        return;

    // Don't hop out if we just jumped in
    if ( pmove->velocity[2] < -180 )
        return; // only hop out if we are moving up

    // See if we are backing up
    flatvelocity[0] = pmove->velocity[0];
    flatvelocity[1] = pmove->velocity[1];
    flatvelocity[2] = 0;

    // Must be moving
    curspeed = VectorNormalize( flatvelocity );
    
    // see if near an edge
    flatforward[0] = pmove->forward[0];
    flatforward[1] = pmove->forward[1];
    flatforward[2] = 0;
    VectorNormalize (flatforward);

    // Are we backing into water from steps or something?  If so, don't pop forward
    if ( curspeed != 0.0 && ( DotProduct( flatvelocity, flatforward ) < 0.0 ) )
        return;

    VectorCopy( pmove->origin, vecStart );
    vecStart[2] += WJ_HEIGHT;

    VectorMA ( vecStart, 24, flatforward, vecEnd );
    
    // Trace, this trace should use the point sized collision hull
    savehull = pmove->usehull;
    pmove->usehull = 2;

    tr = NS_PlayerTrace(pmove, vecStart, vecEnd, PM_NORMAL, -1 );

    if ( tr.fraction < 1.0 && fabs( tr.plane.normal[2] ) < 0.1f )  // Facing a near vertical wall?
    {
        vecStart[2] += pmove->player_maxs[ savehull ][2] - WJ_HEIGHT;
        VectorMA( vecStart, 24, flatforward, vecEnd );
        VectorMA( vec3_origin, -50, tr.plane.normal, pmove->movedir );

        tr = NS_PlayerTrace(pmove, vecStart, vecEnd, PM_NORMAL, -1 );       

        if ( tr.fraction == 1.0 )
        {
            pmove->waterjumptime = 2000;
            pmove->velocity[2] = 225;
            pmove->oldbuttons |= IN_JUMP;
            pmove->flags |= FL_WATERJUMP;
        }
    }
    
    // Reset the collision hull
    pmove->usehull = savehull;
}

void PM_CheckFalling( void )
{
    if(!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
    {
        if ((pmove->onground != -1) && !pmove->dead)
        {
            // Slow marines down on landing after a jump
            const int theFallPenaltyThreshold = pmove->maxspeed;//BALANCE_VAR(kUnencumberedPlayerSpeed);
            if(pmove->flFallVelocity > theFallPenaltyThreshold)
            {
                if(pmove->iuser3 == AVH_USER3_MARINE_PLAYER)
                {
                    VectorScale(pmove->velocity, .3f, pmove->velocity);
                    
                    if(pmove->runfuncs)
                    {
                        //#ifdef AVH_CLIENT
                        //pmove->Con_Printf("Player landed.\n");
                        //#endif
                    }
                }
            }

            // Play landing sound if we landed hard enough
            if(pmove->flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD)
            {
                float fvol = 0.5;
				char theFallPainSound[64];                
                
                int theSilenceUpgradeLevel = AvHGetAlienUpgradeLevel(pmove->iuser4, MASK_UPGRADE_6);
                float theVolumeScalar = 1.0f - theSilenceUpgradeLevel/3.0f;
                
                float theFallPainVolume = 1.0f*theVolumeScalar;
                
                string theAlienExtension;
                bool theIsAlien = NS_GetIsPlayerAlien(theAlienExtension);

				if(theIsAlien  || pmove->iuser3 == AVH_USER3_ALIEN_EMBRYO )
                {
					sprintf(theFallPainSound, kFallPainSoundFormat, pmove->iuser3);
                }
				else 
				{
					if ( GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_13) )
					{
						sprintf(theFallPainSound, kFallPainSoundFormat, 2);
					}
					else
					{
						sprintf(theFallPainSound, kFallPainSoundFormat, 1);
					}
				}
                
                if ( pmove->waterlevel > 0 )
                {
                }
                else if ( pmove->flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED )
                {
                    // NOTE:  In the original game dll , there were no breaks after these cases, causing the first one to 
                    // cascade into the second
                    //switch ( RandomLong(0,1) )
                    //{
                    //case 0:
                    //pmove->PM_PlaySound( CHAN_VOICE, "player/pl_fallpain2.wav", 1, ATTN_NORM, 0, PITCH_NORM );
                    //break;
                    //case 1:
                    pmove->PM_PlaySound( CHAN_VOICE, theFallPainSound, theFallPainVolume, ATTN_NORM, 0, PITCH_NORM );
                    //  break;
                    //}
                    fvol = 1.0;
                }
                else if ( pmove->flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED / 2 )
                {
                    qboolean tfc = false;
                    tfc = atoi( pmove->PM_Info_ValueForKey( pmove->physinfo, "tfc" ) ) == 1 ? true : false;
                    
                    if ( tfc )
                    {
                        pmove->PM_PlaySound( CHAN_VOICE, theFallPainSound, theFallPainVolume, ATTN_NORM, 0, PITCH_NORM );
                    }
                    
                    fvol = 0.85;
                }
                else if ( pmove->flFallVelocity < PLAYER_MIN_BOUNCE_SPEED )
                {
                    fvol = 0;
                }
                
                if ( fvol > 0.0 )
                {
                    // Play landing step right away
                    pmove->flTimeStepSound = 0;
                    
                    PM_UpdateStepSound();
                    
                    // play step sound for current texture
                    PM_PlayStepSound( PM_MapTextureTypeStepType( pmove->chtexturetype ), fvol );
                    
                    // Knock the screen around a little bit, temporary effect
                    pmove->punchangle[ 2 ] = pmove->flFallVelocity * 0.013; // punch z axis
                    
                    if ( pmove->punchangle[ 0 ] > 8 )
                    {
                        pmove->punchangle[ 0 ] = 8;
                    }
                }
            }
        }
        
        if ( pmove->onground != -1 ) 
        {       
            pmove->flFallVelocity = 0;
        }
    }
}

/*
=================
PM_PlayWaterSounds

  =================
*/
void PM_PlayWaterSounds( void )
{
    // Did we enter or leave water?
    if  ( ( pmove->oldwaterlevel == 0 && pmove->waterlevel != 0 ) ||
        ( pmove->oldwaterlevel != 0 && pmove->waterlevel == 0 ) )
    {
        int theSilenceUpgradeLevel = AvHGetAlienUpgradeLevel(pmove->iuser4, MASK_UPGRADE_6);
        float theVolume = 1.0f - theSilenceUpgradeLevel/3.0f;

        switch ( pmove->RandomLong(0,3) )
        {
        case 0:
            pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade1.wav", theVolume, ATTN_NORM, 0, PITCH_NORM );
            break;
        case 1:
            pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade2.wav", theVolume, ATTN_NORM, 0, PITCH_NORM );
            break;
        case 2:
            pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade3.wav", theVolume, ATTN_NORM, 0, PITCH_NORM );
            break;
        case 3:
            pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade4.wav", theVolume, ATTN_NORM, 0, PITCH_NORM );
            break;
        }
    }
}

/*
===============
PM_CalcRoll

===============
*/
float PM_CalcRoll (vec3_t angles, vec3_t velocity, float rollangle, float rollspeed )
{
    float   sign;
    float   side;
    float   value;
    vec3_t  forward, right, up;
    
    AngleVectors (angles, forward, right, up);
    
    side = DotProduct (velocity, right);
    
    sign = side < 0 ? -1 : 1;
    
    side = fabs(side);
    
    value = rollangle;
    
    if (side < rollspeed)
    {
        side = side * value / rollspeed;
    }
    else
    {
        side = value;
    }
  
    return side * sign;
}

/*
=============
PM_DropPunchAngle

=============
*/
void PM_DropPunchAngle ( vec3_t punchangle )
{
    float   len;
    
    len = VectorNormalize ( punchangle );
    len -= (10.0 + len * 0.5) * pmove->frametime;
    len = max( len, 0.0 );
    VectorScale ( punchangle, len, punchangle);
}

/*
==============
PM_CheckParamters

==============
*/
void PM_CheckParamters( void )
{
    float spd;
    float maxspeed;
    vec3_t  v_angle;

    spd = ( pmove->cmd.forwardmove * pmove->cmd.forwardmove ) +
          ( pmove->cmd.sidemove * pmove->cmd.sidemove ) +
          ( pmove->cmd.upmove * pmove->cmd.upmove );
    spd = sqrt( spd );

    maxspeed = pmove->clientmaxspeed; //atof( pmove->PM_Info_ValueForKey( pmove->physinfo, "maxspd" ) );
    if ( maxspeed != 0.0 )
    {
        pmove->maxspeed = min( maxspeed, pmove->maxspeed );
    }

    if ( ( spd != 0.0 ) &&
         ( spd > pmove->maxspeed ) )
    {
        float fRatio = pmove->maxspeed / spd;
        pmove->cmd.forwardmove *= fRatio;
        pmove->cmd.sidemove    *= fRatio;
        pmove->cmd.upmove      *= fRatio;
    }

    bool theIsImmobilized = GetHasUpgrade(pmove->iuser4, MASK_PLAYER_STUNNED) || GetHasUpgrade(pmove->iuser4, MASK_ALIEN_EMBRYO);

    if ( pmove->flags & FL_FROZEN ||
         pmove->flags & FL_ONTRAIN || 
         theIsImmobilized ||
         pmove->dead )
    {
        pmove->cmd.forwardmove = 0;
        pmove->cmd.sidemove    = 0;
        pmove->cmd.upmove      = 0;
        pmove->cmd.viewangles[0] = 0;
        pmove->cmd.viewangles[1] = 0;
        pmove->cmd.viewangles[2] = 0;
        pmove->cmd.buttons = 0;
    }


    PM_DropPunchAngle( pmove->punchangle );

    // Take angles from command.
    if(!theIsImmobilized)
    {
        if ( !pmove->dead)
        {
            VectorCopy ( pmove->cmd.viewangles, v_angle );         
            VectorAdd( v_angle, pmove->punchangle, v_angle );
        
            // Set up view angles.
            pmove->angles[ROLL] =   PM_CalcRoll ( v_angle, pmove->velocity, pmove->movevars->rollangle, pmove->movevars->rollspeed )*4;
            pmove->angles[PITCH] =  v_angle[PITCH];
            pmove->angles[YAW]   =  v_angle[YAW];
        }
        else
        {
            VectorCopy( pmove->oldangles, pmove->angles );
        }
    }
    else
    {
        VectorCopy( pmove->oldangles, pmove->angles );
    }
    
    // Set dead player view_offset
    if ( pmove->dead )
    {
        pmove->view_ofs[2] = PM_DEAD_VIEWHEIGHT;
    }

    // Adjust client view angles to match values used on server.
    if (pmove->angles[YAW] > 180.0f)
    {
        pmove->angles[YAW] -= 360.0f;
    }

    // Set default stepsize
    pmove->movevars->stepsize = NS_GetStepsize(pmove->iuser3);
}

void PM_ReduceTimers( void )
{
    if ( pmove->flTimeStepSound > 0 )
    {
        pmove->flTimeStepSound -= pmove->cmd.msec;
        if ( pmove->flTimeStepSound < 0 )
        {
            pmove->flTimeStepSound = 0;
        }
    }
    if ( pmove->flDuckTime > 0 )
    {
        pmove->flDuckTime -= pmove->cmd.msec;
        if ( pmove->flDuckTime < 0 )
        {
            pmove->flDuckTime = 0;
        }
    }
    if ( pmove->flSwimTime > 0 )
    {
        pmove->flSwimTime -= pmove->cmd.msec;
        if ( pmove->flSwimTime < 0 )
        {
            pmove->flSwimTime = 0;
        }
    }
}

qboolean PM_CanFlap()
{
    qboolean theCanFlap = false;
    
    if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3)
    {
        if((pmove->onground == -1) && !(pmove->oldbuttons & IN_JUMP))
        {
            if(AvHMUHasEnoughAlienEnergy(pmove->fuser3, kAlienEnergyFlap))
            {
                // Can't hold the button down
                theCanFlap = true;
            }
        }
    }
    return theCanFlap;
}

qboolean PM_CanGlide()
{
    
    if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3)
    {
        if((pmove->onground == -1))
        {
            return true;
        }
    }
            
    return false;

}


qboolean PM_CanWalljump()
{
    vec3_t front;
    pmtrace_t trace;
    qboolean theCanWalljump = false;

    if(pmove->iuser3 == AVH_USER3_ALIEN_PLAYER4)
    {
        front[0] = pmove->origin[0] + pmove->player_maxs[pmove->usehull][0] + 2;
        front[1] = pmove->origin[1] + pmove->player_maxs[pmove->usehull][1] + 2;
        front[2] = pmove->origin[2] + pmove->player_mins[pmove->usehull][2] + 1;

        trace = NS_PlayerTrace (pmove, pmove->origin, front, PM_WORLD_ONLY, -1 );
        
        if ( (trace.fraction < 1 ))
        {
            theCanWalljump = true;
        }
    }
    return theCanWalljump;
}


void PM_Overwatch()
{
    #ifdef AVH_CLIENT
    gOverwatchTargetRange = -1;
    #endif

    // If overwatch is on and we have a target
//  if(GetHasUpgrade(pmove->iuser4, MASK_MARINE_OVERWATCH))
//  {
//      // Get target
//      int thePhysIndex = (int)pmove->fuser1;
//      if(thePhysIndex != -1)
//      {
//          physent_t* theTarget = NULL;//AvHSUGetEntity(thePhysIndex);
//
//          //= &(pmove->physents[ thePhysIndex ]);
//          //physent_t* theTarget = gEngfuncs.pEventAPI->EV_GetPhysent(thePhysIndex);
//          
//          if(theTarget)
//          {
//              // TODO: Take gun offset into account with vecMid?
//              vec3_t vecMid;
//              vec3_t vecMidEnemy;
//              vec3_t vecDirToEnemy;
//              vec3_t vec;
//              
//              VectorAdd(pmove->origin, pmove->view_ofs, vecMid);
//              
//              VectorCopy(theTarget->origin, vecMidEnemy);//theTarget->BodyTarget( vecMid );
//              
//              // Right now just point at enemy
//              //UTIL_MakeVectors()
//              VectorSubtract(vecMidEnemy, vecMid, vecDirToEnemy);
//              // = UTIL_VecToAngles(vecDirToEnemy);
//              VectorAngles(vecDirToEnemy, vec);
//              
//              //vec.x = -vec.x;
//              vec[0] = -vec[0];
//              
//              //  if (vec.y > 360)
//              //      vec.y -= 360;
//              //  
//              //  if (vec.y < 0)
//              //      vec.y += 360;
//              
//
//              // Set new view angles, set iHasNewViewAngles so it isn't reset on us
//              VectorCopy(vec, pmove->angles);
//              
//              #ifdef AVH_CLIENT
//              VectorCopy(pmove->angles, gTopDownViewAngles);
//              iHasNewViewAngles = true;
//              #endif
//              
//              AngleVectors (pmove->angles, pmove->forward, pmove->right, pmove->up);
//              //VectorCopy(vec, this->pev->v_angle);
//              
//              // Save target range
//              #ifdef AVH_CLIENT
//              gOverwatchTargetRange = Length(vecDirToEnemy);
//              #endif
//              pmove->fuser2 = Length(vecDirToEnemy)/100.0f;
//          }
//      }
//  }
}

bool PM_TopDown()
{
    bool theInTopDownMode = false;

    if(GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        // Reset this, it seems to be getting set by the engine
        //pmove->movetype = MOVETYPE_FLY;

        pmove->forward[0] = pmove->forward[1] = 0;
        pmove->forward[2] = -1;
        
        pmove->right[0] = 1;
        pmove->right[1] = pmove->right[2] = 0;

        pmove->up[0] = pmove->up[2] = 0;
        pmove->up[1] = 1;

        theInTopDownMode = true;

        const AvHMapExtents& theMapExtents = GetMapExtents();
            
        float theMinX = theMapExtents.GetMinMapX();
        float theMinY = theMapExtents.GetMinMapY();
        float theMaxX = theMapExtents.GetMaxMapX();
        float theMaxY = theMapExtents.GetMaxMapY();
        float theMaxZ = theMapExtents.GetMaxViewHeight();
        float theMinZ = theMapExtents.GetMinViewHeight();

        // Modify the max to make sure commander stays inside the world
        vec3_t theStartPos;
        VectorCopy(pmove->origin, theStartPos);
        theStartPos[2] = theMaxZ;

        vec3_t theNewStartPos;
        vec3_t theEndPos;
        VectorCopy(pmove->origin, theEndPos);
        theEndPos[2] = theMinZ;

        float theMaxCommanderHeight = theMaxZ;

        AvHSHUGetFirstNonSolidPoint((float*)theStartPos, (float*)theEndPos, (float*)theNewStartPos);

        //theMaxZ = min(theMaxZ, theNewStartPos[2]);
        
//      #ifdef AVH_CLIENT
//      extern DebugPointListType gSquareDebugLocations;
//      DebugPoint theDebugPoint(theNewStartPos[0], theNewStartPos[1], theMaxZ - theNewStartPos[2]);
//      gSquareDebugLocations.push_back(theDebugPoint);
//      #endif
        
        float       speed, drop, friction, control, newspeed;
        float       currentspeed, addspeed, accelspeed;
        int         i;
        vec3_t      wishvel;
        vec3_t      theAngles;
        float       fmove = 0.0f;
        float       smove = 0.0f;
        float       umove = 0.0f;
        vec3_t      wishdir;
        float       wishspeed;
        
        // Don't scroll when COMMANDER_MOUSECOORD set, it's indicating a world position, not a move
        //if(pmove->cmd.impulse != COMMANDER_MOUSECOORD)
        //{
//          #ifdef AVH_CLIENT
//          if ( pmove->runfuncs )
//          {
//              // Set spectator flag
//              iIsSpectator = SPEC_IS_SPECTATOR;
//          }
//          #endif

            // Move around in normal spectator method
            // friction
            speed = Length (pmove->velocity);
            if (speed < 1)
            {
                VectorCopy (vec3_origin, pmove->velocity)
            }
            else
            {
                drop = 0;
                
                friction = pmove->movevars->friction*5.0;   // extra friction
                //friction = pmove->movevars->friction*.9f;
                control = speed < pmove->movevars->stopspeed ? pmove->movevars->stopspeed : speed;
                drop += control*friction*pmove->frametime;
                
                // scale the velocity
                newspeed = speed - drop;
                if (newspeed < 0)
                    newspeed = 0;
                newspeed /= speed;

                VectorScale (pmove->velocity, newspeed, pmove->velocity);
            }
            
            // If player moved mouse wheel, move camera up and down if we didn't just move over a view entity
            qboolean theFoundEntity = false;
            //float theDesiredHeight = PM_GetDesiredTopDownCameraHeight(theFoundEntity);
            
//          gHeightLevel += (pmove->cmd.forwardmove/300.0f);
//          float theDesiredHeight = theMaxViewHeight + gHeightLevel;
//
//          // Note: To have a nice smooth zoom-up effect, comment out this next line, but you'll have to
//          // fix the bouncing from the drop/control/friction code above.  That's where the problem is.
//          // You'll also have to add an initial upwards velocity in AvHPlayer::StartTopDown()
//          pmove->origin[2] = theDesiredHeight;
//          
//          const float kSwoopingTolerance = 1.0f;
//          float theDiff = theDesiredHeight - pmove->origin[2];
//          float theFabsDiff = fabs(theDiff);
//          
//          if(theFabsDiff > kSwoopingTolerance)
//          {
//              // When we get near, don't use sudden moves, move slowly
//              float theAmount = min(theFabsDiff*10, 1000);
//              
//              //fmove = theDiff;//pmove->cmd.forwardmove;
//              if(theDiff > 0)
//                  fmove = -theAmount;
//              else
//                  fmove = theAmount;
//          }
//          else
//          {
//              fmove = 0.0f;
//          }
            
            // accelerate
            fmove = pmove->cmd.forwardmove;
            smove = pmove->cmd.sidemove;
            umove = pmove->cmd.upmove;
            
            float theMoveTotal = fabs(fmove) + fabs(smove) + fabs(umove);
            if(theMoveTotal > 5.0f)
            {
//              VectorNormalize (pmove->forward);
//              VectorNormalize (pmove->right);
//              VectorNormalize (pmove->up);
//              
//              for (i=0 ; i<3 ; i++)
//              {
//                  wishvel[i] = pmove->forward[i]*fmove + pmove->right[i]*smove + pmove->up[i]*umove;
//              }
//              //wishvel[2] += pmove->cmd.upmove;
//              
                PM_GetWishVelocity(wishvel);
                VectorCopy (wishvel, wishdir);

                wishspeed = VectorNormalize(wishdir);
                
                //
                // clamp to server defined max speed
                //
                if (wishspeed > pmove->movevars->maxspeed)
                {
                    VectorScale (wishvel, pmove->movevars->maxspeed/wishspeed, wishvel);
                    wishspeed = pmove->movevars->maxspeed;
                }
                
                currentspeed = DotProduct(pmove->velocity, wishdir);
                addspeed = wishspeed - currentspeed;
                if (addspeed > 0)
                {
                    accelspeed = pmove->movevars->accelerate*pmove->frametime*wishspeed;
                    if (accelspeed > addspeed)
                        accelspeed = addspeed;
                    
                    for (i=0 ; i<3 ; i++)
                        pmove->velocity[i] += accelspeed*wishdir[i];    
                    
                    // move
                    VectorMA (pmove->origin, pmove->frametime, pmove->velocity, pmove->origin);
                }
            }
            else
            {
                wishvel[0] = wishvel[1] = wishvel[2] = 0.0f;
                wishdir[0] = wishdir[1] = wishdir[2] = 0.0f;
                pmove->velocity[2] = 0.0f;
            }
        //}
    
        // Set view down
        theAngles[0] = kTopDownYaw;
        theAngles[1] = kTopDownPitch;
        theAngles[2] = kTopDownRoll;
        
        // Set angles facing down so observer knows which way to point
        VectorCopy(theAngles, pmove->angles);
        
        // Set origin
        //pmove->origin[2] = 1080;
        //pmove->origin[2] = PM_GetDesiredTopDownCameraHeight();
        
        #ifdef AVH_CLIENT
        if ( pmove->runfuncs )
        {
            VectorCopy(theAngles, gTopDownViewAngles);
            //iHasNewViewAngles = true;

            // Set view origin to our real origin but at our highest map extents.  This is needed to the commander's origin is actually inside the world so 
            // he receives nearby events, but so he looks like he's outside the world. Changing this?  Make sure AvHPlayer::GetVisualOrigin() is updated also.
            VectorCopy(pmove->origin, gTopDownViewOrigin);
            gTopDownViewOrigin[2] = theMaxCommanderHeight;

            //iHasNewViewOrigin = true;
        }
        #endif
    
        AngleVectors (pmove->angles, pmove->forward, pmove->right, pmove->up);
        
        // Are we zooming to an area?
        if(pmove->cmd.impulse == COMMANDER_MOVETO)
        {
            pmove->origin[0] = pmove->cmd.upmove*kWorldPosNetworkConstant;
            pmove->origin[1] = pmove->cmd.sidemove*kWorldPosNetworkConstant;
            VectorCopy(vec3_origin, pmove->velocity)
        }
        
        // Clip position to map extents
        float theCurrentX = pmove->origin[0];
        float theCurrentY = pmove->origin[1];
        //float theCurrentZ = pmove->origin[2];

        pmove->origin[0] = min(max(theMinX, theCurrentX), theMaxX);
        pmove->origin[1] = min(max(theMinY, theCurrentY), theMaxY);
        pmove->origin[2] = min(max(theMinZ, theNewStartPos[2]), theMaxZ);

        if(pmove->runfuncs)
        {
            // Update view offset
            #ifdef AVH_CLIENT
            // Save our top down height so we can offset camera in view.cpp
            // Changing this?  Make sure AvHPlayer::GetVisualOrigin() is updated also.
            //gTopDownHeight = theMaxCommanderHeight;
            #endif
            
            pmove->view_ofs[2] = theMaxCommanderHeight - pmove->origin[2];

            //pmove->Con_Printf("PMTopDown(): up: %f, side: %f, forward: %f, impulse: %d, velocity: %f\n", pmove->cmd.upmove, pmove->cmd.sidemove, pmove->cmd.forwardmove, pmove->cmd.impulse, Length(pmove->velocity));
        }
    }
    
    // Reset view
    //  else if(pmove->iuser3 == AVH_USER3_VIEW_SPECIAL_LEAVE_COMMANDER)
    //  {
    //      vec3_t      theAngles;
    //      
    //      // Set view down
    //      theAngles[0] = 0;
    //      theAngles[1] = 0;
    //      theAngles[2] = 0;
    //      
    //      // Set angles facing down so observer knows which way to point
    //      VectorCopy(theAngles, pmove->angles);
    //      
    //      #ifdef AVH_CLIENT
    //      if ( pmove->runfuncs )
    //      {
    //          VectorCopy(theAngles, gTopDownViewAngles);
    //          iHasNewViewAngles = true;
    //      }
    //      #endif
    //      
    //      AngleVectors (pmove->angles, pmove->forward, pmove->right, pmove->up);
    //
    //      #ifdef AVH_CLIENT
    //      if ( pmove->runfuncs )
    //      {
    //          iHasNewViewAngles = true;
    //          pmove->iuser3 = 0;
    //      }
    //      #endif
    //
    //      #ifdef AVH_SERVER
    //      pmove->iuser3 = 0;
    //      #endif
    //  }
    return theInTopDownMode;
}

void PM_Jetpack()
{
    bool theHasJetpackUpgrade = GetHasUpgrade(pmove->iuser4, MASK_UPGRADE_7) && (pmove->iuser3 == AVH_USER3_MARINE_PLAYER);
    
    // Turn off jetpack by default
    gIsJetpacking[pmove->player_index] = false;
    
    if(!pmove->dead && theHasJetpackUpgrade)
    {
        bool theJumpHeldDown = (pmove->cmd.buttons & IN_JUMP);
        bool theHasEnoughEnergy = (pmove->fuser3 > (kJetpackMinimumEnergyToJetpack*kNormalizationNetworkFactor));
        float theTimePassed = pmove->frametime;

        // If jump is held down and we have a jetpack, apply upwards force! - added check for digestion - elven
        if(theJumpHeldDown && theHasEnoughEnergy && !GetHasUpgrade(pmove->iuser4, MASK_ENSNARED) && !GetHasUpgrade(pmove->iuser4, MASK_PLAYER_STUNNED) && !GetHasUpgrade(pmove->iuser4, MASK_DIGESTING))
        {
            gIsJetpacking[pmove->player_index] = true;
            
            // Apply upwards force to player
            //pmove->velocity[2] += kJetpackForce;
            
            vec3_t theWishVelocity;
            PM_GetWishVelocity(theWishVelocity);

            const float kBaseScalar = .6f;

            int theMinMarineSpeed = BALANCE_VAR(kBasePlayerSpeed);
            if(theMinMarineSpeed == 0)
            {
                theMinMarineSpeed = 150;
            }

            int theMaxMarineSpeed = BALANCE_VAR(kUnencumberedPlayerSpeed);
            if(theMaxMarineSpeed == 0)
            {
                theMaxMarineSpeed = 220;
            }

            float theWeightScalar = kBaseScalar + (1.0f - kBaseScalar)*((pmove->clientmaxspeed - theMinMarineSpeed)/(theMaxMarineSpeed - theMinMarineSpeed));
            
            pmove->velocity[0] += (theWishVelocity[0]/pmove->clientmaxspeed)*kJetpackLateralScalar;
            pmove->velocity[1] += (theWishVelocity[1]/pmove->clientmaxspeed)*kJetpackLateralScalar;
            pmove->velocity[2] += theTimePassed*theWeightScalar*kJetpackForce;

            // Play an event every so often
            if(pmove->runfuncs /*&& (pmove->RandomLong(0, 2) == 0)*/)
            {
                pmove->PM_PlaybackEventFull(0, pmove->player_index, gJetpackEventID, 0, (float *)pmove->origin, (float *)pmove->origin, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
            }
        }

        float theJetpackEnergy = pmove->fuser3/kNormalizationNetworkFactor;

        AvHMUUpdateJetpackEnergy(gIsJetpacking[pmove->player_index], theTimePassed, theJetpackEnergy);

        pmove->fuser3 = theJetpackEnergy*kNormalizationNetworkFactor;
    }

    //pmove->Con_Printf("Jetpacking: %d\n", gIsJetpacking);
}

/*
=============
PlayerMove

Returns with origin, angles, and velocity modified in place.

Numtouch and touchindex[] will be set if any of the physents
were contacted during the move.
=============
*/
void PM_PlayerMove ( qboolean server )
{
    physent_t *pLadder = NULL;

    // Are we running server code?
    pmove->server = server;                

    if (pmove->cmd.buttons & IN_JUMP)
    {
        int a =0;
    }
    
    int theMaxSpeed = 0;

    // Adjust speeds etc.
    PM_CheckParamters();

    // Assume we don't touch anything
    pmove->numtouch = 0;                    

    // # of msec to apply movement
    pmove->frametime = pmove->cmd.msec * 0.001;    

//  if(pmove->runfuncs)
//  {
//      pmove->Con_Printf("pmove->frametime: %f\n", pmove->frametime);
//  }

    PM_ReduceTimers();

    bool theIsGestating = GetHasUpgrade(pmove->iuser4, MASK_ALIEN_EMBRYO);
    bool theIsParalyzed = GetHasUpgrade(pmove->iuser4, MASK_PLAYER_STUNNED);

    if(!theIsParalyzed && !theIsGestating)
    {
        // Convert view angles to vectors
        AngleVectors (pmove->angles, pmove->forward, pmove->right, pmove->up);
    }

    //if(pmove->cmd.impulse == COMMANDER_MOUSECOORD)
//  if(pmove->iuser3 == AVH_USER3_VIS_SPECIAL_TOPDOWN)
//  {
//      if((pmove->cmd.impulse != COMMANDER_MOVETO) && (pmove->cmd.impulse != MESSAGE_NULL))
//      {
//          return;
//      }
//  }
    
    bool theIsDucking = AvHMUGetCanDuck(pmove->iuser3) && (pmove->flags & FL_DUCKING);
    pmove->usehull = AvHMUGetHull(theIsDucking, pmove->iuser3);

    //pmove->Con_Printf("Using hull: %d\n", pmove->usehull);

    //PM_ShowClipBox();

    // Clear movement when building, giving orders, or anything other than scrolling
    if(GetHasUpgrade(pmove->iuser4, MASK_TOPDOWN))
    {
        if((pmove->cmd.impulse != COMMANDER_SCROLL) && (pmove->cmd.impulse != COMMANDER_MOVETO))
        {
            pmove->cmd.sidemove = pmove->cmd.upmove = pmove->cmd.forwardmove = 0;
            pmove->cmd.buttons = 0;
        }
    }

    if ( pmove->flags & FL_FROZEN )
        return;

    // Special handling for spectator and observers. (iuser1 is set if the player's in observer mode)
    if ( pmove->spectator || pmove->iuser1 > 0 )
    {
        PM_SpectatorMove();
        PM_CategorizePosition();
        return;
    }

//  if(GetHasUpgrade(pmove->iuser4, MASK_MARINE_OVERWATCH))
//  {
//      PM_Overwatch();
//  }

    PM_TopDown();

    // Paralyzed players and embryos can't move
    if(theIsParalyzed || theIsGestating)
    {
        PM_Physics_Toss();
        return;
    }
    
    // Always try and unstick us unless we are in NOCLIP mode
    if ( pmove->movetype != MOVETYPE_NOCLIP && pmove->movetype != MOVETYPE_NONE )
    {
        if ( PM_CheckStuck() )
        {
            return;  // Can't move, we're stuck
        }
    }

    // Now that we are "unstuck", see where we are ( waterlevel and type, pmove->onground ).
    PM_CategorizePosition();

    // Store off the starting water level
    pmove->oldwaterlevel = pmove->waterlevel;

    // If we are not on ground, store off how fast we are moving down
    if ( pmove->onground == -1 )
    {
        pmove->flFallVelocity = -pmove->velocity[2];
    }

	g_onladder[pmove->player_index] = 0;

    PM_Jetpack();

    PM_AlienAbilities();

    NS_UpdateWallsticking();

    // Don't run ladder code if dead or on a train, or a lerk or a skulk
    bool theIsFlyingAlien = (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER3) && (pmove->onground == -1);
    bool theIsSkulk = (pmove->iuser3 == AVH_USER3_ALIEN_PLAYER1);

    if ( !pmove->dead && !(pmove->flags & FL_ONTRAIN) && !gIsJetpacking[pmove->player_index] && !GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING) && !theIsFlyingAlien && !theIsSkulk)
    {
        pLadder = PM_Ladder();
        if ( pLadder )
        {
            g_onladder[pmove->player_index] = 1;
        }
    }

    //pmove->Con_DPrintf("g_onladder: %d\n", g_onladder);
    
    PM_UpdateStepSound();

    PM_Duck();

    // Don't run ladder code if dead or on a train
    if ( !pmove->dead && !(pmove->flags & FL_ONTRAIN) && !gIsJetpacking[pmove->player_index] && !GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
    {
        if ( pLadder )
        {
            PM_LadderMove( pLadder );
        }
        else if ( pmove->movetype != MOVETYPE_WALK &&
                  pmove->movetype != MOVETYPE_NOCLIP )
        {
            // Clear ladder stuff unless player is noclipping
            //  it will be set immediately again next frame if necessary
            pmove->movetype = MOVETYPE_WALK;
        }
    }

    // Slow down, I'm pulling it! (a box maybe) but only when I'm standing on ground
    if ( ( pmove->onground != -1 ) && ( pmove->cmd.buttons & IN_USE) )
    {
        VectorScale( pmove->velocity, 0.3, pmove->velocity );
    }

    // Reset gravity to 1.0, in case we're not gliding anymore.  This will get changed
    // in PM_Jump if player is still holding down the key.
    // This assumes only players in here!  Sounds problematic to me.
//  if(pmove->iuser3 == AVH_USER3_GLIDE)
//  {
//      pmove->gravity = 1.0f;
//  }

    // Handle movement
    switch ( pmove->movetype )
    {
    default:
        pmove->Con_Printf("Bogus pmove player movetype %i on (%i) 0=cl 1=sv\n", pmove->movetype, pmove->server);
        break;

    case MOVETYPE_NONE:
        break;

    case MOVETYPE_NOCLIP:
        PM_NoClip();
        break;

    case MOVETYPE_TOSS:
    case MOVETYPE_BOUNCE:
        PM_Physics_Toss();
        break;

    case MOVETYPE_FLY:
    
        PM_CheckWater();

        // Was jump button pressed?
        // If so, set velocity to 270 away from ladder.  This is currently wrong.
        // Also, set MOVE_TYPE to walk, too.
        if ( pmove->cmd.buttons & IN_JUMP)
        {
            if ( !pLadder )
            {
                PM_Jump ();
            }
        }
        else
        {
            pmove->oldbuttons &= ~IN_JUMP;
        }
        
        // Perform the move accounting for any base velocity.
        VectorAdd (pmove->velocity, pmove->basevelocity, pmove->velocity);
        PM_FlyMove ();
        VectorSubtract (pmove->velocity, pmove->basevelocity, pmove->velocity);
        break;

    case MOVETYPE_WALK:
        if ( !PM_InWater() )
        {
            PM_AddCorrectGravity();
        }

        // If we are leaping out of the water, just update the counters.
        if ( pmove->waterjumptime )
        {
            PM_WaterJump();
            PM_FlyMove();

            // Make sure waterlevel is set correctly
            PM_CheckWater();
            return;
        }

        // If we are swimming in the water, see if we are nudging against a place we can jump up out
        //  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0
        if ( pmove->waterlevel >= 2 ) 
        {
            if ( pmove->waterlevel == 2 )
            {
                PM_CheckWaterJump();
            }

            // If we are falling again, then we must not trying to jump out of water any more.
            if ( pmove->velocity[2] < 0 && pmove->waterjumptime )
            {
                pmove->waterjumptime = 0;
            }

            // Was jump button pressed?
            if (pmove->cmd.buttons & IN_JUMP)
            {
                PM_Jump ();
            }
            else
            {
                pmove->oldbuttons &= ~IN_JUMP;
            }

            // Perform regular water movement
            PM_WaterMove();
            
            VectorSubtract (pmove->velocity, pmove->basevelocity, pmove->velocity);

            // Get a final position
            PM_CategorizePosition();
        }
        else

        // Not underwater
        {

            // Was jump button pressed?
            if ( pmove->cmd.buttons & IN_JUMP )
            {
                if ( !pLadder )
                {
                    PM_Jump ();
                }
            }
            else
            {
                pmove->oldbuttons &= ~IN_JUMP;
            }

            // Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
            //  we don't slow when standing still, relative to the conveyor.
            if ((pmove->onground != -1) || GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
            {
                if(!GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
                    pmove->velocity[2] = 0.0;
                PM_Friction();
            }

            // Make sure velocity is valid.
            PM_CheckVelocity();

            // Are we on ground now
            if ( (pmove->onground != -1) || GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
            {
                PM_WalkMove();
            }
            else
            {
                PM_AirMove();  // Take into account movement when in air.
            }
            
            // Set final flags.
            PM_CategorizePosition();

            //float theVelocityLength = Length(pmove->velocity);
            //pmove->Con_Printf("Max speed: %f, velocity: %f\n", pmove->maxspeed, theVelocityLength);

            // Now pull the base velocity back out.
            // Base velocity is set if you are on a moving object, like
            //  a conveyor (or maybe another monster?)
            VectorSubtract (pmove->velocity, pmove->basevelocity, pmove->velocity );
                
            // Make sure velocity is valid.
            PM_CheckVelocity();

            // Add any remaining gravitational component.
            if ( !PM_InWater() )
            {
                PM_FixupGravityVelocity();
            }

            // If we are on ground, no downward velocity.
            if((pmove->onground != -1) && !GetHasUpgrade(pmove->iuser4, MASK_WALLSTICKING))
            {
                pmove->velocity[2] = 0;
            }

            // See if we landed on the ground with enough force to play
            //  a landing sound.
            PM_CheckFalling();
        }
            

        // Did we enter or leave the water?
        PM_PlayWaterSounds();
        break;
    }
}

void PM_CreateStuckTable( void )
{
    float x, y, z;
    int idx;
    int i;
    float zi[3];

    memset(rgv3tStuckTable, 0, 54 * sizeof(vec3_t));

    idx = 0;
    // Little Moves.
    x = y = 0;
    // Z moves
    for (z = -0.125 ; z <= 0.125 ; z += 0.125)
    {
        rgv3tStuckTable[idx][0] = x;
        rgv3tStuckTable[idx][1] = y;
        rgv3tStuckTable[idx][2] = z;
        idx++;
    }
    x = z = 0;
    // Y moves
    for (y = -0.125 ; y <= 0.125 ; y += 0.125)
    {
        rgv3tStuckTable[idx][0] = x;
        rgv3tStuckTable[idx][1] = y;
        rgv3tStuckTable[idx][2] = z;
        idx++;
    }
    y = z = 0;
    // X moves
    for (x = -0.125 ; x <= 0.125 ; x += 0.125)
    {
        rgv3tStuckTable[idx][0] = x;
        rgv3tStuckTable[idx][1] = y;
        rgv3tStuckTable[idx][2] = z;
        idx++;
    }

    // Remaining multi axis nudges.
    for ( x = - 0.125; x <= 0.125; x += 0.250 )
    {
        for ( y = - 0.125; y <= 0.125; y += 0.250 )
        {
            for ( z = - 0.125; z <= 0.125; z += 0.250 )
            {
                rgv3tStuckTable[idx][0] = x;
                rgv3tStuckTable[idx][1] = y;
                rgv3tStuckTable[idx][2] = z;
                idx++;
            }
        }
    }

    // Big Moves.
    x = y = 0;
    zi[0] = 0.0f;
    zi[1] = 1.0f;
    zi[2] = 6.0f;

    for (i = 0; i < 3; i++)
    {
        // Z moves
        z = zi[i];
        rgv3tStuckTable[idx][0] = x;
        rgv3tStuckTable[idx][1] = y;
        rgv3tStuckTable[idx][2] = z;
        idx++;
    }

    x = z = 0;

    // Y moves
    for (y = -2.0f ; y <= 2.0f ; y += 2.0)
    {
        rgv3tStuckTable[idx][0] = x;
        rgv3tStuckTable[idx][1] = y;
        rgv3tStuckTable[idx][2] = z;
        idx++;
    }
    y = z = 0;
    // X moves
    for (x = -2.0f ; x <= 2.0f ; x += 2.0f)
    {
        rgv3tStuckTable[idx][0] = x;
        rgv3tStuckTable[idx][1] = y;
        rgv3tStuckTable[idx][2] = z;
        idx++;
    }

    // Remaining multi axis nudges.
    for (i = 0 ; i < 3; i++)
    {
        z = zi[i];
        
        for (x = -2.0f ; x <= 2.0f ; x += 2.0f)
        {
            for (y = -2.0f ; y <= 2.0f ; y += 2.0)
            {
                rgv3tStuckTable[idx][0] = x;
                rgv3tStuckTable[idx][1] = y;
                rgv3tStuckTable[idx][2] = z;
                idx++;
            }
        }
    }
}




/*
This modume implements the shared player physics code between any particular game and 
the engine.  The same PM_Move routine is built into the game .dll and the client .dll and is
invoked by each side as appropriate.  There should be no distinction, internally, between server
and client.  This will ensure that prediction behaves appropriately.
*/

void PM_Move ( struct playermove_s *ppmove, int server )
{
    assert( pm_shared_initialized );

    pmove = ppmove;

    PM_SetHulls();

    PM_PlayerMove( ( server != 0 ) ? true : false );

    if ( pmove->onground != -1 )
    {
        pmove->flags |= FL_ONGROUND;
    }
    else
    {
        pmove->flags &= ~FL_ONGROUND;
    }

    // In single player, reset friction after each movement to FrictionModifier Triggers work still.
    if ( !pmove->multiplayer && ( pmove->movetype == MOVETYPE_WALK  ) )
    {
        pmove->friction = 1.0f;
    }

    //int theRandomNumber = pmove->RandomLong(0, 100);
    //  
    //#ifdef AVH_CLIENT
    //if(pmove->runfuncs)
    //{
    //  PM_DebugLocations(theRandomNumber);
    //}
    //#endif

    // Remember most predicted player origin (only if runfuncs?)
    if(pmove->runfuncs)
    {
        VectorCopy(pmove->origin, gPredictedPlayerOrigin);
        VectorCopy(pmove->view_ofs, gPredictedPlayerVOfs);
    }

    ///////////////////////////////
    // Begin Max's Code
    ///////////////////////////////     

    // If the current orientation is different than the desired orientation,
    // interpolate between them.

    vec3_t currentAngles;
    vec3_t targetAngles;

    #ifdef AVH_SERVER

    NS_FixWallstickingAngles(pmove->vuser1);
    NS_FixWallstickingAngles(pmove->vuser2);
    
    VectorCopy(pmove->vuser1, currentAngles);
    VectorCopy(pmove->vuser2, targetAngles);

    #endif

    #ifdef AVH_CLIENT

    NS_FixWallstickingAngles(gPlayerAngles);
    NS_FixWallstickingAngles(gTargetPlayerAngles);
    
    VectorCopy(gPlayerAngles, currentAngles);
    VectorCopy(gTargetPlayerAngles, targetAngles);
    
    #endif

    if (VectorCompare(currentAngles, targetAngles) == 0)
    {

        Quat src = Quat(currentAngles).Unit();
        Quat dst = Quat(targetAngles).Unit();
    
        Quat rot = (dst * src.Conjugate()).Unit();

        // Compute the axis and angle we need to rotate about to go from src
        // to dst.

        float angle = acosf(rot.w) * 2;
        float sinAngle = sqrtf(1.0f - rot.w * rot.w);
        
        if (fabs(sinAngle) < 0.0005f)
        {
            sinAngle = 1;
        }

        vec3_t axis;

        axis[0] = rot.x / sinAngle;
        axis[1] = rot.y / sinAngle;
        axis[2] = rot.z / sinAngle;

        // Wrap the angle to the range -PI to PI
        angle = WrapFloat(angle, -M_PI, M_PI);

        // Amount to rotate this frame.
        float frameAngle = kSkulkRotationRate * pmove->frametime;

        if (fabs(angle) <= frameAngle)
        {
            // If we are very close, just jump to the goal orientation.
            VectorCopy(targetAngles, currentAngles);
        }
        else
        {

            Quat final;

            if (angle < 0)
            {
                final = Quat(-frameAngle, axis) * src;
            }
            else
            {
                final = Quat(frameAngle, axis) * src;
            }

            vec3_t xAxis;
            vec3_t yAxis;
            vec3_t zAxis;
            
            final.GetVectors(xAxis, yAxis, zAxis);

            VectorsToAngles(yAxis, xAxis, zAxis, currentAngles);

        }

    }

    #ifdef AVH_SERVER
    VectorCopy(currentAngles, pmove->vuser1);
    #endif
    
    #ifdef AVH_CLIENT
    VectorCopy(currentAngles, gPlayerAngles);
    #endif

    ///////////////////////////////
    // End Max's Code
    /////////////////////////////// 

}

//void PM_GetEntityList(PhysEntListType& outList)
//{
//  physent_t* theTarget = NULL;
//  
//  for (i = 0; i < MAX_PHYSENTS; i++)
//  {
//      theTarget = pmove->physents[i];
//      outList.push_back(theTarget);
//  }
//
//}


int PM_GetVisEntInfo( int ent )
{
    if ( ent >= 0 && ent <= pmove->numvisent )
    {
        return pmove->visents[ ent ].info;
    }
    return -1;
}

int PM_GetPhysEntInfo( int ent )
{
    if ( ent >= 0 && ent <= pmove->numphysent)
    {
        return pmove->physents[ ent ].info;
    }
    return -1;
}

void PM_Init( struct playermove_s *ppmove )
{
    assert( !pm_shared_initialized );
    
    pmove = ppmove;
    
    PM_CreateStuckTable();
    PM_InitTextureTypes();
    PM_InitBoxHull();
    
    pm_shared_initialized = 1;
}


bool PM_ViewTraceEntity(float inOriginX, float inOriginY, float inOriginZ, float inDirX, float inDirY, float inDirZ, int& outIndex)
{
    bool theSuccess = false;

    vec3_t theTraceEnd;
    vec3_t theNormal;
    theNormal[0] = inDirX;
    theNormal[1] = inDirY;
    theNormal[2] = inDirZ;
    VectorScale(theNormal, 8012, theTraceEnd);

    vec3_t theTraceStart;
    theTraceStart[0] = inOriginX;
    theTraceStart[1]= inOriginY;
    theTraceStart[2] = inOriginZ;
    
    pmtrace_t* theTrace = NULL;
    bool theDone = false;
    
    do
    {
        theTrace = pmove->PM_TraceLine(theTraceStart, theTraceEnd, PM_TRACELINE_ANYVISIBLE, 2,  -1);

        if(!theTrace || theTrace->fraction < 0.0001f || theTrace->ent > 0)
        {
            theDone = true;
        }
        else
        {
            // Trace again from here
            VectorCopy(theTrace->endpos, theTraceStart);
        }

    } while(!theDone);

    if(theTrace && (theTrace->ent > 0))
    {
        outIndex = theTrace->ent;
        theSuccess = true;
    }
    return theSuccess;
}

bool GetIsEntityAPlayer(int inPhysIndex)
{
    bool theEntIsPlayer = false;

    physent_t* theEntity = NULL;
    if((inPhysIndex > 0) && (inPhysIndex <= pmove->numphysent))
    {
        theEntity = pmove->physents + inPhysIndex;
    }

    if(theEntity)
    {
        const char* theName = theEntity->name;
        if(!strcmp(theName, kAvHPlayerClassName))
        {
            theEntIsPlayer = true;
        }
    }

    return theEntIsPlayer;
}


bool PM_ViewTracePlayer(float inOriginX, float inOriginY, float inOriginZ, float inEndX, float inEndY, float inEndZ, int& outIndex)
{
    bool theSuccess = false;

    #ifdef AVH_CLIENT
    //gSquareDebugLocations.clear();
    #endif

    vec3_t theTraceEnd;
    //vec3_t theNormal;
    //theNormal[0] = inDirX;
    //theNormal[1] = inDirY;
    //theNormal[2] = inDirZ;
    //VectorScale(theNormal, 8012, theTraceEnd);
    theTraceEnd[0] = inEndX;
    theTraceEnd[1] = inEndY;
    theTraceEnd[2] = inEndZ;
    
    vec3_t theTraceStart;
    theTraceStart[0] = inOriginX;
    theTraceStart[1]= inOriginY;
    theTraceStart[2] = inOriginZ;
    
    #ifdef AVH_CLIENT
    DebugPoint thePoint;
    thePoint.x = theTraceStart[0];
    thePoint.y = theTraceStart[1];
    thePoint.z = theTraceStart[2];
    //gSquareDebugLocations.push_back(thePoint);

    thePoint.x = theTraceEnd[0];
    thePoint.y = theTraceEnd[1];
    thePoint.z = theTraceEnd[2];
    //gSquareDebugLocations.push_back(thePoint);
    #endif  
    
//  pmtrace_t* theTrace = NULL;
//  bool theDone = false;
    
//  do
//  {
        pmtrace_t theTraceStruct;

        theTraceStruct = NS_PlayerTrace(pmove, theTraceStart, theTraceEnd, PM_NORMAL, -1);
        int theEntIndex = theTraceStruct.ent;

//      //pmtrace_t     (*PM_PlayerTrace) (vec3_t start, vec3_t end, int traceFlags, int ignore_pe );
//      //theTrace = pmove->PM_TraceLine(theTraceStart, theTraceEnd, PM_TRACELINE_ANYVISIBLE, 2,  -1);
//      bool theEntityIsAPlayer = false;
//      
//      if(!theTrace || theTrace->fraction < 0.0001f || (theEntityIsAPlayer = GetIsEntityAPlayer(theTrace->ent)))
//      {
//          theDone = true;
//      }
//      else
//      {
//          // Trace again from here
//          VectorCopy(theTrace->endpos, theTraceStart);
//      }

        //if(!theDone || theEntityIsAPlayer)
        if(theEntIndex > 0)
        {
            #ifdef AVH_CLIENT
            DebugPoint thePoint;
            thePoint.x = theTraceStruct.endpos[0];
            thePoint.y = theTraceStruct.endpos[1];
            thePoint.z = theTraceStruct.endpos[2];
            //gSquareDebugLocations.push_back(thePoint);
            #endif
        }
        
//  } while(!theDone);
    
    if(theEntIndex > 0)
    //if(theTrace && (theTrace->ent > 0))
    {
        outIndex = theEntIndex;
        //outIndex = theTrace->ent;
        theSuccess = true;
    }
    return theSuccess;
}



