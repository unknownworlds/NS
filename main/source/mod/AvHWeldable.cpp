//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHWeldable.cpp $
// $Date: 2002/10/03 18:49:43 $
//
//-------------------------------------------------------------------------------
// $Log: AvHWeldable.cpp,v $
// Revision 1.2  2002/10/03 18:49:43  Flayra
// - Changes for welding order completion
//
// Revision 1.1  2002/05/23 02:32:39  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHWeldable.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHServerUtil.h"

LINK_ENTITY_TO_CLASS(keWeldable, AvHWeldable);

const float kPlayerTouchInterval = 1.0f;

AvHWeldable::AvHWeldable()
{
	// Reset "template" values
	this->mNonUpgradedMaxHealth = 100;
	this->mMaxHealth = this->mNonUpgradedMaxHealth;
	this->mThinkInterval = .1f;
	this->mMaterial = matWood;
	
	this->Init();
}

void AvHWeldable::AddChecksum(Checksum& inChecksum)
{
	AvHBaseEntity::AddChecksum(inChecksum);

	inChecksum.AddChecksum("AvHWeldable::mWelded", this->mWelded);
	inChecksum.AddFloatChecksum("AvHWeldable::mBuildTime", this->mBuildTime);
	inChecksum.AddFloatChecksum("AvHWeldable::mTimeBuilt", this->mTimeBuilt);
	inChecksum.AddFloatChecksum("AvHWeldable::mMaxHealth", this->mMaxHealth);
	inChecksum.AddChecksum("AvHWeldable::mUseState", this->mUseState);
}

void AvHWeldable::Init()
{
	// Reset "non-template" values
	this->mWelded = false;
	this->mWeldOpens = false;
	this->mUseState = false;
	this->mDestroyed = false;
	
	this->mTimeBuilt = 0;
	this->mTimeLastPlayerTouch = -1;
}


// Called by the welder when fired at a weldable
void AvHWeldable::AddBuildTime(float inTime)
{
	if(this->GetCanBeWelded())
	{
		this->mTimeBuilt += inTime;
		if(this->mTimeBuilt >= this->mBuildTime)
		{
			this->mWelded = true;
			this->TriggerFinished();
		}
		this->UpdateEntityState();
	}
}

bool AvHWeldable::GetCanBeWelded() const
{
	return (!this->mWelded && /*this->mUseState &&*/ !this->mDestroyed && ( this->mWeldOpens || (this->mTimeLastPlayerTouch == -1) || (gpGlobals->time > (this->mTimeLastPlayerTouch + kPlayerTouchInterval)  ) ) );
}

float AvHWeldable::GetNormalizedBuildPercentage() const
{
	bool theIsBuilding, theIsResearching;
	float thePercentageBuilt;
	AvHSHUGetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, theIsBuilding, theIsResearching, thePercentageBuilt);
	
	return thePercentageBuilt;	
}

bool AvHWeldable::GetIsWelded() const
{
	return this->mWelded;
}

bool AvHWeldable::GetWeldOpens() const
{
	return this->mWeldOpens;
}

void AvHWeldable::KeyValue( KeyValueData* pkvd )
{
	// "Health to destroy once welded (-1 infinite)" : "-1"
	if (FStrEq(pkvd->szKeyName, "weldableHealth"))
	{
		this->mNonUpgradedMaxHealth = atof(pkvd->szValue);
		this->mMaxHealth = this->mNonUpgradedMaxHealth;
		pkvd->fHandled = TRUE;
	}
	// "Seconds to weld" : "20"
	else if (FStrEq(pkvd->szKeyName, "weldableTime"))
	{
		this->mBuildTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	// "Build sounds filespec" : "sc/scv_build"
	else if (FStrEq(pkvd->szKeyName, "weldableSounds"))
	{
		pkvd->fHandled = TRUE;
	}
	// "Target to trigger on break" : ""
	else if (FStrEq(pkvd->szKeyName, "targetOnBreak"))
	{
		this->mTargetOnBreak = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "targetOnUse"))
	{
		this->mTargetOnUse = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	// "Target to trigger on finish" : ""
	else if (FStrEq(pkvd->szKeyName, "targetOnFinish"))
	{
		this->mTargetOnFinish = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "material"))
	{
		int i = atoi( pkvd->szValue);
		
		// 0:glass, 1:metal, 2:flesh, 3:wood
		
		if ((i < 0) || (i >= matLastMaterial))
			this->mMaterial = matWood;
		else
			this->mMaterial = (Materials)i;
		
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseEntity::KeyValue(pkvd);
	}
}

void AvHWeldable::Killed( entvars_t *pevAttacker, int iGib )
{
	AvHSUExplodeEntity(this, this->mMaterial);

	AvHBaseEntity::Killed(pevAttacker, iGib);

	this->TriggerBroken();
}

void AvHWeldable::NotifyUpgrade(AvHUpgradeMask inUpgradeMask)
{

}

void AvHWeldable::EndTrace(void)
{

}

