/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
*   Modified by Charlie Cleveland:
*
* $Workfile: hl_weapons.cpp $
* $Date: 2002/10/24 21:11:52 $
*
* -------------------------------------------------------------------------------
*  $Log: hl_weapons.cpp,v $
*  Revision 1.31  2002/10/24 21:11:52  Flayra
*  - Updated jetpack effect because it was really buggy this old way
*
*  Revision 1.30  2002/10/16 02:12:10  Flayra
*  - Valve anti-cheat integrated!
*
*  Revision 1.29  2002/10/16 00:36:40  Flayra
*  - Removed blink fail
*
*  Revision 1.28  2002/08/31 18:01:59  Flayra
*  - Work at VALVe
*
*  Revision 1.27  2002/07/08 16:09:03  Flayra
*  - Removed unneeded code here, so random numbers were generated properly on both client and server
*
*  Revision 1.26  2002/07/01 20:56:31  Flayra
*  - Added babbler gun
*
*  Revision 1.25  2002/06/25 17:03:01  Flayra
*  - Removed old weapons, added new weapons, fixed mines, iuser3 enables and disables weapons
*
* ===============================================================================
****/
#include "../hud.h"
#include "../cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "../demo.h"
#include "common/usercmd.h"
#include "common/event_flags.h"
#include "common/event_api.h"
#include "cl_dll/com_weapons.h"
#include "cl_dll/ammo.h"
#include "cl_dll/ammohistory.h"
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "mod/AvHEvents.h"

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

#include "usercmd.h"
#include "entity_state.h"
#include "demo_api.h"
#include "pm_defs.h"
#include "event_api.h"
#include "r_efx.h"

#include "../hud_iface.h"
#include "../com_weapons.h"

#include "mod/AvHMarineWeapons.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHAlienAbilities.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHMovementUtil.h"

#include "engine/APIProxy.h"
#include "cl_dll/Exports.h"

extern globalvars_t *gpGlobals;
extern bool gIsJetpacking;

// Pool of client side entities/entvars_t
static entvars_t	ev[ 32 ];
static int			num_ents = 0;

// The entity we'll use to represent the local client
static CBasePlayer	player;

// Local version of game .dll global variables ( time, etc. )
static globalvars_t	Globals; 

static CBasePlayerWeapon *g_pWpns[ 32 ];

vec3_t previousorigin;

// HLDM Weapon placeholder entities.
//CGlock				g_Glock;

// NS weapons
AvHKnife			gKnife;
AvHMachineGun		gMachineGun;
AvHPistol			gPistol;
AvHSonicGun			gSonicGun;
AvHHeavyMachineGun	gHeavyMachineGun;
AvHGrenadeGun		gGrenadeGun;
AvHGrenade          gGrenade;
AvHWelder			gWelder;
AvHMine				gMine;
AvHSpitGun			gSpitGun;
AvHClaws			gClaws;
AvHSpore			gSpores;
AvHBite				gBite;
AvHBite2			gBite2;
AvHSpikeGun			gSpikeGun;
AvHSwipe			gSwipe;
AvHWebSpinner		gWebSpinner;
AvHPrimalScream		gPrimalScream;
AvHParasiteGun		gParasite;
AvHUmbraGun			gUmbra;
AvHBlinkGun			gBlink;
AvHDivineWind		gDivineWind;
//AvHParalysisGun		gParalysisGun;
AvHBileBombGun		gBileBomb;
AvHAcidRocketGun	gAcidRocket;
AvHHealingSpray		gHealingSpray;
AvHMetabolize		gMetabolize;
AvHDevour			gDevour;
AvHStomp			gStomp;

// Alien abilities
AvHLeap				gLeap;
AvHCharge			gCharge;

// Jetpack events
int gJetpackEventID;
//int gWallJumpEventID;
//int gFlightEventID;
int gTeleportEventID;
int gPhaseInEventID;
int gSiegeHitEventID;
int gSiegeViewHitEventID;
int gCommanderPointsAwardedEventID;
int gBlinkEffectSuccessEventID;

//bool gPlayingJetpack = false;
//CGlock g_Glock;
//CCrowbar g_Crowbar;
//CPython g_Python;
//CMP5 g_Mp5;
//CCrossbow g_Crossbow;
//CShotgun g_Shotgun;
//CRpg g_Rpg;
//CGauss g_Gauss;
//CEgon g_Egon;
//CHgun g_HGun;
//CHandGrenade g_HandGren;
//CSatchel g_Satchel;
//CTripmine g_Tripmine;
//CSqueak g_Snark;

/*
======================
AlertMessage

Print debug messages to console
======================
*/
void AlertMessage( ALERT_TYPE atype, char *szFmt, ... )
{
	va_list		argptr;
	static char	string[1024];
	
	va_start (argptr, szFmt);
	vsprintf (string, szFmt,argptr);
	va_end (argptr);

	gEngfuncs.Con_Printf( "cl:  " );
	gEngfuncs.Con_Printf( string );
}

// Client-side effects for jetpack
void CheckJetpack()
{
	// if local player is jetpacking, play effects immediately
//	if(gIsJetpacking && !gPlayingJetpack)
//	{	
//		cl_entity_t* thePlayer;
//		thePlayer = gEngfuncs.GetLocalPlayer();
//		ASSERT(thePlayer);
//		
//		// Play event locally, server will tell everyone else to play event
//		gEngfuncs.pEventAPI->EV_PlaybackEvent(0, NULL, gJetpackEventID, 0, thePlayer->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
//		
//		//gPlayingJetpack = true;
//	}
	// Check to turn it off too (in case there's a network anomaly or the game resets or something, just trying to be safe)
	//else if(!gIsJetpacking && (gPlayingJetpack))
	//{
	//	gEngfuncs.pEventAPI->EV_PlaybackEvent(0, NULL, gEndJetpackEventID, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
	//	
	//	gPlayingJetpack = false;
	//}
}

