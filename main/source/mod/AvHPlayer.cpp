//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHPlayer.cpp $
// $Date: 2002/11/22 21:18:24 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPlayer.cpp,v $
// Revision 1.86  2002/11/22 21:18:24  Flayra
// - Potentially fixed strange Onos collision crash
// - Don't allow player to join team after he's seen another team
// - "lastinv" support
// - Fixed perma-cloak bug after losing last sensory chamber whilst cloaked
// - Started fixing commander PAS problem
// - Fixed readyroom "ghost player" exploit when F4 during REIN
// - Draw damage in debug, never otherwise
//
// Revision 1.85  2002/11/15 04:42:50  Flayra
// - Regenerate now returns true if healing was successful
// - Logging changes and fixes
//
// Revision 1.84  2002/11/13 01:49:08  Flayra
// - Proper death message logging for Psychostats
//
// Revision 1.83  2002/11/12 22:39:25  Flayra
// - Logging changes for Psychostats compatibility
//
// Revision 1.82  2002/11/12 18:44:54  Flayra
// - Added mp_logdetail support for damage messages
// - Changed the alien ability anti-exploit code to try to co-exist with scripters
//
// Revision 1.81  2002/11/12 02:28:39  Flayra
// - Fixed problems with armor not being updated when armor upgrades completed
// - Aliens now keep same percentage of health and armor when morphing
// - Much better logging, up to standard
// - Don't add enemy buildings to hive sight unless parasited (solves blip spam)
// - Removed draw damage from public build
// - Changes to minimap to less overflows at end of big games
//
// Revision 1.80  2002/11/06 01:38:54  Flayra
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
// - Regeneration update
//
// Revision 1.79  2002/11/05 06:17:26  Flayra
// - Balance changes
//
// Revision 1.78  2002/10/28 20:36:18  Flayra
// - Updated auth mask
//
// Revision 1.77  2002/10/25 21:48:01  Flayra
// - Added more auth masks
// - Moved mSkin to pev->skin so playerclass could hold what used to be mPlayMode could be used to quickly perform culling, to try to prevent overflowage at the end of large games
//
// Revision 1.76  2002/10/24 21:40:02  Flayra
// - Reworked jetpack effect
// - Authicons update
// - Set builder for alien buildings, so turret kills can credit builder
// - Alien easter eggs
// - Network optimizations after game reset on huge (20-32) player games
// - Allow server ops to disable auth icons
// - Alien energy tweaks
// - Show unbuilt hives in hive sight
// - Move alien energy updating fully into shared code
// - Tried to fix full health ring showing for dead selected players
// - Moved help text client-side
// - Cache info_locations and gamma until map change
// - Skin fixes
//
// Revision 1.75  2002/10/20 21:10:57  Flayra
// - Optimizations
//
// Revision 1.74  2002/10/20 16:36:37  Flayra
// - Code optimization!  Took forever to find.
//
// Revision 1.73  2002/10/20 02:36:14  Flayra
// - Regular update
//
// Revision 1.72  2002/10/19 22:33:44  Flayra
// - Various server optimizations
//
// Revision 1.71  2002/10/18 22:21:54  Flayra
// - Limit alien buildings in sphere
// - Fix various spawning problems (morphing as level 5, redemption for level 5)
// - Max motd length fix
//
// Revision 1.70  2002/10/17 17:34:09  Flayra
// - Authmask update
// - Part 1 of persistent weapons fix (found with Grendel)
//
// Revision 1.69  2002/10/16 20:55:40  Flayra
// - Debug code for tracking down death animation problems
// - Updated auth masks
//
// Revision 1.68  2002/10/16 01:05:38  Flayra
// - Sent health as short for big aliens
// - Fixed sayings not triggering commander alerts
// - Now name changes are queued until the next match
// - Added authmask support
// - Egg idle sounds play more frequently, refactored too
// - Fixed preserving model in ready room after game end
// - Profiling of AddToFullPack
// - Fix for falling through lifts when morphing on them (untested)
//
// Revision 1.67  2002/10/04 18:04:07  Flayra
// - Fixed floating gestation sacs
// - Aliens now fall all the way to ground during countdown (instead of floating and shaking)
//
// Revision 1.66  2002/10/03 20:24:39  Flayra
// - Changes for "more resources required"
//
// Revision 1.65  2002/10/03 19:32:06  Flayra
// - Reworked orders completely
// - Send max resources to players
// - Heavy armor sped up slightly
// - Kill players who illegally try to use alien abilities
// - Moved energy to a new variable, send health for aliens
// - Only freeze players during countdown
// - Removed slowdown when taking damage
// - Send blips in two messages, one friendly and one enemy (old bad hack)
//
// Revision 1.64  2002/09/25 20:50:03  Flayra
// - Added 3 new sayings
// - Frame-rate independent updating
// - Don't allow player to kill self while commanding
//
// Revision 1.63  2002/09/23 22:27:52  Flayra
// - Added skin support
// - Added client connected/disconnected hooks for particle system propagation optimizations
// - Removed power armor, added heavy armor
// - Fixed death animations
// - Added hook to see if commander has given an order and to see if he's idle
// - Bound resources for aliens
// - Soldiers asking for ammo and health trigger commander alert
// - Added gestation anims
// - Slowed down Onos movement
// - When cheats are enabled, purchases are free
//
// Revision 1.62  2002/09/09 20:04:53  Flayra
// - Added commander voting
// - Commander score is now the average of the rest of his players (reverted back when he leaves CC)
// - Fixed bug where upgrades were getting removed and then add repeatedly
// - Added multiple skins for marines
// - Play sound when aliens lose an upgrade
// - Changed fov to 90 for all aliens for software compatibility
// - Added hiveinfo drawing
// - Tweaked marine and alien speeds (to compensate for loss of drastic alien fov)
//
// Revision 1.61  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.60  2002/08/16 02:42:57  Flayra
// - New damage types
// - Much more efficient blip calculation (at least it should be, I haven't measured it so who really knows)
// - New way of representing ensnare state for shared code (disabling jumping, jetpacking)
// - Removed old overwatch code
// - Store health in fuser2 for drawing health for commander
// - Swap bile bomb and umbra
//
// Revision 1.59  2002/08/09 01:10:30  Flayra
// - Keep previous model when a game is over and going back to ready room
// - Refactoring for scoreboard
// - Support for "jump" animation
// - Freeze player before game starts
// - Reset score when leaving a team
//
// Revision 1.58  2002/08/02 21:52:18  Flayra
// - Cleaned up jetpacks, made webbing useful again, help messages, added "orderself" cheat, changed gestation messages for new tooltip system, added GetHasAvailableUpgrades() for help system, removed particle template messages, slowed down particle template update rate to reduce overflows (woo!)
//
// Revision 1.57  2002/07/28 19:21:28  Flayra
// - Balance changes after/during RC4a
//
// Revision 1.56  2002/07/26 23:07:53  Flayra
// - Numerical feedback
// - New artwork for marine, with jetpack as body group (this code doesn't work)
// - Misc. fixes (alien vision mode not being preserved when it should, and staying when it shouldn't)
//
// Revision 1.55  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.54  2002/07/23 17:17:57  Flayra
// - Added power armor, added "talking" state for hive sight, changes for new resource model, removed max buildings, create buildings with random orientation, added stimpack, tweaked redemption, added new hive sight info
//
// Revision 1.53  2002/07/10 14:44:39  Flayra
// - Draw chat text while dead (bug #280)
//
// Revision 1.52  2002/07/08 17:15:39  Flayra
// - Added validation of all impulses (assumes invalid by default, instead of valid), reset players like all other entities, ensnaring fixes, players are temporarily invulnerable when they spawn, preserve health and armor when using command station, fixed up redemption, add hooks for commander voting and going back to the ready room, models can't be changed via the console anymore
//
// Revision 1.51  2002/07/01 22:41:40  Flayra
// - Removed outdated overwatch target and tension events
//
// Revision 1.50  2002/07/01 21:43:16  Flayra
// - Removed outdated adrenaline concept, added new alien sight mode, primal scream, removed flashlight battery message, fixed morphing problems for level 5 (and others), tweaked marine and alien speeds, fixed triple speed upgrade problem, disabled overwatch, moved player assets out of precache() (wasn't being called reliably), get full energy after a lifeform change, hive sight only draws out of sight now, added scent of fear
//
// Revision 1.49  2002/06/25 18:13:57  Flayra
// - Added death animations, added more general animation support, leaps do damage, general construct effects, new alien inventory system, added charging, info_locations, galloping, new alien building code, better morphing system (prevents getting stuck), more builder error messages, clear deaths on game end, hide health while gestating
//
// Revision 1.48  2002/06/10 20:03:13  Flayra
// - Updated for new minimap (remember killed position).  Updated blood so marines aren't bloody, and aliens emit yellow blood.  Removed slowing when hit (now players fly back when hit though), UpdateBlips() hack (fix when time)
//
// Revision 1.47  2002/06/03 16:54:59  Flayra
// - Added more effective player classes for scoreboard, send player class every time role changes (more network usage, always updated), changed hive sight to always be visible when under attack, all entities added to hive sight, not just players
//
// Revision 1.46  2002/05/28 18:03:40  Flayra
// - Refactored size for role code for movement chambers, deal with inventory properly (entity leak), increased web ensnaring effects (put weapon away!), reinforcement refactoring, tweaked speeds so marines are a little slower, and speed upgrade works properly again (now level 1 can generally outrun marines), added recycling support, play destroy egg sound when killed when morphing, new hive sight, EffectivePlayerClassChanged() refactoring
//
// Revision 1.45  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHMessage.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHEntities.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMarineWeapons.h"
#include "dlls/client.h"
#include "dlls/util.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHTitles.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHParticleTemplate.h"
#include "common/vector_util.h"
#include "dlls/roach.h"
#include "mod/AvHSelectionHelper.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHDramaticPriority.h"
#include "mod/AvHHulls.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHParticleSystemEntity.h"
#include "mod/AvHAlienAbilities.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHMarineTurret.h"
#include "mod/AvHSiegeTurret.h"
#include "mod/AvHBlipConstants.h"
#include "mod/AvHParticleConstants.h"
#include "util/MathUtil.h"
#include "types.h"

#include "mod/AvHNetworkMessages.h"
#include "mod/AvHNexusServer.h"

std::string GetLogStringForPlayer( edict_t *pEntity );

extern int gJetpackEventID;
extern int gAlienSightOnEventID;
extern int gAlienSightOffEventID;
extern int gStartOverwatchEventID;
extern int gEndOverwatchEventID;
extern int gRegenerationEventID;
extern int gStartCloakEventID;
extern int gEndCloakEventID;
extern int gNumFullPackCalls;
extern int gWeaponAnimationEventID;
extern int gMetabolizeSuccessEventID;
extern int gPhaseInEventID;

// Yucky globals
extern AvHParticleTemplateListServer    gParticleTemplateList;
extern AvHSoundListManager              gSoundListManager;
extern cvar_t                           allow_spectators;
extern cvar_t                           avh_marinereinforcementcost;

#ifdef DEBUG
extern cvar_t                           avh_spawninvulnerabletime;
#endif

AvHSelectionHelper                      gSelectionHelper;
extern vec3_t                           gPMDebugPoint;
extern void ResetPlayerPVS( edict_t *client, int clientnum );

LINK_ENTITY_TO_CLASS( player, AvHPlayer );

const float kTransitionFadeTime = .6f;
const float kSpawnInFadeTime = .9f;

AvHPlayer::AvHPlayer()
{
    this->Init();
	//TODO: find out lifecycle of entity vs. lifecycle of client and reset only when we have a new client.
	this->InitBalanceVariables();
}

void AvHPlayer::AddDebugEnemyBlip(float inX, float inY, float inZ)
{
    this->mEnemyBlips.AddBlip(inX, inY, inZ);
}

void AvHPlayer::AddPoints( int score, BOOL bAllowNegativeScore )
{
    // Positive score always adds
    if ( score < 0 )
    {
        if ( !bAllowNegativeScore )
        {
            if ( this->mScore < 0 )		// Can't go more negative
                return;
            
            if ( -score > this->mScore )	// Will this go negative?
            {
                score = -this->mScore;		// Sum will be 0
            }
        }
    }
    
    this->mScore += score;
    
    this->EffectivePlayerClassChanged();
}

void AvHPlayer::AwardKill( entvars_t* inTargetPEV)
{
    // Don't award points in new resource model
    //CBaseEntity* inTarget = CBaseEntity::Instance(inTargetPEV);
    //GetGameRules()->AwardPointsToPlayer(this, inTarget);
}

int AvHPlayer::BloodColor(void)
{
    int theBloodColor = DONT_BLEED;

    if(this->GetIsMarine() && !this->GetHasHeavyArmor())
    {
        theBloodColor = BLOOD_COLOR_RED;
    }
    else if(this->GetIsAlien())
    {
        theBloodColor = BLOOD_COLOR_YELLOW;
    }

    return theBloodColor;
}

void AvHPlayer::AcquireOverwatchTarget()
{
    ASSERT(this->mInOverwatch);
    
    // Find closest enemy in FOV

    // Find cockroaches for now
    CBaseEntity* theCurrentTarget = NULL;
    float theCurrentRange = 1000000;

    //FOR_ALL_ENTITIES(kRoachClassName, CRoach*)
    FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
    if(this->pev->team != theEntity->pev->team)
    {
        float theDistance = (theEntity->pev->origin - this->pev->origin).Length();
        if(this->GetIsEntityInSight(theEntity) && (theDistance <= theCurrentRange) && theEntity->GetIsRelevant())
        {
            theCurrentTarget = theEntity;
        }
    }
    END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
    //END_FOR_ALL_ENTITIES(kRoachClassName)

    if(theCurrentTarget)
    {
        this->mOverwatchTarget = ENTINDEX(theCurrentTarget->edict());
        this->pev->fuser1 = this->mOverwatchTarget;
        
        // Playback target event
        //PLAYBACK_EVENT_FULL(0, this->edict(), gTargetOverwatchEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
    }
}

bool AvHPlayer::AttemptToBuildAlienStructure(AvHMessageID inMessageID)
{
    bool theSuccess = false;

    string theErrorMessage;
    int theBuildingPointCost = 0;
    bool thePurchaseAllowed = this->GetPurchaseAllowed(inMessageID, theBuildingPointCost, &theErrorMessage);

    if(thePurchaseAllowed)
    {
        if(inMessageID == ALIEN_BUILD_HIVE)
        {
            // See if there is an inactive hive within range
            UTIL_MakeVectors(this->pev->v_angle);
            
            Vector theStart = this->GetGunPosition();
            Vector theEnd = theStart + gpGlobals->v_forward*50;
            
            // Collide with world to find potential build site
            TraceResult theTR;
            UTIL_TraceLine(theStart, theEnd, dont_ignore_monsters, this->edict(), &theTR);
            
            Vector theLocation = theTR.vecEndPos;
            
            // Do we have enough points?
            if(AvHSHUGetIsSiteValidForBuild(ALIEN_BUILD_HIVE, &theLocation, this->entindex()))
            {
                // Get the hive at this location
                CBaseEntity* theBaseEntity = NULL;
                AvHHive* theNearestHive = NULL;
                
                // Find the nearest hive
                while((theBaseEntity = UTIL_FindEntityByClassname(theBaseEntity, kesTeamHive)) != NULL)
                {
                    if(theBaseEntity)
                    {
                        AvHHive* theCurrentHive = dynamic_cast<AvHHive*>(theBaseEntity);
                        if(theCurrentHive)
                        {
                            float theCurrentDistance = VectorDistance(theLocation, theCurrentHive->pev->origin);
                            if(!theNearestHive || (theCurrentDistance < VectorDistance(theLocation, theNearestHive->pev->origin)))
                            {
                                theNearestHive = theCurrentHive;
                            }
                        }
                    }
                }
                
                if(theNearestHive)
                {
                    // Make sure another hive isn't already building for this team
                    bool theAnotherHiveBuilding = false;
                    
                    FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
                        if((theEntity->pev->team == this->pev->team) && theEntity->GetIsSpawning())
                        {
                            theAnotherHiveBuilding = true;
                            break;
                        }
                    END_FOR_ALL_ENTITIES(kesTeamHive);
                        
                    if(!theAnotherHiveBuilding)
                    {
                        // If so, set it as growing
                        if(theNearestHive->StartSpawningForTeam(this->GetTeam()))
                        {
                            AvHSUBuildingJustCreated(inMessageID, theNearestHive, this);
                            theSuccess = true;
                        }
                        else
                        {
                            this->SendMessage(kHelpTextHiveBlocked);
                        }
                    }
                    else
                    {
                        this->SendMessage(kHelpTextOtherHiveBuilding);
                    }
                }
            }
            else
            {
                this->SendMessage(kHelpTextEmptyHiveNotNearby);
            }
        }
        else
        {
            char* theClassName = NULL;
            if(AvHSHUGetBuildTechClassName(inMessageID, theClassName))
            {
                // Make sure we haven't exceeded the limit
                int theNumBuildings = 0;
                FOR_ALL_ENTITIES(theClassName, CBaseEntity*)
                    if(theEntity->pev->team == this->pev->team)
                    {
                        theNumBuildings++;
                    }
                END_FOR_ALL_ENTITIES(theClassName);
                    
                // Now check to make sure the space is big enough to hold the building
                UTIL_MakeVectors(this->pev->v_angle);
                
                const int kAimRange = 48;
                Vector theStart = this->GetGunPosition();
                Vector theEnd = theStart + gpGlobals->v_forward*kAimRange;
                
                // Collide with world to find potential build site
                TraceResult theTR;
                UTIL_TraceLine(theStart, theEnd, dont_ignore_monsters, this->edict(), &theTR);
                
                Vector theLocation = theTR.vecEndPos;
                  
                // Check if collision point is valid for building
                if(AvHSHUGetIsSiteValidForBuild(inMessageID, &theLocation))
                {
                    // Make sure there aren't too many buildings in this area already
                    int theNumBuildingsNearby = UTIL_CountEntitiesInSphere(theLocation, BALANCE_VAR(kBuildingVisibilityRadius), theClassName);
                    if(theNumBuildingsNearby < BALANCE_VAR(kNumSameAlienStructuresAllowedInRadius) || FStrEq(theClassName, kwsAlienResourceTower))//voogru: allow the building of rt's regardless of how many may be close by (for maps that have a lot of nodes close to each other)
                    {
                        // Create the new building
                        CBaseEntity* theEntity = CBaseEntity::Create(theClassName, theLocation, AvHSUGetRandomBuildingAngles());
                        
                        // Set building's team
                        theEntity->pev->team = this->pev->team;
                        
                        AvHSUBuildingJustCreated(inMessageID, theEntity, this);
                        
                        // Set owner (this prevents collisions between the entity and it's owner though)
                        //theEntity->pev->owner = ENT(this->pev);

						//voogru: I've moved this here because whats the point of playing the sound if the building didnt get placed? (it was after " Vector theLocation = theTR.vecEndPos;")
						// Play sound
						char* theSoundEffect = kAlienBuildingSound1;

						if(RANDOM_LONG(0, 1) == 1)
							theSoundEffect = kAlienBuildingSound2;

						EMIT_SOUND(this->edict(), CHAN_AUTO, theSoundEffect, this->GetAlienAdjustedEventVolume(), ATTN_NORM);
                        
                        theSuccess = true;
                    }
                    else
                    {
                        this->SendMessage(kTooManyStructuresOfThisTypeNearby);
                    }
                }
                else
                {
                    // Play hive easter egg sometimes when trying to build a hive where one already exists
                    if(inMessageID == ALIEN_BUILD_HIVE)
                    {
                        //// If we are close to a hive we already own
                        //AvHHive* theHive = AvHSUGetRandomActiveHive((AvHTeamNumber)this->pev->team);
                        //if(theHive)
                        //{
                        //    float theDistance = VectorDistance(theHive->pev->origin, this->pev->origin);
                        //    if(theDistance <= 300)
                        //    {
                        //        // Randomly play easter egg
                        //        if(RANDOM_LONG(0, 10) == 0)
                        //        {
                        //            EMIT_SOUND(this->edict(), CHAN_AUTO, kMyHiveEasterEgg, 1.0f, ATTN_NORM);
                        //        }
                        //    }
                        //}
                    }
                }
            }
            else
            {
                this->PlayHUDSound(HUD_SOUND_ALIEN_MORE);
            }
        }
    }
    else
    {
        this->SendMessage(theErrorMessage.c_str());
    }
    
    return theSuccess;
}

bool AvHPlayer::BuildTech(AvHMessageID inBuildID, const Vector& inPickRay)
{
    bool theSuccess = false;

    //AvHSUSetIsDebugging(true);

    // If valid
    if(AvHSHUGetIsBuildTech(inBuildID))
    {
        // Make sure this is a valid place to build
        Vector theLocation;
        if(AvHSHUTraceAndGetIsSiteValidForBuild(inBuildID, this->GetVisualOrigin(), inPickRay, &theLocation))
        {
            // Decrement resources
            string theErrorMessage;
            int theCost = 0;
            bool thePurchaseAllowed = this->GetPurchaseAllowed(inBuildID, theCost, &theErrorMessage);
            if(thePurchaseAllowed)
            {
                // Count how many entities on our team we have in area
                int theNumFriendlyEntitiesInArea = 0;
                CBaseEntity* theEntity = NULL;
                while((theEntity = UTIL_FindEntityInSphere(theEntity, theLocation, BALANCE_VAR(kBuildingVisibilityRadius))) != NULL)
                {
                    // Don't count players
                    if(!theEntity->IsPlayer() && (theEntity->pev->team == this->pev->team))
                    {
                        theNumFriendlyEntitiesInArea++;
                    }
                }

                if(theNumFriendlyEntitiesInArea < BALANCE_VAR(kMaxMarineEntitiesAllowedInRadius))
                {
                    // Build it!
                    theSuccess = (AvHSUBuildTechForPlayer(inBuildID, theLocation, this) != NULL);
                
                    // Inform structure about build if possible
                    if(theSuccess)
                    {
                        if(this->mSelected.size() > 0)
                        {
                            // Get selected structure and inform
                            int theFirstEntitySelected = *this->mSelected.begin();
                            AvHBaseBuildable* theBaseBuildable = dynamic_cast<AvHBaseBuildable*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theFirstEntitySelected)));
                            if(theBaseBuildable)
                            {
                                theBaseBuildable->TechnologyBuilt(inBuildID);
                            }
                        }

                        this->PayPurchaseCost(theCost);
                    }
                }
                else
                {
                    this->SendMessage(kTooManyStructuresInArea);
                }
            }
            else
            {
                this->PlayHUDSound(HUD_SOUND_MARINE_MORE);
            }
        }
    }

    //AvHSUSetIsDebugging(false);
    
    // If successful
        // Send confirmation if needed
    // else
        // Send failure so client is updated
    
    return theSuccess;
}

bool AvHPlayer::GroupMessage(AvHMessageID inGroupMessage)
{
    bool theSuccess = false;

    if((inGroupMessage >= GROUP_CREATE_1) && (inGroupMessage < (GROUP_CREATE_1 + kNumHotkeyGroups)))
    {
        int theOffset = (int)(inGroupMessage - GROUP_CREATE_1);
        ASSERT(theOffset >= 0);
        ASSERT(theOffset < kNumHotkeyGroups);
        if(this->mSelected.size() > 0)
        {
            this->GetTeamPointer()->SetGroup(theOffset, this->mSelected);
            AvHUser3 theUser3 = this->GetTeamPointer()->GetGroupType(theOffset);

            AvHHUDSound theHudSound = HUD_SOUND_SELECT;
            if(theUser3 == AVH_USER3_MARINE_PLAYER)
            {
                theHudSound = AvHHUDSound(HUD_SOUND_SQUAD1 + theOffset);
            }

            this->PlayHUDSound(theHudSound);

            // If this is a squad, tell all the players in the squad also.  This also tells them they are in the squad.
            if(theUser3 == AVH_USER3_MARINE_PLAYER)
            {
                // Loop through them
                for(EntityListType::iterator theIter = this->mSelected.begin(); theIter != this->mSelected.end(); theIter++)
                {
                    AvHPlayer* thePlayer = NULL;
                    AvHSUGetEntityFromIndex(*theIter, thePlayer);
                    if(thePlayer)
                    {
                        thePlayer->PlayHUDSound(theHudSound);
                    }
                }
            }

            // Now run through all the hotgroups and remove these entities from them (entities can only be part of one group)
            for(int theCurrentIndex = 0; theCurrentIndex < kNumHotkeyGroups; theCurrentIndex++)
            {
                if(theCurrentIndex != theOffset)
                {
                    //this->mGroups[theCurrentIndex];
                    EntityListType theCurrentGroup = this->GetTeamPointer()->GetGroup(theCurrentIndex);

                    // Remove all members of mSelected from this group
                    for(EntityListType::iterator theIterator = theCurrentGroup.begin(); theIterator != theCurrentGroup.end(); /* nothing */)
                    {
                        // If the current entity is in selection
                        EntityListType::iterator theFindIter = std::find(this->mSelected.begin(), this->mSelected.end(), *theIterator);
                        bool theEntityInSelection = (theFindIter != this->mSelected.end());
                        if(theEntityInSelection)
                        {
                            // Remove it from this list
                            theIterator = theCurrentGroup.erase(theIterator);
                        }
                        else
                        {
                            // Else, increment
                            theIterator++;
                        }
                    }

                    // Set the group again
                    this->GetTeamPointer()->SetGroup(theCurrentIndex, theCurrentGroup);
                }
            }
        }
        theSuccess = true;
    }
    else if((inGroupMessage >= GROUP_SELECT_1) && (inGroupMessage < (GROUP_SELECT_1 + kNumHotkeyGroups)))
    {
        int theOffset = (int)(inGroupMessage - GROUP_SELECT_1);
        ASSERT(theOffset >= 0);
        ASSERT(theOffset < kNumHotkeyGroups);

        //const EntityListType& theGroup = this->mGroups[theOffset];
        EntityListType theGroup = this->GetTeamPointer()->GetGroup(theOffset);
        if(theGroup.size() > 0)
        {
            if(this->mSelected != theGroup)
            {
                this->mSelected = theGroup;
                this->mTrackingEntity = 0;
            }
            else
            {
                // If we received the same select message twice in a row, go instead to the last place we went to
                if(inGroupMessage == this->mLastSelectEvent)
                {
                    // Go to last saved position
                    VectorCopy(this->mPositionBeforeLastGotoGroup, this->pev->origin);

                    // Clear last select so hitting it again will go to group
                    this->mLastSelectEvent = MESSAGE_NULL;
                }
                else
                {
                    // Find nearest entity in group and track it
                    int theNearestGroupEntity = 0;
                    float theClosestDistance = sqrt(2*kMaxMapDimension*kMaxMapDimension);
                    for(EntityListType::const_iterator theIter = theGroup.begin(); theIter != theGroup.end(); theIter++)
                    {
                        CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theIter));
                        ASSERT(theBaseEntity);
                        float theCurrentDistance = VectorDistance(theBaseEntity->pev->origin, this->pev->origin);
                        if(theCurrentDistance < theClosestDistance)
                        {
                            theNearestGroupEntity = *theIter;
                            theClosestDistance = theCurrentDistance;
                        }
                    }
                    
                    this->mTrackingEntity = theNearestGroupEntity;
                    
                    // Move player in vicinity of player so entity is in PVS
                    if(this->mTrackingEntity > 0)
                    {
                        CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mTrackingEntity));
                        if(theBaseEntity)
                        {
                            // Save last position so we can jump back to it easily
                            VectorCopy(this->pev->origin, this->mPositionBeforeLastGotoGroup);
                            this->mLastSelectEvent = inGroupMessage;
                    
                            // Goto group
                            VectorCopy(theBaseEntity->pev->origin, this->pev->origin);
                        }
                    }
                }
            }
        }

        theSuccess = true;
    }

    return theSuccess;
}

bool AvHPlayer::GetCenterPositionForGroup(int inGroupNumber, float& outX, float& outY)
{
    bool theSuccess = false;
        
    if((inGroupNumber >= 1) && (inGroupNumber <= kNumHotkeyGroups))
    {
        int theNumFound = 0;
        Vector theLocation;
        theLocation.x = theLocation.y = theLocation.z = 0.0f;

        //EntityListType& theGroup = this->mGroups[inGroupNumber-1];
        EntityListType theGroup = this->GetTeamPointer()->GetGroup(inGroupNumber-1);
        for(EntityListType::iterator theIter = theGroup.begin(); theIter != theGroup.end(); theIter++)
        {
            Vector theEntityLocation;
            if(AvHSHUGetEntityLocation(*theIter, theEntityLocation))
            {
                theLocation.x += theEntityLocation.x;
                theLocation.y += theEntityLocation.y;

                theNumFound++;
            }
        }

        if(theNumFound > 0)
        {
            outX = theLocation.x/theNumFound;
            outY = theLocation.y/theNumFound;

            theSuccess = true;
        }
    }

    return theSuccess;
}

string AvHPlayer::GetNetworkID() const
{
    return this->mNetworkID;
}

void AvHPlayer::SetNetworkID(string& inNetworkID)
{
    this->mNetworkID = inNetworkID;
}

string AvHPlayer::GetNetworkAddress() const
{
	return this->mNetworkAddress;
}

void AvHPlayer::SetNetworkAddress(string& inNetworkAddress)
{
	this->mNetworkAddress = inNetworkAddress;
}

void AvHPlayer::ClearRoleAbilities()
{
    // Clear all abilities that don't stick around between changing role, like all alien purchaseable upgrades
    //SetUpgradeMask(&this->pev->iuser4, ALIEN_ABILITY_2, false);
    this->pev->iuser3 = AVH_USER3_NONE;
    this->pev->iuser4 &= ~MASK_ALIEN_EMBRYO;
    this->pev->iuser4 &= ~MASK_ALIEN_MOVEMENT;
    this->mIsScreaming = false;
    this->mAlienSightActive = false;
    this->mDesiredRoomType = 0;
}

void AvHPlayer::ClearUserVariables()
{
    this->pev->iuser1 = 0;
    this->pev->iuser2 = 0;
    this->pev->iuser3 = 0;
    this->pev->iuser4 = 0;
    this->pev->fuser1 = 0;
    this->pev->fuser2 = 0;
    this->pev->fuser3 = 0;
    this->pev->fuser4 = 0;
}

void AvHPlayer::SetScore(int inScore)
{
    this->mScore = inScore;
}

int AvHPlayer::GetScore() const
{
    return this->mScore;
}

CBaseEntity* AvHPlayer::CreateAndDrop(const char* inItemName)
{
	UTIL_MakeVectors(pev->v_angle);

    CBaseEntity* theEntity = CBaseEntity::Create(inItemName, pev->origin + gpGlobals->v_forward * 10, pev->angles, edict() );
    
    theEntity->pev->angles.x = 0;
    theEntity->pev->angles.z = 0;
    //theEntity->PackWeapon( pWeapon );
    theEntity->pev->velocity = gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100;
    
    return theEntity;
}

void AvHPlayer::DeployCurrent()
{
    if(this->m_pActiveItem /*&& !this->GetIsBeingDigested()*/)
    {
        CBasePlayerWeapon* theCurrentWeapon = (CBasePlayerWeapon *)this->m_pActiveItem->GetWeaponPtr();
        if(theCurrentWeapon && theCurrentWeapon->CanDeploy())
        {
            theCurrentWeapon->Deploy();
        }
    }
    else
    {
        // No current weapon, so get the best weapon we have.
        g_pGameRules->GetNextBestWeapon( this, NULL );
    }
}


// Drop the current item, not weapon, if any.  
bool AvHPlayer::DropItem(const char* inItemName)
{
    bool theSuccess = false;
    
    if(!GetGameRules()->GetIsCombatMode())
    {
        CBasePlayerItem* theItem = this->m_pActiveItem;
        
        if(inItemName)
        {
            bool theIsDone = false;
			theItem = NULL; //we're looking for an item by name, don't bias with active item

            // Find item with this name
            for(int i = 0; (i < MAX_ITEM_TYPES) && !theIsDone; i++)
            {
                CBasePlayerItem* theCurrentItem = this->m_rgpPlayerItems[i];
                while(theCurrentItem && !theIsDone)
                {
                    if(FStrEq(STRING(theCurrentItem->pev->classname), inItemName))
                    {
                        theIsDone = true;
                        theItem = theCurrentItem;
                    }
                    theCurrentItem = theCurrentItem->m_pNext;
                }
            }
        }

        if(theItem)
        {
            AvHBasePlayerWeapon* theOriginalDroppedWeapon = dynamic_cast<AvHBasePlayerWeapon*>(theItem);
            if(theOriginalDroppedWeapon && theOriginalDroppedWeapon->GetIsDroppable())      
            {
                // Dropping current weapon turns off overwatch
                this->TurnOffOverwatch();
                
                //char theItemName[256];
                //strcpy(theItemName, STRING(theItem->pev->classname));
                //this->DropPlayerItem(theItemName);
                
                // Get ammo left, if it's a weapon
                int theAmmoLeft = -1;
                if(theOriginalDroppedWeapon)
                {
                    theAmmoLeft = theOriginalDroppedWeapon->m_iClip;
                    GetGameRules()->GetNextBestWeapon(this, theItem);
                }
                
                CBaseEntity* theDroppedEntity = this->CreateAndDrop(STRING(theItem->pev->classname));
                if(theAmmoLeft != -1)
                {
                    CBasePlayerWeapon* theNewlyDroppedWeapon = dynamic_cast<CBasePlayerWeapon*>(theDroppedEntity);
                    ASSERT(theNewlyDroppedWeapon);
                    
                    // Set ammo, make sure client ammo isn't the same so update is forced
                    theNewlyDroppedWeapon->m_iClip = theAmmoLeft;
                    theNewlyDroppedWeapon->m_iClientClip = theAmmoLeft - 1;
                    
                    // Means "only ammo is that in the clip"
                    theNewlyDroppedWeapon->m_iDefaultAmmo = 0;

					ItemInfo theItemInfo;

					if(theOriginalDroppedWeapon->GetItemInfo(&theItemInfo) != 0)
					{
						int iAmmoIndex = GetAmmoIndex ( (char *) theItemInfo.pszAmmo1 ); 

						if ( iAmmoIndex != -1 && m_rgAmmo[ iAmmoIndex ] > 0)
							this->DropAmmo((char *)theItemInfo.pszAmmo1, m_rgAmmo[ iAmmoIndex ], theItemInfo.iMaxAmmo1, theItemInfo.iId, theNewlyDroppedWeapon->pev->angles);
					}

                }

                // Finally remove the original
                theOriginalDroppedWeapon->DestroyItem();

                // Weak potential fix for occasional crash.  Saw this when someone hit "lastinv" while holding a knife,
                // and m_pLastItem was garbage, not NULL
                this->m_pLastItem = NULL;
                SetAnimation(PLAYER_PRIME);  //Elven - hack to kill TPRA if we throw a weapon.
                theSuccess = true;
            }
        }
    }

    return theSuccess;
}

void AvHPlayer :: DropAmmo(char *pszAmmoType, int iAmmoAmt, int iMax, int iWeaponID, Vector vecAngles)
{
	if(!pszAmmoType || !iAmmoAmt || !iMax || !iWeaponID)
		return;

	int iAmmoIndex = GetAmmoIndex ( pszAmmoType );

	//Not a valid ammo type.
	if(iAmmoIndex == -1)
		return;

	if(m_rgAmmo[ iAmmoIndex ] > 0)
		m_rgAmmo[ iAmmoIndex ] -= iAmmoAmt;
	else
		return;

	int theSubModel = -1;

	switch(iWeaponID)
	{
	case AVH_WEAPON_PISTOL:		  theSubModel = 3; break;
	case AVH_WEAPON_MG:			  theSubModel = 1; break;
	case AVH_WEAPON_SONIC:		  theSubModel = 0; break;
	case AVH_WEAPON_HMG:		  theSubModel = 2; break;
	case AVH_WEAPON_GRENADE_GUN:  theSubModel = 4; break;
	}

	if(theSubModel == -1)
		return;

	CBaseEntity *theDroppedEntity = this->CreateAndDrop( kwsAmmoPack );
	AvHAmmoPack *theRealAmmoPack = dynamic_cast<AvHAmmoPack*>(theDroppedEntity);

	strncpy(theRealAmmoPack->m_szAmmoType, pszAmmoType, 31);
	theRealAmmoPack->m_iMaxAmmo = iMax;
	theRealAmmoPack->m_iAmmoAmt = iAmmoAmt;
	theRealAmmoPack->m_iWeaponID = iWeaponID;
	theRealAmmoPack->pev->body = theSubModel;
	theRealAmmoPack->pev->angles = vecAngles;

	theRealAmmoPack->SetThink( &CBaseEntity::SUB_Remove );
	theRealAmmoPack->pev->nextthink = gpGlobals->time + AvHSUGetWeaponStayTime();
}

void AvHPlayer::EffectivePlayerClassChanged()
{
    this->mEffectivePlayerClassChanged = true;
}

void AvHPlayer::NeedsTeamUpdate()
{
    this->mNeedsTeamUpdate = true;
}

void AvHPlayer::SendTeamUpdate()
{
    this->mSendTeamUpdate = true;
}

bool AvHPlayer::ExecuteAlienMorphMessage(AvHMessageID inMessageID, bool inInstantaneous)
{
    bool theMessageExecuted = false;

    string theErrorMessage;
    bool theHadEnoughPoints = false;
    
    switch(inMessageID)
    {
    case ALIEN_LIFEFORM_ONE:
    case ALIEN_LIFEFORM_TWO:
    case ALIEN_LIFEFORM_THREE:
    case ALIEN_LIFEFORM_FOUR:
    case ALIEN_LIFEFORM_FIVE:
        
    case ALIEN_EVOLUTION_ONE:
    case ALIEN_EVOLUTION_TWO:
    case ALIEN_EVOLUTION_THREE:
        //case ALIEN_EVOLUTION_FOUR:
        //case ALIEN_EVOLUTION_FIVE:
        //case ALIEN_EVOLUTION_SIX:
    case ALIEN_EVOLUTION_SEVEN:
    case ALIEN_EVOLUTION_EIGHT:
    case ALIEN_EVOLUTION_NINE:
    case ALIEN_EVOLUTION_TEN:
    case ALIEN_EVOLUTION_ELEVEN:
    case ALIEN_EVOLUTION_TWELVE:
    case ALIEN_HIVE_TWO_UNLOCK:
    case ALIEN_HIVE_THREE_UNLOCK:
        
        // Now only allow upgrading from level1
        if(this->GetCanGestate(inMessageID, theErrorMessage))
        {
            // Stay as current lifeform by default
            bool theCheckDucking = true;
            int theTargetIuser3 = this->pev->iuser3;
            switch(inMessageID)
            {
            case ALIEN_LIFEFORM_ONE:
                theTargetIuser3 = AVH_USER3_ALIEN_PLAYER1;
                break;
            case ALIEN_LIFEFORM_TWO:
                theTargetIuser3 = AVH_USER3_ALIEN_PLAYER2;
                break;
            case ALIEN_LIFEFORM_THREE:
                theTargetIuser3 = AVH_USER3_ALIEN_PLAYER3;
                break;
            case ALIEN_LIFEFORM_FOUR:
                theTargetIuser3 = AVH_USER3_ALIEN_PLAYER4;
                break;
            case ALIEN_LIFEFORM_FIVE:
                theTargetIuser3 = AVH_USER3_ALIEN_PLAYER5;
                theCheckDucking = false;
                break;
            }
            
            int theTargetHull = AvHMUGetHull(theCheckDucking, theTargetIuser3);
            vec3_t theOrigin;
            GetNewOrigin((AvHUser3)theTargetIuser3, theCheckDucking, theOrigin); 
			
			// removed by puzl to fix gestating in vents.
			//theOrigin.z += 5;

            bool theIsEnoughRoom = AvHSUGetIsEnoughRoomForHull(theOrigin, theTargetHull, this->edict());
			//voogru: try again but higher
			if(!theIsEnoughRoom)
			{

				theOrigin.z += AvHMUGetOriginOffsetForMessageID(inMessageID);

				theIsEnoughRoom = AvHSUGetIsEnoughRoomForHull(theOrigin, theTargetHull, this->edict());
			}

            if(theIsEnoughRoom || inInstantaneous)
            {
                if(!theIsEnoughRoom)
                {
                    int a = 0;
                }
                
                this->Evolve(inMessageID, inInstantaneous);
                theMessageExecuted = true;
            }
            else
            {
                this->SendMessage(kNeedMoreRoomToGestate);
            }
        }
        else
        {
            this->SendMessage(theErrorMessage.c_str());
        }
        break;
    }

    return theMessageExecuted;
}

bool AvHPlayer::ExecuteMessage(AvHMessageID inMessageID, bool inInstantaneous, bool inForce)
{
	bool theMessageExecuted = false;

	AvHTeam* theTeam = this->GetTeamPointer();
	string theErrorMessage;

	// If valid
	if((inMessageID != MESSAGE_NULL) && theTeam)
	{
		bool theIsMarine = theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE;
		bool theIsAlien = theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN;
		bool theGameStarted = GetGameRules()->GetGameStarted();
		bool theIsInTopDownMode = this->GetIsInTopDownMode();
		bool theIsCombat = GetGameRules()->GetIsCombatMode();
		bool theIsBeingDigested = this->GetIsBeingDigested();

		// If we can purchase it
		string theErrorMessage;
		int theCost = 0;

		if(this->GetPurchaseAllowed(inMessageID, theCost, &theErrorMessage) || inForce)
		{
			// If we're in Combat mode
			bool theIsAvailable = true;

			if(theIsCombat)
			{
				// Try to execute message
				theMessageExecuted = this->ExecuteCombatMessage(inMessageID, theIsAvailable, inForce);
				if(theMessageExecuted)
				{
					this->PurchaseCombatUpgrade(inMessageID);            
				}
			}

			// If it's a fall-through alien upgrade, try it next
			if(!theMessageExecuted && theIsAlien && !theIsBeingDigested)
			{
				// Try to morph or build
				theMessageExecuted = this->ExecuteAlienMorphMessage(inMessageID, inInstantaneous);
				if(!theMessageExecuted && !theIsCombat)
				{
					switch(inMessageID)
					{
					case ALIEN_BUILD_RESOURCES:
					case ALIEN_BUILD_OFFENSE_CHAMBER:
					case ALIEN_BUILD_DEFENSE_CHAMBER:
					case ALIEN_BUILD_SENSORY_CHAMBER:
					case ALIEN_BUILD_MOVEMENT_CHAMBER:
					case ALIEN_BUILD_HIVE:
						theMessageExecuted = this->AttemptToBuildAlienStructure(inMessageID);
						break;
					}
				}
			}

			// Add upgrade to our list to be preserved on respawn
			if(theMessageExecuted && theIsCombat)
				this->mCombatNodes.SetResearchDone(inMessageID);

			if(theMessageExecuted)
				this->PayPurchaseCost(theCost);
		}

		// tankefugl: 0000971
		int theIssuedOrderIcon = -1;
		// :tankefugl

		if(theIsMarine 
			&& !theIsInTopDownMode 
			&& !theIsBeingDigested)
		{
			switch (inMessageID)
			{
			case WEAPON_RELOAD:
				if(theGameStarted)
					this->ReloadWeapon();
				break;
			case WEAPON_DROP:
				if(!this->DropItem())
					this->SendMessageOnce(kWeaponCantBeDropped, TOOLTIP);
				break;

			case ADMIN_VOTEDOWNCOMMANDER:
				if(theTeam->PlayerVote(this->entindex(), theErrorMessage))
					theMessageExecuted = true;
				else
					this->SendMessage(theErrorMessage.c_str());
				break;
				// Talk to your teammates
			case SAYING_1:
			case SAYING_2:
			case SAYING_3:
			case SAYING_4:
			case SAYING_5:
			case SAYING_6:
			case SAYING_7:
			case SAYING_8:
			case SAYING_9:
			case ORDER_REQUEST:
			case ORDER_ACK:
				if(this->PlaySaying(inMessageID))
				{
					AvHAlertType theAlertType = ALERT_NONE;
					AvHMessageID theMessageID = MESSAGE_NULL;

					if(this->GetIsMarine())
					{
						switch(inMessageID)
						{
						// tankefugl: 0000971
						// decides whether icon updates should be sent
						case SAYING_1:
							theIssuedOrderIcon = TEAMMATE_MARINE_ORDER_FOLLOW;
							break;

						case SAYING_2:
							theIssuedOrderIcon = TEAMMATE_MARINE_ORDER_COVER;
							break;

						case SAYING_8:
							theIssuedOrderIcon = TEAMMATE_MARINE_ORDER_WELD;
							break;
						// :tankefugl

						case SAYING_5:
							theAlertType = ALERT_SOLDIER_NEEDS_AMMO;
							theMessageID = COMMANDER_NEXTAMMO;
							break;

						case SAYING_4:
							theAlertType = ALERT_SOLDIER_NEEDS_HEALTH;
							theMessageID = COMMANDER_NEXTHEALTH;
							break;

						case ORDER_REQUEST:
							theAlertType = ALERT_ORDER_NEEDED;
							theMessageID = COMMANDER_NEXTIDLE;
							break;
						}

						// Send alert for commander
						if(theAlertType != ALERT_NONE)
						{
							GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, theAlertType, this->entindex(), theMessageID);
						}
					}

					theMessageExecuted = true;
				}
				break;
			}
		}
		else if(theIsAlien && !theIsBeingDigested)
		{
			switch (inMessageID)
			{
				// Talk to your teammates
			case SAYING_1:
			case SAYING_2:
			case SAYING_3:
			case SAYING_4:
			case SAYING_5:
			case SAYING_6:
			case SAYING_7:
			case SAYING_8:
			case SAYING_9:
				if(this->PlaySaying(inMessageID))
					theMessageExecuted = true;
				break;

			case ALIEN_ABILITY_LEAP:
				this->StartLeap();
				break;
			case 100:
				// Eat flashlight event.  Add special mode for alien view here?
				if(!this->mAlienSightActive)
					PLAYBACK_EVENT_FULL(FEV_HOSTONLY, this->edict(), gAlienSightOnEventID, 0, this->pev->origin, (float *)&g_vecZero, this->GetAlienAdjustedEventVolume(), 0.0, 0, 0, 0, 0 );
				else
					PLAYBACK_EVENT_FULL(FEV_HOSTONLY, this->edict(), gAlienSightOffEventID, 0, this->pev->origin, (float *)&g_vecZero, this->GetAlienAdjustedEventVolume(), 0.0, 0, 0, 0, 0 );
				this->mAlienSightActive = !this->mAlienSightActive;
				theMessageExecuted = true;
				break;
			}
			// tankefugl: 0000971
			// decides whether icon updates should be sent
			switch (inMessageID)
			{
				case SAYING_1:
					theIssuedOrderIcon = TEAMMATE_ALIEN_ORDER_FOLLOW;
					break;
				case SAYING_2:
					theIssuedOrderIcon = TEAMMATE_ALIEN_ORDER_COVER;
					break;
				case SAYING_4:
				case SAYING_8:
					theIssuedOrderIcon = TEAMMATE_ALIEN_ORDER_HEAL;
					break;
			}
			// :tankefugl
		}

		// tankefugl: 0000971 and 0000992
		if (theIssuedOrderIcon > -1)
		{
			int theOrderTarget = 0;

			vec3_t vecDir;
			VectorCopy(this->GetAutoaimVector(0.0f), vecDir);
			VectorNormalize(vecDir);

			float currentResult = 0.0f;

			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*);
				float dotResult = 0.0f;
				float theDistance = 0.0f;
				vec3_t vecDistance;
				int theTraced = 0;
				vec3_t vecFrom, vecTo;

				if ((theEntity->entindex() != this->entindex()) && (theEntity->GetTeam() == this->GetTeam()))
				{
					VectorSubtract(theEntity->pev->origin, this->pev->origin, vecDistance);
					// theDistance = Length(vecDistance);

					VectorNormalize(vecDistance);
					dotResult = DotProduct(vecDistance, vecDir);
					if ((dotResult > 0.9f) && (dotResult > currentResult))
					{
						TraceResult theTrace;
						vecFrom = AvHSHUGetRealLocation(this->pev->origin, this->pev->mins, this->pev->maxs);
						vecTo = AvHSHUGetRealLocation(theEntity->pev->origin, theEntity->pev->mins, theEntity->pev->maxs);
						UTIL_TraceLine(vecFrom, vecTo, ignore_monsters, this->edict(), &theTrace);
						if (theTrace.flFraction == 1.0f)
						{
							theTraced = 1;
							currentResult = dotResult;
							theOrderTarget = theEntity->entindex();
						}
					}
				}
				//ALERT(at_console, "-------------------\n");
				//ALERT(at_console, UTIL_VarArgs("vecDir %f %f %f\n", vecDir[0], vecDir[1], vecDir[2]));
				//ALERT(at_console, UTIL_VarArgs("vecDistance %f %f %f\n", vecDistance[0], vecDistance[1], vecDistance[2]));
				//ALERT(at_console, UTIL_VarArgs("vecFrom %f %f %f\n", vecFrom[0], vecFrom[1], vecFrom[2]));
				//ALERT(at_console, UTIL_VarArgs("vecTo %f %f %f\n", vecTo[0], vecTo[1], vecTo[2]));
				//ALERT(at_console, UTIL_VarArgs("dotResult %f\n", dotResult));
				//ALERT(at_console, UTIL_VarArgs("currentResult %f\n", currentResult));
				//ALERT(at_console, UTIL_VarArgs("theTraced %d\n", theTraced));
				//ALERT(at_console, UTIL_VarArgs("theOrderTarget %d\n", theOrderTarget));
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName);

//			ALERT(at_console, UTIL_VarArgs("theIssuedOrderIcon %d source %d target %d\n", theIssuedOrderIcon, this->entindex(), theOrderTarget));

			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*);
				if(theEntity->GetTeam() == this->GetTeam())
				{
					NetMsg_IssueOrder(theEntity->pev, theIssuedOrderIcon, this->entindex(), theOrderTarget);
				}
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName);

		}

		// Common messages here
		if(!theMessageExecuted)
		{
			switch(inMessageID)
			{
			case WEAPON_NEXT:
				this->NextWeapon();
				theMessageExecuted = true;
				break;
			case ADMIN_READYROOM:
				if(this->GetPlayMode() != PLAYMODE_READYROOM)
				{
					this->SetPlayMode(PLAYMODE_READYROOM);
					theMessageExecuted = true;
				}
				break;
			}
		}
	}
    return theMessageExecuted;
}

bool AvHPlayer::GetIsAlienSightActive() const
{
    return this->mAlienSightActive;
}

void AvHPlayer::SetDesiredNetName(string inDesiredNetName)
{
    this->mDesiredNetName = inDesiredNetName;
}

void AvHPlayer::SetDesiredRoomType(int inRoomType, bool inForceUpdate)
{
    this->mDesiredRoomType = inRoomType;

    if(inForceUpdate)
    {
        this->mClientDesiredRoomType = -1;
    }
}

void AvHPlayer::ResetPlayerPVS()
{
    // Force recompute of PVS
    ::ResetPlayerPVS(this->edict(), this->entindex());
}

void AvHPlayer::SetPosition(const Vector& inNewPosition)
{
    // Set new position
    VectorCopy(inNewPosition, this->pev->origin);

    this->ResetPlayerPVS();
}

// Play sounds quieter when the alien has the silence upgrade
float AvHPlayer::GetAlienAdjustedEventVolume() const
{
    return AvHPlayerUpgrade::GetSilenceVolumeLevel((AvHUser3)this->pev->iuser3, this->pev->iuser4);
}



void AvHPlayer::GetAnimationForActivity(int inActivity, char outAnimation[64], bool inGaitSequence)
{
    strcpy(outAnimation, "");

    bool theCanCrouch = !((this->GetUser3() == AVH_USER3_ALIEN_PLAYER1) || (this->GetUser3() == AVH_USER3_ALIEN_PLAYER2) || (this->GetUser3() == AVH_USER3_ALIEN_PLAYER3));
    bool theIsCrouched = FBitSet(this->pev->flags, FL_DUCKING);
    bool theIsAlien = this->GetIsAlien();
    bool theIsGestating = (this->GetUser3() == AVH_USER3_ALIEN_EMBRYO);
    bool theIsDeathAnim = false;
    bool theIsReloading = false;
    int theDebugAnimations = BALANCE_VAR(kDebugAnimations);
	
    //bool theIsBlinking = this->GetIsBlinking();

    switch(inActivity)
    {
    case ACT_RELOAD:
        if(!theIsAlien)
		{
            strcat(outAnimation, this->m_szAnimExtention);
			strcat(outAnimation, "_reload");
	
			theIsReloading = true;
        };
        break;
	// updated by Elven for TPRAs
     case ACT_RELOAD_START: 
        if(!theIsAlien)
        {
			strcat(outAnimation, this->m_szAnimExtention);
			strcat(outAnimation, "_reload_start");
			theIsReloading = true;
		};
		break;
	case ACT_RELOAD_INSERT:
        if(!theIsAlien)
        {
			 strcat(outAnimation, this->m_szAnimExtention);
			strcat(outAnimation, "_reload_insert");
			theIsReloading = true;
		};
		break;
	case ACT_RELOAD_END:
        if(!theIsAlien)
        {
			strcat(outAnimation, this->m_szAnimExtention);
			strcat(outAnimation, "_reload_end");
			theIsReloading = true;
		};
		break;
    case ACT_RANGE_PRIME:
        if(theCanCrouch && theIsCrouched)
        {
            strcpy(outAnimation, "crouch_" );
        }
        strcat(outAnimation, this->m_szAnimExtention);
        strcat(outAnimation, "_prime");
        break;

    case ACT_RANGE_ATTACK1:
        if(theCanCrouch && theIsCrouched)
        {
            strcpy(outAnimation, "crouch_" );
        }
        strcat(outAnimation, this->m_szAnimExtention);
        if(theIsAlien)
        {
            strcat(outAnimation, "_alien");
        }
        else
        {
            strcat(outAnimation, "_fire");
        }
        break;

    case ACT_HOP:
        strcat(outAnimation, "jump");
        break;

    case ACT_WALK:
        if(inGaitSequence || !strcmp(this->m_szAnimExtention, ""))
        {
            strcat(outAnimation, "walk");
        }
        else
        {
            if(theCanCrouch && theIsCrouched)
            {
                strcpy(outAnimation, "crouch_" );
            }
            strcat(outAnimation, this->m_szAnimExtention);
            if(theCanCrouch)
            {
                if(theIsReloading)
                {
                    strcat(outAnimation, "_reload");
                }
                else
                {
                    strcat(outAnimation, "_look");
                }
            }
        }
        break;

    case ACT_RUN:
        if(inGaitSequence || !strcmp(this->m_szAnimExtention, ""))
        {
            strcat(outAnimation, "run");
        }
        else
        {
            if(theCanCrouch && theIsCrouched)
            {
                strcpy(outAnimation, "crouch_" );
            }
            strcat(outAnimation, this->m_szAnimExtention);
            if(theCanCrouch)
            {
                //if(theIsReloading)
                //{
                //  strcat(outAnimation, "_reload");
                //}
                //else
                //{
                if(theIsReloading)
                {
                    strcpy(outAnimation, this->m_szAnimExtention);
                    strcat(outAnimation, "_reload");
                }
                else
                {
                    strcat(outAnimation, "_look");
                }
                //}
            }
        }
        break;

    case ACT_CROUCHIDLE:
        if(theCanCrouch)
        {
            if(inGaitSequence)
            {
                strcat(outAnimation, "crouch_idle");
            }
        }
        break;
    case ACT_CROUCH:
        if(theCanCrouch)
        {
            if(inGaitSequence)
            {
                strcat(outAnimation, "crawl");
            }
            else
            {
            }
        }
        break;
    case ACT_IDLE:
        // Weird hack/fix for gyrating and ready room spazzing
        //if(this->GetPlayMode() != PLAYMODE_READYROOM)
        //{
            if(inGaitSequence)
            {
                if(theIsReloading)
                {
                    strcat(outAnimation, "_reload");
                }
                else
                {
                    strcat(outAnimation, "idle1");
                }
            }
            else
            {
                strcat(outAnimation, "look_idle");
            }
        //}
        break;

    case ACT_DIESIMPLE:
    case ACT_DIEVIOLENT:
        if(this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER3)
        {
            strcpy(outAnimation, "death1_die");
        }
        else
        {
            if(theIsCrouched)
            {
                strcpy(outAnimation, "crouch_die");
            }
            else
            {
                switch(RANDOM_LONG(0, 2))
                {
                case 0:
                    strcpy(outAnimation, "death1_die");
                    break;
                case 1:
                    strcpy(outAnimation, "death2_die");
                    break;
                case 2:
                    strcpy(outAnimation, "death3_die");
                    break;
                }
            }
        }
        theIsDeathAnim = true;
        break;
    case ACT_DIE_HEADSHOT:
        strcpy(outAnimation, "head_die");
        theIsDeathAnim = true;
        break;
    case ACT_DIE_GUTSHOT:
        strcpy(outAnimation, "gut_die");
        theIsDeathAnim = true;
        break;
    case ACT_DIEBACKWARD:
        // Hack for skulk until artwork can be updated (it has no back_die)
        if(this->pev->iuser3 != AVH_USER3_ALIEN_PLAYER1)
        {
            strcpy(outAnimation, "back_die");
        }
        else
        {
            strcpy(outAnimation, "gut_die");
        }
        theIsDeathAnim = true;
        break;
    case ACT_DIEFORWARD:
        strcpy(outAnimation, "forward_die");
        theIsDeathAnim = true;
        break;
    case ACT_SWIM:
        // die
        strcpy(outAnimation, "treadwater");
        break;
    }

    if(theIsGestating)
    {
        float theFullTimeToGestate = GetGameRules()->GetBuildTimeForMessageID(this->mEvolution);
        bool theAlmostDoneGestation = (gpGlobals->time > (this->mTimeGestationStarted + theFullTimeToGestate*.75f));

        switch(inActivity)
        {
        case ACT_SMALL_FLINCH:
        case ACT_BIG_FLINCH:
        case ACT_FLINCH_HEAD:
        case ACT_FLINCH_CHEST:
        case ACT_FLINCH_STOMACH:
        case ACT_FLINCH_LEFTARM:
        case ACT_FLINCH_RIGHTARM:
        case ACT_FLINCH_LEFTLEG:
        case ACT_FLINCH_RIGHTLEG:
            if(RANDOM_LONG(0, 1))
            {
                // flinch1
                strcpy(outAnimation, "flinch1");
            }
            else
            {
                // flinch2
                strcpy(outAnimation, "flinch2");
            }
            break;

        case ACT_DIESIMPLE:
        case ACT_DIEBACKWARD:
        case ACT_DIEFORWARD:
        case ACT_DIEVIOLENT:
            // die
            strcpy(outAnimation, "die");
            theIsDeathAnim = true;
            break;

        default:
            if(theAlmostDoneGestation)
            {
                // gestation_fast
                strcpy(outAnimation, "gestation_fast");
            }
            else
            {
                // gestation
                strcpy(outAnimation, "gestation");
            }
            break;
        }
    }

#ifdef DEBUG
    if(theDebugAnimations && theIsDeathAnim)
    {
        char theMessage[256];
        sprintf(theMessage, "AvHPlayer::GetAnimationForActivity(death) returned %s\n", outAnimation);
        ALERT(at_console, theMessage);
    }
#endif
}

bool AvHPlayer::GetCanGestate(AvHMessageID inMessageID, string& outErrorMessage)
{
    bool theCanGestate = false;

    int theNumHives = this->GetTeamPointer()->GetNumActiveHives();
    
    if(this->pev->iuser3 != AVH_USER3_ALIEN_EMBRYO)
    {
        if(this->mDigestee <= 0)
        {
            bool theIsRangeRequirementMet = true;

				if(theIsRangeRequirementMet)
				{
					theCanGestate = true;
				}
				else
				{
					// TODO: Add error message
				}
		}
        else
        {
            outErrorMessage = kNotWhileDigesting;
        }
    }
    
    return theCanGestate;
}

bool AvHPlayer::GetCanCommand(string& outErrorMessage)
{
    bool theCanCommand = false;

    // Jack up command station use time to avoid huge kick-everyone-off-server bug (I think this is an overflow issue)
    // I think the overflow issue is fixed, but design-wise, this prevents annoying pop-out/pop-back in attacking (Reaver popping)
    const int theCommandStationReuseTime = BALANCE_VAR(kCommandStationReuseTime);
    
    // Have we been banned from the command station?
    AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData();
    if(!theServerPlayerData || (theServerPlayerData->GetTimeVotedDown() == -1))
    {
        //if(!this->GetCurrentWeaponCannotHolster())
        if(this->m_pActiveItem)
        {
            CBasePlayerWeapon* theCurrentWeapon = (CBasePlayerWeapon *)this->m_pActiveItem->GetWeaponPtr();
            if(theCurrentWeapon && theCurrentWeapon->CanHolster())
            {
                float theLastTime = this->GetLastTimeInCommandStation();
                if((theLastTime == -1) || (gpGlobals->time > (theLastTime + theCommandStationReuseTime)))
                {
                    if(!this->GetIsEnsnared())
                    {
                        theCanCommand = true;
                    }
                }
                else
                {
                    outErrorMessage = kCommandStationWaitTime;
                }
            }
            else
            {
                outErrorMessage = kWeaponPreventingCommandStation;
            }
        }
    }
    else
    {
        outErrorMessage = kBannedFromCommand;
    }

    return theCanCommand;
}

bool AvHPlayer::GetCanReceiveResources() const
{
    bool theCanReceiveResources = true;

    if(this->GetIsAlien())
    {
        theCanReceiveResources = false;

        if(this->GetIsRelevant() && this->IsAlive())
        {
            theCanReceiveResources = true;
        }
    }

    return theCanReceiveResources;
}

int AvHPlayer::GetEffectivePlayerClass()
{
    AvHPlayerClass theEffectivePlayerClass = PLAYERCLASS_NONE;
    
    if(this->GetUser3() == AVH_USER3_COMMANDER_PLAYER)
    {
        theEffectivePlayerClass = PLAYERCLASS_COMMANDER;
    }
    // Players can be both reinforcing and observer, but reinforcing takes precedence
    else if(this->GetPlayMode() == PLAYMODE_REINFORCING)
    {
        theEffectivePlayerClass = PLAYERCLASS_REINFORCING;
    }
    // Digesting overrides other states
    else if(this->GetIsBeingDigested())
    {
        theEffectivePlayerClass = PLAYERCLASS_ALIVE_DIGESTING;
    }
    else if((this->pev->team == 0) && this->IsObserver())
    {
        theEffectivePlayerClass = PLAYERCLASS_SPECTATOR;
    }
    else
    {
        // Check team type
        if(this->GetIsMarine())
        {
            if(this->IsAlive())
            {
                if(this->GetHasHeavyArmor())
                {
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_HEAVY_MARINE;
                }
				else if (this->GetHasJetpack()) {
					theEffectivePlayerClass = PLAYERCLASS_ALIVE_JETPACK_MARINE;
				}
                else
                {
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_MARINE;
                }
            }
            else
            {
                theEffectivePlayerClass = PLAYERCLASS_DEAD_MARINE;
            }
        }
        else if(this->GetIsAlien())
        {
            if(this->IsAlive())
            {
                switch(this->pev->iuser3)
                {
                case AVH_USER3_ALIEN_PLAYER1:
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_LEVEL1;
                    break;
                case AVH_USER3_ALIEN_PLAYER2:
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_LEVEL2;
                    break;
                case AVH_USER3_ALIEN_PLAYER3:
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_LEVEL3;
                    break;
                case AVH_USER3_ALIEN_PLAYER4:
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_LEVEL4;
                    break;
                case AVH_USER3_ALIEN_PLAYER5:
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_LEVEL5;
                    break;
                case AVH_USER3_ALIEN_EMBRYO:
                    theEffectivePlayerClass = PLAYERCLASS_ALIVE_GESTATING;
                    break;
                }
            }
            else
            {
                theEffectivePlayerClass = PLAYERCLASS_DEAD_ALIEN;
            }
        }
    }
    
    return theEffectivePlayerClass;
}

AvHServerPlayerData* AvHPlayer::GetServerPlayerData()
{
    AvHServerPlayerData* theServerPlayerData = NULL;

    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam)
    {
        theServerPlayerData = theTeam->GetServerPlayerData(this->edict());
    }

    return theServerPlayerData;
}

bool AvHPlayer::GetCurrentWeaponCannotHolster() const
{
    bool theCannotHolster = false;
    
    if(m_pActiveItem)
    {
        theCannotHolster = !this->m_pActiveItem->CanHolster();
    }
    
    return theCannotHolster;
}

bool AvHPlayer::GetInReadyRoom() const
{
    return (this->GetPlayMode() == PLAYMODE_READYROOM);
}

bool AvHPlayer::GetIsAlien(bool inIncludeSpectating) const
{
    bool theIsAlien = false;
    AvHTeam* theTeam = this->GetTeamPointer(inIncludeSpectating);

    if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN))
    {
        theIsAlien = true;
    }

    return theIsAlien;
}

bool AvHPlayer::GetIsMarine(bool inIncludeSpectating) const
{
    bool theIsMarine = false;
    AvHTeam* theTeam = this->GetTeamPointer(inIncludeSpectating);
    
    if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE))
    {
        theIsMarine = true;
    }
    
    return theIsMarine;
}

bool AvHPlayer::GetIsInTopDownMode(bool inIncludeSpectating) const
{
    return this->mInTopDownMode;
}

AvHPlayer* AvHPlayer::GetCommander(void)
{
    AvHPlayer* theCommander = NULL;
    AvHTeam* theMarinesTeam = this->GetTeamPointer();
    if(theMarinesTeam)
    {
        int theCommanderIndex = theMarinesTeam->GetCommander();
        AvHSUGetEntityFromIndex(theCommanderIndex, theCommander);
    }
    
    return theCommander;
}

bool AvHPlayer::GetHasBeenSpectator(void) const
{
    return this->mHasBeenSpectator;
}

AvHPlayMode AvHPlayer::GetPlayMode(bool inIncludeSpectating) const
{
    int theVarToUse = this->pev->playerclass;

    if(inIncludeSpectating && (this->pev->iuser1 == OBS_IN_EYE))
    {
        AvHPlayer* theEntity = NULL;
        if(AvHSUGetEntityFromIndex(this->pev->iuser2, theEntity))
        {
            theVarToUse = theEntity->pev->playerclass;
        }
    }

    return (AvHPlayMode)theVarToUse;
}

bool AvHPlayer::GetIsValidReinforcementFor(AvHTeamNumber inTeam) const
{
    bool theSuccess = false;

    if(this->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT)
    {
        if(this->pev->team == inTeam)
        {
            theSuccess = true;
        }
    }

    return theSuccess;
}

int AvHPlayer::GetPointValue(void) const
{
    int thePointValue = BALANCE_VAR(kScoringKillPlayer);

    if(this->GetIsAlien())
    {   
        switch(this->pev->iuser3)
        {
        case AVH_USER3_ALIEN_PLAYER2:
            thePointValue = BALANCE_VAR(kScoringKillPlayerGorge);
            break;
        case AVH_USER3_ALIEN_PLAYER3:
            thePointValue = BALANCE_VAR(kScoringKillPlayerLerk);
            break;
        case AVH_USER3_ALIEN_PLAYER4:
            thePointValue = BALANCE_VAR(kScoringKillPlayerFade);
            break;
        case AVH_USER3_ALIEN_PLAYER5:
            thePointValue = BALANCE_VAR(kScoringKillPlayerOnos);
            break;
        }
    }
    else if(this->GetIsMarine())
    {
        if(this->GetHasJetpack())
        {
            thePointValue = BALANCE_VAR(kScoringKillPlayerJetpack);
        }
        else if(this->GetHasHeavyArmor())
        {
            thePointValue = BALANCE_VAR(kScoringKillPlayerHA);
        }
    }

    return thePointValue;
}

vec3_t AvHPlayer::GetVisualOrigin() const
{
    vec3_t theOrigin = this->pev->origin;

    //theOrigin[2] += this->pev->view_ofs[2];
    if(this->mInTopDownMode)
    {
        theOrigin[2] = GetGameRules()->GetMapExtents().GetMaxViewHeight();
    }
    
    return theOrigin;
}


int AvHPlayer::GetRespawnCost() const
{
    int theRespawnCost = 0;
    const AvHGameplay& theGameplay = GetGameRules()->GetGameplay();
    
    if(this->GetClassType() == AVH_CLASS_TYPE_MARINE)
    {
        // This function shouldn't be used now that there are reinforcements...rework a bit?
        theRespawnCost = 0;
    }
    else if(this->GetClassType() == AVH_CLASS_TYPE_ALIEN)
    {
        theRespawnCost = theGameplay.GetAlienRespawnCost();
    }
    return theRespawnCost;
}

bool AvHPlayer::GetHasItem(const char *szName)
{
    bool theHasItem = false;
    
    for(int i = 0; (i < MAX_ITEM_TYPES) && !theHasItem; i++)
    {
        CBasePlayerItem* theCurrentItem = this->m_rgpPlayerItems[i];
        while(theCurrentItem && !theHasItem)
        {
            if(FStrEq(STRING(theCurrentItem->pev->classname), szName))
            {
                theHasItem = true;
            }
            theCurrentItem = theCurrentItem->m_pNext;
        }
    }
    
    return theHasItem;
}

void AvHPlayer::GiveNamedItem(const char *szName, bool inSendMessage)
{
    // Check to make sure we don't have this weapon before giving it.  Fixes potential problems where
    // weapon settings would be reset if a duplicate weapon was given
    if(!this->GetHasItem(szName))
    {
        // Send visible pickup message after game has started
        bool theSendMessage = GetGameRules()->GetGameStarted();
        CBasePlayer::GiveNamedItem(szName, theSendMessage);
    }
}

int AvHPlayer::GetNumberOfItems()
{
    int theNumItems = 0;
    
    for(int i = 0; i < MAX_ITEM_TYPES; i++)
    {
        CBasePlayerItem* theCurrentItem = this->m_rgpPlayerItems[i];
        while(theCurrentItem)
        {
            theNumItems++;
            theCurrentItem = theCurrentItem->m_pNext;
        }
    }
    
    return theNumItems;
}

void AvHPlayer::GiveResources(float inResources)
{
    this->SetResources(this->GetResources() + inResources);
}

void AvHPlayer::StartLeap()
{
    // Make sure player has leap
    if(this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER1)
    {
        this->mTimeLeapEnd = gpGlobals->time + kLeapDuration;
    }
}

void AvHPlayer::PlayerTouch(CBaseEntity* inOther)
{
    if(this && this->pev && inOther && this->m_pActiveItem && this->m_pActiveItem->pev)
    {
        // Uncloak when we touch enemy players to prevent blocking
        if(inOther->IsPlayer() && inOther->IsAlive() && (inOther->pev->team != this->pev->team))
        {
            this->Uncloak();
        }

        // Don't do "touch" damage too quickly
        float theTouchDamageInterval = BALANCE_VAR(kTouchDamageInterval);
        if((this->mTimeOfLastTouchDamage == -1) || (gpGlobals->time > (this->mTimeOfLastTouchDamage + theTouchDamageInterval)))
        {
			entvars_t* theAttacker = this->pev;
			entvars_t* theInflictor = this->m_pActiveItem->pev;
			            
            float theScalar = 1.0f;
            if((this->mTimeLeapEnd != -1) && (gpGlobals->time < this->mTimeLeapEnd))
            {
                // Do damage to entity
                if(GetGameRules()->CanEntityDoDamageTo(this, inOther, &theScalar))
                {
                    float theDamage = BALANCE_VAR(kLeapDamage)*theScalar*theTouchDamageInterval;
                    inOther->TakeDamage(theInflictor, theAttacker, theDamage, NS_DMG_NORMAL);

                    this->mTimeOfLastTouchDamage = gpGlobals->time;
                }
            }
            
            // Are we charging?
            if(GetHasUpgrade(this->pev->iuser4, MASK_ALIEN_MOVEMENT) /*&& !this->GetIsBlinking()*/)
            {
                if(GetGameRules()->CanEntityDoDamageTo(this, inOther, &theScalar))
                {
                    float theDamage = BALANCE_VAR(kChargeDamage)*theScalar*theTouchDamageInterval;

                    inOther->TakeDamage(theInflictor, theAttacker, theDamage, NS_DMG_NORMAL);
            
                    if(inOther->IsPlayer() && !inOther->IsAlive())
                    {
                        EMIT_SOUND(ENT(this->pev), CHAN_WEAPON, kChargeKillSound, 1.0, ATTN_NORM);
                    }

                    this->mTimeOfLastTouchDamage = gpGlobals->time;
                }
            }
        }
    }
}

AvHTeamNumber AvHPlayer::GetTeam(bool inIncludeSpectating) const
{
    AvHTeamNumber theTeamNumber = (AvHTeamNumber)this->pev->team;

    if(inIncludeSpectating)
    {
        CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
        if(theSpectatingEntity)
        {
            theTeamNumber = (AvHTeamNumber)theSpectatingEntity->pev->team;
        }
    }

    return theTeamNumber;
}

AvHTeam* AvHPlayer::GetTeamPointer(bool inIncludeSpectating) const
{
    AvHTeamNumber theTeamNumber = this->GetTeam(inIncludeSpectating);

    AvHTeam* theTeamPointer = GetGameRules()->GetTeam(theTeamNumber);

    return theTeamPointer;
}

float AvHPlayer::GetKilledX() const
{
    return this->mKilledX;
}

float AvHPlayer::GetKilledY() const
{
    return this->mKilledY;
}

AvHClassType AvHPlayer::GetClassType(void) const
{
    AvHClassType theClassType = AVH_CLASS_TYPE_UNDEFINED;

    AvHTeamNumber theTeamNumber = (AvHTeamNumber)(this->pev->team);
    const AvHTeam* theTeam = GetGameRules()->GetTeam(theTeamNumber);
    if(theTeam)
    {
        theClassType = theTeam->GetTeamType();
    }

    ASSERT(theClassType >= AVH_CLASS_TYPE_UNDEFINED);
    ASSERT(theClassType <= AVH_CLASS_TYPE_AUTOASSIGN);

    return theClassType;
}


bool AvHPlayer::GetPurchaseAllowed(AvHMessageID inUpgrade, int& outCost, string* outErrorMessage) const
{
    bool thePurchaseAllowed = false;
    bool theGameStarted = GetGameRules()->GetGameStarted();
    bool theIsMarine = this->GetTeamPointer()->GetTeamType() == AVH_CLASS_TYPE_MARINE;
    bool theIsAlien = this->GetIsAlien();
    int theNumHives = this->GetTeamPointer()->GetNumActiveHives();
    bool theIsBuilder = (this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER2);
    bool theIsResearchTech = AvHSHUGetIsResearchTech(inUpgrade);
    bool theIsCombatMode = GetGameRules()->GetIsCombatMode();
    string theErrorMessage;

    // Check tech tree first
    const AvHTechTree* theTechNodes = &this->GetTeamPointer()->GetTechNodes();

    // If combat, use Combat nodes
    if(GetGameRules()->GetIsCombatMode())
    {
        theTechNodes = &this->mCombatNodes;
    }

    if(theTechNodes->GetIsMessageAvailable(inUpgrade))
    {
        if(theIsResearchTech && (this->pev->iuser3 == AVH_USER3_COMMANDER_PLAYER) && theIsMarine)
        {
            thePurchaseAllowed = true;
        }
        else
        {
            if(theIsCombatMode && (this->GetExperienceLevel() <= (this->mExperienceLevelsSpent + GetGameRules()->GetCostForMessageID(inUpgrade))))
            {
                return false;
            }
        
            switch(inUpgrade)
            {
            case ALIEN_EVOLUTION_ONE:
            case ALIEN_EVOLUTION_TWO:
            case ALIEN_EVOLUTION_THREE:
            //case ALIEN_EVOLUTION_FOUR:
            //case ALIEN_EVOLUTION_FIVE:
            //case ALIEN_EVOLUTION_SIX:
            case ALIEN_EVOLUTION_SEVEN:
            case ALIEN_EVOLUTION_EIGHT:
            case ALIEN_EVOLUTION_NINE:
            case ALIEN_EVOLUTION_TEN:
            case ALIEN_EVOLUTION_ELEVEN:
            case ALIEN_EVOLUTION_TWELVE:
                // If we are an alien and we don't have the upgrade
                if(theIsAlien)
                {
                    AvHUpgradeMask theUpgradeMask = MASK_NONE;
                    if(AvHGetAlienUpgradeMask(inUpgrade, theUpgradeMask))
                    {
                        if(!GetHasUpgrade(this->pev->iuser4, theUpgradeMask))
                        {
                            AvHAlienUpgradeCategory theCategory = ALIEN_UPGRADE_CATEGORY_INVALID;
                            if(AvHGetAlienUpgradeCategory(inUpgrade, theCategory))
                            {
                                // Now make sure we have an unspent upgrade available
                                AvHTeam* theTeamPointer = this->GetTeamPointer();
                                if(theTeamPointer && (AvHGetHasFreeUpgradeCategory(theCategory, theTeamPointer->GetAlienUpgrades(), this->pev->iuser4) || GetGameRules()->GetIsCombatMode()))
                                {
                                    thePurchaseAllowed = true;
                                }
                                else
                                {
                                    theErrorMessage = kUpgradeNotAvailable;
                                }
                            }
                        }
                    }
                }
                break;
        
            case ALIEN_LIFEFORM_ONE:
                if(theIsAlien && theGameStarted && (this->GetUser3() != AVH_USER3_ALIEN_PLAYER1))
                {
                    thePurchaseAllowed = true;
                }
                break;
            case ALIEN_LIFEFORM_TWO:
                if(theIsAlien && theGameStarted && (this->GetUser3() != AVH_USER3_ALIEN_PLAYER2))
                {
                    thePurchaseAllowed = true;
                }
                break;
            case ALIEN_LIFEFORM_THREE:
                if(theIsAlien && theGameStarted && (this->GetUser3() != AVH_USER3_ALIEN_PLAYER3))
                {
    //              if(theNumHives >= 1)
    //              {
                        thePurchaseAllowed = true;
    //              }
    //              else
    //              {
    //                  outErrorMessage = kNeedOneHiveToGestate;
    //              }
                }
                break;
            case ALIEN_LIFEFORM_FOUR:
                if(theIsAlien && theGameStarted && (this->GetUser3() != AVH_USER3_ALIEN_PLAYER4))
                {
    //              if(theNumHives >= 2)
    //              {
                        thePurchaseAllowed = true;
    //              }
    //              else
    //              {
    //                  outErrorMessage = kNeedTwoHivesToGestate;
    //              }
                }
                break;
            case ALIEN_LIFEFORM_FIVE:
                if(theIsAlien && theGameStarted && (this->GetUser3() != AVH_USER3_ALIEN_PLAYER5))
                {
    //              if(theNumHives >= 3)
    //              {
                        thePurchaseAllowed = true;
    //              }
    //              else
    //              {
    //                  outErrorMessage = kNeedThreeHivesToGestate;
    //              }
                }
                break;
        
            case ALIEN_BUILD_RESOURCES:
            case ALIEN_BUILD_HIVE:
                if(theIsBuilder)
                {
                    thePurchaseAllowed = true;
                }
                else
                {
                    theErrorMessage = kMustBeBuilder;
                }
                break;
        
            case ALIEN_BUILD_OFFENSE_CHAMBER:
                if(theIsBuilder)
                {
                    if(theNumHives >= 1)
                    {
                        thePurchaseAllowed = true;
                    }
                    else
                    {
                        theErrorMessage = kNeedOneHiveForStructure;
                    }
                }
                else
                {
                    theErrorMessage = kMustBeBuilder;
                }
                break;
        
            // Make sure we have a hive that can provide this tech
            case ALIEN_BUILD_DEFENSE_CHAMBER:
            case ALIEN_BUILD_MOVEMENT_CHAMBER:
            case ALIEN_BUILD_SENSORY_CHAMBER:
                if(theIsBuilder)
                {
                    FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
                        if(theEntity && theEntity->GetIsActive() && (theEntity->pev->team == this->pev->team))
                        {
                            AvHMessageID theTechnology = theEntity->GetTechnology();
                            if((theTechnology == inUpgrade) || (theTechnology == MESSAGE_NULL))
                            {
                                thePurchaseAllowed = true;
                                break;
                            }
                        }
                    END_FOR_ALL_ENTITIES(kesTeamHive);
        
                    if(!thePurchaseAllowed)
                    {
                        AvHTeam* theTeam = this->GetTeamPointer();
                        if(theTeam)
                        {
    //                      int theNumHives = theTeam->GetNumActiveHives();
    //                      switch(theNumHives)
    //                      {
    //                      case 0:
    //                          outErrorMessage = kNeedOneHiveForStructure;
    //                          break;
    //                      case 1:
    //                          outErrorMessage = kNeedTwoHivesForStructure;
    //                          break;
    //                      case 2:
    //                          outErrorMessage = kNeedThreeHivesForStructure;
    //                          break;
    //                      }
        
                            theErrorMessage = kNeedsAnotherHiveForStructure;
                        }
                    }
                }
                else
                {
                    theErrorMessage = kMustBeBuilder;
                }
                break;
        
            default:
                thePurchaseAllowed = true;
                break;
            }
        }
    }

    if(thePurchaseAllowed)
    {
        // This is either resources, energy, or levels
        int theCost = GetGameRules()->GetCostForMessageID(inUpgrade);
    
        if(GetGameRules()->GetIsCombatMode())
        {
            int theLevelsFree = max(this->GetExperienceLevel() - this->GetExperienceLevelsSpent() - 1, 0);
            if(theCost > theLevelsFree)
            {
                thePurchaseAllowed = false;
            }
        }
        else
        {
            if(AvHSHUGetDoesTechCostEnergy(inUpgrade) || GetGameRules()->GetIsCheatEnabled(kcLowCost))
            {
                theCost = 0;
            }
        
            if(this->GetResources() < theCost)
            {
                thePurchaseAllowed = false;
            }
        }

        outCost = theCost;
    }

    if(outErrorMessage)
    {
        *outErrorMessage = theErrorMessage;
    }

    return thePurchaseAllowed;
}

int AvHPlayer::GiveAmmo( int iAmount, char *szName, int iMax )
{
    int theReturnValue = -1;
    int theRealAmount = iAmount;
    string theRealName = szName;
    char theRealNameArray[256];
    int theRealMax = iMax;

    // TODO: Change this to a more generic call on AvHBasePlayerWeapon for next client update
    AvHBasePlayerWeapon* theBaseWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
    if(theBaseWeapon != NULL && theBaseWeapon->GetCanBeResupplied())
    {
        if(FStrEq(szName, kwsGenericAmmo))
        {
            AvHBasePlayerWeapon* theWeaponToGiveTo = theBaseWeapon;
            if(theBaseWeapon->UsesAmmo())
            {
                // Translate iAmount, szName and iMax into the ammo for our current weapon
                ItemInfo theItemInfo;
                if(this->m_pActiveItem->GetItemInfo(&theItemInfo) != 0)
                {
                    //theRealAmount = theBaseWeapon->m_iDefaultAmmo;
                    theRealAmount = theItemInfo.iMaxClip;
                    theRealName = theItemInfo.pszAmmo1;
                    theRealMax = theItemInfo.iMaxAmmo1;
                }
            }
        }
    }
    strcpy(theRealNameArray, theRealName.c_str());
    theReturnValue = CBasePlayer::GiveAmmo(theRealAmount, theRealNameArray, theRealMax);

    return theReturnValue;
}


bool AvHPlayer::GetShouldResupplyAmmo()
{
	bool theResupply = false;

    if(this->GetIsMarine() && this->GetIsRelevant())
    {
        AvHBasePlayerWeapon* theBaseWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
        if(theBaseWeapon != NULL && (theBaseWeapon->m_iId != AVH_WEAPON_MINE))
        {
            AvHBasePlayerWeapon* theWeaponToGiveTo = theBaseWeapon;
            if(theBaseWeapon->UsesAmmo())
            {
                // Translate iAmount, szName and iMax into the ammo for our current weapon
                ItemInfo theItemInfo;
                if(this->m_pActiveItem->GetItemInfo(&theItemInfo) != 0)
                {
                    int theMaxAmmo = theItemInfo.iMaxAmmo1;
                    if(theMaxAmmo > 0)
                    {
                        int i = GetAmmoIndex(theItemInfo.pszAmmo1);
                        
                        if ((i > 0) && (i < MAX_AMMO_SLOTS))
                        {
							
							int theCurrentAmmo = this->m_rgAmmo[i];

							if (theCurrentAmmo < theWeaponToGiveTo->GetClipSize()) {
								theResupply = true;
							}
                        }
                    }
                }
            }
        }
	}
	return theResupply;
}

float AvHPlayer::GetCurrentWeaponAmmoPercentage() 
{
    float thePercentage = 1.0f;

    if(this->GetIsMarine() && this->GetIsRelevant())
    {
        AvHBasePlayerWeapon* theBaseWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
        if(theBaseWeapon != NULL && (theBaseWeapon->m_iId != AVH_WEAPON_MINE))
        {
            AvHBasePlayerWeapon* theWeaponToGiveTo = theBaseWeapon;
            if(theBaseWeapon->UsesAmmo())
            {
                // Translate iAmount, szName and iMax into the ammo for our current weapon
                ItemInfo theItemInfo;
                if(this->m_pActiveItem->GetItemInfo(&theItemInfo) != 0)
                {
                    int theMaxAmmo = theItemInfo.iMaxAmmo1;
                    if(theMaxAmmo > 0)
                    {
                        int i = GetAmmoIndex(theItemInfo.pszAmmo1);
                        
                        if ((i > 0) && (i < MAX_AMMO_SLOTS))
                        {
                            int theCurrentAmmo = this->m_rgAmmo[i];// + theWeaponToGiveTo->GetShotsInClip();
                            thePercentage = (float)theCurrentAmmo/theMaxAmmo;
                        }
                    }
                }
            }
        }
    }

    return thePercentage;
}

bool AvHPlayer::GetEnemySighted(void) const
{
    return this->mEnemySighted;
}

bool AvHPlayer::GetIsFiring(void) const
{
    bool theIsFiring = false;
    if(this->pev->button & IN_ATTACK)
    {
        AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
        if(theWeapon && theWeapon->m_iClip > 0)
        {
            theIsFiring = true;
        }
    }
    return theIsFiring;
}

bool AvHPlayer::GetIsInOverwatch(void) const
{
    return this->mInOverwatch;
}

bool AvHPlayer::GetIsSpectatingTeam(AvHTeamNumber inTeamNumber) const
{
    bool theIsSpectatingTeam = false;

    CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
    if(theSpectatingEntity)
    {
        if((AvHTeamNumber)theSpectatingEntity->pev->team == inTeamNumber)
        {
            theIsSpectatingTeam = true;
        }
    }

    return theIsSpectatingTeam;
}

bool AvHPlayer::GetIsSpectatingPlayer(int inPlayerNumber) const
{
    bool theIsSpectatingPlayer = false;
    
    CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
    if(theSpectatingEntity)
    {
        if(theSpectatingEntity->entindex() == inPlayerNumber)
        {
            theIsSpectatingPlayer = true;
        }
    }
    
    return theIsSpectatingPlayer;
}

bool AvHPlayer::GetIsSpeaking(void) const
{
    return this->mIsSpeaking;
}

AvHMessageID AvHPlayer::GetLastSaying() const
{
    return this->mLastSaying;
}

bool AvHPlayer::GetOrdersRequested(void) const
{
    return this->mOrdersRequested;
}

bool AvHPlayer::GetOrderAcknowledged(void) const
{
    return this->mOrderAcknowledged;
}

string AvHPlayer::GetPlayerName() const
{
    string thePlayerName = (this->pev->netname && STRING(this->pev->netname)[0] != 0 ) ? STRING(this->pev->netname) : "unconnected" ;
    return thePlayerName;
}

int AvHPlayer::GetRelevantWeight(void) const
{
    float theRelevantWeight = 0;

    // Add current weapon if any
    //CBasePlayerWeapon* theActiveWeapon = dynamic_cast<CBasePlayerWeapon*>(this->m_pActiveItem);
    //if(theActiveWeapon)
    //{
    //  theRelevantWeight += this->GetRelevantWeightForWeapon(theActiveWeapon);
    //}

    // Run through items and tally up the ones that we count
    for(int i = 0; i< MAX_ITEM_TYPES; i++)
    {
        AvHBasePlayerWeapon* theCurrentWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_rgpPlayerItems[i]);
        while(theCurrentWeapon)
        {
            int theWeight = this->GetRelevantWeightForWeapon(theCurrentWeapon);

            // Active items count full, count less when stowed
            float theMultiplier = (theCurrentWeapon == this->m_pActiveItem) ? 1.0f : .7f;
            theRelevantWeight += theWeight*theMultiplier;
            theCurrentWeapon = dynamic_cast<AvHBasePlayerWeapon*>(theCurrentWeapon->m_pNext);
        }
    }

    return (int)(theRelevantWeight);
}

int AvHPlayer::GetRelevantWeightForWeapon(AvHBasePlayerWeapon* inWeapon) const
{
    ASSERT(inWeapon != NULL);

    AvHWeaponID theWeaponID = (AvHWeaponID)inWeapon->m_iId;
    int theNumRounds = 0;
    if(inWeapon->UsesAmmo())
    {
        int theAmmoIndex = inWeapon->PrimaryAmmoIndex();
        ASSERT(theAmmoIndex >= 0);
        ASSERT(theAmmoIndex < MAX_AMMO_SLOTS);
        theNumRounds = max(inWeapon->m_iClip + this->m_rgAmmo[theAmmoIndex], 0);
    }

    return GetGameRules()->GetWeightForItemAndAmmo(theWeaponID, theNumRounds);
}

AvHUser3 AvHPlayer::GetPreviousUser3(bool inIncludeSpectating) const
{
    
    AvHUser3 theUser3 = this->mPreviousUser3;

    if(inIncludeSpectating)
    {
        const CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
        if(theSpectatingEntity)
        {
            const AvHPlayer* theSpectatingPlayer = dynamic_cast<const AvHPlayer*>(theSpectatingEntity);
            if(theSpectatingPlayer)
            {
                theUser3 = theSpectatingPlayer->GetPreviousUser3();
            }
        }
    }

    return theUser3;

}

AvHUser3 AvHPlayer::GetUser3(bool inIncludeSpectating) const
{
    AvHUser3 theUser3 = (AvHUser3)this->pev->iuser3;

    if(inIncludeSpectating)
    {
        const CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
        if(theSpectatingEntity)
        {
            const AvHPlayer* theSpectatingPlayer = dynamic_cast<const AvHPlayer*>(theSpectatingEntity);
            if(theSpectatingPlayer)
            {
                theUser3 = theSpectatingPlayer->GetUser3();
            }
        }
    }

    return theUser3;
}

AvHMessageID AvHPlayer::GetEvolution(bool inIncludeSpectating) const
{

    AvHMessageID theEvolution = mEvolution;

    if(inIncludeSpectating)
    {
        const CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
        if(theSpectatingEntity)
        {
            const AvHPlayer* theSpectatingPlayer = dynamic_cast<const AvHPlayer*>(theSpectatingEntity);
            if(theSpectatingPlayer)
            {
                theEvolution = theSpectatingPlayer->GetEvolution();
            }
        }
    }

    return theEvolution;    
    
}

bool AvHPlayer::GetSpecialPASOrigin(Vector& outOrigin)
{
    bool theUseSpecial = (this->GetUser3() == AVH_USER3_COMMANDER_PLAYER);

    if(theUseSpecial)
    {
        VectorCopy(this->mSpecialPASOrigin, outOrigin);
    }

    return theUseSpecial;
}

// Don't check validity.  ASSERT if an error is encountered.
void AvHPlayer::GiveUpgrade(AvHMessageID inUpgrade)
{
}

void AvHPlayer::GiveTeamUpgrade(AvHMessageID inUpgrade, bool inGive)
{
    AvHUser3 theUser3 = (AvHUser3)this->pev->iuser3;
    int thePlayerLevel = this->GetExperienceLevel();
    float theHealthPercentage = this->pev->health/AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, theUser3, thePlayerLevel);
    float theArmorPercentage = this->pev->armorvalue/AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, theUser3);

    // If we're in combat mode, handle upgrades differently
    
    ProcessGenericUpgrade(this->pev->iuser4, inUpgrade, inGive);

    //AvHTeam* theTeam = this->GetTeamPointer();
    //if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE))
    //{
        AvHUpgradeMask theMask = ProcessGenericUpgrade(this->pev->iuser4, inUpgrade, inGive);

        if(inUpgrade == BUILD_HEAVY)
        {
            this->EffectivePlayerClassChanged();
        }
    //}

    this->pev->health = theHealthPercentage*AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, theUser3, thePlayerLevel);
    this->pev->armorvalue = theArmorPercentage*AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, theUser3);
}

float AvHPlayer::GetResources(bool inIncludeSpectating) const
{
    float theResources = this->mResources;

    if(inIncludeSpectating && (this->pev->iuser1 == OBS_IN_EYE))
    {
        AvHPlayer* theEntity = NULL;
        if(AvHSUGetEntityFromIndex(this->pev->iuser2, theEntity))
        {
            theResources = theEntity->GetResources(false);
        }
    }

    const AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam) 
    {
        if(theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE)
        {
            theResources = theTeam->GetTeamResources();
        }
    }
    
    return theResources;
}

void AvHPlayer::GetSpeeds(int& outBaseSpeed, int& outUnemcumberedSpeed) const
{
    // Use marine speed when in ready room (ie, AVH_CLASS_TYPE_UNDEFINED)
    //int theBaseSpeed = 160;
    // Counter-strike speeds are around 260 for knife running, and 215 with the para (according to Adrian's memory)
    int theBaseSpeed = BALANCE_VAR(kBasePlayerSpeed);
    int theUnencumberedSpeed = BALANCE_VAR(kUnencumberedPlayerSpeed);

    if(this->IsObserver())
    {
        theBaseSpeed = theUnencumberedSpeed = 1000;
    }
    else if(this->GetClassType() == AVH_CLASS_TYPE_MARINE)
    {
        if(this->pev->iuser3 == AVH_USER3_COMMANDER_PLAYER)
        {
            if(this->mInTopDownMode)
            {
                // For scrolling
                theBaseSpeed = theUnencumberedSpeed = BALANCE_VAR(kCommanderPlayerSpeed);
            }
        }
        else if(this->GetHasHeavyArmor())
        {
            float theHeavyMultiplier = BALANCE_VAR(kHeavySpeedMultiplier);
            theBaseSpeed *= theHeavyMultiplier;
            theUnencumberedSpeed *= theHeavyMultiplier;
        }
        else if(this->GetHasJetpack())
        {
            float theJetpackMultiplier = BALANCE_VAR(kJetpackSpeedMultiplier);
            theBaseSpeed *= theJetpackMultiplier;
            theUnencumberedSpeed *= theJetpackMultiplier;
        }
        
        if(GetHasUpgrade(this->pev->iuser4, MASK_BUFFED))
        {
            const float kStimpackSpeedMultiplier = 1 + BALANCE_VAR(kCatalystSpeedIncrease);
            theBaseSpeed *= kStimpackSpeedMultiplier;
            theUnencumberedSpeed *= kStimpackSpeedMultiplier;
        }
    }
    else if(this->GetClassType() == AVH_CLASS_TYPE_ALIEN)
    {
        int theSpeedUpgradeLevel = 0;
        if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_4))
        {
            theSpeedUpgradeLevel = 1;

            if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_12))
            {
                theSpeedUpgradeLevel = 2;
            }
            else if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_13))
            {
                theSpeedUpgradeLevel = 3;
            }
        }

        // When gestating
        float theAlienBaseSpeed = 0;
        int theSpeedUpgradeAmount = BALANCE_VAR(kAlienCelerityBonus);
        const float kChargingFactor = 2.0f;

        switch(this->pev->iuser3)
        {
            // Take into account speed upgrade, if this player has it
            case AVH_USER3_ALIEN_PLAYER1:
                theAlienBaseSpeed = BALANCE_VAR(kSkulkBaseSpeed);
                break;

            case AVH_USER3_ALIEN_PLAYER2:
                theAlienBaseSpeed = BALANCE_VAR(kGorgeBaseSpeed);
                break;

            case AVH_USER3_ALIEN_PLAYER3:
                theAlienBaseSpeed = BALANCE_VAR(kLerkBaseSpeed);
                
                // Compensate for airpseed multiplier, so lerk gets proper speed increase in main mode of locomotion
                //theSpeedUpgradeAmount /= BALANCE_VAR(kAirspeedMultiplier);
                break;

            case AVH_USER3_ALIEN_PLAYER4:
                theAlienBaseSpeed = BALANCE_VAR(kFadeBaseSpeed);
                break;
            
            case AVH_USER3_ALIEN_PLAYER5:
                //theAlienBaseSpeed = this->mMaxGallopSpeed;
                theAlienBaseSpeed = BALANCE_VAR(kOnosBaseSpeed);

                if(GetHasUpgrade(this->pev->iuser4, MASK_ALIEN_MOVEMENT))
                {
                    theAlienBaseSpeed *= kChargingFactor;
                    theSpeedUpgradeAmount *= kChargingFactor;
                }
                break;
        }

        theUnencumberedSpeed = theBaseSpeed = theAlienBaseSpeed + (theSpeedUpgradeLevel*theSpeedUpgradeAmount);

        // Update kMaxPlayerSpeed to make sure footsteps are played correctly
        ASSERT(theUnencumberedSpeed <= kMaxGroundPlayerSpeed);
    }

    outBaseSpeed = theBaseSpeed;
    outUnemcumberedSpeed = theUnencumberedSpeed;

    // Take into account ensnare
    if(this->GetIsEnsnared())
    {
        float thePercentageComplete = (gpGlobals->time - this->mLastTimeEnsnared)/(this->mTimeToBeUnensnared - this->mLastTimeEnsnared);
        thePercentageComplete = max(min(thePercentageComplete, 1.0f), 0.0f);
        //ASSERT(thePercentageComplete >= 0.0f);
        //ASSERT(thePercentageComplete <= 1.0f);
        
        float theSpeedFactor = min(BALANCE_VAR(kEnsnareMinimumSpeedFactor) + (1.0f - BALANCE_VAR(kEnsnareMinimumSpeedFactor))*thePercentageComplete, BALANCE_VAR(kEnsnareMaximumSpeedFactor));
        ASSERT(theSpeedFactor >= BALANCE_VAR(kEnsnareMinimumSpeedFactor));
        ASSERT(theSpeedFactor <= 1.0f);
        
        outBaseSpeed *= theSpeedFactor;
        outUnemcumberedSpeed *= theSpeedFactor;
    }

    // Reduce speed a bit when player has carapace
    if(this->GetIsAlien())
    {
        int theCarapaceLevel = AvHPlayerUpgrade::GetArmorUpgrade((AvHUser3)this->pev->iuser3, this->pev->iuser4);
        float theSpeedFactor = (1.0f - BALANCE_VAR(kCarapaceSlowFactor)*theCarapaceLevel);

        outBaseSpeed *= theSpeedFactor;
        outUnemcumberedSpeed *= theSpeedFactor;
    }

    if(this->GetIsMetabolizing())
    {
        float theMetabolizeSpeedFactor = BALANCE_VAR(kMetabolizeSpeedFactor);
        outBaseSpeed *= theMetabolizeSpeedFactor;
        outUnemcumberedSpeed *= theMetabolizeSpeedFactor;
    }

    if(this->GetIsDigesting())
    {
        float theDigestingSpeedFactor = BALANCE_VAR(kDigestingSpeedFactor);
        outBaseSpeed *= theDigestingSpeedFactor;
        outUnemcumberedSpeed *= theDigestingSpeedFactor;
    }
    
    if(this->GetIsStunned())
    {
        // MUHAHAHA!
        outBaseSpeed = outUnemcumberedSpeed = 0.0f;
    }

    // If we're a fade using blink, increase our speed massively
//  if(this->GetIsBlinking())
//  {
//      outBaseSpeed *= 10.0f;
//      outUnemcumberedSpeed = outBaseSpeed;
//  }

//    if(GetGameRules()->GetIsCombatMode())
//    {
//        int theSpeedIncrease = (this->GetExperienceLevel() - 1)*BALANCE_VAR(kCombatLevelSpeedIncrease);
//
//        outBaseSpeed += theSpeedIncrease;
//        outUnemcumberedSpeed += theSpeedIncrease;
//    }

    if(GetGameRules()->GetIsCheatEnabled(kcHighSpeed))
    {
        outBaseSpeed = outUnemcumberedSpeed = kMaxGroundPlayerSpeed*.7f;
    }
}

int AvHPlayer::GetMaxWalkSpeed() const
{
    return this->mMaxWalkSpeed;
}

void AvHPlayer::PickSkin()
{
    int theSkin = 0;
                   
    if(this->GetIsMarine())
    {
        theSkin = RANDOM_LONG(0, 1);
    }
    
    this->SetSkin(theSkin);
}

void AvHPlayer::SetSkin(int inSkin)
{
    if(this->pev)
    {
        this->pev->skin = inSkin;
    }
}

void AvHPlayer::GiveOrderToSelection(AvHOrder& inOrder)
{
    // Set the player list as the selected players
    EntityListType theReceivers = this->mSelected;
    
    // Set the order for the team
    AvHTeam* theTeam = this->GetTeamPointer();
    ASSERT(theTeam);

	int theOrderID=inOrder.GetOrderID();
	if(GetGameRules()->GetIsCheatEnabled(kcOrderSelf))
    {
		AvHOrder theOrder=inOrder;
		int theSelfIndex = this->entindex();
	    theOrder.SetReceiver(theSelfIndex);
		theOrder.SetOrderID();
	    theTeam->SetOrder(theOrder);

	}
	if ( this->mSelected.size() > 0 ) {
		for(EntityListType::iterator theIter = theReceivers.begin(); theIter != theReceivers.end(); theIter++)
		{
			if ( inOrder.GetTargetIndex() != *theIter ) 
			{
				AvHOrder theOrder=inOrder;
				theOrder.SetReceiver(*theIter);
				theOrder.SetOrderID();
				theTeam->SetOrder(theOrder);
			}
		}
	}
    

    // set this to true to indicate they don't need help
    this->mHasGivenOrder = true;
}

bool AvHPlayer::GiveOrderToSelection(AvHOrderType inOrder, Vector inNormRay)
{
    bool theSuccess = false;
    AvHOrder theNewOrder;

    Vector theOrigin = this->GetVisualOrigin();

//  #ifdef DEBUG
//  vec3_t theStartPoint;
//  VectorMA(theOrigin, kSelectionStartRange, inNormRay, theStartPoint);
//  
//  vec3_t theEndPoint;
//  VectorMA(theOrigin, kSelectionEndRange, inNormRay, theEndPoint);
//  
//  vec3_t theValidOrigin;
//  AvHSHUServerGetFirstNonSolidPoint(theStartPoint, theEndPoint, theValidOrigin);
//
//  theValidOrigin.z -= BALANCE_VAR(kBiteDamage);
//
//  CBaseEntity* pEnt = CBaseEntity::Create(kwsDebugEntity, theValidOrigin, Vector(0, 0, 0));
//  ASSERT(pEnt);
//  pEnt->pev->movetype = MOVETYPE_FLY;
//  pEnt->pev->solid = SOLID_NOT;
//  #endif

    if(AvHCreateSpecificOrder((AvHTeamNumber)(this->pev->team), theOrigin, inOrder, inNormRay, theNewOrder))
    {
        this->GiveOrderToSelection(theNewOrder);

        theSuccess = true;
    }

    return theSuccess;
}

void AvHPlayer::HolsterCurrent()
{
    if(this->m_pActiveItem)
    {
        
        CBasePlayerWeapon* theCurrentWeapon = (CBasePlayerWeapon *)this->m_pActiveItem->GetWeaponPtr();

        if(theCurrentWeapon && theCurrentWeapon->CanHolster())
        {
            theCurrentWeapon->Holster();
        }

    }
}

void AvHPlayer::Kick()
{
    char theCommandBuffer[256];
    sprintf(theCommandBuffer, "kick \"%s\"\n", STRING(this->pev->netname));
    SERVER_COMMAND(theCommandBuffer);
}

void AvHPlayer::ImpulseCommands()
{
    AvHMessageID theMessage = (AvHMessageID)pev->impulse;
    
    this->PlayerUse();

    bool theHandledMessage = false;

    if(theMessage != MESSAGE_NULL)
    {
        // These things can only happen during play, not in ready room, reinforcement or observer modes
        if(this->GetPlayMode() == PLAYMODE_PLAYING || (GetGameRules()->GetCheatsEnabled()))
        {
            if(this->ExecuteMessage(theMessage))
            {
                theHandledMessage = true;
            }
        }
        
        if(!theHandledMessage)
        {
            CBasePlayer::ImpulseCommands();
        }
        
        // Very important I found out
        this->pev->impulse = 0;
    }
}

void AvHPlayer::ItemPostFrame(void)
{
    // Check if player tried to do something while we were in the ready room.  If so, display tutorial message.
    if(this->GetPlayMode() == PLAYMODE_READYROOM)
    {
        if((this->pev->button & IN_ATTACK) || (this->pev->button & IN_ATTACK2) || (this->pev->button & IN_RELOAD))
        {
            this->SendMessageOnce(kReadyRoomMessage, TOOLTIP);
        }
    }

    if(!this->GetIsBeingDigested())
    {
        CBasePlayer::ItemPostFrame();
    }
    
    this->UpdateAmbientSounds();
}

void AvHPlayer::Init()
{
    int i;
    
    // Copy the server variables from the game rules.

    AvHGamerules* theGameRules = GetGameRules();

    mServerVariableList.clear();
    
    for (i = 0; i < theGameRules->GetNumServerVariables(); ++i)
    {
        mServerVariableList.push_back(ServerVariable());
        mServerVariableList.back().mName = theGameRules->GetServerVariable(i);
    }
    
    // Reset to default team
    strcpy(this->m_szTeamName, kUndefinedTeam);

    this->mResources = 0;
    this->mScore = 0;
    this->mSavedCombatFrags = 0;
    this->mLastModelIndex = -1;

    this->mFirstUpdate = true;
    this->mNewMap = true;

    this->mHasSeenTeamA = false;
    this->mHasSeenTeamB = false;

    this->mPendingCommand = NULL;
    this->mIsSpeaking = false;
    this->mOrdersRequested = false;
    this->mOrderAcknowledged = false;
    this->mEnemySighted = false;
    this->mTriggerUncloak = false;
    this->mTimeOfLastSaying = 0;
    this->mLastSaying = MESSAGE_NULL;
    this->mTimeOfLastEnemySighting = 0;
    this->mClientInTopDownMode = false;
    this->mInTopDownMode = false;
    this->mClientCommander = -1;

    vec3_t theOrigin( 0, 0, 0 );
    VectorCopy(theOrigin, this->mPositionBeforeTopDown);
    VectorCopy(theOrigin, this->mAnglesBeforeTopDown);
    VectorCopy(theOrigin, this->mViewAnglesBeforeTopDown);
    VectorCopy(theOrigin, this->mViewOfsBeforeTopDown);
    VectorCopy(theOrigin, this->mLastGallopViewDirection);
    this->mAnimExtensionBeforeTopDown = "";
    this->mTimeStartedTopDown = -1;

    if(this->pev)
    {
        this->pev->team = TEAM_IND;
        this->ClearUserVariables();

        this->pev->rendermode = kRenderNormal;
        this->pev->renderfx = kRenderFxNone;
        this->pev->renderamt = 0;
        this->pev->skin = 0;
        this->pev->solid = SOLID_NOT;
        this->pev->frags = 0;
        this->m_iDeaths = 0;
        this->pev->playerclass = PLAYMODE_UNDEFINED;
    }

    this->mClientInOverwatch = false;
    this->mInOverwatch = false;
    this->mOverwatchEnabled = true;
    this->mOverwatchTarget = -1;
    this->mTimeOfLastOverwatchPreventingAction = -1;
    this->mTimeLastSeenOverwatchTarget = 0;
    this->mOverwatchFiredThisThink = false;

	// tankefugl: 0000953 
	this->mTimeLastJoinTeam = -1;
	// tankefugl

    // alien upgrades
    this->mTimeOfLastRegeneration = -1;
    this->mTimeOfLastPheromone = -1;
    this->mMaxGallopSpeed = 0;

    this->mClientResearchingTech = MESSAGE_NULL;

    this->mAttackOneDown = false;
    this->mAttackTwoDown = false;
    this->mPlacingBuilding = false;

    this->mPreviousUser3 = AVH_USER3_NONE;
    this->mSavedJetpackEnergy = 0;

    this->mHasBeenSpectator = false;
    this->mHasLeftReadyRoom = false;
    this->mQueuedThinkMessage = "";

    // Clear out tech nodes
    this->mClientTechNodes.Clear();
    this->mClientTechSlotList.clear();

    // Clear out map locations
    this->mClientInfoLocations.clear();

    // Clear out hive info
    this->mClientHiveInfo.clear();

    this->mClientGamma = kDefaultMapGamma;
    
    // Clear sent message list
    // Don't clear this message, don't keep sending tutorial messages
    //this->mSentMessageList.clear();
    
    //this->mClientBlipList.clear();
    //this->mBlipList.clear();

    this->mAlienSightActive = false;
    this->mNumHives = 0;

    this->mSpecialPASOrigin.x = this->mSpecialPASOrigin.y = this->mSpecialPASOrigin.z = 0.0f;
    this->mClientSpecialPASOrigin.x = this->mClientSpecialPASOrigin.y = this->mClientSpecialPASOrigin.z = 0.0f;
    this->mTimeOfLastPASUpdate = -1;

	// puzl: 984
	// record the last time the player attempted to go to the readyroom
	this->mTimeOfLastF4 = -1.0f;

    this->mTimeOfLastTeleport = -1;
    this->mTimeOfLastHelpText = -1;
    this->mTimeOfLastUse = -1;
    this->mTimeLeapEnd = -1;
    this->mTimeOfLastRedeem = -1;

    memset(&this->mClientDebugCSPInfo, 0, sizeof(weapon_data_t));
    memset(&this->mDebugCSPInfo, 0, sizeof(weapon_data_t));
    this->mClientNextAttack = 0;

    this->mMaxWalkSpeed = CBasePlayer::GetMaxWalkSpeed();
    this->mTimeToBeUnensnared = -1;
    this->mLastTimeEnsnared = -1;
    this->mLastTimeStartedPlaying = -1;

    this->mTimeToBeFreeToMove = -1;
    this->mTimeToEndCatalyst = -1;
    
    this->mLastTimeInCommandStation = -1;
    this->mLastTimeRedemptionTriggered = -1;
    this->mLastTimeCheckedRedemption = -1;

    this->mJetpackEnergy = 1.0f;
    this->mJetpacking = false;
    this->mLastPowerArmorThink = -1;
    this->mLastInventoryThink = -1;

    this->mTimeLastPlaying = -1;
    this->mTimeGestationStarted = -1;

    this->mEvolution = MESSAGE_NULL;
    this->mHealthPercentBefore = -1;
    this->mArmorPercentBefore = -1;

    this->mTimeStartedScream = -1;
    //this->mNumParticleTemplatesSent = 0;
    //this->mTimeOfLastParticleTemplateSending = -1;

    this->mEnemyBlips.Clear();
    this->mFriendlyBlips.Clear();

    this->mSelected.clear();
    this->mClientTrackingEntity = this->mTrackingEntity = 0;
    this->mClientSelectAllGroup.clear();

    for(i = 0; i < kNumHotkeyGroups; i++)
    {
        this->mClientGroups[i].clear();
        this->mClientGroupAlerts[i] = ALERT_NONE;
    }

    this->mProgressBarEntityIndex = -1;
    // Don't set this, must propagate
    //this->mClientProgressBarEntityIndex = -1;
    this->mProgressBarParam = -1;
    this->mTimeProgressBarTriggered = -1;
    this->mTimeOfLastFogTrigger = -1;
    this->mFogExpireTime = -1;
    this->mCurrentFogEntity = -1;

    // Don't set this, we need to propagate lack of fog
    //this->mClientCurrentFogEntity = -1;

    //this->mUpgrades.clear();
    this->mClientOrders.clear();
    
    this->mMouseWorldPos = this->mAttackOnePressedWorldPos = this->mAttackTwoPressedWorldPos = theOrigin;

    this->mClientEntityHierarchy.Clear();

    this->mKilledX = this->mKilledY = -1;

    this->mHasGivenOrder = false;
    this->mTimeOfLastSignificantCommanderAction = -1;
    this->mPreThinkTicks = 0;

    this->mDesiredNetName = "";

    this->mTimeOfLastClassAndTeamUpdate = -1;
    this->mEffectivePlayerClassChanged = false;
    this->mNeedsTeamUpdate = false;
    this->mSendTeamUpdate = false;
    this->mSendSpawnScreenFade = false;
    this->mClientMenuTechSlots = 0;

    memset(&this->mClientRequests, 0, sizeof(int)*kNumRequestTypes);

    this->mDigestee = 0;
    this->mTimeOfLastDigestDamage = 0;
    this->mTimeOfLastCombatThink = 0;
    this->mDesiredRoomType = this->mClientDesiredRoomType = 0;

    this->mTimeOfLastConstructUseAnimation = 0;
    this->mTimeOfLastConstructUse = -1;
    this->mTimeOfLastResupply = 0;

    this->mTimeOfMetabolizeEnd = -1;

    this->m_DefaultSpectatingMode = OBS_IN_EYE;
    this->m_DefaultSpectatingTarget = 1;

    this->mLastSelectEvent = MESSAGE_NULL;
    VectorCopy(g_vecZero, this->mPositionBeforeLastGotoGroup);

    this->mTimeOfLastSporeDamage = -1;
    this->mTimeOfLastTouchDamage = -1;

    this->mLastMessageSent = "";

    this->mExperience = 0.0;
    this->mExperienceLevelsSpent = 0;
    this->mClientExperienceLevelsSpent = 0;
    this->mClientPercentToNextLevel = 0.0f;
    this->mCombatNodes.Clear();
    this->mPurchasedCombatUpgrades.clear();
    this->mGiveCombatUpgrades.clear();
}

void AvHPlayer::InitializeFromTeam(float inHealthPercentage, float inArmorPercentage)
{
    // Set base health and armor
    int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
    this->pev->health = this->pev->max_health = max(theMaxHealth*inHealthPercentage,1.0f);//voogru: prevent bug with players evolving down from higher lifeform from getting negative health but still "alive"

    this->pev->armorvalue = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3)*inArmorPercentage;
    
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam)
    {
        ASSERT(theTeam);
        
        // If he's already on the team, this does nothing (his rank keeps over death)
        if(theTeam->AddPlayer(this->entindex()))
        {
            GetGameRules()->RecalculateHandicap();
        
            float theStartingPoints = theTeam->GetInitialPlayerPoints(this->edict());
            if(this->GetIsMarine())
            {
                theStartingPoints += this->GetResources();
            }

            // Set starting resources
            this->SetResources(theStartingPoints, false);

            // If we're in combat mode, we use our own tech nodes instead of our team's
            if(GetGameRules()->GetIsCombatMode())
            {
                AvHTechTree theInitialTechNodes = theTeam->GetTechNodes();

                // Removed restoration of previous levels and experience due to abuse
                //// Restore saved/spent nodes if we've already been playing
                //AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData();
                //if(theServerPlayerData)
                //{
                //    AvHTechNodes theSavedTechNodes = theServerPlayerData->GetCombatNodes();
                //    if(theSavedTechNodes.GetNumNodes() > 0)
                //    {
                //        theInitialTechNodes = theSavedTechNodes;
                //    }
                //            
                //    int a = this->mExperienceLevelsSpent;
                //
                //    this->SetExperienceLevelsSpent(theServerPlayerData->GetExperienceLevelsSpent());
                //
                //    MessageIDListType thePurchasedCombatUpgrades = theServerPlayerData->GetPurchasedCombatUpgrades();
                //    for(MessageIDListType::iterator theIter = thePurchasedCombatUpgrades.begin(); theIter != thePurchasedCombatUpgrades.end(); theIter++)
                //    {
                //        this->PurchaseCombatUpgrade(*theIter);
                //    }
                //}

                // Save it
                this->SetCombatNodes(theInitialTechNodes);

                // Set initial experience (restore old experience if player disconnected)
                this->mExperience = theTeam->GetInitialExperience(this->edict());
            }

            // Random multiracial marines 
            this->PickSkin();
        }
    }
}

bool AvHPlayer::GetIsRelevant(bool inIncludeSpectating) const
{
    bool theIsRelevant = false;

    // Use ourself unless we're spectating
    const AvHPlayer* thePlayer = this;

    if(inIncludeSpectating)
    {
        const CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
        if(theSpectatingEntity)
        {
            const AvHPlayer* theSpectatingPlayer = dynamic_cast<const AvHPlayer*>(theSpectatingEntity);
            if(theSpectatingPlayer)
            {
                thePlayer = theSpectatingPlayer;
            }
        }
    }
    
    if(thePlayer->IsAlive() && (thePlayer->GetPlayMode() == PLAYMODE_PLAYING) && !thePlayer->GetIsSpectator() && (thePlayer->pev->team != TEAM_IND))
    {
        theIsRelevant = true;
    }
    
    return theIsRelevant;
}

bool AvHPlayer::GetCanBeAffectedByEnemies() const
{
    bool theCanBeAffected = this->GetIsRelevant(false) && !this->GetIsTemporarilyInvulnerable() && !this->GetIsBeingDigested() && !this->GetIsInTopDownMode();
    return theCanBeAffected;
}

float AvHPlayer::GetOpacity() const
{
    float theOpacity = AvHCloakable::GetOpacity();

    //if(this->GetIsBlinking())
    //{
    //  theOpacity = .05f;
    //}

    return theOpacity;
}

bool AvHPlayer::GetIsMetabolizing() const
{
    bool theIsMetabolizing = false;

    if((gpGlobals->time < this->mTimeOfMetabolizeEnd) && (this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER4))
    {
        theIsMetabolizing = true;
    }

    return theIsMetabolizing;
}

void AvHPlayer::SetTimeOfMetabolizeEnd(float inTime)
{
    this->mTimeOfMetabolizeEnd = inTime;
}

bool AvHPlayer::GetIsSelected(int inEntityIndex) const
{
    bool theIsSelected = false;

    for(EntityListType::const_iterator theIter = this->mSelected.begin(); theIter != this->mSelected.end(); theIter++)
    {
        if(*theIter == inEntityIndex)
        {
            theIsSelected = true;
            break;
        }
    }

    return theIsSelected;
}

bool AvHPlayer::RemoveSelection(int inEntityIndex)
{
    bool theSuccess = false;

    EntityListType::iterator theFoundIter = std::find(this->mSelected.begin(), this->mSelected.end(), (EntityInfo)inEntityIndex);
    if(theFoundIter != this->mSelected.end())
    {
        this->mSelected.erase(theFoundIter);
        theSuccess = true;
    }

    return theSuccess;
}

void AvHPlayer::SetSelection(int inEntityIndex, bool inClearPreviousSelection)
{
    if(inClearPreviousSelection)
    {
        this->mSelected.clear();
    }

    this->mSelected.push_back(inEntityIndex);
}


bool AvHPlayer::GetIsSpectator() const
{
    return (this->pev->flags & FL_PROXY);
}

void AvHPlayer::SetIsSpectator()
{
    this->pev->flags |= FL_PROXY;
}

float AvHPlayer::GetLastTimeInCommandStation() const
{
    return this->mLastTimeInCommandStation;
}

// Clears the player's impulse and sends "invalid action" notification if not a legal action
void AvHPlayer::ValidateClientMoveEvents()
{
    // If player tries to execute an alien ability that they don't have, stop them dead.
    AvHMessageID theMessageID = (AvHMessageID)this->mCurrentCommand.impulse;

    if(theMessageID != MESSAGE_NULL)
    {
        // Assume it's invalid
        bool theIsValid = false;
        this->mCurrentCommand.impulse = MESSAGE_NULL;
        this->pev->impulse = MESSAGE_NULL;
        const float kMinSayingInterval = 3.0f;

        // Process universally allowable impulses
        switch(theMessageID)
        {
        case COMM_CHAT_PUBLIC:
        case COMM_CHAT_TEAM:
        case COMM_CHAT_NEARBY:
        case IMPULSE_FLASHLIGHT:
        case IMPULSE_SPRAYPAINT:
        case IMPULSE_DEMORECORD:
        case ADMIN_READYROOM:
            theIsValid = true;
            break;

        case SAYING_1:
        case SAYING_2:
        case SAYING_3:
        case SAYING_4:
        case SAYING_5:
        case SAYING_6:
        case SAYING_7:
        case SAYING_8:
        case SAYING_9:
// tankefugl: 0000008
// preventing spamming of request and ack
		case ORDER_REQUEST:
        case ORDER_ACK:
// :tankefugl
			if(GetGameRules()->GetCheatsEnabled() || (gpGlobals->time > (this->mTimeOfLastSaying + kMinSayingInterval)))
            {
                theIsValid = true;
            }
            else
            {
                int a = 0;
            }
            break;

        default:
            if(GetGameRules()->GetIsCombatMode())
            {
                theIsValid = true;
            }
            break;
        }
        
        // If not universally allowable
        AvHTeam* theTeam = this->GetTeamPointer();
        if(!theIsValid && theTeam)
        {
            AvHUser3 theUser3 = this->GetUser3();
        
            // Marines
            if(this->GetIsMarine())
            {
                // Soldiers
                if((theUser3 == AVH_USER3_MARINE_PLAYER) && this->IsAlive())
                {
                    switch(theMessageID)
                    {
                    // Validate orders
					// tankefugl: 0000008
					// preventing spamming of request and ack
                    //case ORDER_REQUEST:
                    //case ORDER_ACK:
					// :tankefugl
                        
                    // Validate weapon switches
                    case WEAPON_NEXT:
                    case WEAPON_RELOAD:
                    case WEAPON_DROP:

                    // Only soldiers can vote
                    case ADMIN_VOTEDOWNCOMMANDER:
                        theIsValid = true;
                    }
                }
                // Commanders
                else if(theUser3 == AVH_USER3_COMMANDER_PLAYER)
                {
                    switch(theMessageID)
                    {
                    // Validate commander movement and input
                    case COMMANDER_MOUSECOORD:
                    case COMMANDER_MOVETO:
                    case COMMANDER_SCROLL:
                    case COMMANDER_DEFAULTORDER:
                    case COMMANDER_SELECTALL:
                    case COMMANDER_REMOVESELECTION:

                    // Validate hotgroup creation and selection
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

                    // Special impulses to handle requests
                    case COMMANDER_NEXTIDLE:
                    case COMMANDER_NEXTAMMO:
                    case COMMANDER_NEXTHEALTH:
                        theIsValid = true;
                    }
                    
                    // Validate research and building
                    // Make sure it comes from the commander and also that the right building is selected and not busy
                    if(AvHSHUGetIsBuildTech(theMessageID) || AvHSHUGetIsResearchTech(theMessageID) || AvHSHUGetDoesTechCostEnergy(theMessageID) || (theMessageID == BUILD_RECYCLE))
                    {
                        if(theTeam->GetTechNodes().GetIsMessageAvailableForSelection(theMessageID, this->mSelected))
                        {
                            // Make sure only one structure is selected, and make sure the structure that's selected allows this action
							if(AvHSHUGetIsBuildTech(theMessageID)) //doesn't depend on selection
							{
								theIsValid = true;
							}
							else
							{
								if(this->mSelected.size() == 1)
								{
									int theEntityIndex = *this->mSelected.begin();
									CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theEntityIndex));
									AvHBaseBuildable* theBaseBuildable = dynamic_cast<AvHBaseBuildable*>(theEntity);
									if(theBaseBuildable && theBaseBuildable->GetIsTechnologyAvailable(theMessageID))
									{
										theIsValid = true;
									}
								}
                            }
                        }
                    }
                }
            }
            // Aliens
            else if(this->GetIsAlien() && this->IsAlive())
            {
                //AvHAlienAbilityWeapon* theAlienAbilityWeapon = dynamic_cast<AvHAlienAbilityWeapon*>(this->m_pActiveItem);
                
                switch(theMessageID)
                {
                    // Validate all alien abilities
                case ALIEN_ABILITY_LEAP:
                case ALIEN_ABILITY_CHARGE:
                    // Check that weapon is active
                    if(this->GetHasActiveAlienWeaponWithImpulse(theMessageID))
					{
                        // Check that we have enough energy, and that it's enabled
                        //if(theAlienAbilityWeapon->IsUseable())
                        //{
                        // Deduct energy cost
                        //ItemInfo theItemInfo;
                        //theAlienAbilityWeapon->GetItemInfo(&theItemInfo);
                        //float theEnergy = AvHMUGetEnergyCost(theItemInfo.iId);
                        theIsValid = true;
                        //}
                    }

                    // Removed by mmcguire.
                    // This code is no longer needed because the pm_shared code
                    // now ignores ability impulses that come from the console.
                    
                    /*
                    else
                    {
                        // If players are trying to use the abilities when not equipped via the impulse key, stop them dead (or worse?)
                        //VectorCopy(g_vecZero, this->pev->velocity);
                        //this->pev->fixangle = TRUE;
                        this->Killed(this->pev, 1);
                        
                        char theString[512];
                        sprintf(theString, "Player \"%s\" executed impulse %d illegally and was killed.\n", STRING(this->pev->netname), theMessageID);
                        ALERT(at_logged, theString);
                    }
                    */
                    break;

                case ALIEN_ABILITY_BLINK:
                    theIsValid = true;
                    break;
                    
                    // Validate lifeform changes and evolutions
                case ALIEN_EVOLUTION_ONE:
                case ALIEN_EVOLUTION_TWO:
                case ALIEN_EVOLUTION_THREE:
                case ALIEN_EVOLUTION_SEVEN:
                case ALIEN_EVOLUTION_EIGHT:
                case ALIEN_EVOLUTION_NINE:
                case ALIEN_EVOLUTION_TEN:
                case ALIEN_EVOLUTION_ELEVEN:
                case ALIEN_EVOLUTION_TWELVE:
                    
                case ALIEN_LIFEFORM_ONE:
                case ALIEN_LIFEFORM_TWO:
                case ALIEN_LIFEFORM_THREE:
                case ALIEN_LIFEFORM_FOUR:
                case ALIEN_LIFEFORM_FIVE:
                    
                case ALIEN_BUILD_RESOURCES:
                case ALIEN_BUILD_OFFENSE_CHAMBER:
                case ALIEN_BUILD_DEFENSE_CHAMBER:
                case ALIEN_BUILD_SENSORY_CHAMBER:
                case ALIEN_BUILD_MOVEMENT_CHAMBER:
                case ALIEN_BUILD_HIVE:
                    
                    // Validate weapon switches
                case WEAPON_NEXT:
                    theIsValid = true;
                    break;
                }
            }
        }
        
        if(theIsValid)
        {
            this->mCurrentCommand.impulse = theMessageID;
            this->pev->impulse = theMessageID;
        }
    }
}

bool AvHPlayer::GetHasActiveAlienWeaponWithImpulse(AvHMessageID inMessageID) const
{
    bool theHasWeapon = false;
    
    for(int i = 0; (i < MAX_ITEM_TYPES) && !theHasWeapon; i++)
    {
        CBasePlayerItem* theCurrentItem = this->m_rgpPlayerItems[i];
        if(theCurrentItem)
        {
            AvHAlienAbilityWeapon* theBaseWeapon = dynamic_cast<AvHAlienAbilityWeapon*>(theCurrentItem);
            while(theBaseWeapon && !theHasWeapon)
            {
                if((theBaseWeapon->GetAbilityImpulse() == inMessageID) && theBaseWeapon->GetEnabledState() && theBaseWeapon->IsUseable())
                {
                    theHasWeapon = true;
                }
                theCurrentItem = theCurrentItem->m_pNext;
                theBaseWeapon = dynamic_cast<AvHAlienAbilityWeapon*>(theCurrentItem);
            }
        }
    }

    return theHasWeapon;
}

bool AvHPlayer::GetHasSeenTeam(AvHTeamNumber inNumber) const
{
    bool theHasBeenOnTeam = false;

    if(inNumber == GetGameRules()->GetTeamA()->GetTeamNumber())
    {
        theHasBeenOnTeam = this->mHasSeenTeamA;
    }
    else if(inNumber == GetGameRules()->GetTeamB()->GetTeamNumber())
    {
        theHasBeenOnTeam = this->mHasSeenTeamB;
    }

    return theHasBeenOnTeam;
}

void AvHPlayer::SetHasSeenTeam(AvHTeamNumber inNumber)
{
    if(inNumber == GetGameRules()->GetTeamA()->GetTeamNumber())
    {
        this->mHasSeenTeamA = true;
    }
    else if(inNumber == GetGameRules()->GetTeamB()->GetTeamNumber())
    {
        this->mHasSeenTeamB = true;
    }
}

float AvHPlayer::GetTimeOfLastSporeDamage() const
{
    return this->mTimeOfLastSporeDamage;
}

void AvHPlayer::SetTimeOfLastSporeDamage(float inTime)
{
    this->mTimeOfLastSporeDamage = inTime;
}

// Assumes that the current input is legal
void AvHPlayer::HandleTopDownInput()
{
    // From CBasePlayer::PreThink():
    bool theAttackOneDown = FBitSet(this->mCurrentCommand.buttons, IN_ATTACK);
    bool theAttackTwoDown = FBitSet(this->mCurrentCommand.buttons, IN_ATTACK2);
    bool theJumpHit = FBitSet(this->mCurrentCommand.buttons, IN_JUMP);
    bool theCrouchDown = FBitSet(this->mCurrentCommand.buttons, IN_DUCK);
    
    // If we are a commander
    if(this->pev->iuser3 == AVH_USER3_COMMANDER_PLAYER)
    {
        this->mLastTimeInCommandStation = gpGlobals->time;

        // Fetch world x and world y from move.
        // Note: there is some inaccuracy here for two reasons.  One, these values were propagated
        // over the network and were optimized for it, and two, the selection assumes that the rays
        // emanate from the CURRENT view origin.  When clicking on one unit, this is right.  When
        // dragging a box around units, the view origin could have moved substantially.  This inaccuracy
        // should really only be noticed when drag selecting huge groups of units.  Send view origin as well?
        this->mMouseWorldPos.x = this->mCurrentCommand.upmove/kSelectionNetworkConstant;
        this->mMouseWorldPos.y = this->mCurrentCommand.sidemove/kSelectionNetworkConstant;
        this->mMouseWorldPos.z = this->mCurrentCommand.forwardmove/kSelectionNetworkConstant;

        // Fetch potential world location of mouse click.  worldx -> cmd->upmove, worldy -> cmd->sidemove
        //if(this->pev->impulse == COMMANDER_MOUSECOORD)
        AvHMessageID theMessageID = (AvHMessageID)this->mCurrentCommand.impulse;

        bool theIsBuildTech = AvHSHUGetIsBuildTech(theMessageID);
        bool theIsResearchTech = AvHSHUGetIsResearchTech(theMessageID);
        bool theIsRecycleMessage = (theMessageID == BUILD_RECYCLE);
            
        if(AvHSHUGetIsGroupMessage(theMessageID))
        {
            this->GroupMessage(theMessageID);
        }
        else if(theMessageID == COMMANDER_SELECTALL)
        {
            bool theClearSelection = true;
                
            // Run through all players on our team
            FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
                if(theEntity->GetTeam() == this->GetTeam())
                {
                    if(/*theEntity->GetIsRelevant() &&*/ (theEntity != this))
                    {
                        this->SetSelection(theEntity->entindex(), theClearSelection);
                        theClearSelection = false;
                    }
                }
                
            END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

            // Set "selectall" group
            //this->mSelectAllGroup = this->mSelected;
            this->GetTeamPointer()->SetSelectAllGroup(this->mSelected);
        }
        else if(theMessageID == COMMANDER_REMOVESELECTION)
        {
            // TODO:
            gSelectionHelper.ClearSelection();
            this->mTrackingEntity = 0;
        }
        else if((theMessageID == COMMANDER_NEXTIDLE) || (theMessageID == COMMANDER_NEXTAMMO) || (theMessageID == COMMANDER_NEXTHEALTH) || theJumpHit)
        {
            // Jump to first idle soldier and remove from list
            bool theSuccess = false;
            AvHTeam* theTeam = this->GetTeamPointer();
            if(theTeam)
            {
                if(theJumpHit)
                {
                    theMessageID = MESSAGE_NULL;
                }

                AvHAlert theAlert;
                if(theTeam->GetLastAlert(theAlert, true, theJumpHit, &theMessageID))
                {
                    int theEntityIndex = theAlert.GetEntityIndex();
                    CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theEntityIndex));
                    if(theBaseEntity)
                    {
                        VectorCopy(theBaseEntity->pev->origin, this->pev->origin);

                        switch(theMessageID)
                        {
                        case COMMANDER_NEXTIDLE:
                        case COMMANDER_NEXTAMMO:
                        case COMMANDER_NEXTHEALTH:
                            // Set selection to player
                            this->mSelected.clear();
                            this->mSelected.push_back(theEntityIndex);
                            break;
                        }
                    }
                }
            }
        }
        else if((theMessageID == COMMANDER_SCROLL) || (theMessageID == COMMANDER_MOVETO))
        {
            // Stop tracking
            this->mTrackingEntity = 0;

            // Commander didn't make a mistake when going to hotgroup, so clear event (so next group select will go to group, not to last position before)
            this->mLastSelectEvent = MESSAGE_NULL;
        }
        else if((theMessageID == COMMANDER_MOUSECOORD) || theIsBuildTech || theIsResearchTech || theIsRecycleMessage /*|| (theMessageID == COMMANDER_DEFAULTORDER)*/)
        {
            bool theAttackOnePressed = (theAttackOneDown && !this->mAttackOneDown);
            bool theAttackTwoPressed = (theAttackTwoDown && !this->mAttackTwoDown);
            bool theAttackOneReleased = (!theAttackOneDown && this->mAttackOneDown);
            bool theAttackTwoReleased = !theAttackTwoDown && this->mAttackTwoDown;
            
            // if left button is now down and it wasn't previously down
            if(theAttackOnePressed)
            {
                // Save world location of press
                this->mAttackOnePressedWorldPos = this->mMouseWorldPos;
            }
            
            // if right button just down
            if(theAttackTwoPressed)
            {
                // remember it's world location
                this->mAttackTwoPressedWorldPos = this->mMouseWorldPos;
            }
            
            Vector theReleasePosition;
            if(theAttackOneReleased || theAttackTwoReleased)
            {
                // Save world position of release. 
                theReleasePosition = this->mMouseWorldPos;
                
                //char theMessage[256];
                //sprintf(theMessage, "LMB released, selecting\n");
                //ClientPrint(this->pev, HUD_PRINTTALK, theMessage);
            }
            
            // Watch for selection events
            if(theMessageID == COMMANDER_MOUSECOORD)
            {
                if(theAttackOneReleased)
                {
                    if(!this->mPlacingBuilding)
                    {
                        if(!AvHToggleUseable(this, this->GetVisualOrigin(), this->mAttackOnePressedWorldPos))
                        {
                            // Clear existing selection
                            //this->mSelected.clear();
                            //this->mClientSelected.clear();
                            
                            gSelectionHelper.QueueSelection(this->GetVisualOrigin(), this->mAttackOnePressedWorldPos, theReleasePosition, (AvHTeamNumber)this->pev->team);
                        }
                        this->mTimeOfLastSignificantCommanderAction = gpGlobals->time;
                    }
                    this->mPlacingBuilding = false;
                }
                else if(theAttackTwoReleased)
                {
                    // Check to be sure that players are selected
                    if(this->mSelected.size() > 0)
                    {
                        bool theNonPlayerSelected = false;
                        for(EntityListType::iterator theIter = this->mSelected.begin(); theIter != this->mSelected.end(); theIter++)
                        {
                            AvHPlayer* thePlayer = NULL;
                            AvHSUGetEntityFromIndex(*theIter, thePlayer);
                            if(!thePlayer)
                            {
                                theNonPlayerSelected = true;
                                break;
                            }
                        }

						if(!theNonPlayerSelected || GetGameRules()->GetIsCheatEnabled(kcOrderSelf))
                        {
                            // Check if right-clicked a useable thing
                            // Trace to see if the commander clicked a useable thing
                            //if(!AvHToggleUseable(this, this->pev->origin, this->mAttackTwoPressedWorldPos))
                            //{

                            if(!this->GiveOrderToSelection(ORDERTYPEL_DEFAULT, this->mAttackTwoPressedWorldPos))
                            {
                                // This location better be off the map or something, default orders should nearly always go through
                                this->SendMessage(kInvalidOrderGiven, TOOLTIP);
                            }
                            else
                            {
                                this->mTimeOfLastSignificantCommanderAction = gpGlobals->time;
                            }
                        }
                    }
                    //}
                }
            }
            // Issue movement order to selection
            //else if(theMessageID == COMMANDER_DEFAULTORDER)
            //{
            //  // Build order
            //  AvHOrder theOrder;
            //  theOrder.SetOrderType(ORDERTYPEL_DEFAULT);
            //
            //  float theWorldX = this->mCurrentCommand.upmove*kWorldPosNetworkConstant;
            //  float theWorldY = this->mCurrentCommand.sidemove*kWorldPosNetworkConstant;
            //  theOrder.SetLocation(vec3_t(theWorldX, theWorldY, 0.0f));
            //
            //  this->GiveOrderToSelection(theOrder);
            //}
            // Watch for build events
            else if(theIsBuildTech)
            {
                this->mPlacingBuilding = true;

                if(GetGameRules()->GetGameStarted())
                {
					// 551 puzl
					// Hack to stop free scans.  This should be reworked as a generic solution for energy based build events.
					bool theCanBuild=true;
					if(this->mSelected.size() > 0) 
					{
						int theEntityForResearch = *this->mSelected.begin();
						CBaseEntity* theEntity = AvHSUGetEntityFromIndex(theEntityForResearch);									

						AvHObservatory* theObs = dynamic_cast<AvHObservatory*>(theEntity);
						if ( theObs ) 
						{
							if ( (theObs->GetIsTechnologyAvailable(BUILD_SCAN)) == false && (theMessageID == BUILD_SCAN) ) 
								theCanBuild = false;
						}
						if(!theObs && theMessageID == BUILD_SCAN) {
							theCanBuild = false;
						}
					}

					if ( theCanBuild ) {
						// puzl
						this->BuildTech(theMessageID, this->mAttackOnePressedWorldPos);
						this->mTimeOfLastSignificantCommanderAction = gpGlobals->time;

// tankefugl: 0001014	                    
//						// If player(s) selected when something built, give default order to it (assumes that players can't be selected along with other non-players)
//						if(AvHSHUGetIsBuilding(theMessageID))
//						{
//							if(this->mSelected.size() > 0)
//							{
//								int theFirstEntitySelected = *this->mSelected.begin();
//								if((theFirstEntitySelected >= 1) && (theFirstEntitySelected <= gpGlobals->maxClients))
//								{
//									if(!this->GiveOrderToSelection(ORDERTYPEL_DEFAULT, this->mAttackOnePressedWorldPos))
//									{
//										this->SendMessage(kInvalidOrderGiven, true);
//									}
//								}
//							}
//						}
// :tankefugl
					}
                }
            }
            else if(theIsResearchTech)
            {
                if(GetGameRules()->GetGameStarted())
                {
                    AvHTeam* theTeam = this->GetTeamPointer();
                    if(theTeam && (theTeam->GetResearchManager().GetIsMessageAvailable(theMessageID)))
                    {
                        if(this->mSelected.size() == 1)
                        {
								int theEntityForResearch = *this->mSelected.begin();
								this->Research(theMessageID, theEntityForResearch);
								this->mTimeOfLastSignificantCommanderAction = gpGlobals->time;
                        }
                    }
                }
            }
            // Check for recycling action
            else if(theIsRecycleMessage)
            {
                for(EntityListType::iterator theIter = this->mSelected.begin(); theIter != this->mSelected.end(); theIter++)
                {
                    AvHBaseBuildable* theBuildable = NULL;
                    AvHSUGetEntityFromIndex(*theIter, theBuildable);
                    if(theBuildable)
                    {
                        if((theBuildable->pev->team == this->pev->team) && (theBuildable->pev->team != 0))
                        {
                            theBuildable->StartRecycle();
                            this->mTimeOfLastSignificantCommanderAction = gpGlobals->time;

                            const char* theBuildableName = STRING(theBuildable->pev->classname);
                            this->LogPlayerAction("recycle", theBuildableName);
                        }
                    }
                }
            }

            // Update 
            this->mAttackOneDown = theAttackOneDown;
            this->mAttackTwoDown = theAttackTwoDown;
        }
    }
    else
    {
        // TODO: Make sure we think this player is a commander to prevent cheating on client
        // TODO: If they sent COMMANDER_MOUSECOORD and they aren't a commander, there's trouble
    }
    
    // Process selections if waiting
    gSelectionHelper.ProcessPendingSelections();

    if(gSelectionHelper.SelectionResultsWaiting())
    {
        EntityListType theNewSelection;
        gSelectionHelper.GetAndClearSelection(theNewSelection);

        //string theMessage = "selecting: ";

        // If crouch is down
        bool theToggleSelection = theCrouchDown;
        if(theToggleSelection)
        {
            bool theNewSelectionIsAllPlayers = true;

            // Check to make sure the whole new selection is all players
            for(EntityListType::iterator theIterator = theNewSelection.begin(); theIterator != theNewSelection.end(); theIterator++)
            {
                if(*theIterator > gpGlobals->maxClients)
                {
                    theNewSelectionIsAllPlayers = false;
                }
            }
            
            // If not, clear the current selection and clear crouch
            if(!theNewSelectionIsAllPlayers)
            {
                theToggleSelection = false;
            }
        }

        if(!theToggleSelection)
        {
            this->mSelected.clear();
        }
        
        // Get player for each one, make sure it's selectable by this player
        for(EntityListType::iterator theIterator = theNewSelection.begin(); theIterator != theNewSelection.end(); /* no increment*/)
        {
            // If this player is selectable
            AvHPlayer* thePlayer = NULL;
            AvHSUGetEntityFromIndex(*theIterator, thePlayer);
            if(!thePlayer || GetGameRules()->GetIsPlayerSelectableByPlayer(thePlayer, this))
            {
                // Add to debug message
                //char theNumber[16];
                //sprintf(theNumber, "%d ", *theIterator);
                //theMessage += string(theNumber);
                int theCurrentEntity = *theIterator;

                // Toggle selection of this player
                if(theToggleSelection)
                {
                    // Is entity is already selected?
                    EntityListType::iterator theFindIter = std::find(this->mSelected.begin(), this->mSelected.end(), theCurrentEntity);
                    bool theEntityIsSelected = (theFindIter != this->mSelected.end());
                    if(theEntityIsSelected)
                    {
                        this->mSelected.erase(theFindIter);
                    }
                    else
                    {
                        this->SetSelection(theCurrentEntity, false);
                    }
                }
                else
                {
                    this->SetSelection(theCurrentEntity, false);
                }

                // Increment
                theIterator++;

                this->mTrackingEntity = 0;
            }
            else
            {
                // erase it from the list
                theIterator = theNewSelection.erase(theIterator);
            }
        }

        // If we selected at least one unit, display debug message
//      if(this->mSelected.size() > 0)
//      {
//          theMessage += "\n";
//          ClientPrint(this->pev, HUD_PRINTTALK, theMessage.c_str());
//      }
    }

    // Temporary for fun
//  AvHTeam* theTeam = this->GetTeamPointer();
//  if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN))
//  {
//      if(theAttackOneDown && (this->GetUser3() != ROLE_ALIEN2) && (this->GetUser3() != ROLE_ALIEN5))
//      {
//          this->PlayRandomRoleSound(kPlayerLevelAttackSoundList, CHAN_WEAPON);
//      }
//  }
}

void AvHPlayer::Jump()
{
    Vector      vecWallCheckDir;// direction we're tracing a line to find a wall when walljumping
    Vector      vecAdjustedVelocity;
    Vector      vecSpot;
    TraceResult tr;
    
    if (FBitSet(pev->flags, FL_WATERJUMP))
        return;
    
    if (pev->waterlevel >= 2)
    {
        return;
    }

    // If this isn't the first frame pressing the jump button, break out.
    if ( !FBitSet( m_afButtonPressed, IN_JUMP ) )
        return;         // don't pogo stick

    if ( !(pev->flags & FL_ONGROUND) || !pev->groundentity )
    {
        return;
    }

// many features in this function use v_forward, so makevectors now.
    UTIL_MakeVectors (pev->angles);

    SetAnimation( PLAYER_JUMP );

    if ( FBitSet(pev->flags, FL_DUCKING ) || FBitSet(m_afPhysicsFlags, PFLAG_DUCKING) )
    {
        //if ( m_fLongJump && (pev->button & IN_DUCK) && gpGlobals->time - m_flDuckTime < 1 && pev->velocity.Length() > 50 )
        if ( (pev->button & IN_DUCK) && gpGlobals->time - m_flDuckTime < 1 && pev->velocity.Length() > 50 )
        {// If jump pressed within a second of duck while moving, long jump!
            SetAnimation( PLAYER_SUPERJUMP );
        }
    }
    
    // If you're standing on a conveyor, add it's velocity to yours (for momentum)
    entvars_t *pevGround = VARS(pev->groundentity);
    if ( pevGround && (pevGround->flags & FL_CONVEYOR) )
    {
        pev->velocity = pev->velocity + pev->basevelocity;
    }
}

void AvHPlayer::Killed( entvars_t *pevAttacker, int iGib )
{
    if(this->GetUser3() != AVH_USER3_COMMANDER_PLAYER)
    {
        // Save death position
        this->mKilledX = this->pev->origin.x;
        this->mKilledY = this->pev->origin.y;
        
        this->mIsScreaming = false;

        this->PackDeadPlayerItems();

        //this->StopDigestion(false);
        this->ResetBehavior(false);

		if(GetGameRules()->GetIsCombatMode())
		{
			this->ProcessCombatDeath();
		}

        // Fade out already performed when we start being digested
        bool theFadeOut = !this->GetIsBeingDigested();

        if(!this->GetIsBeingDigested())
        {
            if(theFadeOut)
            {
                Vector theFadeColor;
                theFadeColor.x = 0;
                theFadeColor.y = 0;
                theFadeColor.z = 0;
                UTIL_ScreenFade(this, theFadeColor, kTransitionFadeTime, 0.0f, 255, FFADE_OUT | FFADE_STAYOUT);
            }
        }
        else
        {
            SetUpgradeMask(&this->pev->iuser4, MASK_DIGESTING, false);
        }
        
        // This line caused a dynamic_cast failure when a shotty killed a flier, then it set it's ensnare state to false,
        // which tried to deploy his current weapon, which got the most recent weapon it used, which was garbage for some reason
        //this->SetEnsnareState(false);
        SetUpgradeMask(&this->pev->iuser4, MASK_ENSNARED, false);
        this->mTimeToBeUnensnared = -1;
        this->mLastTimeEnsnared = -1;
        
        this->mAlienSightActive = false;
        this->mEvolution = MESSAGE_NULL;
		this->SetUsedKilled(false);
        
        this->PlayRandomRoleSound(kPlayerLevelDieSoundList);

        this->Uncloak();
        
        int thePriority = 0;
        bool theIsDramatic = false;
        switch(this->GetUser3(false))
        {
        case AVH_USER3_ALIEN_EMBRYO:
            thePriority = kGestateDeathPriority;
            break;
        case AVH_USER3_ALIEN_PLAYER2:
            thePriority = kGorgeDeathPriority;
            break;
        case AVH_USER3_ALIEN_PLAYER3:
            thePriority = kLerkDeathPriority;
            break;
        case AVH_USER3_ALIEN_PLAYER4:
            thePriority = kFadeDeathPriority;
            break;
        case AVH_USER3_ALIEN_PLAYER5:
            thePriority = kOnosDeathPriority;
            theIsDramatic = true;
            break;
        }

        if(this->GetHasHeavyArmor())
        {
            thePriority = kHeavyDeathPriority;
        }

        if(thePriority > 0)
        {
            GetGameRules()->MarkDramaticEvent(thePriority, this->entindex(), theIsDramatic, (entvars_t*)pevAttacker);
        }       

        // Added this to make player models fade out
        this->pev->solid = SOLID_NOT;
        
        // We can only die when we're playing, not in ready room, reinforcement mode or observation
        // (see ClientKill)
        CBasePlayer::Killed(pevAttacker, iGib);

		//voogru: remove parasite flag if they are a marine.
		if(this->GetIsMarine())
			SetUpgradeMask(&this->pev->iuser4, MASK_PARASITED, false); 
        
        this->EffectivePlayerClassChanged();

        GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_PLAYER_DIED, this->entindex());



//      // Print death anim
//      const char* theMainSequence = this->LookupSequence(this->pev->sequence);
//      if(!theMainSequence)
//      {
//          theMainSequence = "None";
//      }
//
//      const char* theGaitSequence = this->LookupSequence(this->pev->gaitsequence);
//      if(!theGaitSequence)
//      {
//          theGaitSequence = "None";
//      }
//
//      char theMessage[128];
//      sprintf(theMessage, "Death animation: %s, %s", theMainSequence, theGaitSequence);
//      UTIL_SayText(theMessage, this);
    }
}

//Activity AvHPlayer::GetDeathActivity (void)
//{
//  Activity theDeathActivity = ACT_DIESIMPLE;
//
//  switch(RANDOM_LONG(0, 3))
//  {
//  case 1:
//      theDeathActivity = ACT_DIEBACKWARD;
//      break;
//  case 2:
//      theDeathActivity = ACT_DIEFORWARD;
//      break;
//  case 3:
//      theDeathActivity = ACT_DIEVIOLENT;
//      break;
//  }
//
//  return theDeathActivity;
//}


void AvHPlayer::NextWeapon()
{
    if(this->GetIsAbleToAct())
    {
        CBasePlayerWeapon* theActiveWeapon = dynamic_cast<CBasePlayerWeapon*>(this->m_pActiveItem);
        
        if(theActiveWeapon)
        {
            CBasePlayerWeapon* theNextWeapon = dynamic_cast<CBasePlayerWeapon*>(theActiveWeapon->m_pNext);
            if(theNextWeapon)
            {
                this->m_pLastItem = this->m_pActiveItem;
                this->m_pActiveItem->Holster();
                this->m_pActiveItem = theNextWeapon;
                this->m_pActiveItem->Deploy();
            }
            else
            {
                int theSlot = theActiveWeapon->iItemSlot();
                for(int i = 0; i < MAX_ITEM_TYPES; i++)
                {
                    theNextWeapon = dynamic_cast<CBasePlayerWeapon*>(this->m_rgpPlayerItems[(theSlot + i + 1) % MAX_ITEM_TYPES]);
                    if(theNextWeapon)
                    {
                        this->m_pLastItem = this->m_pActiveItem;
                        this->m_pActiveItem->Holster();
                        this->m_pActiveItem = theNextWeapon;
                        this->m_pActiveItem->Deploy();
                        break;
                    }
                }
            }
        }
    }
}

void AvHPlayer::ObserverModeIllegal()
{
    // Blackout screen or something if observers aren't allowed
    UTIL_ScreenFade( CBaseEntity::Instance(this->edict()), Vector(128,0,0), 6, 15, 255, FFADE_OUT | FFADE_MODULATE );
    this->SendMessage(kNoSpectating);
}

void AvHPlayer::PackDeadPlayerItems(void)
{
	//to do - drop everything that's not in the standard loadout + LMG.
    this->DropItem(kwsMachineGun);
    this->DropItem(kwsShotGun);
	this->DropItem(kwsHeavyMachineGun);
	this->DropItem(kwsGrenadeGun);
	this->DropItem(kwsMine);
	this->DropItem(kwsWelder);
}

void AvHPlayer::PlayRandomRoleSound(string inSoundListName, int inChannel, float inVolume)
{
    char theListName[256];
    AvHUser3 theUser3 = this->GetUser3();
    if(theUser3 != AVH_USER3_NONE)
    {
        sprintf(theListName, inSoundListName.c_str(), (int)theUser3);
        gSoundListManager.PlaySoundInList(theListName, this, inChannel, inVolume);
    }
}

float AvHPlayer::GetTimeOfLastConstructUse() const
{
    return this->mTimeOfLastConstructUse;
}

void AvHPlayer::SetTimeOfLastConstructUse(float inTime)
{
    this->mTimeOfLastConstructUse = inTime;
}

void AvHPlayer::PlayerConstructUse()
{
    AvHTeam* theTeam = this->GetTeamPointer();
    ASSERT(theTeam);
    if(theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE)
    {
        this->HolsterWeaponToUse();
    }
    else if(theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
    {
        const float kConstructAnimationInterval = 1.1f;
        const int kConstructAnimationIndex = 4;

        if((this->mTimeOfLastConstructUseAnimation == 0) || (gpGlobals->time > (this->mTimeOfLastConstructUseAnimation + kConstructAnimationInterval)))
        {
            // Play special builder animation
            PLAYBACK_EVENT_FULL(0, this->edict(), gWeaponAnimationEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 0, kConstructAnimationIndex, 0, 0 );

            // Delay idle
            if(this->m_pActiveItem)
            {
                CBasePlayerWeapon* theCurrentWeapon = (CBasePlayerWeapon *)this->m_pActiveItem->GetWeaponPtr();
                if(theCurrentWeapon)
                {
                    theCurrentWeapon->m_flTimeWeaponIdle += 2.0f;
                }
            }

            this->mTimeOfLastConstructUseAnimation = gpGlobals->time;
        }
    }
}

bool AvHPlayer::PlaySaying(AvHMessageID inMessageID)
{
    bool thePlayedSaying = false;
    char theSoundList[256];
    memset(theSoundList, 0, 256*sizeof(char));

    if(this->pev->iuser3 == AVH_USER3_MARINE_PLAYER)
    {
        switch(inMessageID)
        {
        case SAYING_1:
        case SAYING_2:
        case SAYING_3:
        case SAYING_4:
        case SAYING_5:
        case SAYING_6:
        case SAYING_7:
        case SAYING_8:
        case SAYING_9:
            sprintf(theSoundList, kSoldierSayingList, (inMessageID - SAYING_1 + 1));
            break;

        case ORDER_REQUEST:
            strcpy(theSoundList, kSoldierOrderRequestList);
            break;

        case ORDER_ACK:
            strcpy(theSoundList, kSoldierOrderAckList);
            break;
        }
    }
    else if(this->pev->iuser3 == AVH_USER3_COMMANDER_PLAYER)
    {
        switch(inMessageID)
        {
        case SAYING_1:
        case SAYING_2:
        case SAYING_3:
        case SAYING_4:
        case SAYING_5:
        case SAYING_6:
        case SAYING_7:
        case SAYING_8:
        case SAYING_9:
            sprintf(theSoundList, kCommanderSayingList, (inMessageID - SAYING_1 + 1));
            break;
        }
    }
    else if(this->GetIsAlien())
    {
        switch(inMessageID)
        {
        case SAYING_1:
        case SAYING_2:
        case SAYING_3:
        case SAYING_4:
        case SAYING_5:
        case SAYING_6:
            sprintf(theSoundList, kAlienSayingList, (inMessageID - SAYING_1 + 1));
            break;
        }
    }

    if(strcmp(theSoundList, ""))
    {
        if(inMessageID == ORDER_REQUEST)
        {
            this->mOrdersRequested = true;
            this->mOrderAcknowledged = false;
            this->mIsSpeaking = false;
        }
        else if(inMessageID == ORDER_ACK)
        {
            this->mOrderAcknowledged = true;
            this->mIsSpeaking = false;
            this->mOrdersRequested = false;
        }
        else
        {
            this->mIsSpeaking = true;
            this->mOrderAcknowledged = false;
            this->mOrdersRequested = false;
        }

        this->mTimeOfLastSaying = gpGlobals->time;
        this->mLastSaying = inMessageID;

        //int pitch = 95;// + RANDOM_LONG(0,29);
        //EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, theSaying, 1, ATTN_NORM, 0, pitch);
        gSoundListManager.PlaySoundInList(theSoundList, this, CHAN_VOICE, 1.0f);
        
        thePlayedSaying = true;
    }

    return thePlayedSaying;
}

bool AvHPlayer::PlayHUDSound(AvHHUDSound inSound) const
{ 
	return this->PlayHUDSound( inSound, pev->origin[0], pev->origin[1] ); 
}

bool AvHPlayer::PlayHUDSound(AvHHUDSound inSound, float x, float y) const
{
    bool theSuccess = false;
    
    if((inSound > HUD_SOUND_INVALID) && (inSound < HUD_SOUND_MAX))
    {
		NetMsg_PlayHUDNotification( this->pev, 0, inSound, x, y );
        theSuccess = true;
    }
    
    return theSuccess;
}

void AvHPlayer::PlayHUDStructureNotification(AvHMessageID inMessageID, const Vector& inLocation)
{
    if(GetGameRules()->GetGameStarted())
    {
        // This player built a structure.  Tell all his teammates
        FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)

        if(theEntity->GetIsRelevant() && !theEntity->GetIsBeingDigested())
        {
            // Don't send our own messages to ourself unless cheats are enabled
            if(GetGameRules()->GetCheatsEnabled() || (this != theEntity))
            {
                bool theShowNotification = false;

                // Show to friendlies...
                if(theEntity->pev->team == this->pev->team) 
                {
                    theShowNotification = true;
                }
                
                if(theShowNotification)
                {
					NetMsg_PlayHUDNotification( theEntity->pev, 1, inMessageID, inLocation.x, inLocation.y );
                }
            }
        }
        END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
    }
}

void AvHPlayer::ProcessEvolution()
{
	this->SaveHealthArmorPercentages();

    UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kGestationSound, 1.0f, .5, SND_STOP, 100);

    // Go back to the role we were before, unless we're morphing into a new lifeform
    AvHUser3 theNewUser3 = this->mPreviousUser3;

	bool theChangedLifeforms = false;
    switch(this->mEvolution)
    {
    case ALIEN_LIFEFORM_ONE:
        theNewUser3 = AVH_USER3_ALIEN_PLAYER1;
		theChangedLifeforms = true;
        break;
    case ALIEN_LIFEFORM_TWO:
        theNewUser3 = AVH_USER3_ALIEN_PLAYER2;
		theChangedLifeforms = true;
        break;
    case ALIEN_LIFEFORM_THREE:
        theNewUser3 = AVH_USER3_ALIEN_PLAYER3;
		theChangedLifeforms = true;
        break;
    case ALIEN_LIFEFORM_FOUR:
        theNewUser3 = AVH_USER3_ALIEN_PLAYER4;
		theChangedLifeforms = true;
        break;
    case ALIEN_LIFEFORM_FIVE:
        theNewUser3 = AVH_USER3_ALIEN_PLAYER5;
		theChangedLifeforms = true;
        break;
    }

    // This shouldn't be needed now that SetUser3 takes into account moving the origin.
    // Position player a bit higher off the ground so he doesn't get stuck
    //this->pev->origin.z += AvHMUGetOriginOffsetForUser3(theNewUser3);

	if(theChangedLifeforms)
	{
		if(GetGameRules()->GetIsCombatMode())
			this->SetLifeformCombatNodesAvailable(false); // Only allow one lifeform change
		else
		{
			int iUpgrades = MASK_UPGRADE_1 
				| MASK_UPGRADE_2 
				| MASK_UPGRADE_3 
				| MASK_UPGRADE_4 
				| MASK_UPGRADE_5 
				| MASK_UPGRADE_6 
				| MASK_UPGRADE_7 
				| MASK_UPGRADE_8 
				| MASK_UPGRADE_9
				| MASK_UPGRADE_10
				| MASK_UPGRADE_11
				| MASK_UPGRADE_12
				| MASK_UPGRADE_13
				| MASK_UPGRADE_14
				| MASK_UPGRADE_15;

			SetUpgradeMask(&this->pev->iuser4, (AvHUpgradeMask)iUpgrades, false);
		}
	}

    this->SetUser3(theNewUser3, true);

    //this->mPreviousUser3 = User3_UNDEFINED;
    
    //  int theMaxArmor = 0;
    //  float theArmorPercentage = 0;
    //  int theNewMaxArmor = 0;
    
    switch(this->mEvolution)
    {
    case ALIEN_EVOLUTION_ONE:
    case ALIEN_EVOLUTION_TWO:
    case ALIEN_EVOLUTION_THREE:
    case ALIEN_EVOLUTION_SEVEN:
    case ALIEN_EVOLUTION_EIGHT:
    case ALIEN_EVOLUTION_NINE:
    case ALIEN_EVOLUTION_TEN:
    case ALIEN_EVOLUTION_ELEVEN:
    case ALIEN_EVOLUTION_TWELVE:
        
        // If it's the exoskeleton upgrade, upgrade now
        //      theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, this->mUser3);
        //      theArmorPercentage = this->pev->armorvalue/theMaxArmor;
        
        ProcessGenericUpgrade(this->pev->iuser4, this->mEvolution);
        
        // If player has already decided on the direction to upgrade, spend any extra upgrade levels in that category.
        // This has to happen immediately, or else spamming the upgrade button can get another evolution before this is called in InternalAlienUpgradesThink.
        AvHTeam* theTeamPointer = this->GetTeamPointer();
        ASSERT(theTeamPointer);
        AvHAlienUpgradeListType theUpgrades = theTeamPointer->GetAlienUpgrades();
        AvHAddHigherLevelUpgrades(theUpgrades, this->pev->iuser4);
        
        //this->PlayHUDSound(HUD_SOUND_ALIEN_UPGRADECOMPLETE);
        
//      if(this->mEvolution == ALIEN_EVOLUTION_TWO)
//      {
//          theNewMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, this->mUser3);
//          this->pev->armorvalue = theArmorPercentage*theNewMaxArmor;
//      }
        break;
    }

	this->PlayRandomRoleSound(kPlayerLevelSpawnSoundList, CHAN_ITEM, this->GetAlienAdjustedEventVolume());
	this->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;

	this->RevertHealthArmorPercentages();    
	this->mEvolution = MESSAGE_NULL;
}

void AvHPlayer::RevertHealthArmorPercentages()
{
    // Preserve armor and health percentages
    int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
    this->pev->health = max(this->mHealthPercentBefore*theMaxHealth,1.0f);//voogru: prevent bug with players evolving down from higher lifeform from getting negative health but still "alive"

    int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);
    this->pev->armorvalue = max(this->mArmorPercentBefore*theMaxArmor, 0.0f);

	// Assumes a push/pop kind of deal
	this->mHealthPercentBefore = this->mArmorPercentBefore = 1.0f;
}

void AvHPlayer::SaveHealthArmorPercentages()
{
    int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
    this->mHealthPercentBefore = this->pev->health/(float)theMaxHealth;
    this->mHealthPercentBefore = min(max(0.0f, this->mHealthPercentBefore), 1.0f);
    
    int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);
    this->mArmorPercentBefore = this->pev->armorvalue/(float)theMaxArmor;
    this->mArmorPercentBefore = min(max(0.0f, this->mArmorPercentBefore), 1.0f);
}

void AvHPlayer::ProcessResourceAdjustment(AvHMessageID inMessageID)
{
    // TODO: Mark cheater if this isn't the case?
    if(this->pev->iuser3 == AVH_USER3_COMMANDER_PLAYER)
    {
        // Make sure we have resource tower selected
        if(this->mSelected.size() == 1)
        {
            int theEntIndex = *this->mSelected.begin();
            AvHFuncResource* theFuncResource;
            if(AvHSUGetEntityFromIndex(theEntIndex, theFuncResource))
            {
                // Get particle system
                int theParticleSystemIndex = theFuncResource->GetParticleSystemIndex();
                AvHParticleSystemEntity* theParticleSystemEntity = NULL;
                if(AvHSUGetEntityFromIndex(theParticleSystemIndex, theParticleSystemEntity))
                {
                    // Adjust particle system for resource tower
                    
                    // Get custom data
                    uint16 theCustomData = theParticleSystemEntity->GetCustomData();
                    
                    // Adjust custom data by inMessageID
                    ASSERT(false);
                    //switch(inMessageID)
                    //{
                    //case RESOURCE_ADJUST_ONE_UP:
                    //  theCustomData = min(theCustomData+1, 0xF);
                    //  break;
                    //case RESOURCE_ADJUST_ONE_DOWN:
                    //  theCustomData = max(theCustomData-1, 0);
                    //  break;
                    //}
                    
                    // Set custom data again
                    theParticleSystemEntity->SetCustomData(theCustomData);
                }
            }
        }
    }
}

void AvHPlayer::Evolve(AvHMessageID inMessageID, bool inInstantaneous)
{
    // TODO: Put in a waiting time or some other effects?
    if(this->GetTeamPointer()->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
    {
		this->PlayHUDSound(HUD_SOUND_POINTS_SPENT);

        int theDramaticPriority = 0;
        switch(inMessageID)
        {
        case ALIEN_LIFEFORM_ONE:
            theDramaticPriority = kEvolveLevelOnePriority;
            break;
        case ALIEN_LIFEFORM_TWO:
            theDramaticPriority = kEvolveLevelTwoPriority;
            break;
        case ALIEN_LIFEFORM_THREE:
            theDramaticPriority = kEvolveLevelThreePriority;
            break;
        case ALIEN_LIFEFORM_FOUR:
            theDramaticPriority = kEvolveLevelFourPriority;
            break;
        case ALIEN_LIFEFORM_FIVE:
            theDramaticPriority = kEvolveLevelFivePriority;
            break;
        default:
            theDramaticPriority = kEvolveUpgradePriority;
            break;
        }

        if(!inInstantaneous)
        {
            GetGameRules()->MarkDramaticEvent(theDramaticPriority, this);
            
            this->mTimeGestationStarted = gpGlobals->time;
            this->mPreviousUser3 = (AvHUser3)this->pev->iuser3;
            this->mEvolution = inMessageID;
      
            this->SetUser3(AVH_USER3_ALIEN_EMBRYO);
            this->BecomePod();
        }
        else
        {
            this->mPreviousUser3 = (AvHUser3)this->pev->iuser3;
            this->mEvolution = inMessageID;
            this->ProcessEvolution();
        }
    }
}

void AvHPlayer::LogEmitRoleChange()
{
	const char* theUser3Name = AvHSHUGetClassNameFromUser3((AvHUser3)this->pev->iuser3);
	if(theUser3Name != NULL)
	{
		UTIL_LogPrintf("%s changed role to \"%s\"\n", 
			GetLogStringForPlayer( this->edict() ).c_str(),
			theUser3Name
		);
	}
}

void AvHPlayer::LogPlayerAction(const char* inActionDescription, const char* inActionData)
{
    if(inActionDescription && inActionData)
    {
        UTIL_LogPrintf("%s triggered \"%s\" (type \"%s\")\n", 
			GetLogStringForPlayer( this->edict() ).c_str(),
			inActionDescription, 
			inActionData);
    }
}

void AvHPlayer::LogPlayerActionPlayer(CBasePlayer* inActionPlayer, const char* inAction)
{
    if(inAction)
    {
        UTIL_LogPrintf("%s triggered \"%s\" against %s\n", 
			GetLogStringForPlayer( inActionPlayer->edict() ).c_str(),
			inAction, 
			GetLogStringForPlayer( this->edict() ).c_str()
		);
    }
}

void AvHPlayer::LogPlayerAttackedPlayer(CBasePlayer* inAttackingPlayer, const char* inWeaponName, float inDamage)
{
    if(inWeaponName)
    {
        edict_t* theAttacker = inAttackingPlayer->edict();
        edict_t* theReceiver = this->edict();

        bool theLogAttack = false;
        int theLogDetail = CVAR_GET_FLOAT(kvLogDetail);

        if((theLogDetail > 0) && (theAttacker->v.team != theReceiver->v.team))
        {
            theLogAttack = true;
        }
        else if((theLogDetail > 1) && (theAttacker->v.team == theReceiver->v.team))
        {
            theLogAttack = true;
        }
        else if(theLogDetail > 2)
        {
            theLogAttack = true;
        }
        
        if(theLogAttack)
        {
            // Remove "weapon_" prefix
            string theKillerName(inWeaponName);
            AvHSHUMakeViewFriendlyKillerName(theKillerName);
            int theDamage = (int)inDamage;
            
            UTIL_LogPrintf("%s attacked %s with \"%s\" (damage \"%d\")\n", 
				GetLogStringForPlayer( theAttacker ).c_str(),
				GetLogStringForPlayer( theReceiver ).c_str(),
				theKillerName.c_str(), 
				theDamage
			);
        }
    }
}

void AvHPlayer::LogPlayerKilledPlayer(CBasePlayer* inAttackingPlayer, const char* inWeaponName)
{
    if(inWeaponName)
    {
        edict_t* theAttacker = inAttackingPlayer->edict();
        edict_t* theReceiver = this->edict();
        
        bool theLogAttack = false;
        int theLogDetail = CVAR_GET_FLOAT(kvLogDetail);
        
        if((theLogDetail > 0) && (theAttacker->v.team != theReceiver->v.team))
        {
            theLogAttack = true;
        }
        else if((theLogDetail > 1) && (theAttacker->v.team == theReceiver->v.team))
        {
            theLogAttack = true;
        }
        else if(theLogDetail > 2)
        {
            theLogAttack = true;
        }
        
        if(theLogAttack)
        {
            // Remove "weapon_" prefix
            string theKillerName(inWeaponName);
            AvHSHUMakeViewFriendlyKillerName(theKillerName);
            
            UTIL_LogPrintf("%s killed %s with \"%s\"\n", 
				GetLogStringForPlayer( theAttacker ).c_str(),
				GetLogStringForPlayer( theReceiver ).c_str(),
				theKillerName.c_str()
			);
        }
    }
}

void AvHPlayer::Research(AvHMessageID inUpgrade, int inEntityIndex)
{
    if(this->GetUser3() == AVH_USER3_COMMANDER_PLAYER)
    {
        bool theIsResearchable;
        int theResearchCost;
        float theResearchTime;
        AvHTeam* theTeam = this->GetTeamPointer();

        CBaseEntity* theEntity = AvHSUGetEntityFromIndex(inEntityIndex);

        if(theEntity && theTeam && (theEntity->pev->team == this->pev->team))
        {
            AvHResearchManager& theResearchManager = theTeam->GetResearchManager();
            if(inUpgrade == MESSAGE_CANCEL)
            {
                // Remember research tech and time done
                float thePercentageComplete = 0.0f;
                AvHMessageID theCancelledTechnology = MESSAGE_NULL;
                if(theResearchManager.CancelResearch(inEntityIndex, thePercentageComplete, theCancelledTechnology))
                {
                    if(theResearchManager.GetResearchInfo(theCancelledTechnology, theIsResearchable, theResearchCost, theResearchTime))
                    {
                        ASSERT(thePercentageComplete >= 0.0f);
                        ASSERT(thePercentageComplete < 1.0f);
                        const float kRefundFactor = 1.0f;
                        float theRefund = kRefundFactor*(1.0f - thePercentageComplete)*theResearchCost;
                        theRefund = min(theRefund, (float)theResearchCost);
                        this->SetResources(this->GetResources() + theRefund);
                    }

                    char* theResearchName = NULL;
                    if(AvHSHUGetResearchTechName(inUpgrade, theResearchName))
                    {
                        this->LogPlayerAction("research_cancel", theResearchName);
                    }
                }
            }
            else if(theResearchManager.GetResearchInfo(inUpgrade, theIsResearchable, theResearchCost, theResearchTime))
            {
                // Look up cost, deduct from player
                if(this->GetResources() >= theResearchCost)
                {
                    // Remember research tech and time done
                    if(theResearchManager.SetResearching(inUpgrade, inEntityIndex))
                    {
                        this->PayPurchaseCost(theResearchCost);
						// Tell everyone on this team about research and time research done, so their
						// HUD is updated, and so they can't then walk up to station and start researching something else
						this->PlayHUDStructureNotification(inUpgrade, theEntity->pev->origin);
                    
	                    char* theResearchName = NULL;
						if(AvHSHUGetResearchTechName(inUpgrade, theResearchName))
						{
							this->LogPlayerAction("research_start", theResearchName);
						}
					}
                }
                else
                {
                    this->PlayHUDSound(HUD_SOUND_MARINE_MORE);
                }
            }
        }
    }
}

// Digestion functions
int AvHPlayer::GetDigestee() const
{
    return this->mDigestee;
}

void AvHPlayer::SetDigestee(int inPlayerID)
{
    this->mDigestee = inPlayerID;
}

void AvHPlayer::StartDigestion(int inDigestee)
{
    AvHPlayer* theDigestee = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inDigestee)));
    if(theDigestee)
    {
        theDigestee->SetBeingDigestedMode(true);
        SetUpgradeMask(&this->pev->iuser4, MASK_DIGESTING, true);
        
        this->SetDigestee(inDigestee);

        EMIT_SOUND(ENT(this->pev), CHAN_VOICE, kDevourSwallowSound, this->GetAlienAdjustedEventVolume(), ATTN_NORM);
        
        this->mTimeOfLastDigestDamage = gpGlobals->time;
    }
}

void AvHPlayer::StopDigestion(bool inDigested)
{
    // Play digest complete sound and stop digestion
    int theDigesteeIndex = this->GetDigestee();
    this->SetDigestee(0);
    
    AvHPlayer* theDigestee = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theDigesteeIndex)));
    if(theDigestee)
    {
        if(inDigested)
        {
            this->SetDesiredRoomType(0, true);

			EMIT_SOUND(ENT(this->pev), CHAN_VOICE, kDevourCompleteSound, this->GetAlienAdjustedEventVolume(), ATTN_NORM);
        }
        else
        {
            theDigestee->SetBeingDigestedMode(false);
            
            EMIT_SOUND(ENT(this->pev), CHAN_VOICE, kDevourCancelSound, this->GetAlienAdjustedEventVolume(), ATTN_NORM);
        }
    }

    SetUpgradeMask(&this->pev->iuser4, MASK_DIGESTING, false);

}

void AvHPlayer::InternalDigestionThink()
{
    // If we have a digestee
    int theDigesteeIndex = this->GetDigestee();
    AvHPlayer* theDigestee = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theDigesteeIndex)));
    if(theDigestee)
    {
        bool thePlayerWasDigested = false;

        // If digestee is alive and still in the game (hasn't disconnected or switched teams)
        if(theDigestee->GetIsRelevant())
        {
			if(RANDOM_LONG(0, 110) == 0)
			{
				// Play digesting sound occasionally
				EMIT_SOUND(this->edict(), CHAN_AUTO, kDigestingSound, this->GetAlienAdjustedEventVolume(), ATTN_NORM);
			}

            // Do damage to digestee 
            float theTimePassed = gpGlobals->time - this->mTimeOfLastDigestDamage;
            if(theTimePassed > 1.0f)
            {
                // Find devour weapon if we have one, so death message appears properly
                entvars_t* theInflictor = this->pev;
                CBasePlayerItem* theDevourWeapon = this->HasNamedPlayerItem(kwsDevour);
                if(theDevourWeapon)
                {
                    theInflictor = theDevourWeapon->pev;
                }

				const float theCombatModeScalar = GetGameRules()->GetIsCombatMode() ? BALANCE_VAR(kCombatModeTimeScalar) : 1.0f;
                theDigestee->pev->takedamage = DAMAGE_YES;
                float theDamage = theTimePassed*BALANCE_VAR(kDevourDamage)*(1.0f/theCombatModeScalar);
                theDigestee->TakeDamage(theInflictor, this->pev, theDamage, DMG_DROWN);
                theDigestee->pev->takedamage = DAMAGE_NO;

                // Get health back too
                this->Heal(theDamage, false);
        
                this->mTimeOfLastDigestDamage = gpGlobals->time;
            }

            // Set the digestee's position to our own
            VectorCopy(this->pev->origin, theDigestee->pev->origin);
            VectorCopy(this->pev->angles, theDigestee->pev->angles);
            
            // Set status bar estimating how long before player will be digested (for both digestee and digester)
            theDigestee->TriggerProgressBar(theDigesteeIndex, 3);
            this->TriggerProgressBar(theDigesteeIndex, 3);
            
            // Set fuser3 appropriately
            int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(theDigestee->pev->iuser4, theDigestee->GetUser3(), this->GetExperienceLevel());
            float theDigestingScalar = (((float)theMaxHealth - theDigestee->pev->health)/(float)theMaxHealth);
            /*this->pev->fuser3 =*/ theDigestee->pev->fuser3 = theDigestingScalar*kNormalizationNetworkFactor;

            // Set sound effects as player gets more and more digested
            int theDesiredRoomType = 26; // strange sounds right before you die
            if(theDigestingScalar < .33f)
            {
                // Water 1
                theDesiredRoomType = 14;
            }
            else if(theDigestingScalar < .66f)
            {
                // Water 2
                theDesiredRoomType = 15;
            }
            else if(theDigestingScalar < .9f)
            {
                // Water 3
                theDesiredRoomType = 16;
            }
            theDigestee->SetDesiredRoomType(theDesiredRoomType);

            if(theDigestee->pev->health <= 0)
            {
                thePlayerWasDigested = true;
            }
        }
        
        // If digestee is dead and no longer relevant
        if(!theDigestee->IsAlive() || !theDigestee->GetIsRelevant() || (theDigestee->GetTeam() == this->GetTeam()))
        {
            this->StopDigestion(thePlayerWasDigested);
        }
    }
}

bool AvHPlayer::GetDoesCurrentStateStopOverwatch() const
{
    AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
    const AvHTeam* theTeam = this->GetTeamPointer();

    bool theCurrentStateStopsOverwatch = false;
//  bool theCurrentStateStopsOverwatch = ((this->pev->button != 0 && !this->mOverwatchFiredThisThink) || /*(this->pev->velocity.Length() > kOverwatchBreakingVelocity) ||*/ !theWeapon || ((this->pev->iuser3 != AVH_USER3_MARINE_PLAYER) && !GetHasUpgrade(this->pev->iuser4, MASK_MARINE_OVERWATCH)));
//
//  // Overwatch not valid for alien players
//  if(theTeam && (theTeam->GetTeamType() != AVH_CLASS_TYPE_MARINE))
//  {
//      theCurrentStateStopsOverwatch = true;
//  }

    return theCurrentStateStopsOverwatch;
}

bool AvHPlayer::GetIsEntityInSight(CBaseEntity* inEntity)
{

    bool theSuccess = false;
	AvHTeam* theTeam = this->GetTeamPointer();
	// Elven - we don't want marines who are being digested to be able to sight anything.
    if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE) && GetHasUpgrade(this->pev->iuser4, MASK_DIGESTING))
	{
		return theSuccess;
	}
    // Check if the entitiy is in the player's view frustum.
    UTIL_MakeVectors ( pev->v_angle );
    
    Vector center = inEntity->pev->origin + (inEntity->pev->maxs + inEntity->pev->mins) / 2;
    Vector sightLine = center - (pev->origin + pev->view_ofs);
    
    Vector hSightLine = sightLine - DotProduct(sightLine, gpGlobals->v_up) * gpGlobals->v_up;
    Vector vSightLine = sightLine - DotProduct(sightLine, gpGlobals->v_right) * gpGlobals->v_right;
    
    float hDot = DotProduct(hSightLine.Normalize(), gpGlobals->v_forward);
    float vDot = DotProduct(vSightLine.Normalize(), gpGlobals->v_forward);
    
    float hAngle = acosf(hDot) * 180 / M_PI;
    float vAngle = acosf(vDot) * 180 / M_PI;

    if (hAngle > 180) hAngle -= 360;
    if (vAngle > 180) vAngle -= 360;
    
    float aspect = 1.333; // 640/480

    if (fabs(hAngle) <= pev->fov / 2 && fabs(vAngle) <= pev->fov / (2 * aspect))
    {
        if (FVisible(inEntity))
        {
			
            theSuccess = true;
        }
    }

//    if(GET_RUN_CODE(4096))
//    {
//        if (!theSuccess)
//        {
//
//            UTIL_MakeVectors ( pev->v_angle );
//
//            const float kMaxDistanceForSighting = 10000;
//
//            // Trace a ray in the direction the player is aiming.
//
//            Vector theStart = EyePosition();
//            Vector theEnd;
//            VectorMA(theStart, kMaxDistanceForSighting, gpGlobals->v_forward, theEnd);
//
//	        TraceResult tr;
//            UTIL_TraceLine(theStart, theEnd, dont_ignore_monsters, ignore_glass, ENT(pev), &tr);
//
//            if (tr.flFraction != 1 && tr.pHit == ENT(inEntity->pev))
//            {
//                int a = 0;
//                theSuccess = true;
//            }
//    
//        }
//    }
    // TODO: Make this better so we can see edges of things?  What about big aliens?

    return theSuccess;

}

char* AvHPlayer::GetPlayerModelKeyName()
{
    // Default to marine in ready room
    char* theModelKeyName = kSoldierName;

    AvHUser3 theUser3 = this->GetUser3();

    switch(theUser3)
    {
    case AVH_USER3_MARINE_PLAYER:
        theModelKeyName = kSoldierName;
        if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_13))
        {
            theModelKeyName = kHeavyName;
        }
        break;
    case AVH_USER3_COMMANDER_PLAYER:
        theModelKeyName = kCommanderName;
        break;
    case AVH_USER3_ALIEN_PLAYER1:
        theModelKeyName = kAlien1Name;
        break;
    case AVH_USER3_ALIEN_PLAYER2:
        theModelKeyName = kAlien2Name;
        break;
    case AVH_USER3_ALIEN_PLAYER3:
        theModelKeyName = kAlien3Name;
        break;
    case AVH_USER3_ALIEN_PLAYER4:
        theModelKeyName = kAlien4Name;
        break;
    case AVH_USER3_ALIEN_PLAYER5:
        theModelKeyName = kAlien5Name;
        break;
    case AVH_USER3_ALIEN_EMBRYO:
        theModelKeyName = kAlienGestationName;
        break;
    }

    return theModelKeyName;
}

void AvHPlayer::HandleOverwatch(void)
{
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE))
    {
        this->mOverwatchFiredThisThink = false;
        
        // If in overwatch
        if(this->mInOverwatch)
        {
            // do we have a target we're firing at?
            if(this->mOverwatchTarget != -1)
            {
                // is target around?
                edict_t* theTargetEdict = INDEXENT(this->mOverwatchTarget);
                if(!FNullEnt(theTargetEdict))
                {
                    // if so, move aim toward it but not farther than x degrees from starting pos
                    CBaseEntity* theTarget = CBaseEntity::Instance(theTargetEdict);
                    this->TurnOverwatchTowardsTarget(theTarget);
                    
                    // is it within our weapon range?  
                    float theTargetDistance = (theTarget->pev->origin - this->pev->origin).Length();
                    AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
                    if(theWeapon)
                    {
                        if(theWeapon->GetRange() >= theTargetDistance)
                        {
                            // do we have a clear line of sight to it?
                            bool theCanSeeTarget = this->GetIsEntityInSight(theTarget);
                            if(theCanSeeTarget || (gpGlobals->time - this->mTimeLastSeenOverwatchTarget < kOverwatchKeepFiringAfterMissingTargetTime))
                            {
                                // do we have ammo left?
                                if(!theWeapon->UsesAmmo() || (theWeapon->m_iClip > 0))
                                {
                                    // FIRE!
                                    this->pev->button |= IN_ATTACK;
                                    this->mOverwatchFiredThisThink = true;
                                }
                                
                                // Update last time we saw our target
                                if(theCanSeeTarget)
                                {
                                    this->mTimeLastSeenOverwatchTarget = gpGlobals->time;
                                    
                                    // Playback event to increase tension, but keep network usage down (once or twice a second?)
                                    if(this->pev->fuser2 != -1)
                                    {
                                        //if(RANDOM_LONG(0, 100) == 0)
                                        //{
                                        //PLAYBACK_EVENT_FULL(0, this->edict(), gTensionOverwatchEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
                                        //}
                                    }
                                }
                            }
                            
                            // if not, has it been a long time since we've seen it, or did we see it die?  Reset.
                            if( (gpGlobals->time - this->mTimeLastSeenOverwatchTarget > kOverwatchLostTargetTime) || 
                                (theCanSeeTarget && !theTarget->IsAlive()))
                            {
                                this->ResetOverwatch();
                            }
                        }
                    }
                    else
                    {
                        // This can happen when dropping weapons or switching roles
                        this->TurnOffOverwatch();
                    }
                }
                else
                {
                    this->ResetOverwatch();
                }
            }
            // is there a new target in range?
            else
            {
                this->AcquireOverwatchTarget();
            }
            
            // see if we moved so we're out
            if(this->GetDoesCurrentStateStopOverwatch())
            {
                this->TurnOffOverwatch();
            }
        }
        else
        {
            if(this->GetDoesCurrentStateStopOverwatch())
            {
                this->mTimeOfLastOverwatchPreventingAction = gpGlobals->time;
            }
            
            // if overwatch is enabled, see if we've been still long enough to put us into it
            if(this->mOverwatchEnabled && !(this->pev->flags & FL_FAKECLIENT))
            {
                if(this->mTimeOfLastOverwatchPreventingAction != -1)
                {
                    if(gpGlobals->time - this->mTimeOfLastOverwatchPreventingAction >= kOverwatchAcquireTime)
                    {
                        // if so, set overwatch on, make sure to set the current weapon into overwatch
                        this->TurnOnOverwatch();
                    }
                }
            }
        }
    }
}

void AvHPlayer::InternalAlienUpgradesThink()
{
    // If we're an alien player
    AvHTeam* theTeam = this->GetTeamPointer();

    if(this->GetIsAlien())
    {
        if(theTeam && !GetGameRules()->GetIsCombatMode())
        {
            // Preserve health and armor percentages as we get and remove upgrades
            float theHealthPercentage = this->pev->health/AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
            float theArmorPercentage = this->pev->armorvalue/AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);

            ASSERT(theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN);
            AvHAlienUpgradeListType theUpgrades = theTeam->GetAlienUpgrades();
            
            // If player has already decided on the direction to upgrade, spend any extra upgrade levels in that category
            AvHAddHigherLevelUpgrades(theUpgrades, this->pev->iuser4);
            
            // If we have more upgrades then we should, remove one randomly
            int theNumRemoved = AvHRemoveExcessUpgrades(theUpgrades, this->pev->iuser4);
            if(theNumRemoved > 0)
            {
                // Play a sound indicating this has happened
                this->PlayHUDSound(HUD_SOUND_ALIEN_UPGRADELOST);
            }

            // If we're cloaked, and we no longer have any sensory upgrades, trigger uncloak
            //int theNumSensoryUpgrades = AvHGetNumUpgradesInCategoryInList(theUpgrades, ALIEN_UPGRADE_CATEGORY_SENSORY);
            //if(GetHasUpgrade(this->pev->iuser4, MASK_ALIEN_CLOAKED) && (theNumSensoryUpgrades == 0))
            //{
            //  this->TriggerUncloak();
            //}

            this->pev->health = theHealthPercentage*AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
            this->pev->armorvalue = theArmorPercentage*AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);
        }
        
        if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && this->IsAlive())
        {
            // If we have cloaking, update our cloak state
            this->InternalAlienUpgradesCloakingThink();
            
            // If we have regeneration, heal us
            this->InternalAlienUpgradesRegenerationThink();

            // If we have pheromones, update them
            //this->InternalAlienUpgradesPheromonesThink();
        }
    }
    
    // Update ensnare here
    if(this->GetIsEnsnared())
    {
        if(gpGlobals->time > this->mTimeToBeUnensnared)
        {
            this->SetEnsnareState(false);
        }
    }
    
    // Update stun
    if(this->GetIsStunned())
    {
        if(gpGlobals->time > this->mTimeToBeFreeToMove)
        {
            this->SetIsStunned(false);
        }
    }
}

bool AvHPlayer::GetIsCloaked() const
{
    bool theIsCloaked = false;

    if( (this->GetOpacity() < 0.1f))
    {
        theIsCloaked = true;
    }

    return theIsCloaked;
}

bool AvHPlayer::GetIsPartiallyCloaked() const
{
    bool theIsCloaked = false;

    if( (this->GetOpacity() < 0.6f))
    {
        theIsCloaked = true;
    }

    return theIsCloaked;
}

bool AvHPlayer::GetRandomGameStartedTick(float inApproximateFrameRate)
{
    bool theTimeToTick = false;
    
    if(GetGameRules()->GetGameStarted() && (inApproximateFrameRate > 0))
    {
        ASSERT(this->mPreThinkFrameRate > 0);
        
        int theUpperBound = (int)(this->mPreThinkFrameRate/inApproximateFrameRate);
        if(RANDOM_LONG(1, theUpperBound) == 1)
        {
            theTimeToTick = true;
        }
    }
    
    return theTimeToTick;
}

void AvHPlayer::TriggerUncloak()
{
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && this->IsAlive())
    {
        this->mTriggerUncloak = true;

        SetUpgradeMask(&this->pev->iuser4, MASK_SENSORY_NEARBY, false);

        this->Uncloak();
    }
}

void AvHPlayer::InternalAlienUpgradesPheromonesThink()
{
    const float kPheromoneUpdateInterval = .4f;
    const float kPheromoneBaseRange = 600;
    const int kMaxPheromonePuffs = 3;

    if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_8))
    {
        int theRange = kPheromoneBaseRange;

        if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_14))
        {
            theRange *= 2;
        }
        else if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_15))
        {
            theRange *= 3;
        }

        if(this->mTimeOfLastPheromone == -1 || (gpGlobals->time > this->mTimeOfLastPheromone + kPheromoneUpdateInterval))
        {
            typedef std::map<EntityInfo, float> PlayerDistanceListType;
            PlayerDistanceListType thePlayerDistanceList;

            // Look for players in range to draw pheromones of
            FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
                
                // Find nearest distance to friendly and relevant player
                if(theEntity->GetIsRelevant() && (theEntity->pev->team != this->pev->team) && !GetHasUpgrade(theEntity->pev->iuser4, MASK_TOPDOWN) && (theEntity != this) /*&& !this->GetIsEntityInSight(theEntity)*/)
                {
                    double theDistance = VectorDistance(theEntity->pev->origin, this->pev->origin);
                    if(theDistance < theRange)
                    {
                        // Choose nearest x players to emit from
                        thePlayerDistanceList[theEntity->entindex()] = theDistance;
                    }
                }
            
            END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

            for(int theNumPheromonePuffs = 0; (theNumPheromonePuffs < kMaxPheromonePuffs) && (thePlayerDistanceList.size() > 0); theNumPheromonePuffs++)
            {
                // Find the nearest entity
                PlayerDistanceListType::iterator theClosestIter = thePlayerDistanceList.begin();
                float theClosestDistance = sqrt(kMaxMapDimension*kMaxMapDimension + kMaxMapDimension*kMaxMapDimension);

                for(PlayerDistanceListType::iterator theIter = thePlayerDistanceList.begin(); theIter != thePlayerDistanceList.end(); theIter++)
                {
                    float theCurrentRange = theIter->second;
                    if(theCurrentRange < theClosestDistance)
                    {
                        theClosestDistance = theCurrentRange;
                        theClosestIter = theIter;
                    }
                }
                
                // Play a puff for it
                int theCurrentEntityIndex = theClosestIter->first;
                CBaseEntity* theCurrentEntity = (CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theCurrentEntityIndex)));
                ASSERT(theCurrentEntity);

                AvHSUPlayParticleEvent(kpsPheromoneEffect, this->edict(), theCurrentEntity->pev->origin, FEV_HOSTONLY);

                // Delete that entity
                thePlayerDistanceList.erase(theClosestIter);
            }

            this->mTimeOfLastPheromone = gpGlobals->time;
        }
    }
}

float AvHPlayer::GetCloakTime() const
{
    float theCloakTime = AvHCloakable::GetCloakTime();

    if(this->GetIsAlien())
    {
        // If we have cloaking upgrade, we cloak faster
        int theCloakingLevel = AvHGetAlienUpgradeLevel(this->pev->iuser4, MASK_UPGRADE_7);
        if(theCloakingLevel > 0)
        {
            theCloakTime = BALANCE_VAR(kCloakTime)/theCloakingLevel;
        }
    }

    return theCloakTime;
}

void AvHPlayer::InternalAlienUpgradesCloakingThink()
{
	// joev: 
	// 0000342 - Cloaking no longer depends on speed. 

	// For some reason the lerk moves faster when turning
    //const float kWalkSpeedFactor = (this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER3) ? .95f : .7f;
	//const int kMaxSpeed = this->pev->maxspeed;
    //elven - needs to update the speed factor here to count for celerity - see bug 342
	//int theBaseSpeed, theUnencumberedSpeed;
    //this->GetSpeeds(theBaseSpeed, theUnencumberedSpeed);
    //const int kMaxSpeed = this->pev->maxspeed;
	//const int kMaxSpeed = (theUnencumberedSpeed > this->pev->maxspeed ) ? theUnencumberedSpeed : this->pev->maxspeed;

    //// If player moving too fast or trigger uncloak is set
    //if( ( (this->pev->velocity.Length() > kMaxSpeed*kWalkSpeedFactor) && !GetHasUpgrade(this->pev->iuser4, MASK_SENSORY_NEARBY)) || this->mTriggerUncloak)
    if(this->mTriggerUncloak)
    {
		//Uncloak
        this->Uncloak();
    }
	// :joev
	else
    { 
        // If we have cloaking upgrade
        int theCloakingLevel = AvHGetAlienUpgradeLevel(this->pev->iuser4, MASK_UPGRADE_7);
        if(theCloakingLevel > 0)
        {
			// If time needed to cloak has passed
			// puzl:  864
			float theMaxWalkSpeed=this->pev->maxspeed * ((this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER3) ? .95f : .7f);
			float theSpeed=AvHGetAlienUpgradeLevel(this->pev->iuser4, MASK_SENSORY_NEARBY) ? 0.0f : this->pev->velocity.Length();
			this->SetSpeeds(theSpeed, this->pev->maxspeed*1.05, theMaxWalkSpeed);
//			if ( this->pev->velocity.Length() < theMaxWalkSpeed ) 
//			{
//				ALERT(at_console, "Cloaking\n");
				this->Cloak();
//			}
        }
    }
    
    this->mTriggerUncloak = false;
}

bool AvHPlayer::Redeem()
{
    bool theSuccess = false;

    // Can only be pulled back if we have at least one active hive
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam && (theTeam->GetNumActiveHives() > 0))
    {
        // Bring player back
        if(this->GetTeamPointer()->GetNumActiveHives() > 0)
        {
            // Play redeem effect where it happened so attackers know it happened
            PLAYBACK_EVENT_FULL(0, this->edict(), gStartCloakEventID, 0, this->pev->origin, (float *)&g_vecZero, this->GetAlienAdjustedEventVolume(), 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
            
            edict_t* theNewSpawnPoint = GetGameRules()->SelectSpawnPoint(this);
            if(!FNullEnt(theNewSpawnPoint))
            {
                //// Create building here to test getting stuck
                //const int kOffset = 20;
                //Vector theBaseOrigin = theNewSpawnPoint->v.origin;
                //Vector theRandomOffset(theBaseOrigin.x + RANDOM_LONG(-kOffset, kOffset), theBaseOrigin.y + RANDOM_LONG(-kOffset, kOffset), theBaseOrigin.z + RANDOM_LONG(-kOffset, kOffset));
                //AvHSUBuildTechForPlayer(ALIEN_BUILD_MOVEMENT_CHAMBER, theRandomOffset, this);

                theNewSpawnPoint = GetGameRules()->SelectSpawnPoint(this);
                if(!FNullEnt(theNewSpawnPoint))
                {
                    if(this->GetIsDigesting())
                    {
                        this->StopDigestion(false);
                    }

                    mTimeOfLastRedeem = gpGlobals->time;

                    // Respawn player normally
                    this->InitPlayerFromSpawn(theNewSpawnPoint);
                    
                    PLAYBACK_EVENT_FULL(0, this->edict(), gStartCloakEventID, 0, this->pev->origin, (float *)&g_vecZero, this->GetAlienAdjustedEventVolume(), 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
                }

                theSuccess = true;
            }
        }
    }

    return theSuccess;
}

void AvHPlayer::InternalAlienUpgradesRegenerationThink()
{
	// puzl - 0000856  - Add innate regeneration for all alien players.
	// Add a small and silent innate health and armor regeneration for
	// all alien players, similar to the innate regeneration of all alien
	// chambers. If a player chooses the regeneration upgrade, it replaces
	// the innate regeneration rather than adding to it.

	// If enough time has elapsed to heal
	if((this->mTimeOfLastRegeneration == -1) || (gpGlobals->time - this->mTimeOfLastRegeneration > BALANCE_VAR(kAlienRegenerationTime)))
	{
		int theRegenLevel = AvHGetAlienUpgradeLevel(this->pev->iuser4, MASK_UPGRADE_2);
		int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());

		float theRegenAmount = 0.0f;
		float theRegenPercentage = BALANCE_VAR(kAlienInnateRegenerationPercentage);

		//  If we have regeneration upgrade, multiply the amount by the regen level
		if(theRegenLevel > 0)
		{
			theRegenPercentage = BALANCE_VAR(kAlienRegenerationPercentage);
			theRegenAmount = (theRegenPercentage*theMaxHealth)*theRegenLevel;
		}

		// Innate regeneration is at a fixed rate
		else {
			theRegenAmount = theRegenPercentage*(float)theMaxHealth;
		}
		// Always do at least 1 health of innate regeneration
		theRegenAmount=max(theRegenAmount, 1.0f);

		// Don't play a sound for innate regeneration
		this->Regenerate(theRegenAmount, theRegenLevel > 0 );
	}

	// Do we have the redemption?
    int theRedemptionLevel = AvHGetAlienUpgradeLevel(this->pev->iuser4, MASK_UPGRADE_3);
    if(theRedemptionLevel > 0)
    {
        // Is the player really hurting?
        int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());

        if((this->pev->health < theMaxHealth*BALANCE_VAR(kRedemptionThreshold)) && this->IsAlive())
        {
            const float kPullBackTime = 20.0f;
            if((this->mLastTimeRedemptionTriggered == -1) || (gpGlobals->time > (this->mLastTimeRedemptionTriggered + kPullBackTime)))
            {
                // Chance per second
                float theRedemptionChance = theRedemptionLevel*BALANCE_VAR(kRedemptionChance);

                // How many times is this being called per second?
                float theThinkInterval = 1.0f;

                if(this->mLastTimeCheckedRedemption > 0)
                {
                    // The longer the time between checks, the higher the chance of being redeemed
                    theThinkInterval = (gpGlobals->time - this->mLastTimeCheckedRedemption);
                }

                // Chance per second times seconds elapsed
                if(RANDOM_FLOAT(0, 1.0f) <= theRedemptionChance*theThinkInterval)
                {
                    if(this->Redeem())
                    {
                        this->mLastTimeRedemptionTriggered = gpGlobals->time;
                    }
                }
            }
        }
    }

    this->mLastTimeCheckedRedemption = gpGlobals->time;
}

void AvHPlayer::ProcessEntityBlip(CBaseEntity* inEntity)
{
   	// puzl: 982
	// Make alien hivesight range a balance var
    const float kAlienFriendlyBlipRange = BALANCE_VAR(kHiveSightRange);
    
    // Is player alien?
    bool theIsAlien = this->GetIsAlien(true);
    
    // Is player marine?
    bool theIsMarine = this->GetIsMarine(true);
    
    ASSERT(theIsAlien || theIsMarine);
    // Friendly?
    bool theIsFriendly = ((inEntity->pev->team == 0) || (inEntity->pev->team == this->GetTeam())) ;
	CBaseEntity* theSpectatingEntity = this->GetSpectatingEntity();
	if(theSpectatingEntity)
	{
		theIsFriendly = ((theSpectatingEntity->pev->team == 0) || (theSpectatingEntity->pev->team == inEntity->pev->team));
	}

    // If this player in an alien
    if(theIsAlien && this->IsAlive(true) && !GetHasUpgrade(inEntity->pev->iuser4, MASK_TOPDOWN))
    {
		// elven added - don't let digesting players get rendered on parasite
		bool theEntityIsDigesting = GetHasUpgrade(inEntity->pev->iuser4, MASK_DIGESTING);
		bool theEntityIsParasited = (theEntityIsDigesting) ? false : GetHasUpgrade(inEntity->pev->iuser4, MASK_PARASITED);
		bool theEntityIsNearSensory = (theEntityIsDigesting) ? false : GetHasUpgrade(inEntity->pev->iuser4, MASK_SENSORY_NEARBY);
		bool theEntityIsInHiveSight = (theEntityIsNearSensory || theEntityIsParasited || (GetHasUpgrade(inEntity->pev->iuser4, MASK_VIS_SIGHTED) && inEntity->IsPlayer()) || theIsFriendly || (inEntity->pev->iuser3 == AVH_USER3_HIVE));

        //bool theHasHiveSightUpgrade = true;//GetHasUpgrade(this->pev->iuser4, MASK_ALIEN_UPGRADE_11) || GetGameRules()->GetIsTesting();
        bool theEntityIsInSight = this->GetIsEntityInSight(inEntity);
        
        // If we're processing a relevant player
        AvHPlayer* theOtherPlayer = dynamic_cast<AvHPlayer*>(inEntity);
		bool theIsSpectatingEntity = this->GetIsSpectatingPlayer(inEntity->entindex());
        if(theOtherPlayer && (theOtherPlayer != this) && !theIsSpectatingEntity && theOtherPlayer->GetIsRelevant())
        {
            // Calculate angle and distance to player
            Vector theVectorToEntity = inEntity->pev->origin - this->pev->origin;
            float theDistanceToEntity = theVectorToEntity.Length();

            // If friendly
            if(theEntityIsInHiveSight && theIsFriendly && !theEntityIsInSight)
            {
                int8 theBlipStatus = kFriendlyBlipStatus;
                if(GetGameRules()->GetIsEntityUnderAttack(inEntity->entindex()))
                {
                    theBlipStatus = kVulnerableFriendlyBlipStatus;
                }

                if(theOtherPlayer->GetUser3() == AVH_USER3_ALIEN_PLAYER2)
                {
                    theBlipStatus = kGorgeBlipStatus;
                }
                    
                //if(theOtherPlayer->GetEnemySighted())
                //{
                //  theBlipStatus = 1;
                //}

                if(theBlipStatus || (theDistanceToEntity < kAlienFriendlyBlipRange))
                {
                    // Info is the player index by default
                    int theBlipInfo = theOtherPlayer->entindex();
                    this->mFriendlyBlips.AddBlip(inEntity->pev->origin.x, inEntity->pev->origin.y, inEntity->pev->origin.z, theBlipStatus, theBlipInfo);
                }
            }
            // else if enemy
            else
            {
                // If it's in range
                //if(theDistanceToEntity < kAlienEnemyBlipRange)
                //{

                int8 theBlipStatus = kEnemyBlipStatus;

                bool theDrawBlip = false;

                if(!theIsFriendly && !theEntityIsInSight)
                {
                    // If they're parasited
                    if(theEntityIsParasited)
                    {
                        theDrawBlip = true;
                    }

                    // If we have scent of fear upgrade - don't render if being eaten. changed by elven.
					if( (theEntityIsNearSensory || GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_9)) && !theEntityIsParasited && !GetHasUpgrade(inEntity->pev->iuser4, MASK_DIGESTING))
                    {
                        int theRange = BALANCE_VAR(kScentOfFearRadiusPerLevel);
                        if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_14))
                        {
                            theRange *= 2;
                        }
                        else if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_15))
                        {
                            theRange *= 3;
                        }
                            
                        // ...and blip is within range
							if( (theRange > theDistanceToEntity) || theEntityIsNearSensory )
                        {
//                          int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(theOtherPlayer->pev->iuser4, theOtherPlayer->GetUser3());
//                  
//                          // ...and blip is under attack or weak
//                          if(GetGameRules()->GetIsEntityUnderAttack(theOtherPlayer->entindex()) || (theOtherPlayer->pev->health < (theMaxHealth/3)))
//                          {
                                theBlipStatus = kVulnerableEnemyBlipStatus;
                                theDrawBlip = true;
//                          }
                        }
                    }
                }
                
                // Add it if it's in hive sight, or if we have scent of fear and he's marked
                if(theDrawBlip)
                {
                    this->mEnemyBlips.AddBlip(inEntity->pev->origin.x, inEntity->pev->origin.y, inEntity->pev->origin.z, theBlipStatus);
                }
                //}
            }
        }
        // else if we're processing a generic buildable
        else
        {
            AvHBaseBuildable* theBaseBuildable = dynamic_cast<AvHBaseBuildable*>(inEntity);
            if(theBaseBuildable)
            {
                // If friendly hive
                if(theIsFriendly)
                {
                    // If not visible
                    if(!theEntityIsInSight)
                    {
                        // If it's being hurt
						bool theDrawBlip=false;
                        AvHTeam* theTeam = this->GetTeamPointer();

                        AvHAlertType theAlertType = ALERT_NONE;
                        int8 theBlipStatus = kFriendlyBlipStatus;
                        bool theIsUnderAttack = GetGameRules()->GetIsEntityUnderAttack(theBaseBuildable->entindex());
                        if(theIsUnderAttack)
                        {
                            theBlipStatus = kVulnerableFriendlyBlipStatus;
							theDrawBlip=true;
                        }

						// Add it if relevant
                        AvHHive* theHive = dynamic_cast<AvHHive*>(inEntity);
						if(theHive )
                        {
							theDrawBlip=true;
							if(!theIsUnderAttack)
                            {
                                theBlipStatus = kHiveBlipStatus;
                            }
							if ( theHive && !theHive->GetIsBuilt() )
							{
								theBlipStatus=kUnbuiltHiveBlipsStatus;
							}
                        }
						if ( theDrawBlip == true )
						{
                            // Info is the player index or the kBaseBlipInfoOffset + the iuser3
                            int theBlipInfo = kBaseBlipInfoOffset + inEntity->pev->iuser3;
                            if(theBlipInfo >= sizeof(char)*128)
                            {
                                ASSERT(false);
                            }
							this->mFriendlyBlips.AddBlip(inEntity->pev->origin.x, inEntity->pev->origin.y, inEntity->pev->origin.z, theBlipStatus, theBlipInfo);
						}
                    }
                }
                // If enemy
                else
                {
                    // If it's in hive sight, but not visible
                    if(!theEntityIsInSight && theEntityIsInHiveSight)
                    {
                        // If within range
                        //if(theDistanceToEntity < kAlienEnemyBlipRange)
                        //{
		                // Add it
                        this->mEnemyBlips.AddBlip(inEntity->pev->origin.x, inEntity->pev->origin.y, inEntity->pev->origin.z, kEnemyStructureBlipsStatus);
                        //}
							}
						}
            }
        }
    }
    // else if this player is a marine 
    else if(theIsMarine && this->IsAlive(true))
    {
        bool theTeamHasMotionTracking = GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_8);
        
        // If player is commander (or is any marine with motion tracking?)
        if((this->GetUser3() == AVH_USER3_COMMANDER_PLAYER) || (theTeamHasMotionTracking))
        {
			
             // If enemy, add it to enemy list if "detected" // Elven - we don't want motion blips on aliens visible to us.
			// && !(inEntity->pev->iuser4 & MASK_VIS_SIGHTED) <- this won't work as if I see an alien, other marines won't see MT if they're in another room
			bool visibleToThis = this->GetIsEntityInSight(inEntity);
			if(!theIsFriendly && (inEntity->pev->iuser4 & MASK_VIS_DETECTED) && !visibleToThis && inEntity->IsAlive())//voogru: make sure there alive
            {
                this->mEnemyBlips.AddBlip(inEntity->pev->origin.x, inEntity->pev->origin.y, inEntity->pev->origin.z, kMarineEnemyBlipStatus);
            }
        }
    }
}

void AvHPlayer::ClearBlips()
{
    this->mEnemyBlips.Clear();
    this->mFriendlyBlips.Clear();
}

void AvHPlayer::ClientDisconnected()
{
	this->InitBalanceVariables();
    this->ResetGameNewMap();
    this->ResetEntity();
}

void AvHPlayer::ResetGameNewMap()
{
    this->mNumParticleTemplatesSent = 0;
    this->mTimeOfLastParticleTemplateSending = -1;
    this->mClientGamma = kDefaultMapGamma;
    this->mNewMap = true;
}

void AvHPlayer::InternalCommanderThink()
{
    // Finally, if we are commander, set the special PAS origin to  use
    if(this->GetUser3() == AVH_USER3_COMMANDER_PLAYER)
    {
        // Only update every so often
        const float kUpdatePASInterval = 1.0f;
        float theTime = gpGlobals->time;
        if((this->mTimeOfLastPASUpdate == -1) || (theTime > (this->mTimeOfLastPASUpdate + kUpdatePASInterval)))
        {
            // Default to our last known "real-world" origin, in case there are no other players?
            VectorCopy(this->mPositionBeforeTopDown, this->mSpecialPASOrigin);
            
            // Max map size is 8012x8012
            double theShortestDistance = 64192144;
            
            // Loop through all players
            FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
                
                // Find nearest distance to friendly and relevant player
                if(theEntity->GetIsRelevant() && (theEntity->pev->team == this->pev->team) && (theEntity != this))
                {
                    double theDistance = VectorDistance(theEntity->pev->origin, this->pev->origin);
                    if(theDistance < theShortestDistance)
                    {
                        VectorCopy(theEntity->pev->origin, this->mSpecialPASOrigin);
                        theShortestDistance = theDistance;

                        //SET_VIEW(ENT(this->pev), ENT(theEntity->pev));
                    }
                }
                
            END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
                    
            this->mTimeOfLastPASUpdate = theTime;
        }
    }
}

void AvHPlayer::InternalBoundResources()
{
    // Aliens have a max resource amount, put any that overflows back into the team
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam && this->GetIsAlien())   
    {
        float theMaxResources = theTeam->GetMaxResources((AvHUser3)this->pev->iuser3);
        
        float theExtraResources = this->mResources - theMaxResources;
        if(theExtraResources > 0)
        {
            theTeam->SetTeamResources(theTeam->GetTeamResources() + theExtraResources);
        }
        
        this->mResources = min(this->mResources, theMaxResources);
    }

    if(GetGameRules()->GetIsCombatMode())
    {
        this->mResources = 0;
    }
}

bool AvHPlayer::QueryEnemySighted(CBaseEntity* inEntity)
{
    bool theSuccess = false;

    if((this->pev->team != inEntity->pev->team) && (this->pev->team != TEAM_IND) && (inEntity->pev->team != TEAM_IND))
    {
        if(inEntity != this)
        {
            if(inEntity->IsAlive() && this->GetIsEntityInSight(inEntity))
            {
                AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inEntity);
				//this->CompareToPlayer(inEntity); //voogru: WTF?
                if(!thePlayer || (thePlayer->GetIsRelevant() && !thePlayer->GetIsCloaked()))
                {
                    this->mEnemySighted = true;
                    this->mTimeOfLastEnemySighting = gpGlobals->time;
                    theSuccess = true;
                }
            }
        }
    }
    return theSuccess;
}

void AvHPlayer::InternalAlienThink()
{
    if(this->GetIsAlien() && this->IsAlive())
    {
        // Are we gestating?  Has enough time passed so we are something new?
        if(this->pev->iuser3 == AVH_USER3_ALIEN_EMBRYO)
        {
            float theCurrentTime = gpGlobals->time;
            float theFullTimeToGestate = GetGameRules()->GetBuildTimeForMessageID(this->mEvolution);
            if(GetGameRules()->GetIsTesting())
            {
                theFullTimeToGestate = 1.0f;
            } 
            
            this->TriggerProgressBar(this->entindex(), 3);

            // If changing this, make sure to change spectator behavior in InternalCommonThink
            float theTimeElapsed = theCurrentTime - this->mTimeGestationStarted;
            this->pev->fuser3 = (theTimeElapsed/theFullTimeToGestate)*kNormalizationNetworkFactor;

            if(theTimeElapsed >= theFullTimeToGestate)
            {
                this->ProcessEvolution();

                // Set ourselves crouching so we have a smaller chance of getting stuck
                if(AvHMUGetCanDuck(this->pev->iuser3))
                {
                    SetBits(this->pev->flags, FL_DUCKING);
                }
            }
        }
        
        // Has enough time passed since we started screaming?
        if(this->mIsScreaming)
        {
            if(gpGlobals->time > (this->mTimeStartedScream + BALANCE_VAR(kPrimalScreamDuration)))
            {
                this->mIsScreaming = false;
            }
        }

		// Uncloak if we are charging
		if(GetHasUpgrade(this->pev->iuser4, MASK_ALIEN_MOVEMENT))
		{
			this->TriggerUncloak();
		}
    }
}

void AvHPlayer::InternalCommonThink()
{
    if(GetGameRules()->GetGameStarted())
    {
        this->mPreThinkTicks++;
        
        float theTimePassed = gpGlobals->time - GetGameRules()->GetTimeGameStarted();
        this->mPreThinkFrameRate = this->mPreThinkTicks/theTimePassed;
        
//      if(RANDOM_LONG(0, 125) == 0)
//      {
//          char theMessage[128];
//          sprintf(theMessage, "Num ents: %d\n", GetGameRules()->GetNumEntities());
//          //sprintf(theMessage, "Time passed: %f, ticks: %d, rate: %f, %d\n", theTimePassed, this->mPreThinkTicks, this->mPreThinkFrameRate, gNumFullPackCalls);
//          UTIL_SayText(theMessage, this);
//      }
    }
    else
    {
        this->mPreThinkTicks = 0;
        this->mPreThinkFrameRate = 30;
    }

    // If we're not in the ready room, and the a victor has just been determined, see if it's time to reset us
    // This has to be done at a different time per player, to avoid overflows
    if((GetGameRules()->GetVictoryTeam() != TEAM_IND) && (this->GetPlayMode() != PLAYMODE_READYROOM))
    {
        // Get victory time, see if it's time to reset us
        int thePlayerIndex = this->entindex();
        int theSecondToReset = kVictoryIntermission - 1 - kMaxPlayers/kResetPlayersPerSecond + (thePlayerIndex - 1)/kResetPlayersPerSecond;
        //ASSERT(theSecondToReset >= 0);
        //ASSERT(theSecondToReset < kVictoryIntermission);

        // NOTE: This depends on gamerules not allong players join a team after victory has been declared
        float theVictoryTime = GetGameRules()->GetVictoryTime();
        if(gpGlobals->time > (theVictoryTime + theSecondToReset))
        {
            this->SetPlayMode(PLAYMODE_READYROOM, true);
        }
    }
    
    // Must be called every frame to prevent exploiting
    this->SetModelFromState();
	
    
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam)
    {
        // Clear existing upgrades for marines.  Aliens have their own individual upgrades.
        if(this->GetIsMarine() && !GetGameRules()->GetIsCombatMode())
        {
            int theInvertedUpgradeMask = ~kUpgradeBitMask;
            this->pev->iuser4 &= theInvertedUpgradeMask;
        }
        
        // Set the current upgrades
        this->pev->iuser4 |= theTeam->GetTeamWideUpgrades();
    }
    
    // Update active and inactive inventory
    const float kUpdateInventoryInterval = .5f;
    if(gpGlobals->time > (this->mLastInventoryThink + kUpdateInventoryInterval))
    {
        this->UpdateInventoryEnabledState(this->mNumHives, true);
        this->mLastInventoryThink = gpGlobals->time;
    }
    
    // Remember last time we were playing
    if(this->GetPlayMode() == PLAYMODE_PLAYING)
    {
        this->mTimeLastPlaying = gpGlobals->time;
    }
    
    this->InternalBoundResources();

    // Players keep their health in fuser2
    int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
    int theCurrentHealth = max(0.0f, this->pev->health);
    int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);
    int theCurrentArmor = max(0.0f, this->pev->armorvalue);

    // Draw ring to take into account health and armor for aliens, just health for marines (so gorge and comm know when to heal)
    float theScalar = (float)theCurrentHealth/theMaxHealth;
    if(this->GetIsAlien())
    {
        theScalar = (theCurrentHealth + theCurrentArmor)/(float)(theMaxHealth + theMaxArmor);
    }
    this->pev->fuser2 = theScalar*kNormalizationNetworkFactor;
    
    //float theRandomAngle = RANDOM_FLOAT(0, 50);
    //this->pev->v_angle.x = theRandomAngle;
    //VectorCopy(this->pev->angles, this->mAnglesBeforeTopDown);
    //VectorCopy(this->pev->v_angle, this->mViewAnglesBeforeTopDown);
    
    if(GetGameRules()->GetCountdownStarted() && !GetGameRules()->GetGameStarted() && (GetPlayMode() == PLAYMODE_PLAYING) && !GetGameRules()->GetCheatsEnabled())
    {
        this->pev->flags |= FL_FROZEN;
        DROP_TO_FLOOR(this->edict());
    }
    else
    {
        this->pev->flags &= ~FL_FROZEN;
    }

    // If we have a different desired name, change to it
    if(GetGameRules()->GetArePlayersAllowedToJoinImmediately())
    {
        // If our desired net name hasn't been set, set it now
        if(this->mDesiredNetName != "")
        {
            char* theInfoBuffer = g_engfuncs.pfnGetInfoKeyBuffer(this->edict());

            char theBuffer[256];
            strcpy(theBuffer, this->mDesiredNetName.c_str());
            g_engfuncs.pfnSetClientKeyValue(this->entindex(), theInfoBuffer, "name", theBuffer);

            this->mDesiredNetName = "";
        }
    }

    // If we're spectating a target, set our health and armorvalue to theirs
    // For spectators that are tracking players, move to their location, to prevent problems outside of the PVS
    AvHPlayer* theEntity = NULL;
    if(AvHSUGetEntityFromIndex(this->pev->iuser2, theEntity))
    {
        // If entity is no longer spectatable, jump to the next target
        if(theEntity->GetIsInTopDownMode() || (theEntity->GetPlayMode() != PLAYMODE_PLAYING))
        {
            this->Observer_FindNextPlayer();
        }
        else
        {
            this->pev->health = theEntity->pev->health;
            this->pev->armorvalue = theEntity->pev->armorvalue;
            this->pev->fuser3 = theEntity->pev->fuser3;

            if((theEntity->GetUser3() == AVH_USER3_ALIEN_EMBRYO) || (theEntity->GetIsBeingDigested()))
            {
                this->TriggerProgressBar(theEntity->entindex(), 3);
            }

            // Hacky way to make sure player is in PVS of target
            float theDistance = VectorDistance(theEntity->pev->origin, this->pev->origin);
            if(theDistance > 700)
            {
                VectorCopy(theEntity->pev->origin, this->pev->origin);
            }
        }   
    }

    // If we are digesting a player, process him (process even for marines in case we're testing)
    this->InternalDigestionThink();

    // Update FOV and view height every tick, needed for first-person spectating
    this->SetViewForUser3();

    PropagateServerVariables();

}

void AvHPlayer::PropagateServerVariables()
{
    for (int i = 0; i < (signed)mServerVariableList.size(); ++i)
    {
        std::string theValue = CVAR_GET_STRING( mServerVariableList[i].mName.c_str() );
        
        if ( mServerVariableList[i].mLastValueSent != theValue)
        {
			NetMsg_ServerVar( this->pev, mServerVariableList[i].mName, theValue );
            mServerVariableList[i].mLastValueSent = theValue;
            break; // Only send one message per tick to avoid overflow.
        }
    }
}

void AvHPlayer::InternalMarineThink()
{
    if(this->GetIsMarine() && (this->pev->iuser3 != AVH_USER3_COMMANDER_PLAYER))
    {
        // Slowly heal power armor over time
        if(this->GetHasPowerArmor())
        {
            if(this->mLastPowerArmorThink != -1)
            {
                float theTimePassed = gpGlobals->time - this->mLastPowerArmorThink;
                if(theTimePassed > 0.0f)
                {
                    // Key regen to velocity to enhance leap-frogging, ala Halo
                    float theVelocity = this->pev->velocity.Length();
                    float theRegenFactor = .5f;
                    if(theVelocity > 0)
                    {
                        //theRegenFactor = .3f - .3f*(theVelocity/kMaxGroundPlayerSpeed);
                        theRegenFactor = 0.1f;
                    }
                    theRegenFactor = max(min(theRegenFactor, 1.0f), 0.0f);
                    const float kPowerRegenRate = theRegenFactor*2.0f;
                    
                    int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);
                    if(this->pev->armorvalue < theMaxArmor)
                    {
                        this->pev->armorvalue = min((float)theMaxArmor, this->pev->armorvalue + kPowerRegenRate*theTimePassed);
                    }
                }
            }
            this->mLastPowerArmorThink = gpGlobals->time;
        }

        // Update buffed
        if(this->GetIsCatalysted())
        {
            if(gpGlobals->time > this->mTimeToEndCatalyst)
            {
                this->SetIsCatalysted(false);
            }
        }
    }
}

void AvHPlayer::InternalPreThink()
{
    PROFILE_START()
    this->InternalCommonThink();
    PROFILE_END(kPlayerCommonThink);

    PROFILE_START()
    this->InternalAlienThink();
    PROFILE_END(kPlayerAlienThink)

    PROFILE_START()
    this->InternalMarineThink();
    PROFILE_END(kPlayerMarineThink)

    PROFILE_START()
    this->InternalCommanderThink();
    PROFILE_END(kPlayerCommanderThink)

    PROFILE_START()
    this->InternalAlienUpgradesThink();
    PROFILE_END(kPlayerAlienUpgradesThink)

    PROFILE_START()
    this->InternalCombatThink();
    PROFILE_END(kPlayerCombatThink)
    //this->InternalEnemySightedPreThink(); 
    
    PROFILE_START()
    this->InternalSpeakingThink();
    PROFILE_END(kPlayerSpeakingThink)

    PROFILE_START()
    this->InternalProgressBarThink();
    PROFILE_END(kPlayerProgressBarThink)

    PROFILE_START()
    this->InternalFogThink();
    PROFILE_END(kPlayerFogThink)

    PROFILE_START()
    this->InternalHUDThink();
    PROFILE_END(kPlayerHUDThink)
}

void AvHPlayer::InternalFogThink()
{
    if((this->mTimeOfLastFogTrigger != -1) && (gpGlobals->time > this->mTimeOfLastFogTrigger + this->mFogExpireTime))
    {
        this->mCurrentFogEntity = -1;
    }
}

void AvHPlayer::InternalHUDThink()
{
    // Pull weapon out if we're done using something
    if(this->mTimeOfLastUse != -1)
    {
        const float kUseTime = .5f;
        if(gpGlobals->time > this->mTimeOfLastUse + kUseTime)
        {
            // Don't deploy while you're a commander (happens if you finish building something next to command station and immediately jump in)
            if(!this->GetIsInTopDownMode() && !this->GetIsBeingDigested())
            {
                this->DeployCurrent();
                this->mTimeOfLastUse = -1;
            }
        }
    }

    // Don't hide chat by default
    int theHideHUD = HIDEHUD_WEAPONS | HIDEHUD_FLASHLIGHT | HIDEHUD_HEALTH;

    // Use local player or player we're spectating
    AvHPlayer* theVisiblePlayer = this;
    if(this->pev->iuser1 == OBS_IN_EYE)
    {
        AvHPlayer* theEntity = NULL;
        if(AvHSUGetEntityFromIndex(this->pev->iuser2, theEntity))
        {
            theVisiblePlayer = theEntity;
        }
    }

    AvHPlayMode thePlayMode = theVisiblePlayer->GetPlayMode();
    AvHUser3 theUser3 = theVisiblePlayer->GetUser3();

    if((thePlayMode == PLAYMODE_READYROOM) || (thePlayMode == PLAYMODE_REINFORCING) || (thePlayMode == PLAYMODE_AWAITINGREINFORCEMENT))
    {
        theHideHUD = HIDEHUD_FLASHLIGHT | HIDEHUD_WEAPONS | HIDEHUD_HEALTH;

        // Only hide health when not following a target
        if((thePlayMode == PLAYMODE_AWAITINGREINFORCEMENT) && (this->pev->iuser1 == OBS_IN_EYE))
        {
            //theHideHUD &= ~HIDEHUD_WEAPONS;
            theHideHUD &= ~HIDEHUD_HEALTH;
        }
    }
    else if(thePlayMode == PLAYMODE_PLAYING)
    {
        theHideHUD = 0;

        if(!GetGameRules()->FAllowFlashlight())
        {
            theHideHUD |= HIDEHUD_FLASHLIGHT;
        }

        if(!theVisiblePlayer->pev->viewmodel)
        {
            //theHideHUD |= HIDEHUD_WEAPONS | HIDEHUD_FLASHLIGHT;
        }

        if(theUser3 == AVH_USER3_ALIEN_EMBRYO)
        {
            //theHideHUD |= HIDEHUD_HEALTH;
        }

        if(GetHasUpgrade(theVisiblePlayer->pev->iuser4, MASK_TOPDOWN) || theVisiblePlayer->GetIsBeingDigested())
        {
            theHideHUD |= HIDEHUD_WEAPONS | HIDEHUD_FLASHLIGHT | HIDEHUD_HEALTH;
        }
        else
        {
            
            // If we have no other weapons, hide ammo

            if(!HasWeapons())
            {
                theHideHUD |= HIDEHUD_WEAPONS;
                theHideHUD |= HIDEHUD_FLASHLIGHT;
            }
        
            if(theUser3 == AVH_USER3_ALIEN_EMBRYO)
            {
                theHideHUD |= HIDEHUD_WEAPONS;
                theHideHUD |= HIDEHUD_FLASHLIGHT;
            }
        }
    }
    else if(thePlayMode == PLAYMODE_OBSERVER)
    {
        theHideHUD = HIDEHUD_WEAPONS | HIDEHUD_FLASHLIGHT | HIDEHUD_HEALTH;

        // Only hide health when not following a target
        if((this->pev->iuser1 == OBS_IN_EYE))
        {
            //theHideHUD &= ~HIDEHUD_WEAPONS;
            theHideHUD &= ~HIDEHUD_HEALTH;
        }
    }
    else
    {
        int a = 0;
    }
    
    this->m_iHideHUD = theHideHUD;
}


void AvHPlayer::InternalProgressBarThink()
{
    // If some time has passed since the progress bar was triggered, send down a message to kill it
    const float kProgressBarTimeOut = .2f;
    if(this->mTimeProgressBarTriggered != -1)
    {
        if(gpGlobals->time > this->mTimeProgressBarTriggered + kProgressBarTimeOut)
        {
            this->mTimeProgressBarTriggered = -1;
            this->mProgressBarEntityIndex = -1;
            this->mProgressBarParam = -1;
        }
    }
}

void AvHPlayer::InternalSpeakingThink()
{
    if((this->mIsSpeaking || this->mOrderAcknowledged || this->mOrdersRequested) && (gpGlobals->time - this->mTimeOfLastSaying >= kSpeakingTime))
    {
        this->mIsSpeaking = false;
        this->mOrderAcknowledged = false;
        this->mOrdersRequested = false;
    }
}

void AvHPlayer::PreThink( void )
{
    // Get play mode
    AvHPlayMode thePlayMode = this->GetPlayMode();
    bool theRunThink =  ((thePlayMode == PLAYMODE_READYROOM) && GET_RUN_CODE(8)) || 
        ((thePlayMode == PLAYMODE_OBSERVER) && (GET_RUN_CODE(16))) || 
        (this->GetIsAlien() && GET_RUN_CODE(32)) ||
        (this->GetIsMarine() && GET_RUN_CODE(64));
    
    if(theRunThink)
    {
        PROFILE_START()
        CBasePlayer::PreThink();
        PROFILE_END(kCBasePlayerPreThink)

        PROFILE_START()
        this->InternalPreThink();
        PROFILE_END(kPlayerInternalPreThink)
        
        PROFILE_START()
        this->ValidateClientMoveEvents();
        PROFILE_END(kValidateClientMoveEvents)
        
        PROFILE_START()
        this->HandleTopDownInput();
        PROFILE_END(kHandleTopDownInput)
        
        PROFILE_START()
        this->RecalculateSpeed();
        PROFILE_END(kRecalculateSpeed)
        
        PROFILE_START()
        if(this->mQueuedThinkMessage != "")
        {
            this->SendMessage(this->mQueuedThinkMessage.c_str(), TOOLTIP);
            this->mQueuedThinkMessage = "";
        }
        if(this->mPendingCommand)
        {
            // Is this bad?
            GetGameRules()->ClientCommand(this, this->mPendingCommand);
            this->mPendingCommand = NULL;
        }
        PROFILE_END(kPlayerPreThinkMisc)
    }
}

bool AvHPlayer::PayPurchaseCost(int inCost)
{
    bool theSuccess = false;

    if(GetGameRules()->GetIsCombatMode())
    {
        if(inCost <= (this->GetExperienceLevel() - this->GetExperienceLevelsSpent() - 1))
        {
            this->SetExperienceLevelsSpent(this->GetExperienceLevelsSpent() + inCost);
            theSuccess = true;
        }
    }
    else
    {
        if(inCost <= this->GetResources())
        {
            this->SetResources(this->GetResources() - inCost);
            theSuccess = true;
        }
    }

    return theSuccess;
}

void AvHPlayer::RecalculateSpeed(void)
{
    // Look at inventory and set speed from weight
    int theRelevantWeight = this->GetRelevantWeight();
    
    int theMaxWeight = GetGameRules()->GetMaxWeight();

    int theBaseSpeed, theUnencumberedSpeed;
    this->GetSpeeds(theBaseSpeed, theUnencumberedSpeed);
    this->mMaxWalkSpeed = theUnencumberedSpeed*.75f;
    
    // Calculate the max speed
    int theMaxSpeed = theUnencumberedSpeed - (theRelevantWeight/(float)theMaxWeight)*(theUnencumberedSpeed - theBaseSpeed);
    theMaxSpeed = max(theMaxSpeed, theBaseSpeed);
    theMaxSpeed = min(theMaxSpeed, theUnencumberedSpeed);
    
    // Set it but only if it changed (just in case there's a hidden performance or network cost)
    if(this->pev->maxspeed != theMaxSpeed)
    {
        //char theMessage[256];
        //sprintf(theMessage, "New weight is %d, setting speed to %d.\n", theRelevantWeight, theMaxSpeed);
        //ClientPrint(this->pev, HUD_PRINTTALK, theMessage);

        this->pev->maxspeed = theMaxSpeed;
        g_engfuncs.pfnSetClientMaxspeed( ENT(this->pev), theMaxSpeed);
    }
}

void AvHPlayer::ReloadWeapon(void)
{
    if(this->m_pActiveItem)
    {
        CBasePlayerWeapon* theGun = (CBasePlayerWeapon *)this->m_pActiveItem->GetWeaponPtr();
        if(theGun)
        {
			//SetAnimation(PLAYER_RELOAD);
            theGun->Reload();
        }
    }
}

// Reset stuff
void AvHPlayer::ResetEntity(void)
{
    CBasePlayer::ResetEntity();

    this->mHasSeenTeamA = false;
	this->mHasSeenTeamB = false;

    this->ResetBehavior(true);

    this->UpdateTopDownMode();

    // Preserve items we want to survive init
    bool theSavedNewMap = this->mNewMap;
    string theSavedDesiredNetName = this->mDesiredNetName;
    AvHBaseInfoLocationListType theSavedClientInfoLocations = this->mClientInfoLocations;
        
    this->Init();
        
    this->mNewMap = theSavedNewMap;
    this->mDesiredNetName = theSavedDesiredNetName;
    this->mClientInfoLocations = theSavedClientInfoLocations;
}

void AvHPlayer::ResetOverwatch()
{
    // clear target
    this->mOverwatchTarget = -1;
    this->pev->fuser1 = -1;
    this->pev->fuser2 = -1;
    
    // Set facing back to original facing
    VectorCopy(this->mOverwatchFacing, this->pev->angles);
    this->pev->fixangle = TRUE;
}


#include "engine/studio.h"

void AvHPlayer::SetModelFromState()
{
    // Default to marine in ready room
	char* theModelName = NULL;

    AvHUser3 theUser3 = this->GetUser3();

    switch(theUser3)
	{
	case AVH_USER3_MARINE_PLAYER:
	    theModelName = kMarineSoldierModel;
	    if(this->GetHasHeavyArmor())
	    {
	        theModelName = kHeavySoldierModel;
	    }
	    break;
	case AVH_USER3_COMMANDER_PLAYER:
	    theModelName = kMarineCommanderModel;
	    break;
	case AVH_USER3_ALIEN_PLAYER1:
	    theModelName = kAlienLevelOneModel;
	    break;
	case AVH_USER3_ALIEN_PLAYER2:
	    theModelName = kAlienLevelTwoModel;
	    break;
	case AVH_USER3_ALIEN_PLAYER3:
	    theModelName = kAlienLevelThreeModel;
	    break;
	case AVH_USER3_ALIEN_PLAYER4:
	    theModelName = kAlienLevelFourModel;
	    break;
	case AVH_USER3_ALIEN_PLAYER5:
	    theModelName = kAlienLevelFiveModel;
	    break;
	case AVH_USER3_ALIEN_EMBRYO:
		theModelName = kAlienGestateModel;
	    break;
    default:
        if(this->GetPlayMode() == PLAYMODE_READYROOM)
        {
            theModelName = kReadyRoomModel;
        }
        break;
	}

	//SET_MODEL(ENT(pev), theModelName);

    // Alternative method of setting the model on the server:
    if(theModelName)
    {
        pev->model = MAKE_STRING(theModelName);
        this->mLastModelIndex = MODEL_INDEX(theModelName);
    }

    if(this->mLastModelIndex != 1)
    {
        pev->modelindex = mLastModelIndex;
    }

	// Set body group for marine armor
	this->pev->body = 0;
	if(this->GetHasJetpack())
	{
		this->pev->body = 1;
	}
}

void AvHPlayer::SetMoveTypeForUser3()
{
    switch(this->pev->iuser3)
    {
    case AVH_USER3_ALIEN_EMBRYO:
//      this->pev->movetype = MOVETYPE_PUSH;
//      break;
        
    case AVH_USER3_NONE:
    case AVH_USER3_MARINE_PLAYER:
    case AVH_USER3_ALIEN_PLAYER1:
    case AVH_USER3_ALIEN_PLAYER2:
    case AVH_USER3_ALIEN_PLAYER3:
    case AVH_USER3_ALIEN_PLAYER4:
    case AVH_USER3_ALIEN_PLAYER5:
        this->pev->movetype = MOVETYPE_WALK;
        break;
    }
}

void AvHPlayer::GetSize(Vector& outMinSize, Vector& outMaxSize) const
{
    bool theIsDucking = FBitSet(this->pev->flags, FL_DUCKING);
    
    AvHSHUGetSizeForPlayerUser3((AvHUser3)this->pev->iuser3, outMinSize, outMaxSize, theIsDucking);
}

void AvHPlayer::SetWeaponsForUser3()
{
    AvHUser3 theUser3 = (AvHUser3)this->pev->iuser3;

    bool theTeamHasGrenades = false;

    if(!GetGameRules()->GetIsCombatMode())
    {
        AvHTeam* theTeamPointer = this->GetTeamPointer(false);
        if(theTeamPointer)
        {
            theTeamHasGrenades = theTeamPointer->GetResearchManager().GetTechNodes().GetIsTechResearched(TECH_RESEARCH_GRENADES);
        }
    }

    switch(theUser3)
    {
    case AVH_USER3_NONE:
        break;
        
    case AVH_USER3_MARINE_PLAYER:
        if(this->mPreviousUser3 != AVH_USER3_COMMANDER_PLAYER)
        {
            this->GiveNamedItem(kwsMachineGun);
            this->GiveNamedItem(kwsPistol);
            this->GiveNamedItem(kwsKnife);

            if(theTeamHasGrenades)
            {
                this->GiveNamedItem(kwsGrenade);
            }
        }
        break;
    case AVH_USER3_COMMANDER_PLAYER:
        break;
        
        // NOTE: When moving weapons/abilities around, be sure to change AvHBasePlayerWeapon::GetAnimationExtension(), and AvHSHUGetIsWeaponFocusable().
    case AVH_USER3_ALIEN_PLAYER1:
        this->DestroyAllItems(FALSE);
        this->GiveNamedItem(kwsBiteGun);
        this->GiveNamedItem(kwsParasiteGun);
        this->GiveNamedItem(kwsLeap);
        this->GiveNamedItem(kwsDivineWind);
        this->SwitchWeapon(kwsBiteGun);
        break;
        
    case AVH_USER3_ALIEN_PLAYER2:
        this->DestroyAllItems(FALSE);
        this->GiveNamedItem(kwsHealingSpray);
        this->GiveNamedItem(kwsSpitGun);
        this->GiveNamedItem(kwsBileBombGun);
        this->GiveNamedItem(kwsWebSpinner);
        this->SwitchWeapon(kwsSpitGun);
        break;
        
    case AVH_USER3_ALIEN_PLAYER3:
        this->DestroyAllItems(FALSE);
        this->GiveNamedItem(kwsBite2Gun);
        //this->GiveNamedItem(kwsSpikeGun);
        this->GiveNamedItem(kwsSporeGun);
        this->GiveNamedItem(kwsUmbraGun);
        this->GiveNamedItem(kwsPrimalScream);
        this->SwitchWeapon(kwsBite2Gun);
        break;
        
    case AVH_USER3_ALIEN_PLAYER4:
        this->DestroyAllItems(FALSE);
        this->GiveNamedItem(kwsSwipe);
        this->GiveNamedItem(kwsBlinkGun);
        this->GiveNamedItem(kwsAcidRocketGun);
        this->GiveNamedItem(kwsMetabolize);
        this->SwitchWeapon(kwsSwipe);
        break;
        
    case AVH_USER3_ALIEN_PLAYER5:
        this->DestroyAllItems(FALSE);
        this->GiveNamedItem(kwsClaws);
        this->GiveNamedItem(kwsDevour);
        this->GiveNamedItem(kwsStomp);
        this->GiveNamedItem(kwsCharge);
        this->SwitchWeapon(kwsClaws);
        break;
        
    case AVH_USER3_ALIEN_EMBRYO:
        this->DestroyAllItems(FALSE);
        break;
    }
}


void AvHPlayer::SetSizeForUser3()
{
    AvHUser3 theUser3 = (AvHUser3)this->pev->iuser3;
    Vector theMinSize;
    Vector theMaxSize;

    // Use our previous User3 if we're back in the ready room after a game
    if(this->GetPlayMode() == PLAYMODE_READYROOM)
    {
        if(this->mPreviousUser3 != AVH_USER3_NONE)
        {
            theUser3 = this->mPreviousUser3;
        }
    }

    this->GetSize(theMinSize, theMaxSize);

    UTIL_SetSize(this->pev, theMinSize, theMaxSize);
    UTIL_SetOrigin(this->pev, this->pev->origin );
}

void AvHPlayer::GetViewForUser3(AvHUser3 inUser3, bool inIsDucking, float& outFOV, float& outOffset) const
{

    switch(inUser3)
    {
    case AVH_USER3_NONE:
    case AVH_USER3_MARINE_PLAYER:
    case AVH_USER3_COMMANDER_PLAYER:
    case AVH_USER3_ALIEN_PLAYER4:
    default:
        outFOV    = 90;
        outOffset = inIsDucking ? kDuckingViewHeightPercentage*HULL1_MAXZ: kStandingViewHeightPercentage*HULL0_MAXZ;
        break;

    case AVH_USER3_ALIEN_PLAYER1:
        outFOV    = 105;
        outOffset =  0;
        break;
        
    case AVH_USER3_ALIEN_EMBRYO:
    case AVH_USER3_ALIEN_PLAYER2:
        outFOV    = 100;
        outOffset = 10;
        break;
        
    case AVH_USER3_ALIEN_PLAYER3:
        outFOV = 90;
        outOffset = 10;
        break;

    case AVH_USER3_ALIEN_PLAYER5:
        outFOV = 90;
        outOffset = inIsDucking ? kDuckingViewHeightPercentage*HULL0_MAXZ: kStandingViewHeightPercentage*HULL3_MAXZ;
        break;
        
    }

}

void AvHPlayer::SetViewForUser3()
{
    
    AvHUser3 theEndUser3 = this->GetUser3(true);
    bool theIsDucking = FBitSet(this->pev->flags, FL_DUCKING);

    if (theEndUser3 == AVH_USER3_ALIEN_EMBRYO)
    {
    
        bool theEndIsDucking = true;

        switch(GetEvolution(true))
        {
        case ALIEN_LIFEFORM_ONE:
            theEndUser3 = AVH_USER3_ALIEN_PLAYER1;
            break;
        case ALIEN_LIFEFORM_TWO:
            theEndUser3 = AVH_USER3_ALIEN_PLAYER2;
            break;
        case ALIEN_LIFEFORM_THREE:
            theEndUser3 = AVH_USER3_ALIEN_PLAYER3;
            break;
        case ALIEN_LIFEFORM_FOUR:
            theEndUser3 = AVH_USER3_ALIEN_PLAYER4;
            break;
        case ALIEN_LIFEFORM_FIVE:
            theEndUser3 = AVH_USER3_ALIEN_PLAYER5;
            break;
        default:
            // For upgrades.
            theEndUser3 = GetPreviousUser3(true);
            break;
        }
        
        // Linearly interpolate between the previous lifeform and the new lifeform.

        float theStartFOV;
        float theStartOffset;
    
        float theEndFOV;
        float theEndOffset;

        float amount = pev->fuser3 / kNormalizationNetworkFactor;
        
        AvHUser3 theStartUser3 = GetPreviousUser3(true);

        GetViewForUser3(theStartUser3, theIsDucking, theStartFOV, theStartOffset);
        GetViewForUser3(theEndUser3, true, theEndFOV, theEndOffset);

        // Take into account that the origin will change for the offset.

        Vector theStartMinSize;
        Vector theStartMaxSize;

        AvHSHUGetSizeForPlayerUser3(this->GetUser3(true), theStartMinSize, theStartMaxSize, theIsDucking);

        Vector theEndMinSize;
        Vector theEndMaxSize;

        AvHSHUGetSizeForPlayerUser3(theEndUser3, theEndMinSize, theEndMaxSize, true);
        theEndOffset += theStartMinSize.z - theEndMinSize.z;

        pev->fov         = theStartFOV + amount * (theEndFOV - theStartFOV);
        pev->view_ofs[2] = theStartOffset + amount * (theEndOffset - theStartOffset);

    }
    else
    {
        GetViewForUser3(theEndUser3, theIsDucking, pev->fov, pev->view_ofs[2]);
    }



}

bool AvHPlayer::SendMessage(const char *pMessage, SHOWMESSAGE_TYPE type)
{
    bool theSuccess = false;

    int theNumChars = strlen(pMessage);
    if((theNumChars > 0) && (theNumChars < kMaxPlayerSendMessageLength))
    {
        string theMessage(pMessage);
        if(theMessage != this->mLastMessageSent)
        {
            UTIL_ShowMessage2(pMessage, this, type);
            
            this->mLastMessageSent = theMessage;
            
            theSuccess = true;
        }
        else
        {
            int a = 0;
        }
    }
//  else
//  {
//      // Log error to console
//      char theErrorMessage[10000];
//      sprintf(theErrorMessage, "Can't send message \"%s\" of length %d, max size is %d", pMessage, theNumChars, kMaxPlayerSendMessageLength);
//      ALERT(at_logged, theErrorMessage);
//  }
    
    return theSuccess;
}

bool AvHPlayer::SendMessageOnce(const char *pMessage, SHOWMESSAGE_TYPE type)
{
    bool theSentMessage = false;

    string theMessage(pMessage);

    // Check if message is in sent list
    StringList::iterator theIter = std::find(this->mSentMessageList.begin(), this->mSentMessageList.end(), theMessage);
    if(theIter == this->mSentMessageList.end())
    {
        // If not
        // Call SendMessage
        theSentMessage = this->SendMessage(pMessage, type);

        this->mLastMessageSent = theMessage;

        // Add message to list
        this->mSentMessageList.push_back(theMessage);
    }

    return theSentMessage;
}

bool AvHPlayer::SendMessageNextThink(const char* pMessage)
{
    this->mQueuedThinkMessage = string(pMessage);
    return true;
}

void AvHPlayer::SetCurrentCommand(const struct usercmd_s* inCommand)
{
    memcpy(&this->mCurrentCommand, inCommand, sizeof(*inCommand));
}

void AvHPlayer::SetDebugCSP(weapon_data_t* inWeaponData)
{
    CBasePlayerWeapon* theCurrentWeapon = dynamic_cast<CBasePlayerWeapon*>(this->m_pActiveItem);
    if(theCurrentWeapon && (theCurrentWeapon->m_iId == inWeaponData->m_iId))
    {
        memcpy(&this->mDebugCSPInfo, inWeaponData, sizeof(weapon_data_t));
    }
}

void AvHPlayer::StartObserver( Vector vecPosition, Vector vecViewAngle )
{
    Vector theFadeColor;
    theFadeColor.x = 0;
    theFadeColor.y = 0;
    theFadeColor.z = 0;

    UTIL_ScreenFade(this, theFadeColor, kTransitionFadeTime, 0.0f, 255, FFADE_IN);

    CBasePlayer::StartObserver(vecPosition, vecViewAngle);
}

void AvHPlayer::ResetBehavior(bool inRemoveFromTeam)
{
    // remove observer mode if enabled
    this->StopObserver();
    
    // Leave top down mode if in it
    this->StopTopDownMode();
    
    // Stop digesting if you are
    this->StopDigestion(false);

    // Stop being digested
    this->SetBeingDigestedMode(false);

    // Reset room sounds
    this->SetDesiredRoomType(0, true);
    
    // remove all equipment, but don't drop it (how to do this?)
    this->DestroyAllItems(FALSE);

    if(inRemoveFromTeam)
    {
        AvHTeam* theTeam = this->GetTeamPointer();
        if(theTeam)
        {
            theTeam->RemovePlayer(this->entindex());
        }

        // Clear experience
        this->mExperience = 0.0f;
        this->mExperienceLevelsSpent = 0;
        this->mCombatNodes.Clear();
        this->mPurchasedCombatUpgrades.clear();
        this->mGiveCombatUpgrades.clear();
    }
}

void AvHPlayer::SetPlayMode(AvHPlayMode inPlayMode, bool inForceSpawn)
{
    if(this->pev->playerclass != inPlayMode || inForceSpawn)
    {
        bool theGoingToReadyRoom = (inPlayMode == PLAYMODE_READYROOM);
        this->ResetBehavior(theGoingToReadyRoom);
        
        if(!theGoingToReadyRoom)
        {
            // Clear player
            //this->Init();
            this->ClearUserVariables();
            this->pev->rendermode = kRenderNormal;
            this->pev->renderfx = kRenderFxNone;
            this->pev->renderamt = 0;
        }

        // Clear anim
        this->m_szAnimExtention[0] = '\0';

//      this->mUpgrades.clear();
        
        AvHTeamNumber theTeamNumber = AvHTeamNumber(this->pev->team);
        AvHUser3 theUser3 = AVH_USER3_NONE;
        bool theSetUser3 = false;
        
        string theMessage;

        AvHTeam* theTeam = this->GetTeamPointer(false);
        string theTeamName = kUndefinedTeam;

        switch(inPlayMode)
        {
            // Initialize stuff
        case PLAYMODE_UNDEFINED:
            this->pev->iuser3 = AVH_USER3_NONE;
            this->pev->playerclass = PLAYMODE_UNDEFINED;
            this->pev->team = TEAM_IND;
            respawn(this->pev, FALSE);
            break;
            
        case PLAYMODE_READYROOM:
            this->pev->playerclass = PLAYMODE_READYROOM;

            if(theTeam)
            {
                theTeam->RemovePlayer(this->entindex());
            }

            this->pev->frags = 0;
            this->mScore = 0;
            this->mSavedCombatFrags = 0;
            this->m_iDeaths = 0;
            this->pev->team = TEAM_IND;

            // So we don't have the sideways camera because we're dead
            this->pev->health = 100;
            this->pev->max_health = pev->health;
            this->pev->armorvalue = 0;
            
            respawn(this->pev, FALSE);

            if(this->pev->iuser3 == AVH_USER3_ALIEN_EMBRYO)
            {
                // Stop sound and allow movement, else we're stuck in the ready room
                UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kGestationSound, 1.0f, .5, SND_STOP, 100);
                SetUpgradeMask(&this->pev->iuser4, MASK_ALIEN_EMBRYO, false);
            }
            // Else "commander" draws on the scoreboard and it doesn't make sense to have a commander on the ground
            else if(this->pev->iuser3 == AVH_USER3_COMMANDER_PLAYER)
            {
                this->pev->iuser3 = AVH_USER3_MARINE_PLAYER;
            }
            //else if(this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER5)
            //{
            //  // Set player ducking to improve chances of them not getting stuck
            //  SetBits( m_afPhysicsFlags, PFLAG_DUCKING );
            //  SetBits( this->pev->flags, FL_DUCKING );
            //  this->pev->origin.z += 25;
            //}

            theMessage = kReadyRoomMessage;
            theTeamName = kUndefinedTeam;
            break;

        case PLAYMODE_PLAYING:
            // Don't set playmode if we couldn't respawn
            this->pev->playerclass = PLAYMODE_PLAYING;

            //respawn(this->pev, FALSE);
            
            // Account for both sides, or to let player choose it somehow
            if(this->GetClassType() == AVH_CLASS_TYPE_MARINE)
            {
                theUser3 = AVH_USER3_MARINE_PLAYER;
            }
            else
            {
                theUser3 = AVH_USER3_ALIEN_PLAYER1; // TEMPORARY

                // In combat mode, spawn player in as most advanced lifeform so player doesn't get stuck.
                if(GetGameRules()->GetIsCombatMode() && !GetGameRules()->GetIsHamboneMode())
                {
                    // Find the most advanced lifeform
                    for(MessageIDListType::const_iterator theIter = this->mGiveCombatUpgrades.begin(); theIter != this->mGiveCombatUpgrades.end(); theIter++)
                    {
                        AvHMessageID theCurrentCombatUpgrade = *theIter;
                        switch(theCurrentCombatUpgrade)
                        {
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
                    }
                }
            }

            this->SetUser3(theUser3, true, false);

            respawn(this->pev, FALSE);

            this->SetWeaponsForUser3();

            // In combat mode, add all player upgrades
            if(GetGameRules()->GetIsCombatMode())
            {
                if(GetGameRules()->GetIsHamboneMode())
                {
                    // Reset tech nodes
                    this->mGiveCombatUpgrades.clear();
                    this->mCombatNodes = this->GetTeamPointer()->GetTechNodes();
                    this->mExperienceLevelsSpent = 0;
                }
                else if(!GetGameRules()->GetIsIronMan())
                {
                    // Respend upgrades
                    this->GiveCombatUpgradesOnSpawn();
                }
            }

            theTeam = this->GetTeamPointer();
            theTeamName = (theTeam ? theTeam->GetTeamName() : kUndefinedTeam);

            this->mLastTimeStartedPlaying = gpGlobals->time;
            this->mHasLeftReadyRoom = true;
            break;

        case PLAYMODE_AWAITINGREINFORCEMENT:
            //this->mRole = AVH_USER3_NONE;
            // Preserve team, we could be brought back in
            this->pev->team = theTeamNumber;
            this->pev->playerclass = PLAYMODE_AWAITINGREINFORCEMENT;

            theTeam = this->GetTeamPointer();
            theTeamName = (theTeam ? theTeam->GetTeamName() : kUndefinedTeam);

            this->StartObservingIfNotAlready();
            
            theTeam = this->GetTeamPointer();
            theTeamName = (theTeam ? theTeam->GetTeamName() : kUndefinedTeam);
            theMessage = kReinforcementMessage;
            this->mHasLeftReadyRoom = true;
            break;

        case PLAYMODE_REINFORCING:
            this->pev->team = theTeamNumber;
            this->pev->playerclass = PLAYMODE_REINFORCING;

            theTeam = this->GetTeamPointer();
            theTeamName = (theTeam ? theTeam->GetTeamName() : kUndefinedTeam);
            
            this->StartObservingIfNotAlready();

            this->SendMessage(kReinforcingMessage, TOOLTIP);
            this->mHasLeftReadyRoom = true;
            break;

        case PLAYMODE_OBSERVER:

			if(theTeam)
            {
                theTeam->RemovePlayer(this->entindex());
            }

            // Set observer mode
            this->StartObservingIfNotAlready();

            // Set current team to indeterminate; we aren't allowed to join from spectating
            this->pev->team = TEAM_IND;
            this->pev->playerclass = PLAYMODE_OBSERVER;

            // Remember that we have spectated
            this->mHasBeenSpectator = true;
            theMessage = kObserverMessage;
            theTeamName = kSpectatorTeam;
            this->mHasLeftReadyRoom = true;

            this->SetHasSeenTeam(GetGameRules()->GetTeamA()->GetTeamNumber());
            this->SetHasSeenTeam(GetGameRules()->GetTeamB()->GetTeamNumber());
            break;

		case PLAYMODE_REINFORCINGCOMPLETE:
			this->pev->playerclass = PLAYMODE_REINFORCINGCOMPLETE;
			this->SendMessage(kReinforcementComplete, NORMAL);
			break;
        }

        // Force reset of entities because we just respawned
        //this->ResetPlayerPVS();

        // Inform gamerules of the change
        GetGameRules()->ChangePlayerTeam(this, theTeamName.c_str(), false, false); 
        if(theTeam)
        {
            this->SetHasSeenTeam(theTeam->GetTeamNumber());
        }

        // Inform scoreboard
        this->EffectivePlayerClassChanged();
        
        if(theMessage != "")
        {
            // Send instructions to player
            this->SendMessageNextThink(theMessage.c_str());
        }
    }
}

void AvHPlayer::GetNewOrigin(AvHUser3 inNewUser3, bool inCheckDucking, vec3_t& outOrigin) const
{

    vec3_t theOldMinSize;
    vec3_t theOldMaxSize;

    GetSize(theOldMinSize, theOldMaxSize);
      
    vec3_t theNewMinSize;
    vec3_t theNewMaxSize;
    
    AvHSHUGetSizeForPlayerUser3(inNewUser3, theNewMinSize, theNewMaxSize, inCheckDucking);
    
    VectorCopy(pev->origin, outOrigin);
    outOrigin[2] += theOldMinSize.z - theNewMinSize.z;

}

void AvHPlayer::SetUser3(AvHUser3 inUser3, bool inForceChange, bool inGiveWeapons) 
{
    if((inUser3 != this->pev->iuser3) || inForceChange)
    {

        // Make us duck so that it's easier to gestate in small areas.

        if(AvHMUGetCanDuck(this->pev->iuser3))
        {
            
            SetBits(this->pev->flags, FL_DUCKING);
            SetBits( m_afPhysicsFlags, PFLAG_DUCKING );

            // Important or the animations will get screwed up!

            m_Activity = ACT_RESET;
            SetAnimation( PLAYER_IDLE );

        }

        bool theIsDucking = FBitSet(this->pev->flags, FL_DUCKING);

        // Save off the current size of the player so that we can adjust the
        // origin later.

        Vector theOldMinSize;
        Vector theOldMaxSize;
        
        this->GetSize(theOldMinSize, theOldMaxSize);

        vec3_t theNewOrigin;
        GetNewOrigin(inUser3, true, theNewOrigin);

        this->mPreviousUser3 = (AvHUser3)this->pev->iuser3;

        // Leave old User3
        switch(this->mPreviousUser3)
        {
        case AVH_USER3_MARINE_PLAYER:
            break;
        case AVH_USER3_ALIEN_PLAYER5:
            this->StopDigestion(false);
            break;
        case AVH_USER3_COMMANDER_PLAYER:
            this->StopTopDownMode();
            break;
        }

        string theMessage;
        
        this->pev->iuser3 = inUser3;

        // Drop inventory, clear abilities
        //this->DestroyAllItems(FALSE);

        bool theSavedAlienSightActive = this->mAlienSightActive;

        this->ClearRoleAbilities();
            
        int theSavedUser4 = this->pev->iuser4;
        bool theSavedIsSpectator = this->GetIsSpectator();

        float theJetpackEnergy = mSavedJetpackEnergy;
        mSavedJetpackEnergy = this->pev->fuser3;

        this->ClearUserVariables();

        switch(inUser3)
        {
        case AVH_USER3_NONE:
            this->pev->team = TEAM_IND;
            break;
            
        case AVH_USER3_MARINE_PLAYER:
            this->pev->iuser3 = inUser3;
            theMessage = kSoldierMessage;
            this->pev->movetype = MOVETYPE_WALK;
            break;
        case AVH_USER3_COMMANDER_PLAYER:
            this->pev->iuser3 = inUser3;
            this->StartTopDownMode();
            theMessage = kCommanderMessage;
            break;

        // NOTE: When moving weapons/abilities around, be sure to change AvHBasePlayerWeapon::GetAnimationExtension() also
        case AVH_USER3_ALIEN_PLAYER1:
            this->pev->iuser3 = inUser3;
            this->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;
            break;
            
        case AVH_USER3_ALIEN_PLAYER2:
            this->pev->iuser3 = inUser3;
            this->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;
            break;
        
        case AVH_USER3_ALIEN_PLAYER3:
            this->pev->iuser3 = inUser3;
            this->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;
            break;
        
        case AVH_USER3_ALIEN_PLAYER4:
            this->pev->iuser3 = inUser3;
            this->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;
            break;
            
        case AVH_USER3_ALIEN_PLAYER5:
            this->pev->iuser3 = inUser3;
            this->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;
            this->mLastGallopViewDirection = gpGlobals->v_forward;
            break;
        
        case AVH_USER3_ALIEN_EMBRYO:
            this->pev->iuser3 = inUser3;
            this->pev->iuser4 |= MASK_ALIEN_EMBRYO;
            theMessage = kGestationMessage;
            break;
        }
        
        // Preserve upgrades on a role change
        this->pev->iuser4 |= theSavedUser4;

        // All players are selectable
        this->pev->iuser4 |= MASK_SELECTABLE;

        // Get team-wide upgrades
        AvHTeam* theTeamPointer = this->GetTeamPointer();
        if(theTeamPointer)
        {
            this->pev->iuser4 |= theTeamPointer->GetTeamWideUpgrades();
        }

        if(inGiveWeapons)
        {
            this->SetWeaponsForUser3();
        }
        
        // Adjust the size for the new user3.

        this->SetSizeForUser3();

        // Adjust the origin of the player so that they are still on the ground.

        //Vector theNewMinSize;
        //Vector theNewMaxSize;
        //this->GetSize(theNewMinSize, theNewMaxSize);
        //this->pev->origin[2] += theOldMinSize.z - theNewMinSize.z;

        if(this->mPreviousUser3 != AVH_USER3_COMMANDER_PLAYER)
        {
            UTIL_SetOrigin(this->pev, theNewOrigin);
        }

        this->SetViewForUser3();
        this->SetMoveTypeForUser3();

        if(theSavedIsSpectator)
        {
            this->SetIsSpectator();
        }

        if(this->pev->playerclass == PLAYMODE_AWAITINGREINFORCEMENT)
        {
            this->pev->playerclass = PLAYMODE_PLAYING;
        }
        
        //this->SetModelFromState();
        float theHealthPercentage = 1.0f;
        float theArmorPercentage = 1.0f;
        if((this->mPreviousUser3 != AVH_USER3_NONE) /*&& (inUser3 != AVH_USER3_ALIEN_EMBRYO)*/)
        {
            theHealthPercentage = (float)this->pev->health/AvHPlayerUpgrade::GetMaxHealth(theSavedUser4, this->mPreviousUser3, this->GetExperienceLevel());
            theArmorPercentage = (float)this->pev->armorvalue/AvHPlayerUpgrade::GetMaxArmorLevel(theSavedUser4, this->mPreviousUser3);
        }

        if (mPreviousUser3 == AVH_USER3_COMMANDER_PLAYER)
        {
            // Restore the jetpack energy from when the player went into the CC.
            this->pev->fuser3 = theJetpackEnergy;
        }

        //char theInitializeMessage[128];
        //sprintf(theInitializeMessage, "Initializing to user3: %d with health/armor percentages: %f/%f\n", inUser3, theHealthPercentage, theArmorPercentage);
        //ClientPrint(this->pev, HUD_PRINTTALK, theInitializeMessage);

        this->InitializeFromTeam(theHealthPercentage, theArmorPercentage);

        this->SetModelFromState();

        if(this->GetIsAlien())
        {
            this->mAlienSightActive = theSavedAlienSightActive;
        }
        else
        {
            this->mAlienSightActive = false;
        }

        // Update team
        AvHTeam* theTeam = this->GetTeamPointer();
        if(theTeam)
        {
            theTeam->ProcessRankChange(this, this->mPreviousUser3, this->GetUser3());
        }

        // Update scoreboard
        this->EffectivePlayerClassChanged();
        
        if(theMessage != "")
        {
            // Send instructions to player
            this->SendMessageOnce(theMessage.c_str(), TOOLTIP);
        }
		this->LogEmitRoleChange(); 
    }
}

void AvHPlayer::SetResources(float inResources, bool inPlaySound)
{
    if(!GetGameRules()->GetIsCombatMode())
    {
        if(inResources < 0)
        {
            inResources = 0;
        }

        AvHTeam* theTeam = this->GetTeamPointer();
        ASSERT(theTeam != NULL);

        if(this->GetIsMarine())
        {
            if(inPlaySound)
            {
                this->PlayHUDSound(HUD_SOUND_MARINE_POINTS_RECEIVED);

                AvHPlayer* theCommander = this->GetCommander();
                if(theCommander)
                {
                    theCommander->PlayHUDSound(HUD_SOUND_MARINE_POINTS_RECEIVED);
                }
            }

            theTeam->SetTeamResources(inResources);
        }
        else if(this->GetIsAlien())
        {
            if(inPlaySound)
            {
                this->PlayHUDSound(HUD_SOUND_ALIEN_POINTS_RECEIVED);
            }

            this->mResources = inResources;
        }

        this->InternalBoundResources();

        if(this->GetIsAlien())
        {
            AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData();
            if(theServerPlayerData)
            {
                theServerPlayerData->SetResources(this->mResources);
            }
        }
    }
}

void AvHPlayer::Spawn( void )
{
    CBasePlayer::Spawn();
    //this->PrecacheAndSetPlayerModel();
    
    pev->classname = MAKE_STRING(kAvHPlayerClassName);

    this->mSendSpawnScreenFade = true;

    if(this->pev->playerclass != PLAYMODE_READYROOM)
    {
        this->PlayRandomRoleSound(kPlayerLevelSpawnSoundList, CHAN_ITEM, this->GetAlienAdjustedEventVolume());
    }

    //SET_MODEL(ENT(pev), "models/headcrab.mdl");
    //UTIL_SetSize(pev, Vector(-12, -12, 0), Vector(12, 12, 24));

    SetTouch(&AvHPlayer::PlayerTouch);

    // Stop spectating
    this->pev->iuser1 = 0;
}

void AvHPlayer::StartObservingIfNotAlready(void)
{
    // Prevent this is the cvar is set
    if ( allow_spectators.value )
    {
        //CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);
        if(!this->IsObserver())
            //if(!pPlayer->IsObserver())
        {
            //if(this->mPlayMode == PLAYMODE_AWAITINGREINFORCEMENT)
            //{
            // TODO: Start observer mode in chase cam on friendlies
            //}
            //else
            //{
            edict_t *pentSpawnSpot = GetGameRules()->SelectSpawnPoint( this /*pPlayer*/ );
            if(!FNullEnt(pentSpawnSpot))
            {
                this->StartObserver( VARS(pentSpawnSpot)->origin, VARS(pentSpawnSpot)->angles);
            }
            else
            {
                this->StartObserver( this->pev->origin, this->pev->angles);
            }
        }
    }
    else
    {
        this->ObserverModeIllegal();
    }
}

bool AvHPlayer::SetBeingDigestedMode(bool inBeingDigested)
{
    bool theSuccess = false;
    bool theIsDigesting = this->GetIsBeingDigested();

    SetUpgradeMask(&this->pev->iuser4, MASK_DIGESTING, false);

    if(inBeingDigested && !theIsDigesting)
    {
        // Fade player to black
//      Vector theFadeColor;
//      theFadeColor.x = 0;
//      theFadeColor.y = 0;
//      theFadeColor.z = 0;
//      UTIL_ScreenFade(this, theFadeColor, .7f, 0.0f, 255, FFADE_OUT | FFADE_STAYOUT);
        
        this->HolsterCurrent();

        this->pev->solid = SOLID_NOT;
        this->pev->effects |= EF_NODRAW;
        this->pev->takedamage = DAMAGE_NO;
        
        this->pev->movetype = MOVETYPE_FLY;
        this->m_afPhysicsFlags |= PFLAG_OBSERVER;
        
        ClearBits( m_afPhysicsFlags, PFLAG_DUCKING );
        ClearBits( this->pev->flags, FL_DUCKING );
        VectorCopy(g_vecZero, this->pev->velocity);

        theSuccess = true;
    }
    else if(!inBeingDigested && theIsDigesting)
    {
        // Fade player up from black
        Vector theFadeColor;
        theFadeColor.x = 0;
        theFadeColor.y = 0;
        theFadeColor.z = 0;
        UTIL_ScreenFade(this, theFadeColor, 1.0f, 0.0f, 255, FFADE_IN);
        this->SetDesiredRoomType(0);
        
        this->DeployCurrent();

        // Set physics
        this->pev->solid = SOLID_SLIDEBOX;
        this->pev->effects &= ~EF_NODRAW;
        this->pev->takedamage = DAMAGE_YES;
        
        this->pev->movetype = MOVETYPE_WALK;
        ClearBits(this->m_afPhysicsFlags, PFLAG_OBSERVER);

        // Set player ducking to improve chances of them not getting stuck
        SetBits( m_afPhysicsFlags, PFLAG_DUCKING );
        SetBits( this->pev->flags, FL_DUCKING );
        
        VectorCopy(g_vecZero, this->pev->velocity);
        this->pev->fixangle = TRUE;

        theSuccess = true;
    }

    if(theSuccess)
    {
        // Set digesting flag
        SetUpgradeMask(&this->pev->iuser4, MASK_DIGESTING, inBeingDigested);
        this->EffectivePlayerClassChanged();
    }

    return theSuccess;
}

void AvHPlayer::StartTopDownMode()
{
    if(!this->mInTopDownMode && !this->GetCurrentWeaponCannotHolster())
    {
        Vector theFadeColor;
        theFadeColor.x = 0;
        theFadeColor.y = 0;
        theFadeColor.z = 0;
        UTIL_ScreenFade(this, theFadeColor, kTransitionFadeTime, 0.0f, 255, FFADE_IN);
        
        VectorCopy(this->pev->origin, this->mPositionBeforeTopDown);
        VectorCopy(this->pev->angles, this->mAnglesBeforeTopDown);
        VectorCopy(this->pev->v_angle, this->mViewAnglesBeforeTopDown);
        VectorCopy(this->pev->view_ofs, this->mViewOfsBeforeTopDown);
        this->mAnimExtensionBeforeTopDown = this->m_szAnimExtention;

        this->HolsterCurrent();

        this->mTimeStartedTopDown = gpGlobals->time;

        this->mOverwatchEnabled = false;
        SetUpgradeMask(&this->pev->iuser4, MASK_TOPDOWN);
        this->m_afPhysicsFlags |= PFLAG_OBSERVER;
        this->pev->effects |= EF_NODRAW;
        this->pev->view_ofs = g_vecZero;
        this->pev->gravity = 0;
        
        this->pev->solid = SOLID_NOT;
        this->pev->takedamage = DAMAGE_NO;
        
        //this->pev->movetype = MOVETYPE_NOCLIP;
        //this->pev->movetype = MOVETYPE_WALK;
        this->pev->movetype = MOVETYPE_FLY;
        this->m_afPhysicsFlags |= PFLAG_OBSERVER;

        ClearBits( m_afPhysicsFlags, PFLAG_DUCKING );
        ClearBits( this->pev->flags, FL_DUCKING );
        //this->pev->deadflag = DEAD_RESPAWNABLE;
        //this->pev->deadflag = DEAD_RESPAWNABLE;
        //this->pev->velocity[0] = 0.0f;
        //this->pev->velocity[1] = 0.0f;
        //this->pev->velocity[2] = -1.0f;


//      float theMinViewHeight, theMaxViewHeight;
//      float theMinX, theMaxX;
//      float theMinY, theMaxY;
//      bool theDrawMapBG;
//      GetGameRules()->GetMapExtents(theMinViewHeight, theMaxViewHeight, theMinX, theMinY, theMaxX, theMaxY, theDrawMapBG);

        this->pev->origin.z = GetGameRules()->GetMapExtents().GetMaxViewHeight();

        this->mInTopDownMode = true;

        // Cheesy way to make sure player class change is sent to everyone
        this->EffectivePlayerClassChanged();
    }
}

bool AvHPlayer::GetHasLeftReadyRoom() const
{
    return this->mHasLeftReadyRoom;
}

bool AvHPlayer::GetHasJetpack() const
{
    return this->GetIsMarine() && GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_7);
}

bool AvHPlayer::GetHasHeavyArmor() const
{
    return this->GetIsMarine() && GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_13);
}

bool AvHPlayer::GetHasGivenOrder() const
{
    return this->mHasGivenOrder;
}

void AvHPlayer::SetHasGivenOrder(bool inState)
{
    this->mHasGivenOrder = inState;
}

float AvHPlayer::GetTimeLastF4() const
{
    return this->mTimeOfLastF4;
}

void AvHPlayer::SetTimeLastF4(float inTime)
{
    this->mTimeOfLastF4=inTime;
}

float AvHPlayer::GetTimeStartedTopDown() const
{
    return this->mTimeStartedTopDown;
}

float AvHPlayer::GetTimeOfLastSignificantCommanderAction() const
{
    return this->mTimeOfLastSignificantCommanderAction;
}

bool AvHPlayer::GetHasAvailableUpgrades() const
{
    bool theHasPendingUpgrades = false;

    AvHTeam* theTeamPointer = this->GetTeamPointer();
    if(theTeamPointer)
    {
        AvHAlienUpgradeListType theUpgrades = theTeamPointer->GetAlienUpgrades();
            
        for(int i = ALIEN_UPGRADE_CATEGORY_INVALID + 1; i < ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE; i++)
        {
            AvHAlienUpgradeCategory theCurrentCategory = AvHAlienUpgradeCategory(i);
        
            // Now make sure we have an unspent upgrade available
            if(AvHGetHasFreeUpgradeCategory(theCurrentCategory, theUpgrades, this->pev->iuser4))
            {
                theHasPendingUpgrades = true;
                break;
            }
        }
    }
    
    return theHasPendingUpgrades;
}


bool AvHPlayer::GetHasPowerArmor() const
{
    return this->GetIsMarine() && GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_10);
}

int AvHPlayer::GetHull() const
{
    int theHull = AvHMUGetHull(true, this->pev->iuser3);

    return theHull;
}

bool AvHPlayer::GetIsTemporarilyInvulnerable() const
{
    bool theIsInvulnerable = false;

    if(GetGameRules()->GetIsCombatMode() && (this->GetPlayMode() == PLAYMODE_PLAYING))
    {
        if(this->mLastTimeStartedPlaying != -1)
        {
            float theInvulnerableTimeSeconds = 0.0f;

            #ifdef DEBUG
            theInvulnerableTimeSeconds = avh_spawninvulnerabletime.value;
            #endif

            if(gpGlobals->time < (this->mLastTimeStartedPlaying + theInvulnerableTimeSeconds))
            {
                theIsInvulnerable = true;
            }
        }
    }
    
    if (mTimeOfLastRedeem != -1 && gpGlobals->time < mTimeOfLastRedeem + kRedeemInvulnerableTime)
    {
        theIsInvulnerable = true;
    }

    return theIsInvulnerable;
}

bool AvHPlayer::GetIsEnsnared() const
{
    return GetHasUpgrade(this->pev->iuser4, MASK_ENSNARED);
}

bool AvHPlayer::GetIsAbleToAct() const
{
    return !GetIsInTopDownMode() && !GetIsBeingDigested() && !GetIsEnsnared() && !GetIsStunned();
}

bool AvHPlayer::SetEnsnareState(bool inState)
{
    bool theSuccess = true;

    if(inState)
    {
        // If too ensnared already, don't ensnare further
        if(!this->GetIsEnsnared())
        {
            this->mTimeToBeUnensnared = gpGlobals->time;
        }

        if(!this->GetIsEnsnared() || ((this->mTimeToBeUnensnared + BALANCE_VAR(kEnsnareTime) - gpGlobals->time) < BALANCE_VAR(kMaxEnsnareTime)))
        {
            this->mLastTimeEnsnared = gpGlobals->time;
            this->mTimeToBeUnensnared += BALANCE_VAR(kEnsnareTime);
        
            // Player is defenseless
            this->HolsterCurrent();

            SetUpgradeMask(&this->pev->iuser4, MASK_ENSNARED);
        }
        else
        {
            theSuccess = false;
        }
    }
    else
    {
        SetUpgradeMask(&this->pev->iuser4, MASK_ENSNARED, false);
        this->mTimeToBeUnensnared = -1;
        this->mLastTimeEnsnared = -1;

        this->DeployCurrent();
    }

    return theSuccess;
}

bool AvHPlayer::GetIsStunned() const
{
    return GetHasUpgrade(this->pev->iuser4, MASK_PLAYER_STUNNED);
}

bool AvHPlayer::SetIsStunned(bool inState, float inTime)
{
    bool theSuccess = false;

    // Only able to stun walking players (prevents weird problems with players on ladders, who are treated as flying 
    if(inState && !this->GetIsStunned() && (this->pev->movetype == MOVETYPE_WALK))
    {
        SetUpgradeMask(&this->pev->iuser4, MASK_PLAYER_STUNNED);
        this->mTimeToBeFreeToMove = gpGlobals->time + inTime;

        Vector theFadeColor;
        theFadeColor.x = 255;
        theFadeColor.y = 255;
        theFadeColor.z = 255;
        float theFadeTime = .25f;
        UTIL_ScreenFade(this, theFadeColor, theFadeTime, 0.0f, 128, FFADE_IN/* | FFADE_MODULATE*/);

        theSuccess = true;

        // Clear keys so they aren't held down
        //this->ClearKeys();
    }
    else if(!inState && this->GetIsStunned())
    {
        SetUpgradeMask(&this->pev->iuser4, MASK_PLAYER_STUNNED, false);
        this->mTimeToBeFreeToMove = -1;
    }

    return theSuccess;
}

bool AvHPlayer::GetIsCatalysted() const
{
    return this->GetIsMarine() && GetHasUpgrade(this->pev->iuser4, MASK_BUFFED);
}

void AvHPlayer::SetIsCatalysted(bool inState, float inTime)
{
    if(this->GetIsMarine())
    {
        if(inState && !this->GetIsCatalysted())
        {
            SetUpgradeMask(&this->pev->iuser4, MASK_BUFFED);
            this->mTimeToEndCatalyst = gpGlobals->time + inTime;

            // Trigger screen effect?
        }
        else
        {
            SetUpgradeMask(&this->pev->iuser4, MASK_BUFFED, false);
            this->mTimeToEndCatalyst = -1;
        }
    }
}

bool AvHPlayer::Energize(float inEnergyAmount)
{
    bool theSuccess = false;

    if(AvHMUGiveAlienEnergy(this->pev->fuser3, inEnergyAmount))
    {
        theSuccess = true;
    }

    return theSuccess;
}

bool AvHPlayer::Heal(float inAmount, bool inPlaySound)
{
    int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
    int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, (AvHUser3)this->pev->iuser3);
    bool theDidHeal = false;
	float theAmount = inAmount;
    
    // If we aren't at full health, heal health
	if(this->pev->health < theMaxHealth)
	{
		int theAmountToGive = theAmount;
		theAmount -= (theMaxHealth - this->pev->health); //store relative amount compared to that necessary for complete heal
		this->pev->health = min((float)theMaxHealth, this->pev->health + theAmountToGive);
		theDidHeal = true;
	}
	else if(this->pev->armorvalue < theMaxArmor)
    {
        this->pev->armorvalue = min((float)theMaxArmor, this->pev->armorvalue + theAmount);
        theDidHeal = true;
    }
    
    // Play regen event
    if(theDidHeal)
    {
        if(inPlaySound)
        {
            // Play regeneration event
            PLAYBACK_EVENT_FULL(0, this->edict(), gRegenerationEventID, 0, this->pev->origin, (float *)&g_vecZero, this->GetAlienAdjustedEventVolume(), 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
        }
    }
    
    return theDidHeal;
}

bool AvHPlayer::Regenerate(float inRegenerationAmount, bool inPlaySound)
{
    bool theDidRegenerate = this->Heal(inRegenerationAmount, inPlaySound);

    if(theDidRegenerate)
    {
        this->mTimeOfLastRegeneration = gpGlobals->time;
    }

    return theDidRegenerate;
}

bool AvHPlayer::GetCanBeResupplied() const
{
    bool theCanBeResupplied = false;

    const float theResupplyTime = BALANCE_VAR(kResupplyTime);

    if((this->mTimeOfLastResupply == 0) || (gpGlobals->time > (this->mTimeOfLastResupply + theResupplyTime)))
    {
        if(this->m_pActiveItem)
        {
            AvHBasePlayerWeapon* theBaseWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem->GetWeaponPtr());
            if(theBaseWeapon && theBaseWeapon->CanHolster() && theBaseWeapon->GetCanBeResupplied())
            {
                theCanBeResupplied = true;
            }
        }

        // If we don't have max health, or we need ammo
        if(this->pev->health < this->pev->max_health)
        {
            theCanBeResupplied = true;
        }
    }
    return theCanBeResupplied;
}

bool AvHPlayer::Resupply(bool inGiveHealth)
{
    bool theSuccess = false;

    if(this->m_pActiveItem)
    {
        AvHBasePlayerWeapon* theBaseWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem->GetWeaponPtr());
        if(theBaseWeapon && theBaseWeapon->Resupply())
        {
            theSuccess = true;
        }

        if(inGiveHealth)
        {
			// puzl: 1017 armoury gives 10 health per use
            if(AvHHealth::GiveHealth(this, BALANCE_VAR(kPointsPerArmouryHealth)))
            {
                // Play event for each person helped
                //PLAYBACK_EVENT_FULL(0, this->edict(), gPhaseInEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
                theSuccess = true;
            }
        }
        
        this->mTimeOfLastResupply = gpGlobals->time;
    }
    
    return theSuccess;
}

bool AvHPlayer::GetIsScreaming()
{
    return this->mIsScreaming;
}

void AvHPlayer::StartScreaming()
{
    this->mIsScreaming = true;
    this->mTimeStartedScream = gpGlobals->time;
}

bool AvHPlayer::StopTopDownMode()
{
    bool theSuccess = false;

    if(this->mInTopDownMode)
    {
        Vector theFadeColor;
        theFadeColor.x = 0;
        theFadeColor.y = 0;
        theFadeColor.z = 0;
        UTIL_ScreenFade(this, theFadeColor, kTransitionFadeTime, 0.0f, 255, FFADE_IN);
        
        this->DeployCurrent();
        
        this->mOverwatchEnabled = true;
        this->pev->effects &= ~EF_NODRAW;
        this->pev->view_ofs = g_vecZero;
        SetUpgradeMask(&this->pev->iuser4, MASK_TOPDOWN, false);
        
        // TODO: Make sure original gravity is 1?
        this->pev->gravity = 1;
        
        this->pev->solid = SOLID_SLIDEBOX;
        this->pev->takedamage = DAMAGE_YES;
        
        //this->pev->movetype = MOVETYPE_NONE;
        this->pev->movetype = MOVETYPE_WALK;
        ClearBits(this->m_afPhysicsFlags, PFLAG_OBSERVER);
        this->pev->deadflag = DEAD_NO;
        
        VectorCopy(this->mPositionBeforeTopDown, this->pev->origin);
        VectorCopy(this->mAnglesBeforeTopDown, this->pev->angles);
        VectorCopy(this->mViewAnglesBeforeTopDown, this->pev->v_angle);
        VectorCopy(this->mViewOfsBeforeTopDown, this->pev->view_ofs);
        strcpy(this->m_szAnimExtention, this->mAnimExtensionBeforeTopDown.c_str());

        VectorCopy(g_vecZero, this->pev->velocity);
        this->pev->fixangle = TRUE;
        this->mInTopDownMode = false;
        
        AvHTeamNumber theStationTeamNumber = (AvHTeamNumber)this->pev->team;
        const char* theTarget = (theStationTeamNumber == TEAM_ONE) ? kTargetCommandStationLogoutTeamOne : kTargetCommandStationLogoutTeamTwo;
        FireTargets(theTarget, NULL, NULL, USE_TOGGLE, 0.0f);

        this->mSelected.clear();

        // Need to reset groups when logging out, so they are re-propagated when logging in after another potential commander (see AvHHud::ResetTopDownUI)
        for(int i = 0; i < kNumHotkeyGroups; i++)
        {
            this->mClientGroupAlerts[i] = ALERT_NONE;

            this->mClientGroups[i].clear();
        }
        
        this->mClientSelectAllGroup.clear();

        theSuccess = true;
    }

    return theSuccess;
}

void AvHPlayer::SetPendingCommand(char* inCommand)
{
    this->mPendingCommand = inCommand;
}

void AvHPlayer::TriggerFog(int inFogEntity, float inFogExpireTime)
{
    this->mCurrentFogEntity = inFogEntity;
    this->mFogExpireTime = inFogExpireTime;

    // Allows resetting of fog entity
    if(inFogEntity > -1)
    {
        this->mTimeOfLastFogTrigger = gpGlobals->time;
    }
}

void AvHPlayer::TriggerProgressBar(int inEntityID, int inParam)
{
    ASSERT(inEntityID >= 0);

    this->mProgressBarEntityIndex = inEntityID;
    this->mProgressBarParam = inParam;
    this->mTimeProgressBarTriggered = gpGlobals->time;
}

float AvHPlayer::GetTimeOfLastTeleport() const
{
    return this->mTimeOfLastTeleport;
}

float AvHPlayer::GetTimeLastPlaying() const
{
    return this->mTimeLastPlaying;
}


bool AvHPlayer::HolsterWeaponToUse()
{
    bool theSuccess = false;

    if(!this->GetCurrentWeaponCannotHolster())
    {
        this->HolsterCurrent();
        
        this->mTimeOfLastUse = gpGlobals->time;

        theSuccess = true;
    }
    return theSuccess;
}

void AvHPlayer::SetTimeOfLastTeleport(float inTime)
{
    this->mTimeOfLastTeleport = inTime;
}

void AvHPlayer::BecomePod()
{
    //ASSERT(this->mRole != AVH_USER3_ALIEN_EMBRYO);
    
    this->HolsterCurrent();
    
    ClearBits(this->m_afPhysicsFlags, PFLAG_DUCKING);
    ClearBits(this->pev->flags, FL_DUCKING);
    
    //EMIT_SOUND_DYN(ENT(this->pev), CHAN_VOICE, kGestationSound, 1, ATTN_NORM, 0, 100);

	float flSilenceLevel = this->GetAlienAdjustedEventVolume();

	if(flSilenceLevel > 0.0)
		UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kGestationSound, flSilenceLevel, 2.0, 0, 100);
}

bool AvHPlayer::SwitchWeapon(const char* inString)
{
    bool theSuccess = false;

    if(!this->GetIsEnsnared())
    {
        CBasePlayerWeapon* theCurrentWeapon;
        
        for (int i = 0 ; i < MAX_ITEM_TYPES ; i++ )
        {
            theCurrentWeapon = dynamic_cast<CBasePlayerWeapon*>(this->m_rgpPlayerItems[i]);
            while( theCurrentWeapon )
            {
                if(FClassnameIs(theCurrentWeapon->pev, inString))
                {
                    // this weapon is from the same category. 
                    if ( theCurrentWeapon->CanDeploy() )
                    {
                        theSuccess = CBasePlayer::SwitchWeapon( theCurrentWeapon );
                    }
                    break;
                }
                theCurrentWeapon = dynamic_cast<CBasePlayerWeapon*>(theCurrentWeapon->m_pNext);
            }
        }
    }

    return theSuccess;
}

//BOOL AvHPlayer::SwitchWeapon( CBasePlayerItem* inWeapon )
//{
//  CBasePlayerWeapon* theCurrentWeapon = dynamic_cast<CBasePlayerWeapon*>(this->m_pActiveItem);
//  CBasePlayerItem* theWeapon = inWeapon;
//  BOOL theSuccess = TRUE;
//
//  if(!inWeapon)
//  {
//      if(theCurrentWeapon)
//      {
//          theCurrentWeapon->RetireWeapon();
//      }
//      else
//      {
//          theSuccess = FALSE;
//      }
//  }
//  else
//  {
//      CBasePlayer::SwitchWeapon(theWeapon);
//  }
//
//  return theSuccess;
//}


void AvHPlayer:: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
    if ( pev->takedamage && GetCanBeAffectedByEnemies())
    {
        m_LastHitGroup = ptr->iHitgroup;

        // No locational damage in NS.

        /*  
        switch ( ptr->iHitgroup )
        {
        case HITGROUP_GENERIC:
            break;
        case HITGROUP_HEAD:
            flDamage *= gSkillData.plrHead;
            break;
        case HITGROUP_CHEST:
            flDamage *= gSkillData.plrChest;
            break;
        case HITGROUP_STOMACH:
            flDamage *= gSkillData.plrStomach;
            break;
        case HITGROUP_LEFTARM:
        case HITGROUP_RIGHTARM:
            flDamage *= gSkillData.plrArm;
            break;
        case HITGROUP_LEFTLEG:
        case HITGROUP_RIGHTLEG:
            flDamage *= gSkillData.plrLeg;
            break;
        default:
            break;
        }
        */

        // Player's aren't affected by structural damage, so don't create blood
        // if that's the damage type.

        if (!(bitsDamageType & NS_DMG_STRUCTURAL))
        {
            SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);
            TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
        }

        AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
    
    }
}


int AvHPlayer::TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	//Even out the damage
	//flDamage = ceil(flDamage);

    int theReturnValue = 0;
    
    if(GetGameRules()->GetGameStarted() && !this->GetIsTemporarilyInvulnerable())
    {
        // Take into account handicap
        if(!pevAttacker)
        {
            pevAttacker = pevInflictor;
        }

        if(!pevInflictor)
        {
            pevInflictor = pevAttacker;
        }

        AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(pevAttacker->team));
        if(theTeam)
        {
            float theHandicap = theTeam->GetHandicap();
            flDamage *= theHandicap;
        }
        
        if(GetGameRules()->GetIsCheatEnabled(kcHighDamage))
        {
            flDamage *= 10;
        }
        
        if(bitsDamageType & NS_DMG_STRUCTURAL)
        {
            flDamage = 0.0f;
        }

        // Do half damage to the heavy armor of HA and Onos
        if(bitsDamageType & NS_DMG_LIGHT)
        {
            if(this->GetHasHeavyArmor() || (this->pev->iuser3 == AVH_USER3_ALIEN_PLAYER5))
            {
                flDamage *= .5f;
            }
        }

        // If we're metabolizing, convert the damage to energy
//      if(this->GetIsMetabolizing())
//      {
//          const float theFactor = BALANCE_VAR(kMetabolizeDamageEnergyFactor);
//          float theEnergy = (flDamage/100.f)*theFactor;
//          AvHMUGiveAlienEnergy(this->pev->fuser3, theEnergy);
//
//          if((this->mTimeOfLastMetabolizeEvent == -1) || (gpGlobals->time > (this->mTimeOfLastMetabolizeEvent + 1.0f)))
//          {
//              // Playback metabolize success event
//              PLAYBACK_EVENT_FULL(0, this->edict(), gMetabolizeSuccessEventID, 0, this->pev->origin, (float *)&g_vecZero, this->GetAlienAdjustedEventVolume(), 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
//
//              this->mTimeOfLastMetabolizeEvent = gpGlobals->time;;
//          }
//
//          theReturnValue = 0;
//      }
//      else
//      {
            
            theReturnValue = CBasePlayer::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
            if(theReturnValue > 0)
            {
                float theSlowDownFactor = .8f;
                float theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
                
                if(flDamage > theMaxHealth/2.0f)
                {
                    this->PlayRandomRoleSound(kPlayerLevelWoundSoundList, CHAN_BODY, 1.0);
                    theSlowDownFactor = .3f;
                }
                else if(flDamage >= theMaxHealth/5.0f)
                {
                    this->PlayRandomRoleSound(kPlayerLevelPainSoundList, CHAN_BODY, .8f);
                    theSlowDownFactor = .5f;
                }
                
                // Slow down when hit
                //VectorScale(this->pev->velocity, theSlowDownFactor, this->pev->velocity);
            
                if(!pevAttacker || (this->pev->team != pevAttacker->team) && (pevAttacker->team != 0))
                {
                    GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_PLAYER_ENGAGE, this->entindex());
                }
            
                if(pevAttacker)
                {
                    CBasePlayer* inAttackingPlayer = dynamic_cast<CBasePlayer*>(CBaseEntity::Instance(ENT(pevAttacker)));
                    const char* inWeaponName = STRING(pevInflictor->classname);
                    if(inAttackingPlayer && inWeaponName)
                    {
                        this->LogPlayerAttackedPlayer(inAttackingPlayer, inWeaponName, flDamage);
                    }
                }
            
                bool theDrawDamage = (CVAR_GET_FLOAT(kvDrawDamage) > 0);
                
                if(theDrawDamage)
                {
                    this->PlaybackNumericalEvent(kNumericalInfoHealthEvent, (int)(-flDamage));
                }
            
                this->Uncloak();
            }
//      }
    }

    return theReturnValue;
}

void AvHPlayer::PlaybackNumericalEvent(int inEventID, int inNumber)
{
    Vector theMinSize;
    Vector theMaxSize;
    this->GetSize(theMinSize, theMaxSize);
    
    Vector theStartPos = this->pev->origin;
    theStartPos.z += theMaxSize.z;
    
    // Draw for everyone (team = 0 after flDamage parameter)
    AvHSUPlayNumericEvent(inNumber, this->edict(), theStartPos, 0, inEventID, this->pev->team);
}


//const char*   AvHPlayer::TeamID( void )
//{
//  AvHTeam* theTeam = this->GetTeamPointer();
//  const char* theTeamName = (theTeam ? theTeam->GetTeamName() : kUndefinedTeam);
//  //const char* theTeamName = this->GetPlayerModelKeyName();
//  return theTeamName;
//}

void AvHPlayer::TurnOffOverwatch()
{
    this->mInOverwatch = false;
    AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
    if(theWeapon)
    {
        theWeapon->SetOverwatchState(false);
    }

    //VectorCopy(this->mOverwatchFacing, this->pev->angles);
    //this->pev->fixangle = TRUE;

    //ASSERT(this->pev->iuser4 == AVH_USER4_OVERWATCH);
    //this->pev->iuser4 &= ~MASK_MARINE_OVERWATCH;
    this->pev->fuser1 = -1;
    this->pev->fuser2 = -1;
}

void AvHPlayer::TurnOnOverwatch()
{
    this->mInOverwatch = true;
    
    // Remember facing when we entered overwatch
    VectorCopy(this->pev->angles, this->mOverwatchFacing);
    
    // if so, set overwatch on, make sure to set the current weapon into overwatch
    AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pActiveItem);
    ASSERT(theWeapon);
    theWeapon->SetOverwatchState(true);
    
    // Flip on overwatch, clear target, it will be acquired in think
    this->mOverwatchTarget = -1;
    this->pev->fuser1 = -1;
    this->pev->fuser2 = -1;
    //this->pev->iuser4 |= MASK_MARINE_OVERWATCH;
}

void AvHPlayer::TurnOverwatchTowardsTarget(CBaseEntity* theTarget)
{
    // TODO: Take gun offset into account with vecMid?
    Vector vecMid = pev->origin + pev->view_ofs;
    Vector vecMidEnemy = theTarget->BodyTarget( vecMid );
    
    // Right now just point at enemy
    Vector vecDirToEnemy = vecMidEnemy - vecMid;
    Vector vec = UTIL_VecToAngles(vecDirToEnemy);
    
    vec.x = -vec.x;
    
//  if (vec.y > 360)
//      vec.y -= 360;
//  
//  if (vec.y < 0)
//      vec.y += 360;
    
    VectorCopy(vec, this->pev->angles);
    VectorCopy(vec, this->pev->v_angle);
    this->pev->fixangle = TRUE;
}

bool AvHPlayer::RunClientScript(const string& inScriptName)
{
    // Returns false if client scripts aren't enabled
    bool theSuccess = false;

    this->mPendingClientScripts.push_back(inScriptName);
    theSuccess = true;

    return theSuccess;
}

void AvHPlayer::PrintWeaponListToClient(CBaseEntity *theAvHPlayer) {
		char msg[1024];
		sprintf(msg, "Weapons for %s:\n", this->GetPlayerName());	
		ClientPrint(theAvHPlayer->pev, HUD_PRINTNOTIFY, msg);

        for(int i = 0; i < MAX_ITEM_TYPES; i++)
        {
            AvHBasePlayerWeapon* theActiveWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_rgpPlayerItems[i]);
            while(theActiveWeapon)
            {
                theActiveWeapon->PrintWeaponToClient(theAvHPlayer);

                // Next weapon
                theActiveWeapon = dynamic_cast<AvHBasePlayerWeapon*>(theActiveWeapon->m_pNext);
            }
        }
}
void AvHPlayer::UpdateInventoryEnabledState(int inNumActiveHives, bool inForceUpdate)
{
    // Have we not yet updated our weapons with this # of hives?
    if((inNumActiveHives != this->mNumHives) || (inForceUpdate))
    {
        for(int i = 0; i < MAX_ITEM_TYPES; i++)
        {
            AvHBasePlayerWeapon* theActiveWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_rgpPlayerItems[i]);
            while(theActiveWeapon)
            {
                theActiveWeapon->UpdateInventoryEnabledState(inNumActiveHives);

                // Next weapon
                theActiveWeapon = dynamic_cast<AvHBasePlayerWeapon*>(theActiveWeapon->m_pNext);
            }
        }
    }

    // Save # of hives we've last updated with
    this->mNumHives = inNumActiveHives;
}

bool AvHPlayer::GetIsBeingDigested() const
{
    bool theIsBeingDigested = false;

    if(GetHasUpgrade(this->pev->iuser4, MASK_DIGESTING))
    {
        if(this->pev->effects & EF_NODRAW)
        {
            theIsBeingDigested = true;
        }
    }

    return theIsBeingDigested;
}

bool AvHPlayer::GetIsDigesting() const
{
    bool theIsDigesting = false;
    
    if(GetHasUpgrade(this->pev->iuser4, MASK_DIGESTING))
    {
        if(! (this->pev->effects & EF_NODRAW))
        {
            theIsDigesting = true;
        }
    }
    
    return theIsDigesting;
}

void AvHPlayer::UpdateAmbientSounds()
{
    AvHClassType theClassType = this->GetClassType();
    if(theClassType == AVH_CLASS_TYPE_MARINE)
    {
    }
    else if(theClassType == AVH_CLASS_TYPE_ALIEN)
    {
        // Get role
        AvHUser3 theUser3 = this->GetUser3();
        int theAlienLevel = theUser3 - AVH_USER3_COMMANDER_PLAYER;
        int theVelocity = this->pev->velocity.Length();
        bool theIsMoving = theVelocity > 100;
        int theSilenceLevel = AvHGetAlienUpgradeLevel(this->pev->iuser4, MASK_UPGRADE_6);

//      if(RANDOM_LONG(0, 40) == 0)
//      {
//          char theVelocityMsg[128];
//          sprintf(theVelocityMsg, "alien moving at velocity: %d", theVelocity);
//          ClientPrint(this->pev, HUD_PRINTTALK, theVelocityMsg);
//      }

        // if moving, check chance for playing moving sound
        if(!this->GetIsCloaked() && !this->mIsScreaming /*&& !this->GetIsBlinking()*/)
        {
            float theSilenceVolumeFactor = this->GetAlienAdjustedEventVolume();

            if(theIsMoving)
            {
                int theBaseSpeed, theMaxSpeed;
                this->GetSpeeds(theBaseSpeed, theMaxSpeed);
                
                float theAlienSoundFreq = 0.003f;
                float theChanceOfPlayingSound = theAlienSoundFreq*(theVelocity/((float)theMaxSpeed));
                if(RANDOM_FLOAT(0, 1) < theChanceOfPlayingSound)
                {
                    float theVolume = RANDOM_FLOAT(.5, 1.0)*theSilenceVolumeFactor;
                    if(theVolume > 0.01f)
                    {
                        this->PlayRandomRoleSound(kPlayerLevelMoveSoundList, CHAN_VOICE, theVolume);
                    }
                }
            }
            else
            {
                if(GetHasUpgrade(this->pev->iuser4, MASK_BUFFED))
                {
					float theSilenceVolumeFactor = this->GetAlienAdjustedEventVolume();

                    // If player is part of primal scream, scream defiance!
                    if(RANDOM_FLOAT(0, 1) < .02f && theSilenceVolumeFactor > 0.0)
                    {
                        EMIT_SOUND(this->edict(), CHAN_VOICE, kPrimalScreamResponseSound, theSilenceVolumeFactor, ATTN_NORM);
                    }   
                }
                else
                {
                    bool theIsGestating = (this->GetUser3() == AVH_USER3_ALIEN_EMBRYO);
                        
                    // if idle, check chance for playing idle sound
                    float theBaseChance = 0.0005f;
                    if(theIsGestating)
                    {
                        theBaseChance *= 10;
                    }
                    
                    if(RANDOM_FLOAT(0, 1) < theBaseChance)
                    {
                        float theVolume = RANDOM_FLOAT(.2, .4)*theSilenceVolumeFactor;
                        if(theVolume > 0.01f)
                        {
                            if(theIsGestating)
                            {
                                EMIT_SOUND(this->edict(), CHAN_AUTO, kEggIdleSound, theVolume, ATTN_NORM);
                            }
                            else
                            {
                                this->PlayRandomRoleSound(kPlayerLevelIdleSoundList, CHAN_VOICE, theVolume);
                            }
                        }
                    }
                }
            }
        }
    }

    if(this->mDesiredRoomType != this->mClientDesiredRoomType)
    {
        MESSAGE_BEGIN( MSG_ONE, SVC_ROOMTYPE, NULL, this->pev);
            WRITE_SHORT( (short)this->mDesiredRoomType);
        MESSAGE_END();

        this->mClientDesiredRoomType = this->mDesiredRoomType;
    }
}                                                                                            

void AvHPlayer::UpdateAlienUI()
{
    AvHTeam* theTeamPointer = this->GetTeamPointer();
    bool theIsMarine = false;
    bool theIsAlien = false;

    if(this->GetIsAlien())
    {
        theIsAlien = true;
    }
    else if(this->GetIsMarine())
    {
        theIsMarine = true;
    }

    // Update when going back to ready room, so check if not-marine instead of is-alien
    if(!theIsMarine)
    {
        bool theCanRespawn = GetGameRules()->FPlayerCanRespawn(this);

        AvHTeam* theTeamPointer = this->GetTeamPointer();
        if(theTeamPointer)
        {
            AvHAlienUpgradeListType theUpgrades = theTeamPointer->GetAlienUpgrades();

            int thePreSize = theUpgrades.size();
            
            // Trim max upgrades in each category
            AvHRemoveIrrelevantUpgrades(theUpgrades);
            
            if(theUpgrades != this->mClientUpgrades)
            {
				NetMsg_AlienInfo_Upgrades( this->pev, theUpgrades );
                this->mClientUpgrades = theUpgrades;
            }

            HiveInfoListType theTeamHiveInfo = theTeamPointer->GetHiveInfoList();
            if(this->mClientHiveInfo != theTeamHiveInfo)
            {
				NetMsg_AlienInfo_Hives( this->pev, theTeamHiveInfo, this->mClientHiveInfo );
                this->mClientHiveInfo = theTeamHiveInfo;
            }
        }
    }
}

// TODO: Send only changed blips, send only the changes for each blip.
void AvHPlayer::UpdateBlips()
{
    if(this->mEnemyBlips != this->mClientEnemyBlips)
    {
		NetMsg_BlipList( this->pev, false, this->mEnemyBlips );
        this->mClientEnemyBlips = this->mEnemyBlips;
    }
    
    if(this->mFriendlyBlips != this->mClientFriendlyBlips)
    {
		NetMsg_BlipList( this->pev, true, this->mFriendlyBlips );
        this->mClientFriendlyBlips = this->mFriendlyBlips;
    }
}

void AvHPlayer::UpdateClientData( void )
{
    if(GET_RUN_CODE(128))
    {
        //UTIL_LogPrintf("UpdateClientData starting.\n");
        CBasePlayer::UpdateClientData();

        // Update one-shot stuff
        this->UpdateFirst();
        this->UpdateParticleTemplates();
        this->UpdateInfoLocations();
        this->UpdateOrders();
        this->UpdateVUser4();
        this->UpdateProgressBar();
        this->UpdateSetSelect();
        this->UpdateTechNodes();
        this->UpdateBalanceVariables();
        this->UpdateSoundNames();
        this->UpdateTopDownMode();
        this->UpdateEntityHierarchy();
        this->UpdateExperienceLevelsSpent();
        this->UpdateSpawnScreenFade();
        this->UpdateEffectiveClassAndTeam();
        //this->UpdateArmor();
        //this->UpdateOverwatch();
        this->UpdatePendingClientScripts();
        this->UpdateGamma();
        this->UpdateBlips();
        this->UpdateAlienUI();
        this->UpdateFog();
        //this->UpdateDebugCSP();
    }
    //UTIL_LogPrintf("UpdateClientData done.\n");
}

void AvHPlayer::UpdateEffectiveClassAndTeam()
{
    // Don't send too many messages when these get updated really quickly.  Too many messages are being sent on a game reset, and it's not needed.  We only need the most recent message.
    const float kClassAndTeamUpdateRate = .4f;
    if((this->mTimeOfLastClassAndTeamUpdate == -1) || (gpGlobals->time > (this->mTimeOfLastClassAndTeamUpdate + kClassAndTeamUpdateRate)))
    {
        if(this->mEffectivePlayerClassChanged)
        {
            int theAuthMask = this->GetAuthenticationMask();
            int theTotalScore = this->mScore + this->pev->frags /*- this->m_iDeaths*/;
            if(GetGameRules()->GetIsCombatMode())
            {
                int theCurrentLevel = AvHPlayerUpgrade::GetPlayerLevel(this->GetExperience());
                theTotalScore += max((theCurrentLevel - 1), 0);
            }

			ScoreInfo info;
			info.player_index = ENTINDEX(this->edict());
			info.score = theTotalScore;
			info.frags = this->pev->frags;
			info.deaths = this->m_iDeaths;
			info.player_class = this->GetEffectivePlayerClass();
			info.team = GetGameRules()->GetTeamIndex(this->TeamID());
			NetMsg_ScoreInfo( info );
            this->mEffectivePlayerClassChanged = false;
        }
        
        if(this->mNeedsTeamUpdate)
        {
            for (int i = 1; i <= gpGlobals->maxClients; i++ )
            {
                CBasePlayer *plr = (CBasePlayer*)UTIL_PlayerByIndex( i );
                if ( plr && GetGameRules()->IsValidTeam( plr->TeamID() ) )
                {
					NetMsg_TeamInfo( this->pev, plr->entindex(), plr->TeamID() );
                }
            }
            this->mNeedsTeamUpdate = false;
        }
        
        if(this->mSendTeamUpdate)
        {
            // notify everyone's HUD of the team change
			NetMsg_TeamInfo( this->entindex(), this->TeamID() );
            this->mSendTeamUpdate = false;
        }

        this->mTimeOfLastClassAndTeamUpdate = gpGlobals->time;
    }
}

void AvHPlayer::UpdateFirst()
{
    if(this->mFirstUpdate)
    {
        // Tell this player to reset
		int theState = (this->mNewMap ? kGameStatusResetNewMap : kGameStatusReset);
		NetMsg_GameStatus_State( this->pev, theState, GetGameRules()->GetMapMode() );
        
        if(this->mNewMap)
        {
			NetMsg_SetSoundNames( this->pev, true, string() );
            this->mClientSoundNames.clear();
            
            // Send down map extents so players can start computing it
            // Cache this so it isn't computed every round, only the when a player connects or a new map starts?
			const char* theCStrLevelName = STRING(gpGlobals->mapname);
			const AvHMapExtents& theMapExtents = GetGameRules()->GetMapExtents();
			ASSERT(theCStrLevelName);
			ASSERT(!FStrEq(theCStrLevelName, ""));

			float mins[3] = { theMapExtents.GetMinMapX(), theMapExtents.GetMinMapY(), theMapExtents.GetMinViewHeight() };
			float maxs[3] = { theMapExtents.GetMaxMapX(), theMapExtents.GetMaxMapY(), theMapExtents.GetMaxViewHeight() };

			NetMsg_SetupMap_Extents( this->pev, string( theCStrLevelName ), mins, maxs, theMapExtents.GetDrawMapBG() );
        }
        
        this->mFirstUpdate = false;
        this->mNewMap = false;
    }
}

void AvHPlayer::UpdateFog()
{
    if(this->mClientCurrentFogEntity != this->mCurrentFogEntity)
    {
        bool theFogEnabled = this->mCurrentFogEntity > -1;
        int theR, theG, theB;
		float theStart, theEnd;

        if(theFogEnabled)
        {
            AvHFog* theFogEntity = dynamic_cast<AvHFog*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mCurrentFogEntity)));
            ASSERT(theFogEntity);

			theFogEntity->GetFogColor(theR, theG, theB);
			theStart = theFogEntity->GetFogStart();
			theEnd = theFogEntity->GetFogEnd();
        }

		NetMsg_Fog( this->pev, theFogEnabled, theR, theG, theB, theStart, theEnd );
		this->mClientCurrentFogEntity = this->mCurrentFogEntity;
    }
}

void AvHPlayer::UpdateGamma()
{
    float theMapGamma = GetGameRules()->GetMapGamma();
    if(this->mClientGamma != theMapGamma)
    {
        if(!GetGameRules()->GetIsTesting())
        {
			NetMsg_SetGammaRamp( this->pev, theMapGamma );
            this->mClientGamma = theMapGamma;
        }
    }
}

void AvHPlayer::UpdateOrders()
{
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam)
    {
        OrderListType theTeamOrders;
        theTeam->GetOrders(theTeamOrders);
        
        for(OrderListType::iterator theIter = theTeamOrders.begin(); theIter != theTeamOrders.end(); theIter++)
        {
            bool theClientHasOrder = false;
            AvHOrder theClientOrder;
            
            // Does client already have this order?
            for(OrderListType::iterator theClientIter = this->mClientOrders.begin(); theClientIter != this->mClientOrders.end(); theClientIter++)
            {
                if(theIter->GetOrderID() == theClientIter->GetOrderID())
                {
                    theClientHasOrder = true;
                    theClientOrder = *theClientIter;
                    break;
                }
            }
            
            if(!theClientHasOrder || theClientOrder != *theIter)
			{
				NetMsg_SetOrder( this->pev, *theIter );
			}
        }
        
        this->mClientOrders = theTeamOrders;
    }
}

void AvHPlayer::UpdateParticleTemplates()
{
    const float kParticleTemplateRate = 1.0f;
    if(gParticleTemplateList.GetCreatedTemplates())
    {
        // Make sure client clears out all particle systems first
        int theNumberTemplates = gParticleTemplateList.GetNumberTemplates();
        if(theNumberTemplates > this->mNumParticleTemplatesSent)
        {
            if((this->mTimeOfLastParticleTemplateSending == -1) || (gpGlobals->time > this->mTimeOfLastParticleTemplateSending + kParticleTemplateRate))
            {
				AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mNumParticleTemplatesSent);
				ASSERT(theTemplate);
				NetMsg_SetParticleTemplate( this->pev, *theTemplate );
                this->mNumParticleTemplatesSent++;
                this->mTimeOfLastParticleTemplateSending = gpGlobals->time;
            }
        }
    }
}

void AvHPlayer::UpdateInfoLocations()
{
    // Get map location list
    const AvHBaseInfoLocationListType& theInfoLocations = GetGameRules()->GetInfoLocations();

    // Compare with ours, send one down each tick (assumes that previous ones sent don't change)
    int theNumClientInfoLocations = this->mClientInfoLocations.size();
    if((signed)theInfoLocations.size() > theNumClientInfoLocations)
    {
        // Only send one at a time
        AvHBaseInfoLocation theInfoLocation = theInfoLocations[theNumClientInfoLocations];

        vec3_t theMaxExtents = theInfoLocation.GetMaxExtent();
        vec3_t theMinExtents = theInfoLocation.GetMinExtent();
		float mins[3] = { theMinExtents.x, theMinExtents.y, theMinExtents.z };
		float maxs[3] = { theMaxExtents.x, theMaxExtents.y, theMinExtents.z };

		NetMsg_SetupMap_Location( this->pev, theInfoLocation.GetLocationName(), mins, maxs );
        this->mClientInfoLocations.push_back(theInfoLocation);
    }

}

void AvHPlayer::UpdatePendingClientScripts()
{
    if(this->mPendingClientScripts.size() > 0)
    {
		NetMsg_ClientScripts( this->pev, this->mPendingClientScripts );
        this->mPendingClientScripts.clear();
    }
}

void AvHPlayer::UpdateProgressBar()
{
    // TODO: If this is the commander, send him all the progress bars of all his teammates so he can see them!
    
    // Assumes that progress is normalized and stored in one of the fuser variables of the entity index sent down
    if(this->mClientProgressBarEntityIndex != this->mProgressBarEntityIndex)
    {
		NetMsg_ProgressBar( this->pev, this->mProgressBarEntityIndex, this->mProgressBarParam );
        this->mClientProgressBarEntityIndex = this->mProgressBarEntityIndex;
    }
}

void AvHPlayer::UpdateVUser4()
{
    // Update client with resources (as int)
    int theResources = (short)(this->GetResources(true));
    
    if(CVAR_GET_FLOAT(kvTesting))
    {
        theResources = g_engfuncs.pfnNumberOfEntities();
    }

    if(this->pev)
    {
        if(GetGameRules()->GetIsCombatMode())
        {
            this->pev->vuser4.z = this->GetExperience()*kNumericNetworkConstant;
        }
        else
        {
            this->pev->vuser4.z = theResources*kNumericNetworkConstant;
        }
    }
}

void AvHPlayer::UpdateSetSelect()
{
    if(this->GetIsInTopDownMode())
    {
        if((this->mSelected != this->mClientSelected) || (this->mTrackingEntity != this->mClientTrackingEntity))
        {
			Selection selection;
			selection.group_number = 0;
			selection.selected_entities = this->mSelected;
			selection.tracking_entity = max( this->mTrackingEntity, 0 );

			NetMsg_SetSelect( this->pev, selection );

            // Synch up
            this->mClientSelected = this->mSelected;
            this->mClientTrackingEntity = this->mTrackingEntity;
        }

        AvHTeam* theTeam = this->GetTeamPointer();
        ASSERT(theTeam);

        for(int j=0; j < kNumHotkeyGroups; j++)
        {
            EntityListType theGroup = theTeam->GetGroup(j);
            EntityListType& theClientGroup = this->mClientGroups[j];
            AvHUser3 theGroupType = theTeam->GetGroupType(j);
            AvHAlertType theGroupAlert = ALERT_NONE;
            AvHAlertType& theClientGroupAlert = this->mClientGroupAlerts[j];

            // Is group under attack or no longer under attack?
            for(EntityListType::iterator theIter = theGroup.begin(); theIter != theGroup.end(); theIter++)
            {
                if(GetGameRules()->GetIsEntityUnderAttack(*theIter))
                {
                    theGroupAlert = ALERT_UNDER_ATTACK;
                }
            }
        
            if((theClientGroup != theGroup) || (theClientGroupAlert != theGroupAlert))
            {
				Selection selection;
				selection.group_number = j+1;
				selection.selected_entities = theGroup;
				selection.group_type = theGroupType;
				selection.group_alert = theGroupAlert;

				NetMsg_SetSelect( this->pev, selection );
        
                theClientGroup = theGroup;
                theClientGroupAlert = theGroupAlert;
            }
        }

        // See if "selectall" hotgroup has changed and send it if needed
        EntityListType theSelectAllGroup = theTeam->GetSelectAllGroup();
        if(theSelectAllGroup != this->mClientSelectAllGroup)
        {
			Selection selection;
			selection.group_number = kSelectAllHotGroup;
			selection.selected_entities = theSelectAllGroup;

			NetMsg_SetSelect( this->pev, selection );

            this->mClientSelectAllGroup = theSelectAllGroup;
        }
        
        // Check idle soldiers, ammo requests and health requests
        AvHMessageID theRequestList[kNumRequestTypes] = {COMMANDER_NEXTIDLE, COMMANDER_NEXTAMMO, COMMANDER_NEXTHEALTH};
        for(int i = 0; i < kNumRequestTypes; i++)
        {
            AvHMessageID theCurrentRequestType = theRequestList[i];
            int theNumClientRequests = this->mClientRequests[i];
            int theNumTeamRequests = theTeam->GetAlerts(theCurrentRequestType).size();
            if(theNumClientRequests != theNumTeamRequests)
            {
				NetMsg_SetRequest( this->pev, theCurrentRequestType, theNumTeamRequests );
                this->mClientRequests[i] = theNumTeamRequests;
            }
        }
    }
}

void AvHPlayer::UpdateSoundNames()
{
    if(this->pev != NULL )      // Not fully connected yet
    {
        // Send list of special sounds
        const StringList& theSoundNameList = AvHMP3Audio::GetSoundNameList();
        int theNumberOfSounds = theSoundNameList.size();
        int theNumberOfSoundsOnClient = this->mClientSoundNames.size();
        
        ASSERT(theNumberOfSoundsOnClient <= theNumberOfSounds);
        
        // Only send one new sound name every tick, to avoid sending too much data and overflowing too quickly
        if(theNumberOfSounds > theNumberOfSoundsOnClient)
        {
            const char* theSoundNameToSend = theSoundNameList[theNumberOfSoundsOnClient].c_str();
            if(GetGameRules()->GetIsTesting())
            {
                this->SendMessage(theSoundNameToSend);
            }
            ASSERT( strlen(theSoundNameToSend) < 50);
			NetMsg_SetSoundNames( this->pev, false, theSoundNameToSend);
            
            this->mClientSoundNames.push_back(theSoundNameToSend);
        }
    }
}

//TODO: (KGP) there are a lot of expensive per-frame operations here that can be eliminated through careful refactoring.
// 1) make AvHTechTree an abstract interface
// 2) create base case using current AvHTechTree code
// 3) create filter around AvHTechTree that uses override of IsResearchable by MessageID and returns AvHTechNode objects
//    that reflect the filter.
// 4) create AvHTechChangeListener class and use it as basis for decision to send tech nodes
// 5) create NetMsg_SetTechNodeDelta function that bundles state changes for multiple nodes into a single call
// 6) always use a personal copy of AvHTechNodes interface for each player to eliminate the per-frame copy of the team nodes
// 7) use filter class for NS mode aliens and update state of the filter when alien lifeform changes instead of using per-frame update
// Combined, these changes should reduce CPU overhead for tech node update by at least 90%.
void AvHPlayer::UpdateTechNodes()
{
    bool theIsCombatMode = GetGameRules()->GetIsCombatMode();
	bool theIsNSMode = GetGameRules()->GetIsNSMode();
    if((this->GetUser3() == AVH_USER3_COMMANDER_PLAYER) || theIsCombatMode || this->GetIsAlien())
    {
        AvHTeam* theTeam = this->GetTeamPointer();
        if(theTeam)
        {
			// Propagate and use local tech nodes in combat mode, else use team nodes in NS mode
            AvHTechTree theTechNodes = theIsCombatMode ? this->mCombatNodes : theTeam->GetTechNodes();
 
            // Now customize nodes for aliens in NS
            if(theIsNSMode && this->GetIsAlien())
            {
                // Set current lifeform to be unavailable
                AvHMessageID theLifeform = MESSAGE_NULL;
                switch(this->GetUser3())
                {
                case AVH_USER3_ALIEN_PLAYER1:
                    theLifeform = ALIEN_LIFEFORM_ONE;
                    break;
                case AVH_USER3_ALIEN_PLAYER2:
                    theLifeform = ALIEN_LIFEFORM_TWO;
                    break;
                case AVH_USER3_ALIEN_PLAYER3:
                    theLifeform = ALIEN_LIFEFORM_THREE;
                    break;
                case AVH_USER3_ALIEN_PLAYER4:
                    theLifeform = ALIEN_LIFEFORM_FOUR;
                    break;
                case AVH_USER3_ALIEN_PLAYER5:
                    theLifeform = ALIEN_LIFEFORM_FIVE;
                    break;
                }
                theTechNodes.SetIsResearchable(theLifeform, false);

                // If not Gorge, set buildables to be unavailable
                if(theLifeform != ALIEN_LIFEFORM_TWO)
                {
                    theTechNodes.SetIsResearchable(ALIEN_BUILD_HIVE, false);
                    theTechNodes.SetIsResearchable(ALIEN_BUILD_RESOURCES, false);
                    theTechNodes.SetIsResearchable(ALIEN_BUILD_OFFENSE_CHAMBER, false);
                    theTechNodes.SetIsResearchable(ALIEN_BUILD_DEFENSE_CHAMBER, false);
                    theTechNodes.SetIsResearchable(ALIEN_BUILD_MOVEMENT_CHAMBER, false);
                    theTechNodes.SetIsResearchable(ALIEN_BUILD_SENSORY_CHAMBER, false);
                }
                else
                {
                    // If we have the max hives, disable hives

                    bool theHasFreeUpgradeCategory = false;
                    MessageIDListType theUnsupportedUpgrades;
                    theUnsupportedUpgrades.push_back(ALIEN_BUILD_DEFENSE_CHAMBER);
                    theUnsupportedUpgrades.push_back(ALIEN_BUILD_MOVEMENT_CHAMBER);
                    theUnsupportedUpgrades.push_back(ALIEN_BUILD_SENSORY_CHAMBER);

                    FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
                        if(theEntity && theEntity->GetIsActive() && (theEntity->pev->team == this->pev->team))
                        {
                            AvHMessageID theTechnology = theEntity->GetTechnology();
                            if(theTechnology == MESSAGE_NULL)
                            {
                                theHasFreeUpgradeCategory = true;
                                break;
                            }
                            else
                            {
                                // It's supported, so remove from unsupported list
                                theUnsupportedUpgrades.erase(std::find(theUnsupportedUpgrades.begin(), theUnsupportedUpgrades.end(), theTechnology));
                            }
                        }
                    END_FOR_ALL_ENTITIES(kesTeamHive);

                    // If we don't have a free upgrade category
                    if(!theHasFreeUpgradeCategory)
                    {
                        // Remove every unsupported structure
                        for(MessageIDListType::iterator theIter = theUnsupportedUpgrades.begin(); theIter != theUnsupportedUpgrades.end(); theIter++)
                        {
                            theTechNodes.SetIsResearchable(*theIter, false);
                        }
                    }
                }

                // If there are no free upgrades available, disable upgrades
                AvHTeam* theTeamPointer = this->GetTeamPointer();
                if(theTeamPointer)
                {
                    AvHAlienUpgradeListType theUpgrades = theTeamPointer->GetAlienUpgrades();
					if(!AvHGetHasFreeUpgradeCategory(ALIEN_UPGRADE_CATEGORY_DEFENSE, theUpgrades, this->pev->iuser4))
					{
                        theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_ONE, false);
                        theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_TWO, false);
                        theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_THREE, false);
					}
					if(!AvHGetHasFreeUpgradeCategory(ALIEN_UPGRADE_CATEGORY_MOVEMENT, theUpgrades, this->pev->iuser4))
					{
						theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_SEVEN, false);
						theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_EIGHT, false);
						theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_NINE, false);
					}
					if(!AvHGetHasFreeUpgradeCategory(ALIEN_UPGRADE_CATEGORY_SENSORY, theUpgrades, this->pev->iuser4))
					{
                        theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_TEN, false);
                        theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_ELEVEN, false);
                        theTechNodes.SetIsResearchable(ALIEN_EVOLUTION_TWELVE, false);
					}
                }
            }
            
			theTechNodes.GetDelta( this->mClientTechNodes,this->mClientTechDelta );
			if( !mClientTechDelta.empty() )
			{
				const AvHTechNode* Node = NULL;
				MessageIDListType::iterator current, end = mClientTechDelta.end();
				for( current = mClientTechDelta.begin(); current != end; ++current )
				{
					Node = theTechNodes.GetNode(*current);
					if( Node != NULL )
					{
						NetMsg_SetTechNode( this->pev, Node );
						this->mClientTechNodes.InsertNode( Node );
					}
					else
					{
						//TODO: send signal to remove the tech node from the client here...
						this->mClientTechNodes.RemoveNode(*current);
					}
				}
				mClientTechDelta.clear();
				mClientTechNodes = theTechNodes;
			}

            // Propagate any tech slots that have changed
            const AvHTechSlotListType& theTeamTechSlotList = theTeam->GetTechSlotManager().GetTechSlotList();
            if(this->mClientTechSlotList != theTeamTechSlotList)
            {
                // Send any nodes that have changed
                int theCurrentSlot = 0;
                for(AvHTechSlotListType::const_iterator theIter = theTeamTechSlotList.begin(); theIter != theTeamTechSlotList.end(); theIter++, theCurrentSlot++)
                {
                    bool theHasClientTechSlot = false;
                    AvHTechSlots theClientTechSlot;
                    if((signed)this->mClientTechSlotList.size() > theCurrentSlot)
                    {
                        theClientTechSlot = this->mClientTechSlotList[theCurrentSlot];
                        theHasClientTechSlot = true;
                    }

                    AvHTechSlots theServerTechSlot = theTeamTechSlotList[theCurrentSlot];

                    if(!theHasClientTechSlot || (theClientTechSlot != theServerTechSlot))
                    {
						NetMsg_SetTechSlots( this->pev, theServerTechSlot );
                    }
                }

                this->mClientTechSlotList = theTeamTechSlotList;
            }
        }
    }
}

void AvHPlayer::UpdateTopDownMode()
{
    if((this->mClientInTopDownMode != this->mInTopDownMode) || (this->mSpecialPASOrigin != this->mClientSpecialPASOrigin))
    {
		vec3_t& angles = this->mInTopDownMode ? this->mSpecialPASOrigin : this->mAnglesBeforeTopDown;
		float position[3] = { angles.x, angles.y, angles.z };
		NetMsg_SetTopDown_Position( this->pev, this->mInTopDownMode, position );

        this->mClientInTopDownMode = this->mInTopDownMode;
        this->mClientSpecialPASOrigin = this->mSpecialPASOrigin;
    }

    // Send menu tech slots to commander
    AvHTeam* theTeam = this->GetTeamPointer();
    if(theTeam && this->mInTopDownMode)
    {
        int theMenuTechSlots = theTeam->GetMenuTechSlots();
        if(theMenuTechSlots != this->mClientMenuTechSlots)
        {
			NetMsg_SetTopDown_TechSlots( this->pev, theMenuTechSlots );
            this->mClientMenuTechSlots = theMenuTechSlots;
        }
    }
}

void AvHPlayer::UpdateExperienceLevelsSpent()
{
    // If our spent level is different then our client's
    if(this->mClientExperienceLevelsSpent != this->mExperienceLevelsSpent)
    {
		NetMsg_GameStatus_UnspentLevels( this->pev, kGameStatusUnspentLevels, GetGameRules()->GetMapMode(), this->mExperienceLevelsSpent );
        this->mClientExperienceLevelsSpent = this->mExperienceLevelsSpent;
    }
}

void AvHPlayer::UpdateEntityHierarchy()
{
  
    AvHPlayer* player = this;

    // If we're spectating, then use the minimap data for the player we're spectating.

    AvHPlayer* spectatingPlayer = dynamic_cast<AvHPlayer*>(GetSpectatingEntity());

    if (spectatingPlayer)
    {
        player = spectatingPlayer;
    }
    
    AvHTeam* theTeam = player->GetTeamPointer();

    if(theTeam && GetGameRules()->GetGameStarted())
    {
        if (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE ||
            theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
        {
            // Pass in previous version so it can optimize and only send diff
            AvHEntityHierarchy& theEntityList = GetGameRules()->GetEntityHierarchy(player->GetTeam());

            if(theEntityList.SendToNetworkStream(this->mClientEntityHierarchy, this->pev))
            {
                this->mClientEntityHierarchy = theEntityList;
            }
        }
    }

}

void AvHPlayer::UpdateSpawnScreenFade()
{
    if(this->mSendSpawnScreenFade)
    {
        Vector theFadeColor;
        theFadeColor.x = 0;
        theFadeColor.y = 0;
        theFadeColor.z = 0;
        UTIL_ScreenFade(this, theFadeColor, kSpawnInFadeTime, 0.0f, 255, FFADE_IN);
        this->mSendSpawnScreenFade = false;
    }
}


void AvHPlayer::UpdateDebugCSP()
{
    bool theCSPChanged = memcmp(&this->mClientDebugCSPInfo, &this->mDebugCSPInfo, sizeof(weapon_data_t));
    if(theCSPChanged || (this->mClientNextAttack != this->m_flNextAttack))
    {
		NetMsg_DebugCSP( this->pev, this->mDebugCSPInfo, this->m_flNextAttack );
        memcpy(&this->mClientDebugCSPInfo, &this->mDebugCSPInfo, sizeof(weapon_data_t));
        this->mClientNextAttack = this->m_flNextAttack;
    }
}

void AvHPlayer::UpdateOverwatch()
{
    // Update overwatch indicator
    if(this->mClientInOverwatch != this->mInOverwatch)
    {
        // We are entering overwatch
        if(this->mInOverwatch)
        {
            PLAYBACK_EVENT_FULL(0, this->edict(), gStartOverwatchEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
        }
        // We are leaving overwatch
        else
        {
            PLAYBACK_EVENT_FULL(0, this->edict(), gEndOverwatchEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
        }
        this->mClientInOverwatch = this->mInOverwatch;
    }
}

bool AvHPlayer::GetCanUseWeapon() const
{
    return GetIsAbleToAct() && pev->viewmodel;
}

// tankefugl: 0000953
// allows a player to join team only once each inCoolDownTime seconds
bool AvHPlayer::JoinTeamCooledDown(float inCoolDownTime) {
//	UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("Enter: JoinTeamCooledDown(%f), gpGlobals->time = %f, this->mTimeLastJoinTeam = %f", inCoolDownTime, gpGlobals->time, this->mTimeLastJoinTeam));
	if ((this->mTimeLastJoinTeam == -1) || (gpGlobals->time > this->mTimeLastJoinTeam + inCoolDownTime)) 
	{
		this->mTimeLastJoinTeam = gpGlobals->time;
		return true;
	}
	else
		return false;
}
// :tankefugl

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Nexus interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//TODO: flesh this out with admin privileges, etc. once the UPP authorization interface has been expanded
bool AvHPlayer::GetIsAuthorized(AvHAuthAction inAction, int inParameter) const
{
	switch( inAction )
	{
		case AUTH_ACTION_JOIN_TEAM:
		{
			AvHTeamNumber theTeam = (AvHTeamNumber)inParameter;
			switch( theTeam )
			{
			case TEAM_IND:			// ready room & spectator - game allows in all cases
			case TEAM_SPECT:
				return true;
			default: 
				// check it's an active team
				if( theTeam == GetGameRules()->GetTeamA()->GetTeamNumber() || theTeam == GetGameRules()->GetTeamB()->GetTeamNumber() )
				{
					// tankefugl: 0001042 -- allow switching of teams -- placeholder before Nexus
					// if( GetGameRules()->GetCheatsEnabled() ) { return true; }	// cheaters can switch
					// if( !GetGameRules()->GetGameStarted() ) { return true; }	// can switch teams before start
					// if( this->GetHasBeenSpectator() ) { return false; }			// spectators have seen everybody
					// for(int counter = TEAM_ACTIVE_BEGIN; counter < TEAM_ACTIVE_END; counter++)
					// {
					//		if( theTeam != counter && this->GetHasSeenTeam( (AvHTeamNumber)counter ) )
					//		{ return false; }  // we've seen another active team
					// }
					return true;	// haven't seen another team, authorized to join
				}
				return false;		// unknown/inactive team - never grant an unknown permission!
			}
		}
		case AUTH_ACTION_ADJUST_BALANCE:
		{
#ifndef BALANCE_ENABLED
			return false;
#else
			return this->GetIsMember(PLAYERAUTH_DEVELOPER);
#endif
		}
		default:
			return false;			// never grant an unknown permission!
	}
}

bool AvHPlayer::GetIsMember(const string& inAuthGroup) const
{
	return false;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceChangeListener implementation and balance network code
//
// Balance is checked for changes at a set rate determined by the
// BALANCE_UPDATE_MAX_FREQUENCY const below. This prevents the
// balance system logic from bogging down the server if there are 32
// players and the entire system is reloaded.  A maximum of one
// message will be sent with each check.  Note that this system is
// much, much more efficient than the old check of the entire balance
// state every frame!
//
// Due to the setup of the balance system, the BalanceChanageListener
// functions will never be called for non-playtest compiles, so
// there is no need to gaurd with a playtest build #define. The 
// call to UpdateBalanceVariables may benefit from an being #define'd 
// out, but that function has very low overhead anyway.
//
// TODO: move this block (variables and logic) into a discrete class
// and associate that class with the player using an auto_ptr instead 
// of embedding the information into the player class
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const float BALANCE_UPDATE_MAX_FREQUENCY = 0.05;	//maximum frequency at which checks occur

bool AvHPlayer::shouldNotify(const string& name, const BalanceValueType type) const
{
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHPlayer::balanceCleared(void) const
{
	this->mBalanceRemovalList.clear();
	this->mBalanceMapInts.clear();
	this->mBalanceMapFloats.clear();
	this->mBalanceMapStrings.clear();
	NetMsg_BalanceVarClear( this->pev );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// INTEGER
void AvHPlayer::balanceValueInserted(const string& name, const int value) const
{
	this->mBalanceRemovalList.erase(name); //in case we had previous signal for deletion
	this->mBalanceMapInts.insert(BalanceIntCollection::value_type(name,value)); 
}

void AvHPlayer::balanceValueChanged(const string& name, const int old_value, const int new_value) const
{
	this->mBalanceRemovalList.erase(name); //in case we had previous signal for deletion
	this->mBalanceMapInts.insert(BalanceIntCollection::value_type(name,new_value)); 
}

void AvHPlayer::balanceValueRemoved(const string& name, const int old_value) const
{ 
	this->mBalanceMapInts.erase(name); //in case we didn't send it yet
	this->mBalanceRemovalList.insert(name);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FLOAT
void AvHPlayer::balanceValueInserted(const string& name, const float value) const
{ 
	this->mBalanceRemovalList.erase(name); //in case we had previous signal for deletion
	this->mBalanceMapFloats.insert(BalanceFloatCollection::value_type(name,value)); 
}

void AvHPlayer::balanceValueChanged(const string& name, const float old_value, const float new_value) const
{ 
	this->mBalanceRemovalList.erase(name); //in case we had previous signal for deletion
	this->mBalanceMapFloats.insert(BalanceFloatCollection::value_type(name,new_value)); 
}

void AvHPlayer::balanceValueRemoved(const string& name, const float old_value) const
{ 
	this->mBalanceMapFloats.erase(name); //in case we didn't send it yet
	this->mBalanceRemovalList.insert(name);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// STRING
void AvHPlayer::balanceValueInserted(const string& name, const string& value) const
{ 
	this->mBalanceRemovalList.erase(name); //in case we had previous signal for deletion
	this->mBalanceMapStrings.insert(BalanceStringCollection::value_type(name,value)); 
}

void AvHPlayer::balanceValueChanged(const string& name, const string& old_value, const string& new_value) const
{
	this->mBalanceRemovalList.erase(name); //in case we had previous signal for deletion
	this->mBalanceMapStrings.insert(BalanceStringCollection::value_type(name,new_value)); 
}

void AvHPlayer::balanceValueRemoved(const string& name, const string& old_value) const
{ 
	this->mBalanceMapStrings.erase(name); //in case we didn't send it yet
	this->mBalanceRemovalList.insert(name);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHPlayer::InitBalanceVariables(void)
{
	//grab the entire current balance
	BalanceValueContainer* container = BalanceValueContainerFactory::get(BalanceValueContainerFactory::getDefaultFilename());
	this->mBalanceMapStrings = *container->getStringMap();
	this->mBalanceMapInts = *container->getIntMap();
	this->mBalanceMapFloats = *container->getFloatMap();

	//clear the client in preparation to send everything again
	//pev will be null if this is called during construction
	if( this->pev ) { NetMsg_BalanceVarClear( this->pev ); }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AvHPlayer::UpdateBalanceVariables(void)
{
    if(mNextBalanceVarUpdate < gpGlobals->time)
    {
		//only send it if it can be used...
		//CONSIDER: second security setting for read-only transfer
		if(this->GetIsAuthorized(AUTH_ACTION_ADJUST_BALANCE,0))
		{
			//check number of changes we have to send
			int total_changes = this->mBalanceRemovalList.size();
			total_changes += this->mBalanceMapInts.size();
			total_changes += this->mBalanceMapFloats.size();
			total_changes += this->mBalanceMapStrings.size();

			//if we have multiple changes and need to tell client they are a set
			if( total_changes > 1 && !this->mBalanceMessagePending ) //flag multiple changes incoming
			{
				NetMsg_BalanceVarChangesPending( this->pev, true );
				this->mBalanceMessagePending = true;
			}
			//else if we have no more changes -- check to see if we need to send end set signal
			else if( total_changes == 0 )
			{
				if( this->mBalanceMessagePending )
				{
					NetMsg_BalanceVarChangesPending( this->pev, false );
					this->mBalanceMessagePending = false;
				}
			}
			// we have at least one change to make, possibly in a set
			else if(!this->mBalanceRemovalList.empty())
			{
				set<string>::iterator item = this->mBalanceRemovalList.begin();
				NetMsg_BalanceVarRemove( this->pev, *item );
				this->mBalanceRemovalList.erase(item);
			}
			else if(!this->mBalanceMapInts.empty())
			{
				BalanceIntCollection::iterator item = this->mBalanceMapInts.begin();
				NetMsg_BalanceVarInsertInt( this->pev, item->first, item->second );
				this->mBalanceMapInts.erase(item);
			}
			else if(!this->mBalanceMapFloats.empty())
			{
				BalanceFloatCollection::iterator item = this->mBalanceMapFloats.begin();
				NetMsg_BalanceVarInsertFloat( this->pev, item->first, item->second );
				this->mBalanceMapFloats.erase(item);
			}
			else if(!this->mBalanceMapStrings.empty())
			{
				BalanceStringCollection::iterator item = this->mBalanceMapStrings.begin();
				NetMsg_BalanceVarInsertString( this->pev, item->first, item->second );
				this->mBalanceMapStrings.erase(item);
			}
		}
		//update next check of balance message queue
		mNextBalanceVarUpdate = gpGlobals->time + BALANCE_UPDATE_MAX_FREQUENCY;
	}
}
