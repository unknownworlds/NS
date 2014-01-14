//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTeam.h $
// $Date: 2002/11/22 21:15:37 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTeam.h,v $
// Revision 1.32  2002/11/22 21:15:37  Flayra
// - Remove owner of entities when player leaves the team
// - Do damage to aliens without hives
// - Fixed bug where a vote could affect a commander that logged in after a vote had been started.
//
// Revision 1.31  2002/11/15 23:31:26  Flayra
// - Added "ready" verification for tourny mode
//
// Revision 1.30  2002/11/12 02:29:33  Flayra
// - Added better standardized logging
//
// Revision 1.29  2002/10/24 21:43:29  Flayra
// - Alien easter eggs
// - Voting fix
//
// Revision 1.28  2002/10/18 22:23:04  Flayra
// - Added beginnings of alien easter eggs
// - Added "we need builders" alert
//
// Revision 1.27  2002/10/03 19:09:55  Flayra
// - New resource model
// - Orders refactoring
// - Tech tree changes
// - Aliens always get initial points when joining
// - Play gamestart sound
// - New trait available trigger
// - Moved voting stuff to server variables
// - Slowed down hints
//
// Revision 1.26  2002/09/23 22:35:43  Flayra
// - Removed hive donation and put in new system for builders
// - Updated tech tree (jetpacks, heavy armor, moved phase)
// - Resource towers set as built on game start
// - Added tons of commander hints (low resource alerts, tell commander about pressing jump key, commander ejected, select troops and give orders, don't just sit there, etc.)
//
// Revision 1.25  2002/09/09 20:08:26  Flayra
// - Added commander voting
// - Added hive info
// - Changed how commander scoring works
//
// Revision 1.24  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.23  2002/08/02 21:45:20  Flayra
// - Reworked alerts.
//
// Revision 1.22  2002/07/23 17:32:23  Flayra
// - Resource model overhaul (closed system, random initial points), tech tree changes (new marine upgrades), free resource tower at nearest func_resource
//
// Revision 1.21  2002/07/08 17:19:42  Flayra
// - Added handicapping, map validity checking, reinforcements happen independently of teams now
//
// Revision 1.20  2002/06/25 18:21:33  Flayra
// - New enabled/disabled system for alien weapons, better victory detection, updated tech tree (removed old junk, added armory upgrade), fixed bug where a commander that leaves the game hogged the station, update resources less often (optimization)
//
// Revision 1.19  2002/05/28 18:09:51  Flayra
// - Track number of webs for team, fix for premature victory condition (again), fixed bug where alien upgrades weren't being cleared between levels, causing eventual overflows for alien players after many games, recycling support, spawn in command center when game starts
//
// Revision 1.18  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_TEAM_H
#define AVH_TEAM_H

#include "mod/AvHConstants.h"
#include "mod/AvHOrder.h"
#include "mod/AvHSharedTypes.h"
#include "mod/AvHResearchManager.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHServerPlayerData.h"
#include "mod/AvHTechSlotManager.h"
#include "mod/AvHAlert.h"

class AvHPlayer;

class AvHTeam
{
public:
					AvHTeam(AvHTeamNumber inTeamNumber);

	bool			AddPlayer(int inPlayerIndex);
	bool			RemovePlayer(int inPlayerIndex);

	bool			AddResourceTower(int inResourceTowerIndex);
	bool			RemoveResourceTower(int inResourceTowerIndex);

	bool			GetHasAtLeastOneActivePlayer(bool* outHasAtLeastOnePlayerOnTeam = NULL) const;
	bool			GetHasTeamLost() const;
	int				GetNumBuiltCommandStations() const;
	int				GetNumActiveHives() const;

	bool			ProcessRankChange(AvHPlayer* inPlayer, AvHUser3 inOldUser3, AvHUser3 inNewUser3);

	bool			GetIsReady() const;
	void			SetIsReady(bool bIsReady = true);

	AvHPlayer*			GetPlayerFromIndex(int inPlayerIndex);
	const AvHPlayer*	GetPlayerFromIndex(int inPlayerIndex) const;
	
	AvHClassType	GetTeamType(void) const;
	void			SetTeamType(AvHClassType inType);
	const char*		GetTeamTypeString(void) const;
	