//Returns if it's multiplayer.
//Mostly used by the client side weapons.
bool bIsMultiplayer ( void )
{
	return gEngfuncs.GetMaxClients() == 1 ? 0 : 1;
}
//Just loads a v_ model.
void LoadVModel ( char *szViewModel, CBasePlayer *m_pPlayer )
{
	gEngfuncs.CL_LoadModel( szViewModel, &m_pPlayer->pev->viewmodel );
}

/*
=====================
HUD_PrepEntity

Links the raw entity to an entvars_s holder.  If a player is passed in as the owner, then
we set up the m_pPlayer field.
=====================
*/
void HUD_PrepEntity( CBaseEntity *pEntity, CBasePlayer *pWeaponOwner )
{
	typedef	vector<int>	IDListType;
	static IDListType	sIDList;

	memset( &ev[ num_ents ], 0, sizeof( entvars_t ) );
	pEntity->pev = &ev[ num_ents++ ];

	pEntity->Precache();
	pEntity->Spawn();

	if ( pWeaponOwner )
	{
		ItemInfo info;
		
		((CBasePlayerWeapon *)pEntity)->m_pPlayer = pWeaponOwner;
		
		((CBasePlayerWeapon *)pEntity)->GetItemInfo( &info );

		// ASSERT that a weapon with this id isn't in the list
		int theNewID = info.iId;
		IDListType::iterator theIter = std::find(sIDList.begin(), sIDList.end(), theNewID);
		ASSERT(theIter == sIDList.end());

		// Insert id into our list
		sIDList.push_back(theNewID);

		g_pWpns[ theNewID ] = (CBasePlayerWeapon *)pEntity;
	}
}

/*
=====================
CBaseEntity :: Killed

If weapons code "kills" an entity, just set its effects to EF_NODRAW
=====================
*/
void CBaseEntity :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->effects |= EF_NODRAW;
}

/*
=====================
CBasePlayerWeapon :: DefaultReload
=====================
*/
BOOL CBasePlayerWeapon :: DefaultReload( int iClipSize, int iAnim, float fDelay, int body )
{

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return FALSE;

	int j = min(iClipSize - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);	

	if (j == 0)
		return FALSE;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

	//!!UNDONE -- reload sound goes here !!!
	//SendWeaponAnim( iAnim, UseDecrement(), body );

	m_fInReload = TRUE;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + kDeployIdleInterval;
	return TRUE;
}

/*
=====================
CBasePlayerWeapon :: CanDeploy
=====================
*/
BOOL CBasePlayerWeapon :: CanDeploy( void ) 
{
	BOOL bHasAmmo = 0;

	if ( !pszAmmo1() )
	{
		// this weapon doesn't use ammo, can always deploy.
		return TRUE;
	}

	if ( pszAmmo1() )
	{
		bHasAmmo |= (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0);
	}
	if ( pszAmmo2() )
	{
		bHasAmmo |= (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 0);
	}
	if (m_iClip > 0)
	{
		bHasAmmo |= 1;
	}
	if (!bHasAmmo)
	{
		return FALSE;
	}

	return TRUE;
}

/*
=====================
CBasePlayerWeapon :: DefaultDeploy

=====================
*/
BOOL CBasePlayerWeapon :: DefaultDeploy( char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal, int	body )
{
	if ( !CanDeploy() )
		return FALSE;

	gEngfuncs.CL_LoadModel( szViewModel, &m_pPlayer->pev->viewmodel );
	
	SendWeaponAnim( iAnim, skiplocal, body );

	m_pPlayer->m_flNextAttack = this->GetDeployTime();
	m_flTimeWeaponIdle = this->GetDeployTime() + kDeployIdleInterval;
	return TRUE;
}

/*
=====================
CBasePlayerWeapon :: PlayEmptySound

=====================
*/
BOOL CBasePlayerWeapon :: PlayEmptySound( void )
{
	if (m_iPlayEmptySound)
	{
		HUD_PlaySound( "weapons/357_cock1.wav", 0.8 );
		m_iPlayEmptySound = 0;
		return 0;
	}
	return 0;
}

/*
=====================
CBasePlayerWeapon :: ResetEmptySound

=====================
*/
void CBasePlayerWeapon :: ResetEmptySound( void )
{
	m_iPlayEmptySound = 1;
}

/*
=====================
CBasePlayerWeapon::Holster

Put away weapon
=====================
*/
void CBasePlayerWeapon::Holster( int skiplocal /* = 0 */ )
{ 
	m_fInReload = FALSE; // cancel any reload in progress.
	m_pPlayer->pev->viewmodel = 0; 
}

/*
=====================
CBasePlayerWeapon::SendWeaponAnim

Animate weapon model
=====================
*/
void CBasePlayerWeapon::SendWeaponAnim( int iAnim, int skiplocal, int body )
{
	m_pPlayer->pev->weaponanim = iAnim;
	
	HUD_SendWeaponAnim( iAnim, body, 0 );
}

