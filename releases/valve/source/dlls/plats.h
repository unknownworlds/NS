#ifndef CBASEPLATTRAIN_H
#define CBASEPLATTRAIN_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "trains.h"
#include "saverestore.h"

#define SF_PLAT_TOGGLE		0x0001

class CBasePlatTrain : public CBaseToggle
{
public:
	virtual int	ObjectCaps( void ) { return CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void KeyValue( KeyValueData* pkvd);
	void Precache( void );

	// This is done to fix spawn flag collisions between this class and a derived class
	virtual BOOL IsTogglePlat( void ) { return (pev->spawnflags & SF_PLAT_TOGGLE) ? TRUE : FALSE; }

	virtual int	Save( CSave &save );
	virtual int	Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	BYTE	m_bMoveSnd;			// sound a plat makes while moving
	BYTE	m_bStopSnd;			// sound a plat makes when it stops
	float	m_volume;			// Sound volume
};

#endif