	float			GetTeamResources() const;
	void			SetTeamResources(float inTeamResources);
	float			GetTotalResourcesGathered() const;
	void			AddResourcesGathered(float inResources);

	float			GetMaxResources(AvHUser3 inUser3) const;

	const char*		GetTeamName(void) const;
	void			SetTeamName(const char* inTeamName);

	const char*		GetTeamPrettyName(void) const;
	void			SetTeamPrettyName(const char* inTeamName);

	bool			GetIsPlayerOnTeam(int inPlayerIndex) const;
	int				GetPlayerCount(bool inCountOnlyDead = false) const;
	AvHTeamNumber	GetTeamNumber(void) const;
	void			SetTeamNumber(AvHTeamNumber	inTeamNumber);

	float			GetHandicap() const;
	void			SetHandicap(float inHandicap);
	bool			GetIsTechnologyAvailable(AvHMessageID inMessageID) const;

	int				GetDesiredSpawnCount() const;
	float			GetInitialPlayerPoints(edict_t* inEdict);
	float			GetInitialExperience(edict_t* inEdict);

	bool			SendResourcesGatheredScore(bool inThisTeamOnly = true);

	void						AddTeamUpgrade(AvHAlienUpgradeCategory inCategory);
	AvHAlienUpgradeListType		GetAlienUpgrades() const;
	bool						RemoveAlienUpgradeCategory(AvHAlienUpgradeCategory inCategory);

	int				GetTeamWideUpgrades() const;
	void			ProcessTeamUpgrade(AvHMessageID inMessageID, bool inGive);
	
	int				GetNumCommanders(void) const;

	// returns -1 if no commander assigned yet
	int				GetCommander() const;
	void			SetCommander(int inPlayerNumber);
	AvHPlayer*		GetCommanderPlayer();

	void			ResetGame();
	virtual void	SetGameStarted(bool inGameStarted);

	HiveInfoListType	GetHiveInfoList() const;
	
	// Tech nodes for team
	void			InitializeTechNodes();

	const AvHTechTree&	GetTechNodes() const;
	AvHTechTree&		GetTechNodes();

	const AvHTechSlotManager& GetTechSlotManager() const;
	AvHTechSlotManager& GetTechSlotManager();
	
	void			TriggerAddTech(AvHTechID inTechID);
	void			TriggerRemoveTech(AvHTechID inTechID);

	void			AddAlert(const AvHAlert& inAlert, AvHMessageID inMessageID = MESSAGE_NULL);
	bool			GetLastAlert(AvHAlert& outAlert, bool inClearAlert = false, bool inAnyMessage = true, AvHMessageID* ioMessageID = NULL);
	const AvHAlert* GetLastAudioAlert() const;
	float			GetAudioAlertInterval() const;
	int				GetMenuTechSlots() const;
		
	void			GetOrders(OrderListType& outOrderList) const;
	bool			GetDoesPlayerHaveOrder(int inPlayerIndex);
	//void			GetPlayersCompletingOrders(const EntityListType& outPlayerList) const;
	void			SetOrder(const AvHOrder& inOrder);

	void			KillCS();
	void			KillHive();
	void			SpawnHive(Vector* outLocation = NULL, bool inForce = false);

	int				GetNumWebStrands() const;
	void			SetNumWebStrands(int inNumWebStrands);

	void			PlayFunHUDSoundOnce(AvHHUDSound inHUDSound);
	void			PlayHUDSoundForAlivePlayers(AvHHUDSound inHUDSound) const;

	// : Bug 0000767
	void			PlayHUDSoundForAllPlayers(AvHHUDSound inHUDSound) const;
	// :

	bool			PlayerVote(int inPlayerIndex, string& outPlayerMessage);

	const AvHResearchManager&	GetResearchManager() const;
	AvHResearchManager&			GetResearchManager();

	AvHServerPlayerData*		GetServerPlayerData(edict_t* inEdict);
	
	void			Update();
	void			UpdateMenuTechSlots();

	AlertListType	GetAlerts(AvHMessageID inMessageID);

	EntityListType	GetGroup(int inGroupNumber);
	void			SetGroup(int inGroupNumber, EntityListType& inEntityListType);

