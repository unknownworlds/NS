//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Controls placement and order of all weapons
//
// $Workfile: AvHItemInfo.cpp $
// $Date: 2002/10/18 22:20:26 $
//
//-------------------------------------------------------------------------------
// $Log: AvHItemInfo.cpp,v $
// Revision 1.21  2002/10/18 22:20:26  Flayra
// - Don't auto pickup weapons so much
//
// Revision 1.20  2002/10/17 17:33:27  Flayra
// - Fixed missing bacterial spray weapon caused by secondary weapons switch
//
// Revision 1.19  2002/10/16 20:54:02  Flayra
// - Move secondary weapons to secondary slots
//
// Revision 1.18  2002/10/16 00:59:44  Flayra
// - Added concept of secondary weapons
//
// Revision 1.17  2002/10/03 18:56:21  Flayra
// - Swapped umbra and spores
//
// Revision 1.16  2002/08/16 02:38:59  Flayra
// - Swapped around umbra, blink and bilebomb
//
// Revision 1.15  2002/08/09 01:04:13  Flayra
// - Added concept of primary weapon
// - Removed autoreloading for weapons that don't use it, fixing bugs where knife slashes immediately when drawing it, and others
//
// Revision 1.14  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.13  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.12  2002/07/01 21:36:42  Flayra
// - Regular update (new weapons, new placement)
//
// Revision 1.11  2002/06/25 18:03:56  Flayra
// - Mark weapons with hive flags, deleted old weapons, added new weapons, fixed mines, changed weapon IDs
//
// Revision 1.10  2002/06/10 19:57:21  Flayra
// - Regular update (moved alien upgrade chambers around)
//
// Revision 1.9  2002/05/28 17:49:39  Flayra
// - Moved resource tower to second slot
//
// Revision 1.8  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineWeapons.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHMarineWeapons.h"
#include "mod/AvHMarineWeaponConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHAlienAbilities.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "util/MathUtil.h"

typedef enum
{
	AVH_FIRST_SLOT = 0,
	AVH_SECOND_SLOT = 1,
	AVH_THIRD_SLOT = 2,
	AVH_FOURTH_SLOT = 3,
	AVH_FIFTH_SLOT = 4
} AvHSlotType;

// IMPORTANT NOTE:
//   When moving weapons around, be sure to update AvHBasePlayerWeapon::GetAnimationExtension() also.
//
// AVH_FIRST_SLOT
//	1: Machine gun
//	2: Sonic
//	3: HMG
//	4: Bite
//	5: Claws
//	6: Swipe
//	7: Builder spit
//  8: GrenadeGun
//	9: Bite2 //

// AVH_SECOND_SLOT
//	0: Pistol
//	1: Builder bacterial spray
//	2: 
//	3: Parasite
//	4: BlinkGun
//	5: Devour
//	6: Spore
//	7: Spike
//	8: 
//	9: 

// AVH_THIRD_SLOT
//	0: None
//	1: Knife
//	2: Metabolize
//	3: BileBomb
//	4: Stomp
//	5: Leap
//	6: 
//	7: Umbra
//  8: 
//	9: 

// AVH_FOURTH_SLOT
//	0: Mine
//	1: Webspinner
//	2: Grenade
//	3: 
//	4: PrimalScream
//	5: DivineWind
//	6: Charge
//	7: Acid rocket
//	8: Welder

// AVH_FIFTH_SLOT
//	0: None

//order = special primary, primary, special secondary, secondary, tertiary (slot 4 except grenade), melee (slot3, not droppable), grenade (not droppable)
const int kGrenadeWeaponWeight = 1;
const int kMeleeWeaponWeight = 2;
const int kDefaultTertiaryWeaponWeight = 3;
const int kDefaultSecondaryWeaponWeight = 4;
const int kSpecialSecondaryWeaponWeight = 5;
const int kDefaultPrimaryWeaponWeight = 6;
const int kSpecialPrimaryWeaponWeight = 7;

