#include "mod/AvHAlienWeapons.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHSpecials.h"

#include "pm_shared/pm_defs.h"
extern playermove_t*	pmove;

#ifdef AVH_SERVER
#include "mod/AvHPlayer.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#include "mod/AvHHud.h"
extern int g_runfuncs;
#endif

#include "mod/AvHAlienAbilities.h"

LINK_ENTITY_TO_CLASS(kwBlinkGun, AvHBlinkGun);

void AvHBlinkGun::Init()
{
	this->mRange = 0;
	this->mDamage = 0;
	this->mTimeOfNextBlinkEvent = 0;
}

int	AvHBlinkGun::GetBarrelLength() const
{
	return 0;
}

float AvHBlinkGun::GetRateOfFire() const
{
	return BALANCE_VAR(kBlinkROF);
}

BOOL AvHBlinkGun::Deploy()
{
	return AvHAlienWeapon::Deploy();
}

void AvHBlinkGun::ItemPostFrame(void)
{
	AvHAlienWeapon::ItemPostFrame();

	float theClientTimePassedThisTick = (pmove->cmd.msec/1000.0f);
	this->mTimeOfNextBlinkEvent -= theClientTimePassedThisTick;
}

int	AvHBlinkGun::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 7;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_BLINK:
	case AVH_WEAPON_METABOLIZE:
		theDeployAnimation = -1;
		break;
		
	case AVH_WEAPON_SWIPE:
		theDeployAnimation = 9;
		break;
		
	case AVH_WEAPON_ACIDROCKET:
	case AVH_WEAPON_BILEBOMB:
		theDeployAnimation = 11;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHBlinkGun::GetIdleAnimation() const
{
	int theAnimation = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 17, 18);
	return theAnimation;
}

int	AvHBlinkGun::GetShootAnimation() const
{
	return -1;
}

bool AvHBlinkGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHBlinkGun::GetIsDroppable() const
{
	return false;
}

AvHMessageID AvHBlinkGun::GetAbilityImpulse() const
{
	return ALIEN_ABILITY_BLINK;
}

void AvHBlinkGun::FireProjectiles(void)
{
#ifdef AVH_CLIENT
	if(g_runfuncs)
	{
		gHUD.SetAlienAbility(this->GetAbilityImpulse());
	}
#endif
#ifdef AVH_SERVER
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	if(thePlayer)
	{
		thePlayer->TriggerUncloak();
	}
#endif
	if(this->mTimeOfNextBlinkEvent <= 0)
	{
		const float kEventDelay = 2.0f;
		this->PlaybackEvent(this->mBlinkSuccessEvent, this->GetShootAnimation());
		this->mTimeOfNextBlinkEvent = UTIL_WeaponTimeBase() + kEventDelay;
	}
}

bool AvHBlinkGun::GetMustPressTriggerForEachShot() const
{ 
	return false;
}

char* AvHBlinkGun::GetViewModel() const
{
	return kLevel4ViewModel;
}

void AvHBlinkGun::Precache()
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kBlinkSuccessSound);
	
	// No event for firing, only on success or failure
	this->mEvent = 0;
	this->mBlinkSuccessEvent = PRECACHE_EVENT(1, kBlinkEffectSuccessEventName);
}

void AvHBlinkGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_BLINK;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsBlinkGun);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 

bool AvHBlinkGun::UsesAmmo(void) const
{
	return false;
}


