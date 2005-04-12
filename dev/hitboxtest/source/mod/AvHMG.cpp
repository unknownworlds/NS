/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
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

#include "util/nowarnings.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "textrep/TRFactory.h"
#include "mod/AvHWeaponFactory.h"

AvHWeaponFactory gWeaponFactory;

enum avhmg_e
{
	MG_LONGIDLE = 0,
	MG_IDLE1,
	MG_LAUNCH,
	MG_RELOAD,
	MG_DEPLOY,
	MG_FIRE1,
	MG_FIRE2,
	MG_FIRE3,
};


class MG : public AvHBasePlayerWeapon
{
public:
    MG();
//	void Spawn( void );
//	void Precache( void );
//	int iItemSlot( void ) { return 3; }
//	int GetItemInfo(ItemInfo *p);
//	int AddToPlayer( CBasePlayer *pPlayer );
//
//	void PrimaryAttack( void );
//	void SecondaryAttack( void );
//	int SecondaryAmmoIndex( void );
//	BOOL Deploy( void );
//	void Reload( void );
//	void WeaponIdle( void );
//	float m_flNextAnimTime;
//	int m_iShell;
//private:
//	unsigned short m_usMP5;
};
//LINK_ENTITY_TO_CLASS( weapon_mp5, MG );
LINK_ENTITY_TO_CLASS( MachineGun, MG );


//=========================================================
//=========================================================
//int MG::SecondaryAmmoIndex( void )
//{
//	return m_iSecondaryAmmoType;
//}

MG::MG() : AvHBasePlayerWeapon("MachineGun")
{
}
//
//void MG::Spawn( )
//{
//	pev->classname = MAKE_STRING("MachineGun"); // hack to allow for old names
//	Precache( );
//	SET_MODEL(ENT(pev), "models/w_9mmAR.mdl");
//	m_iId = WEAPON_MP5;
//
//	m_iDefaultAmmo = MG_DEFAULT_GIVE;
//
//	FallInit();// get ready to fall down.
//}
//
//
//void MG::Precache( void )
//{
//    // TODO: Put this next line back in ASAP
//    AvHBasePlayerWeapon::Precache();
//
//    //m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shellTE_MODEL
//
//	PRECACHE_MODEL("models/grenade.mdl");	// grenade
//
//	PRECACHE_MODEL("models/w_9mmARclip.mdl");
//	PRECACHE_SOUND("items/9mmclip1.wav");              
//
//	PRECACHE_SOUND("items/clipinsert1.wav");
//	PRECACHE_SOUND("items/cliprelease1.wav");
//    //	PRECACHE_SOUND("items/guncock1.wav");
//}