/*
=====================
CBaseEntity::FireBulletsPlayer

Only produces random numbers to match the server ones.
=====================
*/
Vector CBaseEntity::FireBulletsPlayer ( ULONG cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq, int iDamage, entvars_t *pevAttacker, int shared_rand )
{
	//float x, y, z;
	Vector theShotDirection;

	theShotDirection.x = theShotDirection.y = theShotDirection.z = 0;
		
//	for ( ULONG iShot = 1; iShot <= cShots; iShot++ )
//	{
//		if ( pevAttacker == NULL )
//		{
//			// get circular gaussian spread
//			do {
//					x = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
//					y = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
//					z = x*x+y*y;
//			} while (z > 1);
//		}
//		else
//		{
//			//Use player's random seed.
//			// get circular gaussian spread
//			x = UTIL_SharedRandomFloat( shared_rand + iShot, -0.5, 0.5 ) + UTIL_SharedRandomFloat( shared_rand + ( 1 + iShot ) , -0.5, 0.5 );
//			y = UTIL_SharedRandomFloat( shared_rand + ( 2 + iShot ), -0.5, 0.5 ) + UTIL_SharedRandomFloat( shared_rand + ( 3 + iShot ), -0.5, 0.5 );
//			z = x * x + y * y;
//		}
//
//		UTIL_GetRandomSpreadDir(shared_rand, iShot, vecDirShooting)					
//	}

//    return Vector ( (float)(x * vecSpread.x), (float)(y * vecSpread.y), 0.0f );
	return theShotDirection;
}


bool GetCanUseWeapon()
{
    // This mirrors the functionality of AvHPlayer::GetCanUseWeapon.
    return !gHUD.GetIsInTopDownMode() && !gHUD.GetIsBeingDigested() && !gHUD.GetIsEnsnared() && !gHUD.GetIsStunned() && gEngfuncs.GetViewModel() != NULL;
}

/*
=====================
CBasePlayerWeapon::ItemPostFrame

Handles weapon firing, reloading, etc.
=====================
*/
void CBasePlayerWeapon::ItemPostFrame( void )
{

	if ((m_fInReload) && (m_pPlayer->m_flNextAttack <= 0.0))
	{
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Put code in here to predict reloads (ie, have the ammo on screen update before we get a response) //
///////////////////////////////////////////////////////////////////////////////////////////////////////
//#if 0 // FIXME, need ammo on client to make this work right
//		// complete the reload. 
//		int j = min( iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);	
//
//		// Add them to the clip
//		m_iClip += j;
//		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;
//#else	
//		m_iClip += 10;
//#endif
		m_fInReload = FALSE;
	}

	if ((m_pPlayer->pev->button & IN_ATTACK2) && (m_flNextSecondaryAttack <= 0.0))
	{
        if (GetCanUseWeapon())
        {
		    if ( pszAmmo2() && !m_pPlayer->m_rgAmmo[SecondaryAmmoIndex()] )
		    {
			    m_fFireOnEmpty = TRUE;
		    }

		    SecondaryAttack();
		    m_pPlayer->pev->button &= ~IN_ATTACK2;
        }
	}
    else if ( (m_pPlayer->pev->button & IN_ATTACK) && (m_flNextPrimaryAttack <= 0.0) )
	{
        if (GetCanUseWeapon())
        {
		    if ( (m_iClip == 0 && pszAmmo1()) || 
			    (iMaxClip() == -1 && !m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] ) )
		    {
			    m_fFireOnEmpty = TRUE;
		    }

		    //#ifdef AVH_CLIENT
		    //if((m_iClip == 0) && ?
		    //#endif

		    PrimaryAttack();
        }
	}
	else if ( m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != WEAPON_NOCLIP && !m_fInReload ) 
	{
        if (GetCanUseWeapon())
        {
    		// reload when reload is pressed, or if no buttons are down and weapon is empty.
	    	Reload();
        }
	}
	else if ( !(m_pPlayer->pev->button & (IN_ATTACK|IN_ATTACK2) ) )
	{
        if (GetCanUseWeapon())
        {

		    // no fire buttons down

		    m_fFireOnEmpty = FALSE;

		    // weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
		    if ( m_iClip == 0 && !(iFlags() & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < 0.0 )
		    {
			    // << CGC >> Only reload if we have more ammo to reload with
			    if(m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
			    {
				    Reload();
				    return;
			    }
		    }

		    WeaponIdle( );
        }
		return;
	}
	
	// catch all
	if ( ShouldWeaponIdle() )
	{
		WeaponIdle();
	}
}

/*
=====================
CBasePlayer::SelectItem

  Switch weapons
=====================
*/
void CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
		return;

	CBasePlayerItem *pItem = NULL;

	if (!pItem)
		return;

	
	if (pItem == m_pActiveItem)
		return;

	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pItem;

	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
	}
}

/*
=====================
CBasePlayer::SelectLastItem

=====================
*/
void CBasePlayer::SelectLastItem(void)
{
	if (!m_pLastItem)
	{
		return;
	}

	if ( m_pActiveItem && !m_pActiveItem->CanHolster() )
	{
		return;
	}

	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	CBasePlayerItem *pTemp = m_pActiveItem;
	m_pActiveItem = m_pLastItem;
	m_pLastItem = pTemp;
	m_pActiveItem->Deploy( );
}

/*
=====================
CBasePlayer::Killed

=====================
*/
void CBasePlayer::Killed( entvars_t *pevAttacker, int iGib )
{
	// Holster weapon immediately, to allow it to cleanup
	if ( m_pActiveItem )
		 m_pActiveItem->Holster( );
}

/*
=====================
CBasePlayer::Spawn

=====================
*/
void CBasePlayer::Spawn( void )
{
	if (m_pActiveItem)
		m_pActiveItem->Deploy( );
}

