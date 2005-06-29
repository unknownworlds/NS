#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

#include "mod/AvHAlienWeapons.h"
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
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHConstants.h"
#include "mod/AvHHulls.h"

// Allow assignment within conditional
#pragma warning (disable: 4706)

#ifdef AVH_SERVER

enum w_squeak_e {
	WSQUEAK_IDLE1 = 0,
	WSQUEAK_FIDGET,
	WSQUEAK_JUMP,
	WSQUEAK_RUN,
};

enum squeak_e {
	SQUEAK_IDLE1 = 0,
	SQUEAK_FIDGETFIT,
	SQUEAK_FIDGETNIP,
	SQUEAK_DOWN,
	SQUEAK_UP,
	SQUEAK_THROW
};


class BabblerProjectile : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	//int  Classify( void );
	virtual int	IRelationship(CBaseEntity* inTarget);

	void EXPORT SuperBounceTouch( CBaseEntity *pOther );
	void EXPORT HuntThink( void );
	int  BloodColor( void ) { return BLOOD_COLOR_YELLOW; }
	void Killed( entvars_t *pevAttacker, int iGib );
	void GibMonster( void );
	int	TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);

	static float m_flNextBounceSoundTime;

	// CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	int  m_iMyClass;
};

float BabblerProjectile::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS(kwBabblerProjectile, BabblerProjectile);

//TYPEDESCRIPTION	BabblerProjectile::m_SaveData[] = 
//{
//	DEFINE_FIELD( BabblerProjectile, m_flDie, FIELD_TIME ),
//	DEFINE_FIELD( BabblerProjectile, m_vecTarget, FIELD_VECTOR ),
//	DEFINE_FIELD( BabblerProjectile, m_flNextHunt, FIELD_TIME ),
//	DEFINE_FIELD( BabblerProjectile, m_flNextHit, FIELD_TIME ),
//	DEFINE_FIELD( BabblerProjectile, m_posPrev, FIELD_POSITION_VECTOR ),
//	DEFINE_FIELD( BabblerProjectile, m_hOwner, FIELD_EHANDLE ),
//};
//
//IMPLEMENT_SAVERESTORE( BabblerProjectile, CGrenade );

#define SQUEEK_DETONATE_DELAY	15.0

//int BabblerProjectile :: Classify ( void )
//{
//	if (m_iMyClass != 0)
//		return m_iMyClass; // protect against recursion
//
//	if (m_hEnemy != NULL)
//	{
//		m_iMyClass = CLASS_INSECT; // no one cares about it
//		switch( m_hEnemy->Classify( ) )
//		{
//			case CLASS_PLAYER:
//			case CLASS_HUMAN_PASSIVE:
//			case CLASS_HUMAN_MILITARY:
//				m_iMyClass = 0;
//				return CLASS_ALIEN_MILITARY; // barney's get mad, grunts get mad at it
//		}
//		m_iMyClass = 0;
//	}
//
//	return CLASS_ALIEN_BIOWEAPON;
//}

int BabblerProjectile::IRelationship(CBaseEntity* inTarget)
{
	int theRelationship = R_NO;

	// Don't attack cloaked players
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inTarget);
	if(thePlayer && thePlayer->GetIsCloaked())
	{
		theRelationship = R_NO;
	}
	else
	{
		// Attack all monsters that aren't on our team
		CBaseMonster* theMonsterPointer = dynamic_cast<CBaseMonster*>(inTarget);
		if(theMonsterPointer && (theMonsterPointer->pev->team != this->pev->team))
		{
			theRelationship = R_DL;
		}
		else
		{
			// Look at own team vs. incoming team
			AvHTeamNumber inTeam = (AvHTeamNumber)inTarget->pev->team;
			if(inTeam != TEAM_IND)
			{
				if(inTeam == this->pev->team)
				{
					theRelationship = R_AL;
				}
				else
				{
					// Don't keep switching targets constantly
					theRelationship = R_DL;
				}
			}
		}
	}
	
	return theRelationship;
}

