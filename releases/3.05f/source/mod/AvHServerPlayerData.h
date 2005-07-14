#ifndef AVH_SERVERPLAYERDATA_H
#define AVH_SERVERPLAYERDATA_H

#include "types.h"
#include "mod/AvHConstants.h"
#include "mod/AvHTechNodes.h"
#include "mod/AvHMessageList.h"

class AvHServerPlayerData
{
public:
				AvHServerPlayerData();
				
	float		GetResources() const;
	void		SetResources(float inResources);

	float		GetTimeVotedDown() const;
	void		SetTimeVotedDown(float inTime);

	float		GetExperience() const;
	void		SetExperience(float inExperience);

	const AvHTechNodes& GetCombatNodes();
	void		SetCombatNodes(const AvHTechNodes& inTechNodes);

	const MessageIDListType& GetPurchasedCombatUpgrades() const;
	void		SetPurchasedCombatUpgrades(const MessageIDListType& inPurchasedCombatUpgrades);

	int			GetExperienceLevelsSpent() const;
	void		SetExperienceLevelsSpent(int inExperienceLevelsSpent);

	bool		GetHasJoinedTeam()		 { return mHasJoinedTeam; }
	void		SetHasJoinedTeam(int HasJoinedTeam) { mHasJoinedTeam =  HasJoinedTeam; }

private:
	float			mResources;
	float			mTimeLastVotedDown;
	float			mExperience;

	AvHTechNodes    mCombatNodes;

	MessageIDListType	mPurchasedCombatUpgrades;
	int					mExperienceLevelsSpent;
	bool				mHasJoinedTeam;

};

#endif