	AvHUser3		GetGroupType(int inGroupNumber);

	EntityListType	GetSelectAllGroup();
	void			SetSelectAllGroup(EntityListType& inGroup);

private:
	void			AddTechNode(AvHMessageID inMessageID, AvHTechID inTechID, AvHTechID inPrereq1 = TECH_NULL, AvHTechID inPrereq2 = TECH_NULL, bool inAllowMultiples = true, bool inResearched = false);
	void			SpawnResourceTower();
	
	void			Init();
	void			InitializeCombatTechNodes();
	void			InitializeRandomResourceShares();
	bool			GetTeamHasAbilityToRespawn() const;
	void			ResetServerPlayerData();
	void			RespawnLongestWaitingPlayer();
	void			UpdateAlerts();
	void			UpdateHints();
	void			UpdateCommanderScore();
	void			UpdateInventoryEnabledState();
	void			UpdateReinforcementWave();
	void			UpdateOrders();
	void			UpdateResearch();
	void			UpdateResources();
	void			UpdateServerPlayerData();
	void			UpdateTeamStructures();
	void			UpdatePlayers();
	void			UpdateVoting();
	
	float			WithdrawPointsViaRandomShares(int inPlayerIndex);
	
	AvHClassType	mTeamType;
	string			mTeamName;
	string			mTeamPrettyName;

	AvHTeamNumber	mTeamNumber;
	float			mLastResourceUpdateTime;
	float			mLastCommandScoreUpdateTime;
	float			mLastInjectionTime;
	float			mLastHiveSpawnTime;
	float			mLastPlayerUpdateTime;
	float			mLastServerPlayerDataUpdateTime;
	Vector			mStartingLocation;

	// List of player entindices
	EntityListType	mPlayerList;
	EntityListType	mResourceTowerList;

	OrderListType	mOrderList;

	//EntityListType	mPlayersCompletingOrder;
	
	int				mCommander;
	int				mCommanderWhenVoteStarted;

	// Voting variables
	bool			mVoteStarted;
	int				mVotes;
	int				mPreviousVotes;
	EntityListType	mVotingPlayers;
	int				mVotesNeeded;
	float			mTimeVoteStarted;

	float			mTeamResources;
	float			mHandicap;

	//AvHMessageID	mResearchingTech;
	AvHResearchManager			mResearchManager;
		
	AvHAlienUpgradeListType		mAlienUpgrades;
	int							mTeamWideUpgrades;

	int				mNumWebStrands;

	typedef			vector<int>								RandomResourceShareListType;
	RandomResourceShareListType								mRandomResourceShares;

	typedef			map<int, RandomResourceShareListType>	PlayerSharesListType;
	PlayerSharesListType									mPlayerShares;

	int				mTotalShareWeight;

	typedef			map<string, AvHServerPlayerData>		AvHServerPlayerDataListType;
	AvHServerPlayerDataListType								mServerPlayerData;

	HiveInfoListType	mHiveInfoList;
	float				mTimeOfLastStructureUpdate;

	EntityListType	mLowResourceAlerts;
	float			mTimeLastHintUpdate;
	float			mTimeOfLastTeamHint;

	EntityListType	mCommandersToldAboutJumpKey;

	bool				mPlayFunSoundsThisGame;
	HUDSoundListType	mFunSoundsPlayed;
	bool				mPlayedSeeDeadPeople;

	bool				mIsReady;

	AvHTechSlotManager	mTechSlotManager;

	float				mTotalResourcesGathered;
	int					mClientTotalResourcesGathered;

	int					mMenuTechSlots;
	int					mNumActiveHives;
	int					mNumBuiltCommandStations;

	// Store alerts by message ID.  This message ID is used to fetch then when the commander presses the button on the UI.  General alerts use MESSAGE_NULL.
	typedef map<AvHMessageID, AlertListType>	MessageAlertListType;
	MessageAlertListType						mAlertList;

	EntityListType		mGroups[kNumHotkeyGroups];
	AvHUser3			mGroupTypes[kNumHotkeyGroups];
	EntityListType		mSelectAllGroup;

	float				mTimeReinforcementWaveComplete;
};

#endif