void BabblerProjectile::Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	pev->classname = MAKE_STRING(kwsBabblerProjectile);

	SET_MODEL(ENT(pev), kBabblerModel);
	//UTIL_SetSize(pev, Vector( -4, -4, 0), Vector(4, 4, 8));
	UTIL_SetSize(pev, Vector(HULL1_MINX, HULL1_MINY, HULL1_MINZ), Vector(HULL1_MAXX, HULL1_MAXY, HULL1_MAXZ));
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( &BabblerProjectile::SuperBounceTouch );
	SetThink( &BabblerProjectile::HuntThink );
	pev->nextthink = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time + 1E6;

	pev->flags |= FL_MONSTER;
	pev->takedamage		= DAMAGE_AIM;
	pev->health			= gSkillData.snarkHealth;
	pev->gravity		= 0.5;
	pev->friction		= 0.5;

	m_flDie = gpGlobals->time + SQUEEK_DETONATE_DELAY;

	m_flFieldOfView = 0; // 180 degrees

	if ( pev->owner )
		m_hOwner = Instance( pev->owner );

	m_flNextBounceSoundTime = gpGlobals->time;// reset each time a snark is spawned.

	//pev->sequence = WSQUEAK_RUN;
	pev->sequence = 4;
	ResetSequenceInfo( );
}

void BabblerProjectile::Precache( void )
{
	PRECACHE_UNMODIFIED_MODEL(kBabblerModel);
	
	PRECACHE_UNMODIFIED_SOUND(kBabblerBlastSound);
	PRECACHE_UNMODIFIED_SOUND(kBabblerDieSound);
	PRECACHE_UNMODIFIED_SOUND(kBabblerHunt1Sound);
	PRECACHE_UNMODIFIED_SOUND(kBabblerHunt2Sound);
	PRECACHE_UNMODIFIED_SOUND(kBabblerHunt3Sound);
	PRECACHE_UNMODIFIED_SOUND(kBabblerBiteSound);
	PRECACHE_UNMODIFIED_SOUND("common/bodysplat.wav");
}


void BabblerProjectile :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->model = iStringNull;// make invisible
	SetThink( &BabblerProjectile::SUB_Remove );
	SetTouch( NULL );
	pev->nextthink = gpGlobals->time + 0.1;

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	pev->takedamage = DAMAGE_NO;

	// play squeek blast
	EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, kBabblerBlastSound, 1, 0.5, 0, PITCH_NORM);	

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, SMALL_EXPLOSION_VOLUME, 3.0 );

	UTIL_BloodDrips( pev->origin, g_vecZero, BloodColor(), 80 );

	if (m_hOwner != NULL)
		RadiusDamage ( pev, m_hOwner->pev, kBabblerExplodeDamage, CLASS_NONE, NS_DMG_BLAST);
	else
		RadiusDamage ( pev, pev, kBabblerExplodeDamage, CLASS_NONE, NS_DMG_BLAST );

	// reset owner so death message happens
	if (m_hOwner != NULL)
		pev->owner = m_hOwner->edict();

	CBaseMonster :: Killed( pevAttacker, GIB_ALWAYS );
}

void BabblerProjectile :: GibMonster( void )
{
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "common/bodysplat.wav", 0.75, ATTN_NORM, 0, 200);		
}

int	BabblerProjectile::TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	if(!pevAttacker)
	{
		pevAttacker = pevInflictor;
	}
	
	if(!pevInflictor)
	{
		pevInflictor = pevAttacker;
	}

	// Note: NS_DMG_ORGANIC should affect us fully

	return CGrenade::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}


