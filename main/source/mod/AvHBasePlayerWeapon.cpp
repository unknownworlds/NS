//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBasePlayerWeapon.cpp$
// $Date: 2002/11/22 21:28:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBasePlayerWeapon.cpp,v $
// Revision 1.38  2002/11/22 21:28:15  Flayra
// - mp_consistency changes
//
// Revision 1.37  2002/11/03 04:47:23  Flayra
// - Moved weapon expiring into .dll out of .cfg
//
// Revision 1.36  2002/10/24 21:22:10  Flayra
// - Fixes muzzle-flash showing when firing an empty weapon
//
// Revision 1.35  2002/10/17 17:34:14  Flayra
// - Part 2 of persistent weapons fix (found with Grendel)
//
// Revision 1.34  2002/10/16 20:51:17  Flayra
// - Fixed problem where acid projectile hit player
//
// Revision 1.33  2002/10/03 18:39:24  Flayra
// - Added heavy view models
//
// Revision 1.32  2002/09/23 22:10:46  Flayra
// - Weapons now stick around the way they should (forever when dropped by commander, weapon stay time when dropped by player)
//
// Revision 1.31  2002/08/16 02:33:12  Flayra
// - Added damage types
//
// Revision 1.30  2002/07/24 19:11:45  Flayra
// - Linux issues
//
// Revision 1.29  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.28  2002/07/08 16:47:31  Flayra
// - Reworked bullet firing to add random spread (bug #236), temporarily hacked shotty player animation, removed old adrenaline, don't allow using weapons when invulnerable
//
// Revision 1.27  2002/07/01 21:17:13  Flayra
// - Removed outdated adrenaline concept, made ROF generic for primal scream
//
// Revision 1.26  2002/06/25 17:41:13  Flayra
// - Reworking for correct player animations and new enable/disable state
//
// Revision 1.25  2002/06/10 19:50:37  Flayra
// - First-pass at level 1 animated view model (different anims when running and walking)
//
// Revision 1.24  2002/06/03 16:29:53  Flayra
// - Added resupply (from arsenal), better animation support (for both view model and player model)
//
// Revision 1.23  2002/05/28 17:37:33  Flayra
// - Max entities fix, added animation empty fire
//
// Revision 1.22  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifdef AVH_CLIENT
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/ammo.h"
#include "cl_dll/ammohistory.h"
extern int g_runfuncs;
#endif

#include "util/nowarnings.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayer.h"
#include "common/usercmd.h"
#include "pm_shared/pm_defs.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHMarineWeapons.h"

#ifdef AVH_SERVER
#include "mod/AvHServerUtil.h"
#include "mod/AvHGamerules.h"

extern int				gWelderConstEventID;
#endif

#ifdef AVH_CLIENT
#include "cl_dll/com_weapons.h"
#endif

#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSharedUtil.h"
#include "types.h"						
#include "common/vector_util.h"
#include "util/MathUtil.h"

// extern "C" this guy because delcared in pm_shared.c, not pm_shared.cpp
extern playermove_t*	pmove;
extern cvar_t			weaponstay;

Vector UTIL_GetRandomSpreadDir(unsigned int inSeed, int inShotNumber, const Vector& inBaseDirection, const Vector& inRight, const Vector& inUp, const Vector& inSpread)
{
	// Use player's random seed.
	// get circular gaussian spread
	float x = UTIL_SharedRandomFloat( inSeed + inShotNumber, -0.5, 0.5 ) + UTIL_SharedRandomFloat( inSeed + ( 1 + inShotNumber ) , -0.5, 0.5 );
	float y = UTIL_SharedRandomFloat( inSeed + ( 2 + inShotNumber ), -0.5, 0.5 ) + UTIL_SharedRandomFloat( inSeed + ( 3 + inShotNumber ), -0.5, 0.5 );
	float z = x * x + y * y;
	
	Vector theRandomDir = inBaseDirection + x * inSpread.x * inRight + y * inSpread.y * inUp;
	
	return theRandomDir;
}


AvHBasePlayerWeapon::AvHBasePlayerWeapon()
{
    // reasonable defaults for everything else
    this->mEvent = 0;
	this->mWeaponAnimationEvent = 0;
    this->mStartEvent = 0;
    this->mEndEvent = 0;
	this->mRange = 8012;
	this->mDamage = 10;
	this->mAttackButtonDownLastFrame = false;
	this->mTimeOfLastResupply = -1;
    this->mTimeOfLastPrime = -1;
    this->mWeaponPrimeStarted = false;

#ifdef AVH_SERVER
	this->mInOverwatch = false;
	this->mIsPersistent = false;
	this->mLifetime = -1;
#endif
}
void AvHBasePlayerWeapon::PrintWeaponToClient(CBaseEntity *theAvHPlayer) {
	char msg[1024];
	ItemInfo theItemInfo;
	this->GetItemInfo(&theItemInfo);
	sprintf(msg, "%s iuser3=%d\tenabled = %d\n", theItemInfo.pszName, this->pev->iuser3, this->m_iEnabled);
	ClientPrint(theAvHPlayer->pev, HUD_PRINTNOTIFY, msg);
}