void AvHWeldable::ResetEntity()
{
	this->Init();

	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model));
	
	this->SetPEVFlagsFromState();

	this->pev->iuser3 = AVH_USER3_WELD;
	AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, true, 0.0f);
	this->pev->health = this->mMaxHealth*kBaseHealthPercentage;

	this->UpdateEntityState();
}

void AvHWeldable::StartTrace(void)
{

}

void AvHWeldable::Precache(void)
{
	AvHBaseEntity::Precache();
	
	PRECACHE_MODEL( (char *)STRING(pev->model) );
	
	// Precache every type of func breakable
	//UTIL_PrecacheOther("func_breakable");
	CBreakable::PrecacheAll();
}

void AvHWeldable::SetHealth()
{
	float thePercentageBuilt = this->GetNormalizedBuildPercentage();
	if(this->mMaxHealth == -1)
	{
		this->pev->takedamage = DAMAGE_NO;
		this->pev->health = 100;
	}
	else
	{
		if(!this->mWeldOpens)
		{	
			this->pev->takedamage = DAMAGE_YES;
			this->pev->health = thePercentageBuilt*this->mMaxHealth;
		}
	}
}

void AvHWeldable::SetPEVFlagsFromState()
{
	// Set spawn flags
	if(this->pev->spawnflags & 1)
	{
		this->mUseState = true;
	}
	if(this->pev->spawnflags & 2)
	{
		this->mWeldOpens = true;
	}
	
	if(this->mWeldOpens)
	{
		this->pev->solid = SOLID_BSP;
		this->pev->movetype = MOVETYPE_PUSH;
	}
	else
	{
		this->pev->movetype = MOVETYPE_FLY;//MOVETYPE_PUSH;
		this->pev->solid = 5;
		//this->pev->solid = SOLID_NOT;
	}

	// Reset visuals
	this->pev->rendermode = kRenderNormal;
	this->pev->renderamt = 255;

	// TODO: Do this in combination with above to fix weldable collision and trace problems?
	//this->pev->flags |= FL_WORLDBRUSH;
}

void AvHWeldable::Spawn()
{
	Precache();

	AvHBaseEntity::Spawn();

	// Set model
    pev->classname = MAKE_STRING(kwsWeldableClassName);

	this->SetPEVFlagsFromState();

	// Set use so it can be toggled on and off like a switch, not used by the player 
	SetUse(&AvHWeldable::WeldableUse);
	SetTouch(&AvHWeldable::WeldableTouch);
}

void AvHWeldable::TriggerBroken()
{
	if(this->mTargetOnBreak != "")
	{
		FireTargets(this->mTargetOnBreak.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}
}

void AvHWeldable::TriggerFinished()
{
	if(this->mTargetOnFinish != "")
	{
		FireTargets(this->mTargetOnFinish.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}
}

void AvHWeldable::TriggerUse()
{
	if(this->mTargetOnUse != "")
	{
		FireTargets(this->mTargetOnUse.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}
}

void AvHWeldable::UpdateEntityState()
{
	float theBuiltPercentage = this->mTimeBuilt/this->mBuildTime;
	//this->pev->fuser1 = thePercentageBuilt*kNormalizationNetworkFactor;
	AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, true, theBuiltPercentage);
	this->pev->health = this->mMaxHealth*kBaseHealthPercentage + theBuiltPercentage*(1.0f - kBaseHealthPercentage);
	
	if(this->mWelded)
	{
		// Once built, toggle the solid state and allow it to take damage
		if(this->mWeldOpens)
		{
			this->pev->rendermode = kRenderTransTexture;
			this->pev->renderamt = 0;
			this->pev->solid = 5;

			AvHSUExplodeEntity(this, this->mMaterial);
		}
		else
		{
			this->pev->solid = SOLID_BSP;
			this->pev->movetype = MOVETYPE_PUSH;
			this->pev->rendermode = kRenderNormal;
		}

		// Clear progress bar indicators
		//this->pev->iuser3 = AVH_USER3_NONE;
		this->pev->fuser1 = -1;

		// Closeable welds can take damage
		this->SetHealth();
	}

	// Indicate that it can no longer be the target of a weld
	if(this->mWelded || this->mDestroyed)
	{
		//this->pev->fuser1 = -1;
		AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, true, -1);
	}
}

void AvHWeldable::WeldableTouch(CBaseEntity *pOther)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pOther);
	if(thePlayer && thePlayer->IsAlive())
	{
		this->mTimeLastPlayerTouch = gpGlobals->time;
	}
}

void AvHWeldable::WeldableUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Can't toggle it once it's been welded
	if(!this->mWelded)
	{
		switch(useType)
		{
		case USE_OFF:
			this->mUseState = false;
			break;
			
		case USE_ON:
			this->mUseState = true;
			break;
			
		case USE_SET:
			// Handle this?
			break;
			
		case USE_TOGGLE:
			if(!this->mWelded || this->mDestroyed)
			{
				this->mUseState = !this->mUseState;
				this->TriggerUse();
			}
			break;
		}
	}
}