void BabblerProjectile::HuntThink( void )
{
	// ALERT( at_console, "think\n" );

	if (!IsInWorld())
	{
		SetTouch( NULL );
		UTIL_Remove( this );
		return;
	}
	
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize( );
		pev->health = -1;
		Killed( pev, 0 );
		return;
	}

	// float
	if (pev->waterlevel != 0)
	{
		if (pev->movetype == MOVETYPE_BOUNCE)
		{
			pev->movetype = MOVETYPE_FLY;
		}
		pev->velocity = pev->velocity * 0.9;
		pev->velocity.z += 8.0;
	}
	else if (pev->movetype = MOVETYPE_FLY)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}

	// return if not time to hunt
	if (m_flNextHunt > gpGlobals->time)
		return;

	m_flNextHunt = gpGlobals->time + 2.0;
	
	CBaseEntity *pOther = NULL;
	Vector vecDir;
	TraceResult tr;

	Vector vecFlat = pev->velocity;
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalize( );

	UTIL_MakeVectors( pev->angles );

	if (m_hEnemy == NULL || !m_hEnemy->IsAlive())
	{
		// find target, bounce a bit towards it.
		Look( 512 );
		m_hEnemy = BestVisibleEnemy( );
	}

	// squeek if it's about time blow up
	if ((m_flDie - gpGlobals->time <= 0.5) && (m_flDie - gpGlobals->time >= 0.3))
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, kBabblerDieSound, 1, ATTN_NORM, 0, 100 + RANDOM_LONG(0,0x3F));
		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 256, 0.25 );
	}

	// higher pitch as squeeker gets closer to detonation time
	float flpitch = 155.0 - 60.0 * ((m_flDie - gpGlobals->time) / SQUEEK_DETONATE_DELAY);
	if (flpitch < 80)
		flpitch = 80;

	if (m_hEnemy != NULL)
	{
		if (FVisible( m_hEnemy ))
		{
			vecDir = m_hEnemy->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize( );
		}

		float flVel = pev->velocity.Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;
		
		// ALERT( at_console, "think : enemy\n");

		// ALERT( at_console, "%.0f %.2f %.2f %.2f\n", flVel, m_vecTarget.x, m_vecTarget.y, m_vecTarget.z );

		pev->velocity = pev->velocity * flAdj + m_vecTarget * 300;
	}

	if (pev->flags & FL_ONGROUND)
	{
		pev->avelocity = Vector( 0, 0, 0 );
	}
	else
	{
		if (pev->avelocity == Vector( 0, 0, 0))
		{
			pev->avelocity.x = RANDOM_FLOAT( -100, 100 );
			pev->avelocity.z = RANDOM_FLOAT( -100, 100 );
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0)
	{
		pev->velocity.x = RANDOM_FLOAT( -100, 100 );
		pev->velocity.y = RANDOM_FLOAT( -100, 100 );
	}
	m_posPrev = pev->origin;

	pev->angles = UTIL_VecToAngles( pev->velocity );
	pev->angles.z = 0;
	pev->angles.x = 0;
}


