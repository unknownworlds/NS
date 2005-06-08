#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "func_break.h"
#include "decals.h"
#include "explode.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHPushableBuildable.h"
#include "mod/AvHSharedUtil.h"

char *AvHPushableBuildable :: m_soundNames[3] = { "debris/pushbox1.wav", "debris/pushbox2.wav", "debris/pushbox3.wav" };

AvHPushableBuildable::AvHPushableBuildable(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3) : AvHBaseBuildable(inTechID, inMessageID, inClassName, inUser3)
{
	this->m_lastSound = 0;
	this->m_maxSpeed = 0;
	this->m_soundTime = 0;
}

void AvHPushableBuildable::SetConstructionComplete()
{
	AvHBaseBuildable::SetConstructionComplete();

//	if ( pev->spawnflags & SF_PUSH_BREAKABLE )
//		AvHBaseBuildable::Spawn();
//	else
//		Precache( );

	//pev->movetype	= MOVETYPE_PUSHSTEP;
	this->pev->movetype = MOVETYPE_TOSS;
	pev->solid		= SOLID_BBOX;
	//SET_MODEL( ENT(pev), STRING(pev->model) );
	SET_MODEL( ENT(pev), this->GetModelName());

	Vector theMinSize, theMaxSize;
	AvHSHUGetSizeForTech(this->GetMessageID(), theMinSize, theMaxSize);
	UTIL_SetSize(this->pev, theMinSize, theMaxSize); 

	if ( pev->friction > 399 )
		pev->friction = 399;

	m_maxSpeed = 100;//400 - pev->friction;
	SetBits( pev->flags, FL_FLOAT );
	pev->friction = 0;
	
	pev->origin.z += 1;	// Pick up off of the floor
	UTIL_SetOrigin( pev, pev->origin );

	// Multiply by area of the box's cross-section (assume 1000 units^3 standard volume)
	pev->skin = ( pev->skin * (pev->maxs.x - pev->mins.x) * (pev->maxs.y - pev->mins.y) ) * 0.0005;
	m_soundTime = 0;
}

float AvHPushableBuildable::GetMaxSpeed(void)
{ 
	return m_maxSpeed; 
}

void AvHPushableBuildable :: Precache( void )
{
	AvHBaseBuildable::Precache();

	for ( int i = 0; i < 3; i++ )
	{
		PRECACHE_UNMODIFIED_SOUND( m_soundNames[i] );
	}

	//if ( pev->spawnflags & SF_PUSH_BREAKABLE )
	//	AvHBaseBuildable::Precache( );
}


void AvHPushableBuildable :: KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "size") )
	{
		int bbox = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;

		switch( bbox )
		{
		case 0:	// Point
			UTIL_SetSize(pev, Vector(-8, -8, -8), Vector(8, 8, 8));
			break;

		case 2: // Big Hull!?!?	!!!BUGBUG Figure out what this hull really is
			UTIL_SetSize(pev, VEC_DUCK_HULL_MIN*2, VEC_DUCK_HULL_MAX*2);
			break;

		case 3: // Player duck
			UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			break;

		default:
		case 1: // Player
			UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
			break;
		}

	}
	else if ( FStrEq(pkvd->szKeyName, "buoyancy") )
	{
		pev->skin = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseBuildable::KeyValue( pkvd );
	}
}


// Pull the func_pushable
void AvHPushableBuildable :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if(!this->GetIsBuilt())
	{
		AvHBaseBuildable::Use(pActivator, pCaller, useType, value);
	}
	else
	{
		if ( !pActivator || !pActivator->IsPlayer() )
		{
			if ( pev->spawnflags & SF_PUSH_BREAKABLE )
				this->AvHBaseBuildable::Use( pActivator, pCaller, useType, value );
			return;
		}
		
		if ( pActivator->pev->velocity != g_vecZero )
			Move( pActivator, 0 );
	}
}


void AvHPushableBuildable :: Touch( CBaseEntity *pOther )
{
	if ( FClassnameIs( pOther->pev, "worldspawn" ) )
		return;

	Move( pOther, 1 );
}


void AvHPushableBuildable :: Move( CBaseEntity *pOther, int push )
{
	entvars_t*	pevToucher = pOther->pev;
	int playerTouch = 0;

	// Is entity standing on this pushable ?
	if ( FBitSet(pevToucher->flags,FL_ONGROUND) && pevToucher->groundentity && VARS(pevToucher->groundentity) == pev )
	{
		// Only push if floating
		if ( pev->waterlevel > 0 )
			pev->velocity.z += pevToucher->velocity.z * 0.1;

		return;
	}


	if ( pOther->IsPlayer() )
	{
		if ( push && !(pevToucher->button & (IN_FORWARD|IN_USE)) )	// Don't push unless the player is pushing forward and NOT use (pull)
			return;
		playerTouch = 1;
	}

	float factor;

	if ( playerTouch )
	{
		if ( !(pevToucher->flags & FL_ONGROUND) )	// Don't push away from jumping/falling players unless in water
		{
			if ( pev->waterlevel < 1 )
				return;
			else 
				factor = 0.1;
		}
		else
			factor = 1;
	}
	else 
		factor = 0.25;

	// temporarily increase height to allow it over bumps, but only if it's not on the ground already
	if(FBitSet(this->pev->flags, FL_ONGROUND))
	{
		this->pev->origin.z += 10;
	}
	else
	{
		int a = 0;
	}

	pev->velocity.x += pevToucher->velocity.x * factor;
	pev->velocity.y += pevToucher->velocity.y * factor;

	float length = sqrt( pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y );
	if ( push && (length > GetMaxSpeed()) )
	{
		pev->velocity.x = (pev->velocity.x * GetMaxSpeed() / length );
		pev->velocity.y = (pev->velocity.y * GetMaxSpeed() / length );
	}
	if ( playerTouch )
	{
		pevToucher->velocity.x = pev->velocity.x;
		pevToucher->velocity.y = pev->velocity.y;
		if ( (gpGlobals->time - m_soundTime) > 0.7 )
		{
			m_soundTime = gpGlobals->time;
			if ( length > 0 && FBitSet(pev->flags,FL_ONGROUND) )
			{
				m_lastSound = RANDOM_LONG(0,2);
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound], 0.5, ATTN_NORM);
	//			SetThink( StopSound );
	//			pev->nextthink = pev->ltime + 0.1;
			}
			else
				STOP_SOUND( ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound] );
		}
	}
}

#if 0
void AvHPushableBuildable::StopSound( void )
{
	Vector dist = pev->oldorigin - pev->origin;
	if ( dist.Length() <= 0 )
		STOP_SOUND( ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound] );
}
#endif

