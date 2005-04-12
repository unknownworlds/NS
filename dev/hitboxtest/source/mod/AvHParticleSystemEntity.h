//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParticleSystemEntity.h $
// $Date: 2002/05/23 02:33:20 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleSystemEntity.h,v $
// Revision 1.9  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_PARTICLE_SYSTEM_ENTITY_H
#define AVH_PARTICLE_SYSTEM_ENTITY_H

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "types.h"
#include "mod/AvHParticleTemplate.h"
#include "mod/AvHConstants.h"

class AvHParticleSystemEntity : public CBaseEntity
{
public:
					AvHParticleSystemEntity();

	uint16			GetCustomData() const;
	void			SetCustomData(uint16 inCustomData);

	void			SetTemplateIndex(int inTemplateIndex);

	virtual void	KeyValue( KeyValueData* pkvd );
	virtual void	Precache(void);
	virtual void	SetUseState(USE_TYPE inUseType);
	virtual void	Spawn( void );
	
protected:
	AvHParticleTemplate*	GetCustomTemplate();
	
private:
	void EXPORT				ParticleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT				ParticleThink();
	void EXPORT				ParticleTouch( CBaseEntity *pOther );
	void					UpdateClientData();
	
	int32			mTemplateIndex;
	bool			mIsOn;
	//bool			mClientIsOn;
	bool			mUseState;
	bool			mCreatedTemplate;
	uint32			mHandle;
	float			mTimeParticlesCreated;
	uint16			mCustomData;

	static uint32	sCurrentHandle;
	
};


// Custom particle systems.  It is identical to a particle system entity except it creates
// a new particle template instead of reading one from a .ps.
class AvHCustomParticleSystemEntity : public AvHParticleSystemEntity
{
public:
	virtual void	KeyValue( KeyValueData* pkvd );
};

#endif