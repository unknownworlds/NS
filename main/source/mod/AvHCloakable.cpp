#include "mod/AvHCloakable.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "util/Balance.h"

AvHCloakable::AvHCloakable()
{
	this->Init();
}

void AvHCloakable::Init()
{
	this->mCurrentSpeed = 0.0f;
	this->mMaxSpeed = 0.0f;
	this->mMaxWalkSpeed = 0.0f;
	this->mTimeOfLastCloak = -1;
	this->mTimeOfLastUncloak = -1;
	this->mOpacity = 1.0f;
	this->mTimeOfLastUpdate = 0;
}

bool AvHCloakable::GetCanCloak() const
{
    bool theCanCloak = ((this->mTimeOfLastUncloak == -1) || (this->GetTime() > this->mTimeOfLastUncloak + this->GetUncloakTime()));

    if(!theCanCloak)
    {
        int a = 0;
    }

    return theCanCloak;
}

float AvHCloakable::GetCloakTime() const
{
	return BALANCE_VAR(kCloakTime);
}

bool AvHCloakable::GetIsCloaked() const
{
	return (this->mOpacity < 0.1f);
}

bool AvHCloakable::GetIsPartiallyCloaked() const
{
	return (this->mOpacity < 0.6f);
}

float AvHCloakable::GetUncloakTime() const
{
	return BALANCE_VAR(kUncloakTime);
}

void AvHCloakable::ResetCloaking()
{
	this->Init();
}

void AvHCloakable::Update()
{
	float theCurrentTime = this->GetTime();

	float theTimePassed = theCurrentTime - this->mTimeOfLastUpdate;

	if((this->mTimeOfLastCloak != -1) || (this->mTimeOfLastUncloak != -1))
	{
		float newOpacity=this->mOpacity;
		if( this->mTimeOfLastCloak > this->mTimeOfLastUncloak  )
		{
			// Cloaking
			newOpacity -= theTimePassed/this->GetCloakTime();
			// Adjust for movement
			if ( this->mOpacity < 0.45f && this->mCurrentSpeed > this->mMaxWalkSpeed ) {
				newOpacity=this->mOpacity;
				if ( this->mCurrentSpeed > this->mMaxSpeed ) {
					newOpacity += theTimePassed / this->GetUncloakTime();
				}
				else {
					newOpacity += theTimePassed/this->GetUncloakTime() / 2.0f;

				}
				newOpacity = min(max(0.0f, newOpacity), 0.45f);
			}
		}
		else 
		{
			// Uncloaking
			newOpacity += theTimePassed/this->GetUncloakTime();
		}


		this->mOpacity = min(max(0.0f, newOpacity), 1.0f);
	}
	this->mTimeOfLastUpdate = theCurrentTime;
}

float AvHCloakable::GetOpacity() const
{
	return this->mOpacity;
}

void AvHCloakable::SetSpeeds(float inCurrentSpeed, float inMaxSpeed, float inMaxWalkSpeed) 
{
	this->mCurrentSpeed=inCurrentSpeed;
	this->mMaxSpeed=inMaxSpeed;
	this->mMaxWalkSpeed=inMaxWalkSpeed;
}

void AvHCloakable::Cloak(bool inNoFade)
{
	// : 864
	if ( (this->GetTime() > this->mTimeOfLastUncloak + BALANCE_VAR(kRecloakTime)) || inNoFade ) {
		if(this->GetCanCloak())
		{
			this->mTimeOfLastCloak = this->GetTime();
			this->mTimeOfLastUncloak = -1;

			if(inNoFade)
			{
				this->mOpacity = 0.0f;
			}
		}
	}
}

void AvHCloakable::Uncloak(bool inNoFade)
{
	this->mTimeOfLastUncloak = this->GetTime();
	this->mTimeOfLastCloak = -1;

	if(inNoFade)
	{
		this->mOpacity = 1.0f;
	}
}

float AvHCloakable::GetTime() const
{
	return gpGlobals->time;
}