/*
=====================
UTIL_TraceLine

Don't actually trace, but act like the trace didn't hit anything.
=====================
*/
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	memset( ptr, 0, sizeof( *ptr ) );
	ptr->flFraction = 1.0;
}

/*
=====================
UTIL_ParticleBox

For debugging, draw a box around a player made out of particles
=====================
*/
void UTIL_ParticleBox( CBasePlayer *player, float *mins, float *maxs, float life, unsigned char r, unsigned char g, unsigned char b )
{
	int i;
	vec3_t mmin, mmax;

	for ( i = 0; i < 3; i++ )
	{
		mmin[ i ] = player->pev->origin[ i ] + mins[ i ];
		mmax[ i ] = player->pev->origin[ i ] + maxs[ i ];
	}

	gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&mmin, (float *)&mmax, 5.0, 0, 255, 0 );
}

/*
=====================
UTIL_ParticleBoxes

For debugging, draw boxes for other collidable players
=====================
*/
void UTIL_ParticleBoxes( void )
{
	int idx;
	physent_t *pe;
	cl_entity_t *player;
	vec3_t mins, maxs;
	
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	player = gEngfuncs.GetLocalPlayer();
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( player->index - 1 );	

	for ( idx = 1; idx < 100; idx++ )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( idx );
		if ( !pe )
			break;

		if ( pe->info >= 1 && pe->info <= gEngfuncs.GetMaxClients() )
		{
			mins = pe->origin + pe->mins;
			maxs = pe->origin + pe->maxs;

			gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&mins, (float *)&maxs, 0, 0, 255, 2.0 );
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

/*
=====================
UTIL_ParticleLine

For debugging, draw a line made out of particles
=====================
*/
void UTIL_ParticleLine( CBasePlayer *player, float *start, float *end, float life, unsigned char r, unsigned char g, unsigned char b )
{
	gEngfuncs.pEfxAPI->R_ParticleLine( start, end, r, g, b, life );
}

/*
=====================
CBasePlayerWeapon::PrintState

For debugging, print out state variables to log file
=====================
*/
void CBasePlayerWeapon::PrintState( void )
{
	COM_Log( "c:\\hl.log", "%.4f ", gpGlobals->time );
	COM_Log( "c:\\hl.log", "%.4f ", m_pPlayer->m_flNextAttack );
	COM_Log( "c:\\hl.log", "%.4f ", m_flNextPrimaryAttack );
	COM_Log( "c:\\hl.log", "%.4f ", m_flTimeWeaponIdle - gpGlobals->time);
	COM_Log( "c:\\hl.log", "%i ", m_iClip );
	COM_Log( "c:\\hl.log", "\r\n");
}

/*
=====================
HUD_InitClientWeapons

Set up weapons, player and functions needed to run weapons code client-side.
=====================
*/
void HUD_InitClientWeapons( void )
{
	static int initialized = 0;
	if ( initialized )
		return;

	initialized = 1;

	// Set up pointer ( dummy object )
	gpGlobals = &Globals;

	// Fill in current time ( probably not needed )
	gpGlobals->time = gEngfuncs.GetClientTime();

	// Fake functions
	g_engfuncs.pfnPrecacheModel		= stub_PrecacheModel;
	g_engfuncs.pfnPrecacheSound		= stub_PrecacheSound;
	g_engfuncs.pfnPrecacheEvent		= stub_PrecacheEvent;
	g_engfuncs.pfnNameForFunction	= stub_NameForFunction;
	g_engfuncs.pfnSetModel			= stub_SetModel;
	g_engfuncs.pfnSetClientMaxspeed = HUD_SetMaxSpeed;

	// Handled locally
	g_engfuncs.pfnPlaybackEvent		= HUD_PlaybackEvent;
	g_engfuncs.pfnAlertMessage		= AlertMessage;

	// Pass through to engine
	g_engfuncs.pfnPrecacheEvent		= gEngfuncs.pfnPrecacheEvent;
	g_engfuncs.pfnRandomFloat		= gEngfuncs.pfnRandomFloat;
	g_engfuncs.pfnRandomLong		= gEngfuncs.pfnRandomLong;

	// Allocate a slot for the local player
	HUD_PrepEntity( &player		, NULL );


	// Allocate slot(s) for each weapon that we are going to be predicting
	//HUD_PrepEntity( &g_Glock, &player );
	HUD_PrepEntity( &gKnife, &player);
	HUD_PrepEntity( &gMachineGun, &player);
	HUD_PrepEntity( &gPistol, &player);
	HUD_PrepEntity( &gSonicGun, &player);
	HUD_PrepEntity( &gHeavyMachineGun, &player);
	HUD_PrepEntity( &gGrenadeGun, &player);
    HUD_PrepEntity( &gGrenade, &player);
	HUD_PrepEntity( &gWelder, &player);
	HUD_PrepEntity( &gMine, &player);
	HUD_PrepEntity( &gSpitGun, &player);
	HUD_PrepEntity( &gClaws, &player);
	HUD_PrepEntity( &gSpores, &player);
	HUD_PrepEntity( &gSpikeGun, &player);
	HUD_PrepEntity( &gBite, &player);
	HUD_PrepEntity( &gBite2, &player);
	HUD_PrepEntity( &gSwipe, &player);
	HUD_PrepEntity( &gWebSpinner, &player);
	HUD_PrepEntity( &gPrimalScream, &player);
	//HUD_PrepEntity( &gParalysisGun, &player);

	HUD_PrepEntity( &gBlink, &player);
	HUD_PrepEntity( &gParasite, &player);
	HUD_PrepEntity( &gUmbra, &player);
	HUD_PrepEntity( &gDivineWind, &player);
	HUD_PrepEntity( &gBileBomb, &player);
	HUD_PrepEntity( &gAcidRocket, &player);
	HUD_PrepEntity( &gHealingSpray, &player);
	HUD_PrepEntity( &gMetabolize, &player);
	HUD_PrepEntity( &gStomp, &player);
	HUD_PrepEntity( &gDevour, &player);
	
	HUD_PrepEntity( &gLeap, &player);
	HUD_PrepEntity( &gCharge, &player);

	gJetpackEventID = PRECACHE_EVENT(1, kJetpackEvent);
	//gWallJumpEventID = PRECACHE_EVENT(1, kWallJumpEvent);
	//gFlightEventID = PRECACHE_EVENT(1, kFlightEvent);
	gTeleportEventID = PRECACHE_EVENT(1, kTeleportEvent);
	gPhaseInEventID = PRECACHE_EVENT(1, kPhaseInEvent);
	gSiegeHitEventID = PRECACHE_EVENT(1, kSiegeHitEvent);
	gSiegeViewHitEventID = PRECACHE_EVENT(1, kSiegeViewHitEvent);
	gCommanderPointsAwardedEventID = PRECACHE_EVENT(1, kCommanderPointsAwardedEvent);
	gBlinkEffectSuccessEventID = PRECACHE_EVENT(1, kBlinkEffectSuccessEventName);
}

/*
=====================
=======
	HUD_PrepEntity( &g_Glock	, &player );
	HUD_PrepEntity( &g_Crowbar	, &player );
	HUD_PrepEntity( &g_Python	, &player );
	HUD_PrepEntity( &g_Mp5	, &player );
	HUD_PrepEntity( &g_Crossbow	, &player );
	HUD_PrepEntity( &g_Shotgun	, &player );
	HUD_PrepEntity( &g_Rpg	, &player );
	HUD_PrepEntity( &g_Gauss	, &player );
	HUD_PrepEntity( &g_Egon	, &player );
	HUD_PrepEntity( &g_HGun	, &player );
	HUD_PrepEntity( &g_HandGren	, &player );
	HUD_PrepEntity( &g_Satchel	, &player );
	HUD_PrepEntity( &g_Tripmine	, &player );
	HUD_PrepEntity( &g_Snark	, &player );
}

/*
=====================
HUD_GetLastOrg

Retruns the last position that we stored for egon beam endpoint.
=====================
*/
void HUD_GetLastOrg( float *org )
{
	int i;
	
	// Return last origin
	for ( i = 0; i < 3; i++ )
	{
		org[i] = previousorigin[i];
	}
}

/*
=====================
HUD_SetLastOrg

Remember our exact predicted origin so we can draw the egon to the right position.
=====================
*/
void HUD_SetLastOrg( void )
{
	int i;
	
	// Offset final origin by view_offset
	for ( i = 0; i < 3; i++ )
	{
		previousorigin[i] = g_finalstate->playerstate.origin[i] + g_finalstate->client.view_ofs[ i ];
	}
}


CBasePlayerWeapon* HUD_GetWeaponForID(int inID)
{
	CBasePlayerWeapon* pWeapon = NULL;

	switch(inID)
	{
//	case WEAPON_GLOCK:
//		pWeapon = &g_Glock;
//		break;
	case AVH_WEAPON_KNIFE:
		pWeapon = &gKnife;
		break;
	case AVH_WEAPON_MG:
		pWeapon = &gMachineGun;
		break;
	case AVH_WEAPON_PISTOL:
		pWeapon = &gPistol;
		break;
	case AVH_WEAPON_SONIC:
		pWeapon = &gSonicGun;
		break;
	case AVH_WEAPON_HMG:
		pWeapon = &gHeavyMachineGun;
		break;
	case AVH_WEAPON_GRENADE_GUN:
		pWeapon = &gGrenadeGun;
		break;
    case AVH_WEAPON_GRENADE:
        pWeapon = &gGrenade;
        break;
	case AVH_WEAPON_WELDER:
		pWeapon = &gWelder;
		break;
	case AVH_WEAPON_MINE:
		pWeapon = &gMine;
		break;
	case AVH_WEAPON_SPIT:
		pWeapon = &gSpitGun;
		break;
	case AVH_WEAPON_CLAWS:
		pWeapon = &gClaws;
		break;
	case AVH_WEAPON_SPORES:
		pWeapon = &gSpores;
		break;
	case AVH_WEAPON_SPIKE:
		pWeapon = &gSpikeGun;
		break;
	case AVH_WEAPON_BITE:
		pWeapon = &gBite;
		break;
	case AVH_WEAPON_BITE2:
		pWeapon = &gBite2;
		break;
	case AVH_WEAPON_SWIPE:
		pWeapon = &gSwipe;
		break;
	case AVH_WEAPON_WEBSPINNER:
		pWeapon = &gWebSpinner;
		break;
	case AVH_WEAPON_PRIMALSCREAM:
		pWeapon = &gPrimalScream;
		break;
	case AVH_WEAPON_PARASITE:
		pWeapon = &gParasite;
		break;
	case AVH_WEAPON_UMBRA:
		pWeapon = &gUmbra;
		break;
	case AVH_WEAPON_BLINK:
		pWeapon = &gBlink;
		break;
	case AVH_WEAPON_DIVINEWIND:
		pWeapon = &gDivineWind;
		break;
//	case AVH_WEAPON_PARALYSIS:
//		pWeapon = &gParalysisGun;
//		break;
	case AVH_WEAPON_BILEBOMB:
		pWeapon = &gBileBomb;
		break;
	case AVH_WEAPON_ACIDROCKET:
		pWeapon = &gAcidRocket;
		break;
	case AVH_WEAPON_HEALINGSPRAY:
		pWeapon = &gHealingSpray;
		break;
	case AVH_WEAPON_METABOLIZE:
		pWeapon = &gMetabolize;
		break;
	case AVH_WEAPON_STOMP:
		pWeapon = &gStomp;
		break;
	case AVH_WEAPON_DEVOUR:
		pWeapon = &gDevour;
		break;

	// Abilities
	case AVH_ABILITY_LEAP:
		pWeapon = &gLeap;
		break;
	case AVH_ABILITY_CHARGE:
		pWeapon = &gCharge;
		break;
	}

	return pWeapon;
}


bool HUD_GetWeaponEnabled(int inID)
{
	ASSERT(inID >= 0);
	ASSERT(inID < 32);

	// puzl: 497 - use the enabled state in the associated WEAPON instead of the CBasePlayerWeapon's iuser3
	bool theWeaponEnabled = false;
	CBasePlayerWeapon* theWeapon = g_pWpns[inID];
	if(theWeapon)
	{
		ItemInfo theItemInfo;
		theWeapon->GetItemInfo(&theItemInfo);
		WEAPON *pWeapon = gWR.GetWeapon( theItemInfo.iId );
		if ( pWeapon != 0 ) {
			theWeaponEnabled = (pWeapon->iEnabled == 1);
		}
	}

	return theWeaponEnabled;
}

/*
=====================
HUD_WeaponsPostThink

Run Weapon firing code on client
=====================
*/
void HUD_WeaponsPostThink( local_state_s *from, local_state_s *to, usercmd_t *cmd, double time, unsigned int random_seed )
{
	int i;
	int buttonsChanged;
	CBasePlayerWeapon* pCurrent = NULL;
	weapon_data_t nulldata, *pfrom, *pto;
	static int lasthealth;

	memset( &nulldata, 0, sizeof( nulldata ) );

	HUD_InitClientWeapons();	

	// Get current clock
	gpGlobals->time = time;

	// Fill in data based on selected weapon
	// FIXME, make this a method in each weapon?  where you pass in an entity_state_t *?
	// Store pointer to our destination entity_state_t so we can get our origin, etc. from it
	CBasePlayerWeapon* pWeapon = HUD_GetWeaponForID(from->client.m_iId);

	//  for setting up events on the client
	g_finalstate = to;

	// If we are running events/etc. go ahead and see if we
	//  managed to die between last frame and this one
	// If so, run the appropriate player killed or spawn function
	if ( g_runfuncs )
	{
		if ( to->client.health <= 0 && lasthealth > 0 )
		{
			player.Killed( NULL, 0 );
			
		}
		else if ( to->client.health > 0 && lasthealth <= 0 )
		{
			player.Spawn();
		}

		lasthealth = to->client.health;
	}

	// We are not predicting the current weapon, just bow out here.
	if ( !pWeapon )
		return;

	for ( i = 0; i < 32; i++ )
	{
		pCurrent = g_pWpns[ i ];
		if ( !pCurrent )
		{
			continue;
		}

		pfrom = &from->weapondata[ i ];
		
		pCurrent->m_fInReload			= pfrom->m_fInReload;
		pCurrent->m_fInSpecialReload	= pfrom->m_fInSpecialReload;
//		pCurrent->m_flPumpTime			= pfrom->m_flPumpTime;
		pCurrent->m_iClip				= pfrom->m_iClip;
		pCurrent->m_flNextPrimaryAttack	= pfrom->m_flNextPrimaryAttack;
		pCurrent->m_flNextSecondaryAttack = pfrom->m_flNextSecondaryAttack;
		pCurrent->m_flTimeWeaponIdle	= pfrom->m_flTimeWeaponIdle;
		if(pWeapon && (pWeapon->m_iId == pfrom->m_iId))
		{
			// Predict clip
			gHUD.m_Ammo.SetCurrentClip(pfrom->m_iClip);

			AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(pWeapon);
			if(theWeapon)
			{
				gHUD.SetCurrentWeaponData(pWeapon->m_iId, theWeapon->GetEnabledState());
			}

			//gHUD.SetClientDebugCSP(pfrom, from->client.m_flNextAttack);
		}

		// Tell HUD what energy level is needed to use weapon, so alien HUD can indicate this
		float theEnergyLevel = 0.0f;
		AvHMUGetEnergyCost((AvHWeaponID)(pWeapon->m_iId), theEnergyLevel);
		gHUD.SetCurrentUseableEnergyLevel(theEnergyLevel);

// New SDK stuff...needed?
//		pCurrent->pev->fuser1			= pfrom->fuser1;
		pCurrent->m_flStartThrow		= pfrom->fuser2;
		pCurrent->m_flReleaseThrow		= pfrom->fuser3;
//		pCurrent->m_chargeReady			= pfrom->iuser1;
//		pCurrent->m_fInAttack			= pfrom->iuser2;
		pCurrent->pev->iuser3			= pfrom->iuser3;
	
//		pCurrent->m_iSecondaryAmmoType		= (int)from->client.vuser3[2];
		pCurrent->m_iPrimaryAmmoType		= (int)from->client.vuser4[0];
//		player.m_rgAmmo[ pCurrent->m_iPrimaryAmmoType ]	= (int)from->client.vuser4[1];
//		player.m_rgAmmo[ pCurrent->m_iSecondaryAmmoType ]	= (int)from->client.vuser4[2];
	}

	// For random weapon events, use this seed to seed random # generator
	player.random_seed = random_seed;

	// Get old buttons from previous state.
	player.m_afButtonLast = from->playerstate.oldbuttons;

	// Which buttsons chave changed
	buttonsChanged = (player.m_afButtonLast ^ cmd->buttons);	// These buttons have changed this frame
	
	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	player.m_afButtonPressed =  buttonsChanged & cmd->buttons;	
	// The ones not down are "released"
	player.m_afButtonReleased = buttonsChanged & (~cmd->buttons);

	// Set player variables that weapons code might check/alter
	player.pev->button = cmd->buttons;

	player.pev->velocity = from->client.velocity;
	player.pev->flags = from->client.flags;

	player.pev->deadflag = from->client.deadflag;
	player.pev->waterlevel = from->client.waterlevel;
	player.pev->maxspeed    = from->client.maxspeed;
	player.pev->fov = from->client.fov;
	player.pev->weaponanim = from->client.weaponanim;
	player.pev->viewmodel = from->client.viewmodel;
	player.m_flNextAttack = from->client.m_flNextAttack;
	//player.m_flNextAmmoBurn = from->client.fuser2;
	//player.m_flAmmoStartCharge = from->client.fuser3;

	// Removed this because NS uses vuser1 and vuser2 (and the HL weapons aren't used)
	////Stores all our ammo info, so the client side weapons can use them.
	//player.ammo_9mm			= (int)from->client.vuser1[0];
	//player.ammo_357			= (int)from->client.vuser1[1];
	//player.ammo_argrens		= (int)from->client.vuser1[2];
	//player.ammo_bolts		= (int)from->client.ammo_nails; //is an int anyways...
	//player.ammo_buckshot	= (int)from->client.ammo_shells; 
	//player.ammo_uranium		= (int)from->client.ammo_cells;
	//player.ammo_hornets		= (int)from->client.vuser2[0];
	//player.ammo_rockets		= (int)from->client.ammo_rockets;

	
	// Point to current weapon object
	if ( from->client.m_iId )
	{
		player.m_pActiveItem = g_pWpns[ from->client.m_iId ];
	}

	if ( player.m_pActiveItem->m_iId == WEAPON_RPG )
	{
		 ( ( CRpg * )player.m_pActiveItem)->m_fSpotActive = (int)from->client.vuser2[ 1 ];
		 ( ( CRpg * )player.m_pActiveItem)->m_cActiveRockets = (int)from->client.vuser2[ 2 ];
	}

    // Don't go firing anything if we have died.
	// Or if we don't have a weapon model deployed
	if ( ( player.pev->deadflag != ( DEAD_DISCARDBODY + 1 ) ) && !CL_IsDead() && player.pev->viewmodel && !g_iUser1 )
	{

		if ( player.m_flNextAttack <= 0 )
        {
			pWeapon->ItemPostFrame();
		}
//    	if ( g_runfuncs )
//		{
//			pWeapon->PrintState();
//		}
	}

	// Assume that we are not going to switch weapons
	to->client.m_iId					= from->client.m_iId;

	// Now see if we issued a changeweapon command ( and we're not dead )
	if ( cmd->weaponselect && ( player.pev->deadflag != ( DEAD_DISCARDBODY + 1 ) ) )
	{
		// Switched to a different weapon?
		if ( from->weapondata[ cmd->weaponselect ].m_iId == cmd->weaponselect )
		{
			ASSERT(cmd->weaponselect >= 0);
			ASSERT(cmd->weaponselect < 32);

			CBasePlayerWeapon *pNew = g_pWpns[ cmd->weaponselect ];
			if ( pNew && ( pNew != pWeapon ) && player.m_pActiveItem && player.m_pActiveItem->CanHolster())
			{
				// Put away old weapon
				if (player.m_pActiveItem)
					player.m_pActiveItem->Holster( );
				
				player.m_pLastItem = player.m_pActiveItem;
				player.m_pActiveItem = pNew;

				// Deploy new weapon
				if (player.m_pActiveItem)
				{
					player.m_pActiveItem->Deploy( );
				}

				// Update weapon id so we can predict things correctly.
				to->client.m_iId = cmd->weaponselect;
			}
		}
	}

	// Copy in results of prediction code
	to->client.viewmodel				= player.pev->viewmodel;
	to->client.fov						= player.pev->fov;
	to->client.weaponanim				= player.pev->weaponanim;
	to->client.m_flNextAttack			= player.m_flNextAttack;
	//to->client.fuser2					= player.m_flNextAmmoBurn;
	//to->client.fuser3					= player.m_flAmmoStartCharge;
	to->client.maxspeed					= player.pev->maxspeed;

// Removed this because NS uses vuser1 and vuser2 (and the HL weapons aren't used)
//	//HL Weapons
//	to->client.vuser1[0]				= player.ammo_9mm;
//	to->client.vuser1[1]				= player.ammo_357;
//	to->client.vuser1[2]				= player.ammo_argrens;
//
//	to->client.ammo_nails				= player.ammo_bolts;
//	to->client.ammo_shells				= player.ammo_buckshot;
//	to->client.ammo_cells				= player.ammo_uranium;
//	to->client.vuser2[0]				= player.ammo_hornets;
//	to->client.ammo_rockets				= player.ammo_rockets;

	if ( player.m_pActiveItem->m_iId == WEAPON_RPG )
	{
		 from->client.vuser2[ 1 ] = ( ( CRpg * )player.m_pActiveItem)->m_fSpotActive;
		 from->client.vuser2[ 2 ] = ( ( CRpg * )player.m_pActiveItem)->m_cActiveRockets;
	}

	// Make sure that weapon animation matches what the game .dll is telling us
	//  over the wire ( fixes some animation glitches )
	if ( g_runfuncs && ( HUD_GetWeaponAnim() != to->client.weaponanim ) )
	{
		int body = 2;

		//Pop the model to body 0.
		//if ( pWeapon == &g_Tripmine )
		//	body = 0;
		
		// Force a fixed anim down to viewmodel
		HUD_SendWeaponAnim( to->client.weaponanim, body, 1 );
	}

	for ( i = 0; i < 32; i++ )
	{
		pCurrent = g_pWpns[ i ];

		pto = &to->weapondata[ i ];

		if ( !pCurrent )
		{
			memset( pto, 0, sizeof( weapon_data_t ) );
			continue;
		}
	
		pto->m_fInReload				= pCurrent->m_fInReload;
		pto->m_fInSpecialReload			= pCurrent->m_fInSpecialReload;
//		pto->m_flPumpTime				= pCurrent->m_flPumpTime;
		pto->m_iClip					= pCurrent->m_iClip; 
		pto->m_flNextPrimaryAttack		= pCurrent->m_flNextPrimaryAttack;
		pto->m_flNextSecondaryAttack	= pCurrent->m_flNextSecondaryAttack;
		pto->m_flTimeWeaponIdle			= pCurrent->m_flTimeWeaponIdle;
//		pto->fuser1						= pCurrent->pev->fuser1;
//		pto->fuser2						= pCurrent->m_flStartThrow;
//		pto->fuser3						= pCurrent->m_flReleaseThrow;
//		pto->iuser1						= pCurrent->m_chargeReady;
//		pto->iuser2						= pCurrent->m_fInAttack;
		pto->iuser3						= pCurrent->pev->iuser3;

		// Decrement weapon counters, server does this at same time ( during post think, after doing everything else )
		pto->m_flNextReload				-= cmd->msec / 1000.0;
		pto->m_fNextAimBonus			-= cmd->msec / 1000.0;
		pto->m_flNextPrimaryAttack		-= cmd->msec / 1000.0;
		pto->m_flNextSecondaryAttack	-= cmd->msec / 1000.0;
		pto->m_flTimeWeaponIdle			-= cmd->msec / 1000.0;
		pto->fuser1						-= cmd->msec / 1000.0;

		to->client.vuser3[2]				= pCurrent->m_iSecondaryAmmoType;
		to->client.vuser4 = pCurrent->pev->vuser4;
//		to->client.vuser4[0]				= pCurrent->m_iPrimaryAmmoType;
//		to->client.vuser4[1]				= player.m_rgAmmo[ pCurrent->m_iPrimaryAmmoType ];
//		to->client.vuser4[2]				= player.m_rgAmmo[ pCurrent->m_iSecondaryAmmoType ];

/*		if ( pto->m_flPumpTime != -9999 )
		{
			pto->m_flPumpTime -= cmd->msec / 1000.0;
			if ( pto->m_flPumpTime < -0.001 )
				pto->m_flPumpTime = -0.001;
		}*/

		if ( pto->m_fNextAimBonus < -1.0 )
		{
			pto->m_fNextAimBonus = -1.0;
		}

		if ( pto->m_flNextPrimaryAttack < -1.0 )
		{
			pto->m_flNextPrimaryAttack = -1.0;
		}

		if ( pto->m_flNextSecondaryAttack < -0.001 )
		{
			pto->m_flNextSecondaryAttack = -0.001;
		}

		if ( pto->m_flTimeWeaponIdle < -0.001 )
		{
			pto->m_flTimeWeaponIdle = -0.001;
		}

		if ( pto->m_flNextReload < -0.001 )
		{
			pto->m_flNextReload = -0.001;
		}

		if ( pto->fuser1 < -0.001 )
		{
			pto->fuser1 = -0.001;
		}
	}

	// m_flNextAttack is now part of the weapons, but is part of the player instead
	to->client.m_flNextAttack -= cmd->msec / 1000.0;
	if ( to->client.m_flNextAttack < -0.001 )
	{
		to->client.m_flNextAttack = -0.001;
	}

	to->client.fuser2 -= cmd->msec / 1000.0;
	if ( to->client.fuser2 < -0.001 )
	{
		to->client.fuser2 = -0.001;
	}
	
	to->client.fuser3 -= cmd->msec / 1000.0;
	if ( to->client.fuser3 < -0.001 )
	{
		to->client.fuser3 = -0.001;
    }

	// Store off the last position from the predicted state.
	HUD_SetLastOrg();

	// Wipe it so we can't use it after this frame
	g_finalstate = NULL;
}


/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void CL_DLLEXPORT HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	RecClPostRunCmd(from, to, cmd, runfuncs, time, random_seed);

	g_runfuncs = runfuncs;

#if defined( CLIENT_WEAPONS )

//	if(from->client.m_iId != WEAPON_GLOCK)
//	{
//		int a = 0;
//	}

	if ( cl_lw && cl_lw->value )
	{
		HUD_WeaponsPostThink( from, to, cmd, time, random_seed );
	}
	else
#endif
	{
		to->client.fov = g_lastFOV;
	}

	// Check to see whether too play local jetpack effects
	if(runfuncs)
	{
		static sLastTime = 0;
		float theTimePassed = time - sLastTime;

		//CheckJetpack();
		
		//UpdateJetpackLights();

		sLastTime = time;
	}

	// All games can use FOV state
	g_lastFOV = to->client.fov;
}