void BabblerProjectile::SuperBounceTouch( CBaseEntity *pOther )
{
	float	flpitch;

	TraceResult tr = UTIL_GetGlobalTrace( );

	// don't hit the guy that launched this grenade
	if ( pev->owner && pOther->edict() == pev->owner )
		return;

	// at least until we've bounced once
	pev->owner = NULL;

	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	// higher pitch as squeeker gets closer to detonation time
	flpitch = 155.0 - 60.0 * ((m_flDie - gpGlobals->time) / SQUEEK_DETONATE_DELAY);

	if ( pOther->pev->takedamage && m_flNextAttack < gpGlobals->time )
	{
		// attack!

		// make sure it's me who has touched them
		if (tr.pHit == pOther->edict())
		{
			// and it's not another squeakgrenade
			if (tr.pHit->v.modelindex != pev->modelindex)
			{
				// ALERT( at_console, "hit enemy\n");
				ClearMultiDamage( );
				pOther->TraceAttack(pev, kBabblerBiteDamage, gpGlobals->v_forward, &tr, NS_DMG_BLAST); 
				if (m_hOwner != NULL)
					ApplyMultiDamage( pev, m_hOwner->pev );
				else
					ApplyMultiDamage( pev, pev );

				// m_flDie += 2.0; // add more life

				// make bite sound
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, kBabblerBiteSound, 1.0, ATTN_NORM);//, (int)flpitch);
				//EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, kBabblerBiteSound, 1.0, ATTN_NORM, 0, (int)flpitch);
				m_flNextAttack = gpGlobals->time + 0.5;
			}
		}
		else
		{
			// ALERT( at_console, "been hit\n");
		}
	}

	m_flNextHit = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time;

	// Limit how often snarks can make their bounce sounds to prevent overflows.
	if ( gpGlobals->time < m_flNextBounceSoundTime )
	{
		// too soon!
		return;
	}

	if (!(pev->flags & FL_ONGROUND))
	{
		// play bounce sound
		float flRndSound = RANDOM_FLOAT ( 0 , 1 );

		if ( flRndSound <= 0.33 )
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, kBabblerHunt1Sound, 1, ATTN_NORM, 0, (int)flpitch);		
		else if (flRndSound <= 0.66)
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, kBabblerHunt2Sound, 1, ATTN_NORM, 0, (int)flpitch);
		else 
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, kBabblerHunt3Sound, 1, ATTN_NORM, 0, (int)flpitch);
		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 256, 0.25 );
	}
	else
	{
		// skittering sound
		CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, 100, 0.1 );
	}

	m_flNextBounceSoundTime = gpGlobals->time + 0.5;// half second.
}

#endif





LINK_ENTITY_TO_CLASS(kwBabblerGun, AvHBabblerGun);

void AvHBabblerGun::Init()
{
	this->mRange = kBabblerGunRange;
	this->mROF = kBabblerGunROF;
}

int	AvHBabblerGun::GetBarrelLength() const
{
	return kBabblerGunBarrelLength;
}

int	AvHBabblerGun::GetDeployAnimation() const
{
	return 7;
}

int	AvHBabblerGun::GetIdleAnimation() const
{
	// Must be odd
	int theAnimation = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 1);
	return theAnimation;
}

int	AvHBabblerGun::GetShootAnimation() const
{
	return 4;
}

char* AvHBabblerGun::GetViewModel() const
{
	return kLevel2ViewModel;
}

void AvHBabblerGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kBabblerGunSound);

	UTIL_PrecacheOther(kwsBabblerProjectile);
	
	this->mEvent = PRECACHE_EVENT(1, kBabblerGunEventName);
}


void AvHBabblerGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 
	
	this->Precache();
	
	this->m_iId = AVH_WEAPON_BABBLER;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsBabblerGun);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 


void AvHBabblerGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER

	// TODO: Make sure we don't have too many already?  Play a sound if we have too many?

	UTIL_MakeVectors( this->m_pPlayer->pev->v_angle );
	TraceResult tr;
	Vector trace_origin;

	// HACK HACK:  Ugly hacks to handle change in origin based on new physics code for players
	// Move origin up if crouched and start trace a bit outside of body ( 20 units instead of 16 )
	trace_origin = this->m_pPlayer->pev->origin + this->m_pPlayer->pev->view_ofs;
//	if(this->m_pPlayer->pev->flags & FL_DUCKING)
//	{
//		trace_origin = trace_origin - (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
//	}

	// find place to toss monster
	UTIL_TraceLine( trace_origin + gpGlobals->v_forward * 20, trace_origin + gpGlobals->v_forward * 64, dont_ignore_monsters, NULL, &tr );

	if ( tr.fAllSolid == 0 && tr.fStartSolid == 0 && tr.flFraction > 0.25 )
	{
		CBaseEntity* theProjectile = CBaseEntity::Create(kwsBabblerProjectile, tr.vecEndPos, this->m_pPlayer->pev->v_angle, this->m_pPlayer->edict() );
		theProjectile->pev->velocity = gpGlobals->v_forward * 200 + this->m_pPlayer->pev->velocity;
		theProjectile->pev->team = this->m_pPlayer->pev->team;
		theProjectile->pev->owner = this->m_pPlayer->edict();
	}

	#endif	
}

