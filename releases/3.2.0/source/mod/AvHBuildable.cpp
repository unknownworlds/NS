#include "mod/AvHBuildable.h"
#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHTeam.h"

AvHBuildable::AvHBuildable(AvHTechID inTechID)
{
	this->mTechID = inTechID;
	this->mHasBeenBuilt = false;
	this->mBuilder = -1;
}

int	AvHBuildable::GetBuilder() const
{
	return this->mBuilder;
}

bool AvHBuildable::GetHasBeenBuilt() const
{
	return this->mHasBeenBuilt;
}

bool AvHBuildable::GetSupportsTechID(AvHTechID inTechID) const
{
	return (inTechID == this->mTechID);
}

AvHTechID AvHBuildable::GetTechID() const
{
	return this->mTechID;
}

void AvHBuildable::SetTechID(AvHTechID inTechID)
{
	this->mTechID = inTechID;
}

void AvHBuildable::SetHasBeenKilled()
{
	GetGameRules()->BuildableKilled(this);
}

void AvHBuildable::SetBuilder(int inEntIndex)
{
	this->mBuilder = inEntIndex;
}

void AvHBuildable::SetConstructionComplete(bool inForce)
{
}

void AvHBuildable::SetHasBeenBuilt()
{
	this->mHasBeenBuilt = true;

	GetGameRules()->BuildableBuilt(this);
}

void AvHBuildable::SetResearching(bool inState)
{
}

void AvHBuildable::TriggerAddTech() const
{
	AvHTeamNumber theTeamNumber = this->GetTeamNumber();
	AvHTeam* theTeam = GetGameRules()->GetTeam(theTeamNumber);
	
	if(theTeam)
	{
		theTeam->TriggerAddTech(this->mTechID);
	}
}

void AvHBuildable::TriggerRemoveTech() const
{
	AvHTeamNumber theTeamNumber = this->GetTeamNumber();
	AvHTeam* theTeam = GetGameRules()->GetTeam(theTeamNumber);
	
	if(theTeam)
	{
		theTeam->TriggerRemoveTech(this->mTechID);
		if ( this->mTechID == TECH_ADVANCED_ARMORY )
			theTeam->TriggerRemoveTech(TECH_ARMORY);
	}
}