int	AvHBasePlayerWeapon::AddToPlayer( CBasePlayer *pPlayer )
{
	// Can we predict weapon pick-ups?  I bet we can.
	int theAddedToPlayer = 0;
	
#ifdef AVH_SERVER
	AvHPlayer* inPlayer = dynamic_cast<AvHPlayer*>(pPlayer);
	ASSERT(inPlayer != NULL);

	if(this->GetAllowedForUser3(inPlayer->GetUser3()))
	{
		// Make sure player doesn't have this weapon already
		if(!pPlayer->HasItem(this))
		{
			// If weapon was placed by mapper
			if(this->GetIsPersistent())
			{
				// Create a new weapon and give it to the player
				pPlayer->GiveNamedItem(STRING(this->pev->classname));
				
				this->DestroyItem();
			}
			else
			{
				theAddedToPlayer = CBasePlayerWeapon::AddToPlayer(pPlayer);
				if(theAddedToPlayer)
				{
					// Make sure it's not set for expiration
					SetThink(NULL);
				}
			}
		}
	}
#endif
	
	return theAddedToPlayer;
}

BOOL AvHBasePlayerWeapon::Deploy()
{
	char* theDeploySound = this->GetDeploySound();
	if(theDeploySound)
	{
		EMIT_SOUND(ENT(this->pev), CHAN_WEAPON, this->GetDeploySound(), this->GetDeploySoundVolume(), ATTN_NORM);
	}

	char* theAnimExt = this->GetAnimationExtension();

	BOOL theSuccess = DefaultDeploy(this->GetActiveViewModel(), this->GetPlayerModel(), this->GetDeployAnimation(), theAnimExt);

	// Set a player animatiom here?
	//this->m_pPlayer->SetAnimation(PLAYER_ANIM(iAnim));

	// Set deploy time
	this->m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + this->GetDeployTime();

    this->mTimeOfLastPrime = -1;
    this->mWeaponPrimeStarted = false;

	return theSuccess;
}

BOOL AvHBasePlayerWeapon::DefaultDeploy( char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal, int body)
{
	if (!CanDeploy( ))
		return FALSE;
	
	m_pPlayer->TabulateAmmo();

    // This was causing a crash from hl_weapons.cpp only when connected to a dedicated server and switching weapons
	//#ifdef AVH_SERVER
    //m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);
    //#endif

#ifdef AVH_SERVER
    m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);
    m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);
#else
    gEngfuncs.CL_LoadModel( szViewModel, &m_pPlayer->pev->viewmodel );    
    gEngfuncs.CL_LoadModel( szWeaponModel, &m_pPlayer->pev->weaponmodel );    
#endif

	strcpy( m_pPlayer->m_szAnimExtention, szAnimExt );
	//SendWeaponAnim( iAnim, skiplocal, body );
	this->SendWeaponAnim(iAnim);
	
	// Set the player animation as well
	//this->m_pPlayer->SetAnimation(PLAYER_ANIM(iAnim));
	
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + this->GetDeployTime();
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + this->GetDeployTime() + kDeployIdleInterval;
	
	return TRUE;
}

BOOL AvHBasePlayerWeapon::DefaultReload( int iClipSize, int iAnim, float fDelay, int body)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return FALSE;

	// Don't reload while we're resupplying
	if(this->mTimeOfLastResupply > 0)
	{
		return FALSE;
	}
	
	int j = min(iClipSize - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);	
	
	if (j == 0)
		return FALSE;
	
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;
	
	//!!UNDONE -- reload sound goes here !!!
	//SendWeaponAnim( iAnim, UseDecrement() ? 1 : 0 );
	this->SendWeaponAnim(iAnim);

	// Send reload to all players.  Reloads are initiated server-side, so send down to local client as well
	this->m_pPlayer->pev->weaponanim = iAnim;
	this->PlaybackEvent(this->mWeaponAnimationEvent, iAnim, FEV_RELIABLE);

	// Player model reload animation
	this->m_pPlayer->SetAnimation(PLAYER_RELOAD);
	
	m_fInReload = TRUE;
	
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + kDeployIdleInterval;
	
	return TRUE;
}



char* AvHBasePlayerWeapon::GetActiveViewModel() const
{
	return this->GetViewModel();
}

//BOOL AvHBasePlayerWeapon::Deploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, float inNextAttackTime, int skiplocal)
//{
//	BOOL theSuccess = FALSE;
//
//	if(CanDeploy())
//	{
//		this->m_pPlayer->TabulateAmmo();
//		this->m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);
//		this->m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);
//		strcpy( m_pPlayer->m_szAnimExtention, szAnimExt );
//		SendWeaponAnim( iAnim, skiplocal );
//		
//		this->m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + inNextAttackTime;
//		this->m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
//
//		theSuccess = TRUE;
//	}
//	
//	return theSuccess;
//}

