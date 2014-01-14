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
* Modified by Charles G. Cleveland
*
* $Workfile: ammohistory.h $
* $Date: 2002/06/25 17:05:31 $
*
*-------------------------------------------------------------------------------
* $Log: ammohistory.h,v $
* Revision 1.3  2002/06/25 17:05:31  Flayra
* - Weapon enabling/disabling works differently now
*
*===============================================================================
****/
//
// ammohistory.h
//

// this is the max number of items in each bucket
#define MAX_WEAPON_POSITIONS		10

class WeaponsResource
{
public:
	WeaponsResource( void );
	~WeaponsResource( void );
	void Init( void );
	void Reset( void );

	void LoadWeaponSprites( WEAPON* wp, int custom );
	void LoadAllWeaponSprites( void );

	WEAPON*	GetWeapon( int iId );
	WEAPON* GetWeaponSlot( int slot, int pos );
	WEAPON* GetFirstPos( int iSlot );
	WEAPON* GetNextActivePos( int iSlot, int iSlotPos );

	bool	IsEnabled( WEAPON *p );
	bool	IsSelectable( WEAPON *p );

	bool	HasAmmo( WEAPON *p );
	int		CountAmmo( int iId );
	int		GetAmmo( int iId );
	void	SetAmmo( int iId, int iCount );
	HSPRITE* GetAmmoPicFromWeapon( int iAmmoId, wrect_t& rect );	//TODO: fix bass-ackwards arrangement and store sprites with ammo types

	void	AddWeapon( WEAPON* wp );
	void	PickupWeapon( WEAPON* wp );
	void	DropWeapon( WEAPON* wp );
	void	DropAllWeapons( void );

	//CONSIDER: Should the selection functions be in the menu with the selection variables?
	void	UserCmd_LastInv( void );
	void	UserCmd_MovementOn( void );
	void	UserCmd_MovementOff( void );
	void	SetValidWeapon( void );
	void	SetCurrentWeapon( WEAPON* wp );
	void	SelectSlot( int iSlot, int fAdvance, int iDirection );

	friend CHudAmmo; //for iOldWeaponBits access
private:
	WEAPON	rgWeapons[MAX_WEAPONS];									// current weapon state
	WEAPON*	rgSlots[MAX_WEAPON_SLOTS][MAX_WEAPON_POSITIONS];		// current weapon slot map
	WEAPON*	lastWeapon;												// client-side lastinv

	int		riAmmo[MAX_AMMO_TYPES];									// current ammo counts
	int		iOldWeaponBits;
};

extern WeaponsResource gWR;


#define MAX_HISTORY 12
enum {
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};

class HistoryResource
{
private:
	struct HIST_ITEM {
		int type;
		float DisplayTime;  // the time at which this item should be removed from the history
		int iCount;
		int iId;
	};

	HIST_ITEM rgAmmoHistory[MAX_HISTORY];

public:

	void Init( void )
	{
		Reset();
	}

	void Reset( void )
	{
		memset( rgAmmoHistory, 0, sizeof rgAmmoHistory );
	}

	int iHistoryGap;
	int iCurrentHistorySlot;

	void AddToHistory( int iType, int iId, int iCount = 0 );
	void AddToHistory( int iType, const char *szName, int iCount = 0 );

	void CheckClearHistory( void );
	int DrawAmmoHistory( float flTime );
};

extern HistoryResource gHR;



