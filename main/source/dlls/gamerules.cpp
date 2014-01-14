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
****/
//=========================================================
// GameRules.cpp
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"skill.h"
#include	"game.h"
#include	"mod/AvHSpecials.h"
#include	"mod/AvHHulls.h"

DLL_GLOBAL CGameRules*	g_pGameRules = NULL;
extern DLL_GLOBAL BOOL	g_fGameOver;

int g_teamplay = 1;

//=========================================================
//=========================================================
BOOL CGameRules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	int iAmmoIndex;

	if ( pszAmmoName )
	{
		iAmmoIndex = pPlayer->GetAmmoIndex( pszAmmoName );

		if ( iAmmoIndex > -1 )
		{
			if ( pPlayer->AmmoInventory( iAmmoIndex ) < iMaxCarry )
			{
				// player has room for more of this type of ammo
				return TRUE;
			}
		}
	}

	return FALSE;
}

//=========================================================
//=========================================================
edict_t *CGameRules :: GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
//	edict_t *pentSpawnSpot = EntSelectSpawnPoint( pPlayer );
//
//	pPlayer->InitPlayerFromSpawn(pentSpawnSpot);
//	
//	return pentSpawnSpot;

	ASSERT(FALSE);

	return NULL;
}

//=========================================================
//=========================================================
BOOL CGameRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	// only living players can have items
	if ( pPlayer->pev->deadflag != DEAD_NO )
		return FALSE;

	if ( pWeapon->pszAmmo1() )
	{
		if ( !CanHaveAmmo( pPlayer, pWeapon->pszAmmo1(), pWeapon->iMaxAmmo1() ) )
		{
			// we can't carry anymore ammo for this gun. We can only 
			// have the gun if we aren't already carrying one of this type
			if ( pPlayer->HasPlayerItem( pWeapon ) )
			{
				return FALSE;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if ( pPlayer->HasPlayerItem( pWeapon ) )
		{
			return FALSE;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return TRUE;
}

//=========================================================
// load the SkillData struct with the proper values based on the skill level.
//=========================================================
void CGameRules::RefreshSkillData ( void )
{
}

//=========================================================
// instantiate the proper game rules object
//=========================================================
//CGameRules *InstallGameRules( void )
//{
//	SERVER_COMMAND( "exec game.cfg\n" );
//	SERVER_EXECUTE( );
//
//	if ( !gpGlobals->deathmatch )
//	{
//		// generic half-life
//		g_teamplay = 0;
//		return new CHalfLifeRules;
//	}
//	else
//	{
//		if ( teamplay.value > 0 )
//		{
//			// teamplay
//
//			g_teamplay = 1;
//			return new CHalfLifeTeamplay;
//		}
//		if ((int)gpGlobals->deathmatch == 1)
//		{
//			// vanilla deathmatch
//			g_teamplay = 0;
//			return new CHalfLifeMultiplay;
//		}
//		else
//		{
//			// vanilla deathmatch??
//			g_teamplay = 0;
//			return new CHalfLifeMultiplay;
//		}
//	}
//}