char* AvHBasePlayerWeapon::GetAnimationExtension() const
{
	char* theAnimExt = NULL;

	AvHWeaponID theWeaponID = (AvHWeaponID)this->m_iId;
	switch(theWeaponID)
	{
		case AVH_WEAPON_BITE:
		case AVH_WEAPON_SPIT:
		case AVH_WEAPON_SPIKE:
		case AVH_WEAPON_BITE2:
		case AVH_WEAPON_SWIPE:
		case AVH_WEAPON_CLAWS:
		theAnimExt = "ability1";
		break;

		case AVH_WEAPON_PARASITE:
		case AVH_WEAPON_HEALINGSPRAY:
		case AVH_WEAPON_SPORES:
		case AVH_WEAPON_BLINK:
		case AVH_WEAPON_STOMP:
		theAnimExt = "ability2";
		break;
		
		case AVH_ABILITY_LEAP:
		case AVH_WEAPON_BILEBOMB:
		case AVH_WEAPON_UMBRA:
		case AVH_WEAPON_METABOLIZE:
		case AVH_WEAPON_DEVOUR:
		theAnimExt = "ability3";
		break;

		case AVH_WEAPON_DIVINEWIND:
		case AVH_WEAPON_WEBSPINNER:
		case AVH_WEAPON_PRIMALSCREAM:
		case AVH_WEAPON_ACIDROCKET:
		case AVH_ABILITY_CHARGE:
		theAnimExt = "ability4";
		break;
		
		case AVH_WEAPON_KNIFE:
			theAnimExt = "knife";
			break;
		case AVH_WEAPON_PISTOL:
			theAnimExt = "pistol";
			break;
		case AVH_WEAPON_MG:
			theAnimExt = "lmg";
			break;
		case AVH_WEAPON_SONIC:
			theAnimExt = "shotgun";
			break;
		case AVH_WEAPON_HMG:
			theAnimExt = "hmg";
			break;
		case AVH_WEAPON_WELDER:
			theAnimExt = "welder";
			break;
		case AVH_WEAPON_MINE:
			theAnimExt = "tripmine";
			break;
		case AVH_WEAPON_GRENADE_GUN:
			theAnimExt = "grenadegun";
			break;
        case AVH_WEAPON_GRENADE:
            theAnimExt = "handgrenade";
            break;
		default:
			ASSERT(false);
			break;
	}
	return theAnimExt;
}

int	AvHBasePlayerWeapon::GetClipSize() const
{
	ItemInfo theItemInfo;
	this->GetItemInfo(&theItemInfo);
	int theClipSize = theItemInfo.iMaxClip;

	return theClipSize;
}

int AvHBasePlayerWeapon::GetPrimaryAmmoAmount() const
{
    int theAmmo = 0;

    if(this->m_pPlayer && (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0))
    {
        theAmmo = this->m_pPlayer->m_rgAmmo[this->m_iPrimaryAmmoType];
    }

    return theAmmo;
}

int	AvHBasePlayerWeapon::GetDamageType() const
{
	return NS_DMG_NORMAL;
}

char* AvHBasePlayerWeapon::GetDeploySound() const
{
	return NULL;
}

float AvHBasePlayerWeapon::GetDeploySoundVolume() const
{
	return 1.0f;
}

int	AvHBasePlayerWeapon::GetEmptyShootAnimation() const
{
	return kShootEmptyAnimation;
}

void AvHBasePlayerWeapon::GetEventOrigin(Vector& outOrigin) const
{
	VectorCopy(this->m_pPlayer->pev->origin, outOrigin);
}

void AvHBasePlayerWeapon::GetEventAngles(Vector& outAngles) const
{
	outAngles = this->pev->v_angle + this->pev->punchangle;
}

bool AvHBasePlayerWeapon::GetFiresUnderwater() const
{
	return false;
}

bool AvHBasePlayerWeapon::GetIsFiring() const
{
	return this->mAttackButtonDownLastFrame;
}

bool AvHBasePlayerWeapon::GetHasMuzzleFlash() const
{
	return false;
}

bool AvHBasePlayerWeapon::GetIsCapableOfFiring() const
{
	return !this->UsesAmmo() || (this->m_iClip > 0);
}


int	AvHBasePlayerWeapon::GetDeployAnimation() const
{
	return kDeployAnimation;
}

int	AvHBasePlayerWeapon::GetIdleAnimation() const
{
	int iAnim;
	int theRandomNum = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 1);

	switch(theRandomNum)
	{
	case 0:	
		iAnim = kIdleAnimationOne;	
		break;
		
	default:
	case 1:
        iAnim = kIdleAnimationTwo;
		break;
	}

	return iAnim;
}