//int MG::GetItemInfo(ItemInfo *p)
//{
//	p->pszName = STRING(pev->classname);
//	p->pszAmmo1 = "9mm";
//	p->iMaxAmmo1 = _9MM_MAX_CARRY;
//	p->pszAmmo2 = "ARgrenades";
//	p->iMaxAmmo2 = M203_GRENADE_MAX_CARRY;
//	p->iMaxClip = MG_MAX_CLIP;
//	p->iSlot = 2;
//	p->iPosition = 0;
//	p->iFlags = 0;
//	p->iId = m_iId = WEAPON_MP5;
//	p->iWeight = MG_WEIGHT;
//
//	return 1;
//}
//
//int MG::AddToPlayer( CBasePlayer *pPlayer )
//{
//	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
//	{
//		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
//			WRITE_BYTE( m_iId );
//		MESSAGE_END();
//		return TRUE;
//	}
//	return FALSE;
//}
//
//BOOL MG::Deploy( )
//{
//	return DefaultDeploy( "models/v_9mmAR.mdl", "models/p_9mmAR.mdl", MG_DEPLOY, "mp5" );
//}
//
//
//void MG::PrimaryAttack()
//{
//	// don't fire underwater
//	if (m_pPlayer->pev->waterlevel == 3)
//	{
//		PlayEmptySound( );
//		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
//		return;
//	}
//
//    // CGC:
//	//if (m_iClip <= 0)
//	//{
//	//	PlayEmptySound();
//	//	m_flNextPrimaryAttack = gpGlobals->time + 0.15;
//	//	return;
//	//}
//
//	PLAYBACK_EVENT( 0, m_pPlayer->edict(), m_usMP5 );
//
//	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
//	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
//
//    // CGC:
//	//m_iClip--;
//
//	// player "shoot" animation
//	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
//
//	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
//	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
//	
//	if ( g_pGameRules->IsDeathmatch() )
//	{
//		// optimized multiplayer. Widened to make it easier to hit a moving player
//		m_pPlayer->FireBullets( 1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_MP5, 2 );
//	}
//	else
//	{
//		// single player spread
//		m_pPlayer->FireBullets( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 2 );
//	}
//
//	// CGC
//    //if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
//	//	// HEV suit - indicate out of ammo condition
//	//	m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
//
//	m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.1;
//	if (m_flNextPrimaryAttack < gpGlobals->time)
//		m_flNextPrimaryAttack = gpGlobals->time + 0.1;
//
//	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
//}
//
//
//
//void MG::SecondaryAttack( void )
//{
//	// don't fire underwater
//	if (m_pPlayer->pev->waterlevel == 3)
//	{
//		PlayEmptySound( );
//		m_flNextPrimaryAttack = gpGlobals->time + 0.15;
//		return;
//	}
//
//	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] == 0)
//	{
//		PlayEmptySound( );
//		return;
//	}
//
//	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
//	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
//
//	m_pPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
//	m_pPlayer->m_flStopExtraSoundTime = gpGlobals->time + 0.2;
//			
//	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
//
//	SendWeaponAnim( MG_LAUNCH );
//
//	// player "shoot" animation
//	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
//
//	if ( RANDOM_LONG(0,1) )
//	{
//		// play this sound through BODY channel so we can hear it if player didn't stop firing MP3
//		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher.wav", 0.8, ATTN_NORM);
//	}
//	else
//	{
//		// play this sound through BODY channel so we can hear it if player didn't stop firing MP3
//		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glauncher2.wav", 0.8, ATTN_NORM);
//	}
// 
//	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
//
//	// we don't add in player velocity anymore.
//	CGrenade::ShootContact( m_pPlayer->pev, 
//							m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, 
//							gpGlobals->v_forward * 800 );
//	
//	m_flNextPrimaryAttack = gpGlobals->time + 1;
//	m_flNextSecondaryAttack = gpGlobals->time + 1;
//	m_flTimeWeaponIdle = gpGlobals->time + 5;// idle pretty soon after shooting.
//
//	if (!m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType])
//		// HEV suit - indicate out of ammo condition
//		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
//
//	m_pPlayer->pev->punchangle.x -= 10;
//}
//
//void MG::Reload( void )
//{
//	DefaultReload( MG_MAX_CLIP, MG_RELOAD, 1.5 );
//}
//
//
//
//void MG::WeaponIdle( void )
//{
//	ResetEmptySound( );
//
//	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
//
//	if (m_flTimeWeaponIdle > gpGlobals->time)
//		return;
//
//	int iAnim;
//	switch ( RANDOM_LONG( 0, 1 ) )
//	{
//	case 0:	
//		iAnim = MG_LONGIDLE;	
//		break;
//	
//	default:
//	case 1:
//		iAnim = MG_IDLE1;
//		break;
//	}
//
//	SendWeaponAnim( iAnim );
//
//	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );// how long till we do this again.
//}



class MGAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_9mmARclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_9mmARclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_MP5CLIP_GIVE, "9mm", _9MM_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp5clip, MGAmmoClip );
LINK_ENTITY_TO_CLASS( ammo_9mmAR, MGAmmoClip );



class MGChainammo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_chainammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_chainammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_CHAINBOX_GIVE, "9mm", _9MM_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_9mmbox, MGChainammo );


class MGAmmoGrenade : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_ARgrenade.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_ARgrenade.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( AMMO_M203BOX_GIVE, "ARgrenades", M203_GRENADE_MAX_CARRY ) != -1);

		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp5grenades, MGAmmoGrenade );
LINK_ENTITY_TO_CLASS( ammo_ARgrenades, MGAmmoGrenade );


