int AvHKnife::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_THIRD_SLOT;
	p->iPosition = 1;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kKNDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	//p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iId = AVH_WEAPON_KNIFE;
	p->iWeight = kMeleeWeaponWeight;
	
	return 1;
}

int	AvHKnife::iItemSlot(void)
{
	return AVH_THIRD_SLOT + 1;
}


int AvHMine::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 0;

	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "MineAmmo";
	p->iMaxAmmo1 = 0;//BALANCE_VAR(kMineMaxAmmo);
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kMineDamage);
	p->iMaxClip = BALANCE_VAR(kMineMaxAmmo);
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY;//ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	p->iId = AVH_WEAPON_MINE;
	p->iWeight = kDefaultTertiaryWeaponWeight;

//	p->pszName = STRING(pev->classname);
//	p->pszAmmo1 = "MineAmmo";
//	p->iMaxAmmo1 = kMineMaxAmmo;
//	p->pszAmmo2 = NULL;
//	p->iMaxAmmo2 = -1;
//	p->iMaxClip = WEAPON_NOCLIP;
//	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
//	p->iId = m_iId = AVH_WEAPON_MINE;
//	p->iWeight = 5;
	
	return 1;
}

int	AvHMine::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}


int AvHWelder::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 8;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kWelderDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	//p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iId = AVH_WEAPON_WELDER;
	p->iWeight = kDefaultTertiaryWeaponWeight;
	
	return 1;
}

int	AvHWelder::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}



int AvHMachineGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 1;

	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "MGAmmo";
	p->iMaxAmmo1 = BALANCE_VAR(kMGMaxAmmo);
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kMGDamage);
	p->iMaxClip = BALANCE_VAR(kMGMaxClip);
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | PRIMARY_WEAPON;
	p->iId = AVH_WEAPON_MG;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHMachineGun::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}

int AvHPistol::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 0;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "HGAmmo";
	p->iMaxAmmo1 = BALANCE_VAR(kHGMaxAmmo);
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kHGDamage);
	p->iMaxClip = BALANCE_VAR(kHGMaxClip);
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | SECONDARY_WEAPON;
	p->iId = AVH_WEAPON_PISTOL;
	p->iWeight = kDefaultSecondaryWeaponWeight;
	
	return 1;
}

int	AvHPistol::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}

int AvHSonicGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 2;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "SonicAmmo";
	p->iMaxAmmo1 = BALANCE_VAR(kSGMaxAmmo);
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kSGDamage);
	p->iMaxClip = BALANCE_VAR(kSGMaxClip);
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | PRIMARY_WEAPON;
	p->iId = AVH_WEAPON_SONIC;
	p->iWeight = kSpecialPrimaryWeaponWeight;
	
	return 1;
}

int	AvHSonicGun::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}


int AvHHeavyMachineGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 3;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "HMGAmmo";
	p->iMaxAmmo1 = BALANCE_VAR(kHMGMaxAmmo);
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kHMGDamage);
	p->iMaxClip = BALANCE_VAR(kHMGMaxClip);
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | PRIMARY_WEAPON;
	p->iId = AVH_WEAPON_HMG;
	p->iWeight = kSpecialPrimaryWeaponWeight;
	
	return 1;
}

int	AvHHeavyMachineGun::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}


int AvHGrenadeGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 8;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "GrenadeAmmo";
	p->iMaxAmmo1 = BALANCE_VAR(kGGMaxAmmo);
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kGrenadeDamage);
 	p->iMaxClip = BALANCE_VAR(kGGMaxClip);
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | PRIMARY_WEAPON;
	p->iId = AVH_WEAPON_GRENADE_GUN;
	p->iWeight = kSpecialPrimaryWeaponWeight;
	
	return 1;
}

int	AvHGrenadeGun::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}


int AvHHealingSpray::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 1;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kHealingSprayDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED;
	p->iId = AVH_WEAPON_HEALINGSPRAY;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHHealingSpray::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}





///////////////////
// Alien weapons //
///////////////////

int AvHSpikeGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 7;

	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;//"SpikeAmmo";
	p->iMaxAmmo1 = -1;//kSpikeMaxClip;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kSpikeDamage);
	p->iMaxClip = WEAPON_NOCLIP;//kSpikeMaxClip;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iId = AVH_WEAPON_SPIKE;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHSpikeGun::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}


int AvHUmbraGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_THIRD_SLOT;
	p->iPosition = 7;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;//kSpikeMaxClip;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kUmbraCloudDuration);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_UMBRA;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHUmbraGun::iItemSlot(void)
{
	return AVH_THIRD_SLOT + 1;
}


//int AvHBabblerGun::GetItemInfo(ItemInfo *p) const
//{
//	p->iSlot = AVH_FOURTH_SLOT;
//	p->iPosition = 3;
//
//	p->pszName = STRING(pev->classname);
//	p->pszAmmo1 = "BabblerAmmo";
//	p->iMaxAmmo1 = -1;
//	p->pszAmmo2 = NULL;
//	p->iMaxAmmo2 = -1;
//	p->iMaxClip = WEAPON_NOCLIP;
//	p->iId = AVH_WEAPON_BABBLER;
//	p->iWeight = kDefaultPrimaryWeaponWeight;
//	p->iFlags =  ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED | THREE_HIVES_REQUIRED;
//
//	return 1;
//}
//
//
//int	AvHBabblerGun::iItemSlot(void)
//{
//	return AVH_FOURTH_SLOT + 1;
//}


int AvHBileBombGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_THIRD_SLOT;
	p->iPosition = 3;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;//kSpikeMaxClip;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kBileBombDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_BILEBOMB;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHBileBombGun::iItemSlot(void)
{
	return AVH_THIRD_SLOT + 1;
}



int	AvHSwipe::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 6;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kSwipeDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iId = AVH_WEAPON_SWIPE;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHSwipe::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}




int	AvHClaws::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 5;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kClawsDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iId = AVH_WEAPON_CLAWS;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHClaws::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}



int AvHBite::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 4;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kBiteDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iId = AVH_WEAPON_BITE;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHBite::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}


int AvHBite2::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 9;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kBite2Damage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iId = AVH_WEAPON_BITE2;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHBite2::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}


//int	AvHResourceTowerGun::GetItemInfo(ItemInfo *p) const
//{
//	p->iSlot = AVH_FIRST_SLOT;
//	p->iPosition = 7;
//	
//	p->pszName = STRING(pev->classname);
//	p->pszAmmo1 = NULL;
//	p->iMaxAmmo1 = -1;
//	p->pszAmmo2 = NULL;
//	p->iMaxAmmo2 = -1;
//	p->iMaxClip = WEAPON_NOCLIP;
//	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY + ITEM_FLAG_NOAUTORELOAD + ITEM_FLAG_SELECTONEMPTY;
//	p->iId = AVH_WEAPON_RESOURCE_TOWER;
//	p->iWeight = 7;
//	
//	return 1;
//}
//
//int	AvHResourceTowerGun::iItemSlot(void)
//{
//	return AVH_FIRST_SLOT + 1;
//}



int AvHSpitGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FIRST_SLOT;
	p->iPosition = 7;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kSpitGDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY;
	p->iId = AVH_WEAPON_SPIT;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHSpitGun::iItemSlot(void)
{
	return AVH_FIRST_SLOT + 1;
}




int	AvHSpore::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 6;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = (int)(BALANCE_VAR(kSporeDamage)/BALANCE_VAR(kSporeCloudThinkInterval));
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED;
	p->iId = AVH_WEAPON_SPORES;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHSpore::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}


int	AvHParasiteGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 3;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kParasiteDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED;
	p->iId = AVH_WEAPON_PARASITE;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHParasiteGun::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}