Vector AvHBasePlayerWeapon::GetProjectileSpread() const
{
	return VECTOR_CONE_0DEGREES;
}

float AvHBasePlayerWeapon::ComputeAttackInterval() const
{
    return this->GetRateOfFire();
}

float AvHBasePlayerWeapon::GetRateOfFire() const
{
    return 1;
}

int	AvHBasePlayerWeapon::GetShootAnimation() const
{
	return kShootAnimation;
}

int	AvHBasePlayerWeapon::GetShotsInClip() const
{
	int theShotsInClip = -1;

	if(this->UsesAmmo())
	{
		theShotsInClip = this->m_iClip;
	}

	return theShotsInClip;
}

bool AvHBasePlayerWeapon::GetIsDroppable() const
{
	return true;
}

bool AvHBasePlayerWeapon::GetIsPlayerMoving() const
{
	bool thePlayerIsMoving = false;

//	float kMovingThresholdVelocity = 100;
//	float theCurrentVelocity = 0;
//
//	#ifdef AVH_SERVER
//	theCurrentVelocity = this->m_pPlayer->pev->velocity.Length();
//	#endif
//	
//	#ifdef AVH_CLIENT
//	cl_entity_t* theLocalPlayer = gEngfuncs.GetLocalPlayer();
//	if(theLocalPlayer)
//	{
//		theCurrentVelocity = theLocalPlayer->curstate.velocity.Length();
//	}
//	#endif
//
//	if(theCurrentVelocity > kMovingThresholdVelocity)
//	{
//		thePlayerIsMoving = true;
//	}

	return thePlayerIsMoving;
}

int	AvHBasePlayerWeapon::GetMaxClipsCouldReceive()
{
	int theMaxClips = 0;
	
	ItemInfo theItemInfo;
	this->GetItemInfo(&theItemInfo);
	int theClipSize = theItemInfo.iMaxClip;

	if(theClipSize > 0 && this->UsesAmmo())
	{
		theMaxClips = ((theItemInfo.iMaxAmmo1 - this->m_iClip)/theClipSize) + 1;
	}

	return theMaxClips;
}

AvHWeaponID	AvHBasePlayerWeapon::GetPreviousWeaponID() const
{
	AvHWeaponID theID = AVH_WEAPON_NONE;

	// Look at most recently used weapon and see if we can transition from it
	AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(this->m_pPlayer->m_pLastItem);
	if(theWeapon)
	{
		theID = (AvHWeaponID)(theWeapon->m_iId);
	}

	return theID;
}


float AvHBasePlayerWeapon::GetRange() const
{
	return this->mRange;
}

vec3_t AvHBasePlayerWeapon::GetWorldBarrelPoint() const
{
	ASSERT(this->m_pPlayer);

	Vector vecAiming = gpGlobals->v_forward;
	Vector theWorldBarrelPoint = this->m_pPlayer->GetGunPosition() + vecAiming*this->GetBarrelLength();

	return theWorldBarrelPoint;

}

char* AvHBasePlayerWeapon::GetPlayerModel() const
{
	return kNullModel;
}

char* AvHBasePlayerWeapon::GetPrimeSound() const
{
    return NULL;
}

float AvHBasePlayerWeapon::GetPrimeSoundVolume() const
{
    return 1.0f;
}

char* AvHBasePlayerWeapon::GetViewModel() const
{
	return kNullModel;
}

char* AvHBasePlayerWeapon::GetWorldModel() const
{
	return kNullModel;
}

void AvHBasePlayerWeapon::Holster( int skiplocal)
{
	CBasePlayerWeapon::Holster(skiplocal);

	#ifdef AVH_SERVER
	this->mInOverwatch = false;
	#endif
}

float AvHBasePlayerWeapon::GetTimePassedThisTick() const
{
	float theClientTimePassedThisTick = (pmove->cmd.msec/1000.0f);
	return theClientTimePassedThisTick;
}

void AvHBasePlayerWeapon::ItemPostFrame( void )
{
	CBasePlayerWeapon::ItemPostFrame();
	
	float theClientTimePassedThisTick = this->GetTimePassedThisTick();

	this->m_flNextPrimaryAttack -= theClientTimePassedThisTick;
	this->m_flTimeWeaponIdle -= theClientTimePassedThisTick;
	this->mTimeOfLastResupply -= theClientTimePassedThisTick;
    this->mTimeOfLastPrime -= theClientTimePassedThisTick;
}

