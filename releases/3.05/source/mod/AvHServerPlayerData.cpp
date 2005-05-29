#include "mod/AvHServerPlayerData.h"

AvHServerPlayerData::AvHServerPlayerData()
{
	this->mResources = -1;
	this->mTimeLastVotedDown = -1;
	this->mExperience = 0.0f;
	this->mExperienceLevelsSpent = 0;
	this->mHasJoinedTeam = false;
}
				
float AvHServerPlayerData::GetResources() const
{
	return this->mResources;
}

void AvHServerPlayerData::SetResources(float inResources)
{
	this->mResources = inResources;
}

float AvHServerPlayerData::GetTimeVotedDown() const
{
	return this->mTimeLastVotedDown;
}

void AvHServerPlayerData::SetTimeVotedDown(float inTime)
{
	this->mTimeLastVotedDown = inTime;
}

float AvHServerPlayerData::GetExperience() const
{
	return this->mExperience;
}

void AvHServerPlayerData::SetExperience(float inExperience)
{
	this->mExperience = inExperience;
}

const AvHTechNodes& AvHServerPlayerData::GetCombatNodes()
{
	return this->mCombatNodes;
}

void AvHServerPlayerData::SetCombatNodes(const AvHTechNodes& inTechNodes)
{
	this->mCombatNodes = inTechNodes;
}

const MessageIDListType& AvHServerPlayerData::GetPurchasedCombatUpgrades() const
{
	return this->mPurchasedCombatUpgrades;
}

void AvHServerPlayerData::SetPurchasedCombatUpgrades(const MessageIDListType& inPurchasedCombatUpgrades)
{
	this->mPurchasedCombatUpgrades = inPurchasedCombatUpgrades;
}

int	AvHServerPlayerData::GetExperienceLevelsSpent() const
{
	return this->mExperienceLevelsSpent;
}

void AvHServerPlayerData::SetExperienceLevelsSpent(int inExperienceLevelsSpent)
{
	this->mExperienceLevelsSpent = inExperienceLevelsSpent;
}