int	AvHGrenade::GetItemInfo(ItemInfo *p) const
{
    p->iSlot = AVH_FOURTH_SLOT;
    p->iPosition = 2;

    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "HandGrenadeAmmo";
    p->iMaxAmmo1 = BALANCE_VAR(kHandGrenadeMaxAmmo);
    p->pszAmmo2 = NULL;
    p->iMaxAmmo2 = BALANCE_VAR(kHandGrenadeDamage);
    p->iMaxClip = 1;
    p->iFlags = ITEM_FLAG_EXHAUSTIBLE;
    p->iId = AVH_WEAPON_GRENADE;
    p->iWeight = kGrenadeWeaponWeight;
    
    return 1;
}

int	AvHGrenade::iItemSlot(void)
{
    return AVH_FOURTH_SLOT + 1;
}


int	AvHWebSpinner::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 1;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED | THREE_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_WEBSPINNER;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHWebSpinner::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}

int AvHLeap::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_THIRD_SLOT;
	p->iPosition = 5;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kLeapDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED;
	p->iId = AVH_ABILITY_LEAP;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHLeap::iItemSlot(void)
{
	return AVH_THIRD_SLOT + 1;
}


int AvHPrimalScream::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 4;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "";
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kPrimalScreamRange);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED | THREE_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_PRIMALSCREAM;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHPrimalScream::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}

int	AvHMetabolize::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_THIRD_SLOT;
	p->iPosition = 2;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_METABOLIZE;
	p->iWeight = 7;
	
	return 1;
}

int	AvHMetabolize::iItemSlot(void)
{
	return AVH_THIRD_SLOT + 1;
}



//int	AvHParalysisGun::GetItemInfo(ItemInfo *p)
//{
//	p->iSlot = AVH_SECOND_SLOT;
//	p->iPosition = 5;
//	
//	p->pszName = STRING(pev->classname);
//	p->pszAmmo1 = NULL;
//	p->iMaxAmmo1 = -1;
//	p->pszAmmo2 = NULL;
//	p->iMaxAmmo2 = -1;
//	p->iMaxClip = WEAPON_NOCLIP;
//	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED;
//	p->iId = AVH_WEAPON_PARALYSIS;
//	p->iWeight = 7;
//	
//	return 1;
//}
//
//int	AvHParalysisGun::iItemSlot(void)
//{
//	return AVH_SECOND_SLOT + 1;
//}

int	AvHAcidRocketGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 7;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kAcidRocketDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED | THREE_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_ACIDROCKET;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHAcidRocketGun::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}



int	AvHBlinkGun::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 4;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED;
	p->iId = AVH_WEAPON_BLINK;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHBlinkGun::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}


int	AvHDivineWind::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 5;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kDivineWindDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ITEM_FLAG_SELECTONEMPTY | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED | THREE_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_DIVINEWIND;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHDivineWind::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}



int AvHCharge::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_FOURTH_SLOT;
	p->iPosition = 6;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kChargeDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED | THREE_HIVES_REQUIRED;
	p->iId = AVH_ABILITY_CHARGE;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHCharge::iItemSlot(void)
{
	return AVH_FOURTH_SLOT + 1;
}

int AvHStomp::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_THIRD_SLOT;
	p->iPosition = 4;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = (int)BALANCE_VAR(kStompTime);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED | TWO_HIVES_REQUIRED;
	p->iId = AVH_WEAPON_STOMP;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHStomp::iItemSlot(void)
{
	return AVH_THIRD_SLOT + 1;
}


int AvHDevour::GetItemInfo(ItemInfo *p) const
{
	p->iSlot = AVH_SECOND_SLOT;
	p->iPosition = 5;
	
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = BALANCE_VAR(kDevourDamage);
	p->iMaxClip = WEAPON_NOCLIP;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD | ONE_HIVE_REQUIRED;
	p->iId = AVH_WEAPON_DEVOUR;
	p->iWeight = kDefaultPrimaryWeaponWeight;
	
	return 1;
}

int	AvHDevour::iItemSlot(void)
{
	return AVH_SECOND_SLOT + 1;
}