void AvHBasePlayerWeapon::Precache(void)
{                  
	CBasePlayerWeapon::Precache();

	char* theDeploySound = this->GetDeploySound();
	if(theDeploySound)
	{
		PRECACHE_UNMODIFIED_SOUND(theDeploySound);
	}
    char* thePrimeSound = this->GetPrimeSound();
    if(thePrimeSound)
    {
        PRECACHE_UNMODIFIED_SOUND(thePrimeSound);
    }
	char* thePlayerModel = this->GetPlayerModel();
	if(thePlayerModel)
	{
		PRECACHE_UNMODIFIED_MODEL(thePlayerModel);
	}
	char* theViewModel = this->GetViewModel();
	if(theViewModel)
	{
		PRECACHE_UNMODIFIED_MODEL(theViewModel);
	}
	char* theWorldModel = this->GetWorldModel();
	if(theWorldModel)
	{
		PRECACHE_UNMODIFIED_MODEL(theWorldModel);
	}

	this->mWeaponAnimationEvent = PRECACHE_EVENT(1, kWeaponAnimationEvent);
}

bool AvHBasePlayerWeapon::ProcessValidAttack(void)
{
    bool theAttackIsValid = false;
	
//#ifdef AVH_CLIENT
//	char sz[ 128 ];
//	sprintf(sz, "during check valid, clip is %d\n", this->m_iClip);
//	gEngfuncs.pfnCenterPrint( sz );
//	
//	//gEngfuncs.Con_Printf("during idle, clip is %d\n", sz);
//#endif

//#ifdef AVH_CLIENT
//	if(gHUD.GetInTopDownMode())
//	{
//		return false;
//	}
//#endif
//
//#ifdef AVH_SERVER
//	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
//	if(thePlayer->GetIsInTopDownMode())
//	{
//		return false;
//	}
//#endif

	// Only shoot if deployed and enabled (iuser3 is 0 when disabled, 1 when enabled <from m_fireState>)

	// puzl: 497 call GetEnabledState instead of testing directly
	int enabledState=this->GetEnabledState();

	if(this->m_pPlayer->pev->viewmodel && ( enabledState == 1))
	{
		// don't fire underwater
		if((this->m_pPlayer->pev->waterlevel == 3) && !this->GetFiresUnderwater())
		{
			this->PlayEmptySound();
			
			//this->m_flNextPrimaryAttack = gpGlobals->time + this->mROF;
			this->m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + this->ComputeAttackInterval();
		}
		else if(this->UsesAmmo())
		{
			ASSERT(this->m_iPrimaryAmmoType >= 0);
			if(this->m_iClip > 0)
			{
				if(this->m_flNextPrimaryAttack <= 0)
				{
					if(!this->GetMustPressTriggerForEachShot() || (!this->mAttackButtonDownLastFrame))
					{
						theAttackIsValid = true;
					}
				}
			}
			else
			{
				// Definitely not firing this pull of the trigger
				theAttackIsValid = false;
				
				BOOL theHasAmmo = 0;
				
				if ( pszAmmo1() )
				{
					theHasAmmo |= (this->m_pPlayer->m_rgAmmo[this->m_iPrimaryAmmoType] != 0);
				}
				if ( pszAmmo2() )
				{
					theHasAmmo |= (this->m_pPlayer->m_rgAmmo[this->m_iSecondaryAmmoType] != 0);
				}
				if (this->m_iClip > 0)
				{
					theHasAmmo |= 1;
				}
				
				if(theHasAmmo)
				{
					// Trigger reload
					this->Reload();
				}
				else
				{
					this->PlayEmptySound();

					this->SendWeaponAnim(this->GetEmptyShootAnimation());

					//this->m_pPlayer->SetAnimation(PLAYER_ATTACK1);
				}
			}
		}
		else
		{
			theAttackIsValid = true;
		}
	}

    return theAttackIsValid;

}

bool AvHBasePlayerWeapon::GetIsGunPositionValid() const
{
    return true;
}

void AvHBasePlayerWeapon::DeductCostForShot(void)
{
	this->m_iClip--;

	// On a successful attack, decloak the player if needed
	#ifdef AVH_SERVER
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	if(thePlayer)
	{
		thePlayer->TriggerUncloak();
	}

	int theResourceCost = this->GetResourceCost();
	if(theResourceCost > 0)
	{
		float theNewResources = (thePlayer->GetResources(false) - theResourceCost);
		theNewResources = max(theNewResources, 0.0f);
		thePlayer->SetResources(theNewResources);
	}

	#endif
}

float AvHBasePlayerWeapon::GetReloadTime(void) const
{
	// TODO: Allow weapons to have different reload times
	return 1.5f;
}

int	AvHBasePlayerWeapon::GetReloadAnimation() const
{
	return kReloadAnimation;
}

