#ifndef CTRIPMINE_H
#define CTRIPMINE_H

#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHMarineEquipmentConstants.h"

////class CTripmine : public CBasePlayerWeapon
//class CTripmine : public AvHBasePlayerWeapon
//{
//public:
//
//	int	GetBarrelLength() const
//	{
//		return kMineBarrellLength;
//	}
//
//	void Spawn( void );
//	void Precache( void );
//	int iItemSlot( void ) { return 5; }
//	int GetItemInfo(ItemInfo *p);
//	void SetObjectCollisionBox( void )
//	{
//		//!!!BUGBUG - fix the model!
//		pev->absmin = pev->origin + Vector(-16, -16, -5);
//		pev->absmax = pev->origin + Vector(16, 16, 28); 
//	}
//
//	void PrimaryAttack( void );
//	BOOL Deploy( void );
//	void Holster( int skiplocal = 0 );
//	void WeaponIdle( void );
//
//	virtual BOOL UseDecrement( void )
//	{ 
//#if defined( CLIENT_WEAPONS )
//		return TRUE;
//#else
//		return FALSE;
//#endif
//	}
//
//private:
//	unsigned short m_usTripFire;
//
//};

#endif