void AvHBasePlayerWeapon::PlaybackEvent(unsigned short inEvent, int inIparam2, int inFlags) const
{
	unsigned short theEvent = inEvent;
	if(theEvent != 0)
	{
		// Playback event, sending along enough data so client knows who triggered this event!
		int theWeaponIndex = 0;
		AvHUser3 theUser3 = AVH_USER3_NONE;
		int theUpgrades = 0;
		
		// When predicting weapons, play the event locally, then tell everyone else but us to play it back later
		int flags = inFlags;
		edict_t* theEdict;

		// Pass player random seed to event, so it chooses the right direction for spread
		int theRandomNumber = this->m_pPlayer->random_seed;
		
#if defined( AVH_CLIENT )
		theUser3 = gHUD.GetHUDUser3();
		theUpgrades = gHUD.GetHUDUpgrades();
		theEdict = NULL;
#else
		theUser3 = dynamic_cast<AvHPlayer*>(this->m_pPlayer)->GetUser3();
		theUpgrades = this->m_pPlayer->pev->iuser4;
		theEdict = this->m_pPlayer->edict();
#endif

		// For bullet spread
		//theRandomNumber = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 1, kBulletSpreadGranularity*kBulletSpreadGranularity);
		
		// When in overwatch, the weapon is fired on the server, so the client firing the weapon won't be firing it locally first
#if defined(AVH_SERVER)
		if(this->mInOverwatch)
		{
			flags = 0;
		}
#endif

		// Allow weapon to specify some parameters, so they are available on both client and server
		Vector theEventOrigin;
		this->GetEventOrigin(theEventOrigin);

		Vector theEventAngles;
		this->GetEventAngles(theEventAngles);

		float theVolume = AvHPlayerUpgrade::GetSilenceVolumeLevel(theUser3, theUpgrades);
		
		//( int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );
		PLAYBACK_EVENT_FULL(flags, this->m_pPlayer->edict(), theEvent, 0, (float *)&theEventOrigin, (float *)&theEventAngles, theVolume, 0.0, theRandomNumber, inIparam2, 0, 0 );
	}
}

void AvHBasePlayerWeapon::SetAnimationAndSound(void)
{
	this->m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	this->m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	// player "shoot" animation
	//SendWeaponAnim(this->GetShootAnimation());

	this->m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	if(this->GetHasMuzzleFlash())
	{
		this->m_pPlayer->pev->effects = (int)(this->m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
	}
}

void AvHBasePlayerWeapon::FireProjectiles(void)
{
	Vector vecSrc = this->m_pPlayer->GetGunPosition();
	Vector vecAiming = this->m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	// Fire the bullets and apply damage
	int theRange = this->mRange;
	float theDamage = this->mDamage;

	int theTracerFreq;
	float theDamageMultiplier;
	AvHPlayerUpgrade::GetWeaponUpgrade(this->m_pPlayer->pev->iuser3, this->m_pPlayer->pev->iuser4, &theDamageMultiplier, &theTracerFreq);
	theDamage *= theDamageMultiplier;

	Vector theSpread = this->GetProjectileSpread();
	this->m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, theSpread, theRange, BULLET_PLAYER_MP5, theTracerFreq, theDamage, this->m_pPlayer->pev, this->m_pPlayer->random_seed);

    this->mWeaponPrimeStarted = false;
}

int AvHBasePlayerWeapon::GetPrimeAnimation() const
{
    return -1;
}

float AvHBasePlayerWeapon::GetWeaponPrimeTime() const
{ 
    return -1.0f; 
}

void AvHBasePlayerWeapon::PrimeWeapon()
{
    float thePrimeTime = this->GetWeaponPrimeTime();
    if(thePrimeTime > 0.0f)
    {
        if(!this->GetIsWeaponPriming())
        {
            char* thePrimeSound = this->GetPrimeSound();
            if(thePrimeSound)
            {
                EMIT_SOUND(ENT(this->pev), CHAN_WEAPON, thePrimeSound, this->GetPrimeSoundVolume(), ATTN_NORM);
            }

            this->PlaybackEvent(this->mWeaponAnimationEvent, this->GetPrimeAnimation());

            this->mTimeOfLastPrime = UTIL_WeaponTimeBase() + thePrimeTime;

            this->mWeaponPrimeStarted = true;
        }
    }
}

BOOL AvHBasePlayerWeapon::GetIsWeaponPrimed() const
{ 
    return ((this->GetWeaponPrimeTime() > 0.0f) && this->mWeaponPrimeStarted && (UTIL_WeaponTimeBase() > this->mTimeOfLastPrime));
}

BOOL AvHBasePlayerWeapon::GetIsWeaponPriming() const
{ 
    return ((this->GetWeaponPrimeTime() > 0.0f) && this->mWeaponPrimeStarted && (UTIL_WeaponTimeBase() < this->mTimeOfLastPrime));
}

void AvHBasePlayerWeapon::SetNextAttack(void)
{
//	this->m_flNextPrimaryAttack += this->mROF;
//
//	if(this->m_flNextPrimaryAttack < 0)
//		this->m_flNextPrimaryAttack = this->mROF;

	float theRateOfFire = this->ComputeAttackInterval();

    this->m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + theRateOfFire;

	if(this->m_flNextPrimaryAttack < 0)
	{
		this->m_flNextPrimaryAttack = theRateOfFire;
	}
	
	this->SetNextIdle();
}


void AvHBasePlayerWeapon::PrimaryAttack(void)
{
    if (this->ProcessValidAttack())
    {
        
        if (!this->mAttackButtonDownLastFrame)
        {
            this->PlaybackEvent(this->mStartEvent);
            this->mAttackButtonDownLastFrame = true;
        }
    		
        this->PlaybackEvent(this->mEvent, this->GetShootAnimation());
        this->SetAnimationAndSound();
	
        // If player is too close to a wall, don't actually fire the projectile
        if(this->GetIsGunPositionValid())
        {
            this->FireProjectiles();
        }
        else
        {
            #ifdef DEBUG
            #ifdef AVH_SERVER
            char theMessage[256];
            sprintf(theMessage, "Gun position is not valid, skipping call to FireProjectiles.\n");
            ALERT(at_console, theMessage);
            #endif
            #endif
        }

        this->DeductCostForShot();
        this->SetNextAttack();
        
    }
}

void AvHBasePlayerWeapon::Reload(void)
{
    // Move clip sounds out
	int theReloadAnimation = this->GetReloadAnimation();
	float theReloadTime = this->GetReloadTime();
	int theClipSize = this->GetClipSize();

	this->DefaultReload(theClipSize, theReloadAnimation, theReloadTime);

	// Don't idle for a bit
	this->SetNextIdle();
}

bool AvHBasePlayerWeapon::GetCanBeResupplied() const
{
    bool theCanBeResupplied = false;
    
    if(this->UsesAmmo())
    {
        ItemInfo theItemInfo;
        this->GetItemInfo(&theItemInfo);
        
        int theCurrentPrimary = this->m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];
        int theMaxPrimary = theItemInfo.iMaxAmmo1;
        
        // Add some to primary store
        if(theCurrentPrimary < theMaxPrimary)
        {
            theCanBeResupplied = true;
        }
    }
    
    return theCanBeResupplied;
}

bool AvHBasePlayerWeapon::Resupply()
{
	bool theResupplied = false;

    if(this->GetCanBeResupplied())
    {
        ItemInfo theItemInfo;
        this->GetItemInfo(&theItemInfo);

        // Get amount to add
        int theMaxClip = theItemInfo.iMaxClip;
        
        // Add half the clip each time, rounding up (roughly 3 seconds per clip ()
        int theAmountToAdd = max((theMaxClip+1)/2, 1);

        int theCurrentPrimary = this->m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];
        int theMaxPrimary = theItemInfo.iMaxAmmo1;

        // Add ammo
        this->m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = min(theCurrentPrimary + theAmountToAdd, theMaxPrimary);

        const float theDelay = 1.0f;
		//bugfix - don't let resupply shorten reload time
        this->m_pPlayer->m_flNextAttack = max(this->m_pPlayer->m_flNextAttack,UTIL_WeaponTimeBase() + theDelay);
        this->mTimeOfLastResupply = UTIL_WeaponTimeBase() + theDelay;
	}

	return theResupplied;
}

void AvHBasePlayerWeapon::SendWeaponAnim(int inAnimation, int skiplocal, int body)
{
	if(inAnimation >= 0)
	{
		this->m_pPlayer->pev->weaponanim = inAnimation;

		this->PlaybackEvent(this->mWeaponAnimationEvent, inAnimation);
	}
}

void AvHBasePlayerWeapon::SecondaryAttack(void)
{
}

void AvHBasePlayerWeapon::Spawn()
{
	CBasePlayerWeapon::Spawn();

	this->pev->solid = SOLID_BBOX;
	this->pev->movetype = MOVETYPE_TOSS;
	UTIL_SetOrigin(this->pev, this->pev->origin);
	UTIL_SetSize(this->pev, kMarineItemMinSize, kMarineItemMaxSize);
	
	this->pev->iuser3 = AVH_USER3_MARINEITEM;

	#ifdef AVH_SERVER
	if(!this->GetIsPersistent())
	{
		this->mLifetime = AvHSUGetWeaponStayTime();
	}
	#endif
}

bool AvHBasePlayerWeapon::GetEnabledState() const
{
	bool result=false;
#ifdef AVH_SERVER
	result= (this->m_iEnabled == 1);		
#else
	// puzl: 497 client now uses the enabled state in the appropriate WEAPON
	ItemInfo theItemInfo;
	this->GetItemInfo(&theItemInfo);
	WEAPON *pWeapon = gWR.GetWeapon( theItemInfo.iId );
	if ( pWeapon != 0 ) {
		result=(pWeapon->iEnabled == 1);
	}
#endif
	return result;
}

#ifdef AVH_SERVER
void AvHBasePlayerWeapon::VirtualMaterialize(void)
{
	int theLifetime = this->GetGroundLifetime();
	
	if(theLifetime >= 0)
	{
		SetThink(&AvHBasePlayerWeapon::DestroyItem);
		this->pev->nextthink = gpGlobals->time + theLifetime;
	}
}

int	AvHBasePlayerWeapon::GetGroundLifetime() const
{
	return this->mLifetime;
}

// -1 means never expire
void AvHBasePlayerWeapon::SetGroundLifetime(int inGroundLifetime)
{
	this->mLifetime = inGroundLifetime;
}

int	AvHBasePlayerWeapon::GetResourceCost() const
{
	return 0;
}

void AvHBasePlayerWeapon::VirtualDestroyItem(void)
{
	if(this->GetIsPersistent())
	{
		// Make this weapon invisible until map reset
		this->pev->effects |= EF_NODRAW;
		this->pev->solid = SOLID_NOT;
		this->pev->takedamage = DAMAGE_NO;
		SetThink(NULL);
	}
	else
	{
		CBasePlayerItem::VirtualDestroyItem();
	}
}

void AvHBasePlayerWeapon::ResetEntity()
{
	CBasePlayerWeapon::ResetEntity();

	this->pev->effects = 0;
	this->pev->solid = SOLID_BBOX;
	this->pev->movetype = MOVETYPE_TOSS;
	
	this->pev->takedamage = DAMAGE_YES;

	this->VirtualMaterialize();
}

void AvHBasePlayerWeapon::SetOverwatchState(bool inState)
{
	this->mInOverwatch = inState;
}

void AvHBasePlayerWeapon::UpdateInventoryEnabledState(int inNumActiveHives)
{
	// Process here
	int theEnabledState = 1;
				
	ItemInfo theItemInfo;
	if(this->GetItemInfo(&theItemInfo) != 0)
	{
		int theWeaponFlags = theItemInfo.iFlags;
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
		ASSERT(thePlayer);
	
		// If we don't have the hives required, or we're ensnared
		if	(/*thePlayer->GetIsTemporarilyInvulnerable() ||*/
			!thePlayer->GetIsAbleToAct() || 
			((inNumActiveHives < 1) && (theWeaponFlags & ONE_HIVE_REQUIRED)) ||
			((inNumActiveHives < 2) && (theWeaponFlags & TWO_HIVES_REQUIRED)) ||
			((inNumActiveHives < 3) && (theWeaponFlags & THREE_HIVES_REQUIRED)) ||
			(this->GetResourceCost() > thePlayer->GetResources(false)) )
		{
			// Disable it
			theEnabledState = 0;
		}
	}
	
	// puzl: 497 save the state for when we send the CurWeapon message
	this->m_iEnabled =  theEnabledState;
}

void AvHBasePlayerWeapon::KeyValue(KeyValueData* pkvd)
{
	// Any entity placed by the mapper is persistent
	this->SetPersistent();
	
	if(FStrEq(pkvd->szKeyName, "teamchoice"))
	{
		this->pev->team = (AvHTeamNumber)(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "angles"))
	{
		// TODO: Insert code here
		//pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "lifetime"))
	{
		this->mLifetime = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBasePlayerWeapon::KeyValue(pkvd);
	}
}

// 0 means never expire, -1 means no value was set so use default
int AvHBasePlayerWeapon::GetLifetime() const
{
	int theLifetime = this->mLifetime;
	
	if(theLifetime < 0)
	{
		theLifetime = AvHSUGetWeaponStayTime();
	}
	
	return theLifetime;
}

bool AvHBasePlayerWeapon::GetIsPersistent() const
{
	return this->mIsPersistent;
}

void AvHBasePlayerWeapon::SetPersistent()
{
	this->mIsPersistent = true;
}
#endif

void AvHBasePlayerWeapon::SetNextIdle(void)
{
	float theRandomIdle = UTIL_SharedRandomFloat(this->m_pPlayer->random_seed, 10, 15);

	this->m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + theRandomIdle;
	
	// Just added these next two lines 8/8/01 (trying to fix prediction wackiness)
	if(this->m_flTimeWeaponIdle < 0)
	{
		this->m_flTimeWeaponIdle = theRandomIdle;
	}
}

BOOL AvHBasePlayerWeapon::UseDecrement( void )
{ 
	// If we're predicting, return true
	return TRUE;
}


void AvHBasePlayerWeapon::WeaponIdle(void)
{
//#ifdef AVH_CLIENT
//	char sz[ 128 ];
//	sprintf(sz, "during idle, clip is %d\n", this->m_iClip);
//	gEngfuncs.pfnCenterPrint( sz );
//	
//	//gEngfuncs.Con_Printf("during idle, clip is %d\n", sz);
//#endif

	if(this->mAttackButtonDownLastFrame)
	{
		this->PlaybackEvent(this->mEndEvent);
		this->mAttackButtonDownLastFrame = false;
	}
	
	ResetEmptySound();

	this->m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if(this->m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
	{
		this->SendWeaponAnim(this->GetIdleAnimation());
			
		this->m_pPlayer->SetAnimation(PLAYER_IDLE);
			
		this->SetNextIdle();
	}
}

bool AvHBasePlayerWeapon::UsesAmmo(void) const
{
	return true;
}
