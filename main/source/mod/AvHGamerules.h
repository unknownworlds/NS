//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: NS high-level game rules
//
// $Workfile: AvHGamerules.h $
// $Date: 2002/11/15 04:46:18 $
//
//-------------------------------------------------------------------------------
// $Log: AvHGamerules.h,v $
// Revision 1.52  2002/11/15 04:46:18  Flayra
// - Changes to for profiling and for improving AddToFullPack performance
//
// Revision 1.51  2002/11/12 02:24:57  Flayra
// - HLTV updates
//
// Revision 1.50  2002/11/06 01:40:01  Flayra
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.49  2002/11/03 04:49:45  Flayra
// - Moved constants into .dll out of .cfg
// - Particle systems update much less often, big optimization
// - Team balance fixes
//
// Revision 1.48  2002/10/28 20:34:53  Flayra
// - Reworked game reset slightly, to meter out network usage during game reset
//
// Revision 1.47  2002/10/20 21:10:41  Flayra
// - Optimizations
//
// Revision 1.46  2002/10/16 00:56:22  Flayra
// - Prototyped curl support for NS stats
// - Added player auth stuff (server string)
// - Added concept of secondary weapons
// - Removed disconnect sound
// - Fixed MOTD
// - Added "order needed" alert
//
// Revision 1.45  2002/10/03 18:44:03  Flayra
// - Play disconnected sound quieter because of mass exodus effect
// - Added functionality to allow players to join a team and run around freely before game countdown starts.  Game resets when countdown starts.
// - Added new alien alerts
//
// Revision 1.44  2002/09/23 22:16:01  Flayra
// - Added game victory status logging
// - Fixed commander alerts
// - Added new alerts
// - Particle system changes, only send them down when connecting or map changes
//
// Revision 1.43  2002/09/09 19:51:13  Flayra
// - Removed gamerules dictating alien respawn time, now it's in server variable
//
// Revision 1.42  2002/08/16 02:35:09  Flayra
// - Blips now update once per second, instead of once per player per second
//
// Revision 1.41  2002/08/09 01:01:42  Flayra
// - Removed error condition for map validity, allow marines to have only one primary weapon, adjust weapon weights
//
// Revision 1.40  2002/08/02 22:00:23  Flayra
// - New alert system that's not so annoying and is more helpful.  Tweaks for new help system.
//
// Revision 1.39  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.38  2002/07/23 17:03:21  Flayra
// - Resource model changes, refactoring spawning to fix level 5 redemption bug without code duplication
//
// Revision 1.37  2002/07/08 16:59:14  Flayra
// - Don't pick up empty weapons, added handicapping, check map validity when loaded, reset players just like all other entities, fixed spawn bug code where it was counting non-team spawns
//
// Revision 1.36  2002/07/01 21:32:44  Flayra
// - Visibility update now updates world for primal scream and umbra, don't update reliable network messages every tick (big optimization)
//
// Revision 1.35  2002/06/25 17:59:03  Flayra
// - Added timelimit (switches map after a game finishes), added info_locations, tried adding team balance (not sure it works yet), give resources for kills
//
// Revision 1.34  2002/06/10 19:54:30  Flayra
// - New minimap support, more attempts to fix picking up of alien weapons
//
// Revision 1.33  2002/05/28 17:40:32  Flayra
// - Minimap refactoring, hive sight "under attack", reinforcement refactoring, don't delete entities marked as permanent (it was deleting hives!), allow players to join the opposite team until this can be fixed for real (server retry gets around this code so this is just an inconvenience), things build fast with cheats
//
// Revision 1.32  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_GAMERULES_H
#define AVH_GAMERULES_H

#include "dlls/gamerules.h"
//#include "mod/AvHCOCRuleset.h"
#include "mod/AvHTeam.h"
#include "mod/AvHMarineWeapons.h"
#include "types.h"
#include "mod/AvHMessage.h"
#include "mod/AvHEntityHierarchy.h"
#include "mod/AvHEntities.h"
#include "mod/AvHMiniMap.h"
#include "dlls/teamplay_gamerules.h"
#include "mod/AvHDramaticPriority.h"
#include "mod/AvHMapExtents.h"
#include "util/Tokenizer.h"
#include "mod/AvHSpawn.h"

class AvHPlayer;

class TeamPurchase
{
public:
	TeamPurchase(edict_t* inPlayer, AvHMessageID inMessageID) : mPlayer(inPlayer), mUpgrade(inMessageID)
		{}

	edict_t*		mPlayer;
	AvHMessageID	mUpgrade;
};

typedef vector< pair <string, int> >	MapVoteListType;
typedef map< int, int >					PlayerMapVoteListType;
typedef map< int, float >				PlayerVoteTimeType;
// puzl: 0001073
#ifdef USE_OLDAUTH
typedef vector< pair<string, string> >					AuthIDListType;
typedef map<AvHPlayerAuthentication, AuthIDListType>	AuthMaskListType;
#endif

class AvHGamerules : public CHalfLifeTeamplay //public CHalfLifeMultiplay/*, public AvHCOCRuleset*/
{
public:
						AvHGamerules();
    virtual				~AvHGamerules();

// puzl: 0001073
#ifdef USE_OLDAUTH
	virtual BOOL		GetIsClientAuthorizedToPlay(edict_t* inEntity, bool inDisplayMessage, bool inForcePending = false) const;
	virtual bool		PerformHardAuthorization(AvHPlayer* inPlayer) const;
	virtual int			GetAuthenticationMask(const string& inAuthID) const;
	const AuthIDListType&	GetServerOpList() const;
	void				UpdateUplink();
#endif

	// this is the game name that gets seen in the server browser
	virtual int			AmmoShouldRespawn( CBasePlayerAmmo *pAmmo );
	virtual void		BuildableBuilt(AvHBuildable* inBuildable);
	virtual void		BuildableKilled(AvHBuildable* inBuildable);
	virtual void		BuildMiniMap(AvHPlayer* inPlayer);
	virtual BOOL		CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual void		ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual const char* GetGameDescription(void);

	virtual int			WeaponShouldRespawn( CBasePlayerItem *pWeapon );
	virtual void		DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);

	virtual BOOL		FShouldSwitchWeapon(CBasePlayer* inPlayer, CBasePlayerItem* inWeapon);

    virtual bool        GetCountdownStarted(void) const;
    virtual bool        GetGameStarted(void) const;
	virtual int			GetGameTime() const;
	virtual void		SetGameStarted(bool inGameStarted);
	AvHEntityHierarchy& GetEntityHierarchy(AvHTeamNumber inTeam);
	bool				GetIsPlayerSelectableByPlayer(AvHPlayer* inTargetPlayer, AvHPlayer* inByPlayer);
	virtual int			IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	void				ProcessTeamUpgrade(AvHMessageID inUpgrade, AvHTeamNumber inNumber, int inEntity, bool inGive = true);

	// Playtest functionality
	void				BalanceChanged();

    // This isn't called yet, add in hooks?
    void                ClientKill( edict_t *pEntity );
	virtual BOOL		CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry );// can this player take more of this ammo?
	virtual bool		CanPlayerBeKilled(CBasePlayer* inPlayer);

	virtual void		ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib);
	virtual BOOL		ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual BOOL        ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	virtual void        ClientDisconnected( edict_t *pClient );

	virtual int			DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual int			DeadPlayerWeapons( CBasePlayer *pPlayer );

	virtual BOOL		FAllowMonsters( void );
    virtual BOOL        FPlayerCanRespawn( CBasePlayer *pPlayer );
	virtual BOOL		FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );

	// TODO: Add splash damage parameter and outgoing float percentage damage?  This way splash damage could do some damage in non-tourny mode?
	virtual bool		CanEntityDoDamageTo(const CBaseEntity* inAttacker, const CBaseEntity* inReceiver, float* outScalar = NULL);

	//virtual edict_t*    GetPlayerSpawnSpot( CBasePlayer *pPlayer );
	virtual void		PlayerThink( CBasePlayer *pPlayer );

	void				ComputeWorldChecksum(Checksum& outChecksum) const;
	float				GetMapGamma();
	int					GetNumCommandersOnTeam(AvHTeamNumber inTeam);
    int                 GetNumActiveHives(AvHTeamNumber inTeam) const;
	int					GetNumEntities() const;
	const AvHGameplay&		GetGameplay() const;
	const AvHMapExtents&	GetMapExtents();
	
	virtual BOOL		IsMultiplayer( void );
	virtual BOOL		IsDeathmatch( void );
	virtual BOOL		IsCoOp( void );

	virtual void		InitHUD( CBasePlayer *pPlayer );
	virtual void		PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
	virtual void		PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void		PlayerSpawn( CBasePlayer *pPlayer );
	virtual void		ProcessRespawnCostForPlayer(AvHPlayer* inPlayer);
	
    virtual void		RewardPlayerForKill(AvHPlayer* inPlayer, CBaseEntity* inTarget, entvars_t* inInflictor = NULL);
	bool				RoamingAllowedForPlayer(CBasePlayer* inPlayer) const;
	virtual void		Think(void);
    
    void                RegisterServerVariable(const char* inName);
    int                 GetNumServerVariables() const;
    const std::string&  GetServerVariable(int i) const;

	bool				GetCheatsEnabled(void) const;
	bool				GetIsCheatEnabled(const string& inCheatName) const;
	void				SetCheatEnabled(const string& inCheatName, bool inEnabledState = true);
	
	float				GetFirstScanThinkTime() const;
	bool				GetDrawInvisibleEntities() const;
	bool				GetEntityExists(const char* inName) const;
	bool				GetIsTesting(void) const;
	bool				GetIsValidFutureTeam(AvHPlayer inPlayer, int inTeamNumber) const;
	bool				GetCanJoinTeamInFuture(AvHPlayer* inPlayer, AvHTeamNumber theTeamNumber, string& outString) const;
	const AvHBaseInfoLocationListType&		GetInfoLocations() const;
	int					GetMaxWeight(void) const;
	const char*			GetSpawnEntityName(AvHPlayer* inPlayer) const;
	Vector				GetSpawnAreaCenter(AvHTeamNumber inTeamNumber) const;
	float				GetTimeGameStarted() const;
	int					GetTimeLimit() const;
	int					GetWeightForItemAndAmmo(AvHWeaponID inWeapon, int inNumRounds) const;
	bool				AttemptToJoinTeam(AvHPlayer* inPlayer, AvHTeamNumber theTeamNumber, bool inDisplayErrorMessage = true);
	const AvHTeam*		GetTeam(AvHTeamNumber inTeamNumber) const;
	const AvHTeam*		GetTeamA() const;
	const AvHTeam*		GetTeamB() const;
	AvHTeam*			GetTeam(AvHTeamNumber inTeamNumber);
	AvHTeam*			GetTeamA();
	AvHTeam*			GetTeamB();
    AvHMapMode			GetMapMode(void) const;
	int					GetServerTick() const;
	AvHTeamNumber		GetVictoryTeam() const;
	float				GetVictoryTime() const;
	//void				QueueTeamUpgrade(edict_t* inPlayer, AvHMessageID inUpgrade);
	void				DeleteAndResetEntities();
	void				PlayerDeathEnd(AvHPlayer* inPlayer);
	void				PostWorldPrecacheReset(bool inNewMap);
	void				PreWorldPrecacheReset();
	void				RegisterSpawnPoint(const string& inClassName, const Vector& inOrigin, const Vector& inAngles, const AvHTeamNumber& inTeamNumber);
	void				RespawnPlayer(AvHPlayer* inPlayer);

	void				TriggerAlert(AvHTeamNumber inTeamNumber, AvHAlertType inAlertType, int inEntIndex, AvHMessageID inMessageID = MESSAGE_NULL);
	bool				GetIsEntityUnderAttack(int inEntityIndex) const;

    virtual bool		GetArePlayersAllowedToJoinImmediately(void) const;
	virtual bool		GetIsTournamentMode(void) const;
	virtual bool		GetIsHamboneMode(void) const;
    virtual bool		GetIsIronMan(void) const;
	virtual bool		GetIsCombatMode(void) const;
	virtual AvHTeamNumber	GetCombatAttackingTeamNumber() const;
    virtual bool		GetIsNSMode(void) const;
    virtual bool		GetIsScriptedMode(void) const;
	virtual bool		GetIsTrainingMode(void) const;

	int					GetBaseHealthForMessageID(AvHMessageID inMessageID) const;
	int					GetBuildTimeForMessageID(AvHMessageID inMessageID) const;
	int					GetCostForMessageID(AvHMessageID inMessageID) const;
	
	CBaseEntity*		GetRandomHiveSpawnPoint(CBaseEntity* inPlayer, const Vector& inOrigin, float inMaxDistance) const;
	virtual edict_t*	SelectSpawnPoint(CBaseEntity* inPlayer) const;
	bool				CanPlayerBeacon(CBaseEntity *inPlayer);
	edict_t*			SelectSpawnPoint(CBaseEntity* inEntity, const string& inSpawnEntityName) const;
	const char*			SetDefaultPlayerTeam(CBasePlayer *pPlayer);
	
	void				MarkDramaticEvent(int inPriority, CBaseEntity* inPrimaryEntity, bool inDramatic = false, CBaseEntity* inSecondaryEntity = NULL) const;
	void				MarkDramaticEvent(int inPriority, short inPrimaryEntityIndex, bool inDramatic = false, short inSecondaryEntityIndex = 0) const;
	void				MarkDramaticEvent(int inPriority, short inPrimaryEntityIndex, bool inDramatic, entvars_t* inSecondaryEntity) const;

	virtual void		RecalculateHandicap(void);
    void                ServerExit();
    
    void                VoteMap(int inPlayerIndex, int inMapIndex);
    bool                GetMapVoteStrings(StringList& outMapVoteList);
	void				RemovePlayerFromVotemap(int inPlayerIndex);

protected:
	void				AutoAssignPlayer(AvHPlayer* inPlayer);
	void				PerformMapValidityCheck();
	virtual void		RecalculateMapMode( void );
	bool				GetDeathMatchMode(void) const;
	//void				PutPlayerIntoSpectateMode(AvHPlayer* inPlayer) const;
	virtual void		SendMOTDToClient( edict_t *client );


// puzl: 0001073
#ifdef USE_OLDAUTH

	void				AddAuthStatus(AvHPlayerAuthentication inAuthMask, const string& inWONID, const string& inSteamID);
	void				DisplayVersioning();
	void				InitializeAuthentication();
#endif

private:
	void				AwardExperience(AvHPlayer* inPlayer, int inTargetLevel, bool inAwardFriendliesInRange = true);
	void				CalculateMapExtents();
	void				CalculateMapGamma();
	void				CopyDataToSpawnEntity(const AvHSpawn& inSpawnEntity) const;
    void                JoinTeam(AvHPlayer* inPlayer, AvHTeamNumber theTeamNumber, bool inDisplayErrorMessage, bool inForce);
	void				PreWorldPrecacheInitParticles();
	void				PostWorldPrecacheInitParticles();
    void                InitializeMapVoteList();
    int                 GetVotesNeededForMapChange() const;
	void				InitializeTechNodes();
	void				InternalResetGameRules();
	int					GetNumberOfPlayers() const;
	void				TallyVictoryStats() const;
	void				PostVictoryStatsToWeb(const string& inFormParams) const;
	bool				ReadyToStartCountdown();
	void				ResetGame(bool inPreserveTeams = false);
	void				SendGameTimeUpdate(bool inReliable = false);
	void				ProcessTeamUpgrades();
	void				ResetEntities();
	CBaseEntity*		SelectRandomSpawn(CBaseEntity* inPlayer, const string& inSpawnName) const;
	void				UpdateCheats();
    void                UpdateHLTVProxy();
	void				UpdatePlaytesting();
	void				UpdateCountdown(float inTime);
	void				UpdateEntitiesUnderAttack();
	void				UpdateGameTime();
	void				UpdateLOS();
	void				UpdateScripts();
	void				UpdateServerCommands();
	void				UpdateTimeLimit();
	void				UpdateWorldEntities();
	void				UpdateVictory(void);
	void				UpdateVictoryStatus(void);

	bool				mFirstUpdate;
	bool				mPreserveTeams;
	bool				mGameStarted;
	AvHTeamNumber		mVictoryTeam;
	float				mTimeCountDownStarted;
	float				mTimeGameStarted;
    float				mTimeOfLastHLTVProxyUpdate;
	float				mTimeOfLastGameTimeUpdate;
	float				mTimeSentCountdown;
	float				mTimeLastWontStartMessageSent;
	float				mTimeWorldReset;
	bool				mStartedCountdown;
	bool				mSentCountdownMessage;
	bool				mVictoryDraw;
	AvHTeam				mTeamA;
	AvHTeam				mTeamB;
	float				mVictoryTime;
    AvHMapMode			mMapMode;
// puzl: 0001073
#ifdef USE_OLDAUTH
	bool				mUpdatedUplink;
	AuthIDListType		mServerOpList;
#endif

	float				mLastParticleUpdate;
	float				mLastNetworkUpdate;
	float				mLastWorldEntityUpdate;
	float				mLastCloakableUpdate;
	float				mLastVictoryUpdate;
	float				mLastMapChange;

	float				mTimeOfLastPlaytestUpdate;
	float				mTimeOfLastHandicapUpdate;

	float				mTimeUpdatedScripts;
	
	typedef vector<TeamPurchase>	TeamPurchaseListType;
	TeamPurchaseListType			mPendingTeamUpgrades;

	// Potentially marines vs. marines
	AvHEntityHierarchy	mTeamAEntityHierarchy;
	AvHEntityHierarchy	mTeamBEntityHierarchy;

	AvHGameplay			mGameplay;

	bool				mCalculatedMapGamma;
	float				mMapGamma;

	typedef map<int, float>		EntityUnderAttackListType;
	EntityUnderAttackListType	mEntitiesUnderAttack;

	AvHMiniMap			mMiniMap;

	AvHMapExtents		mMapExtents;

	StringList			mCheats;

	AvHBaseInfoLocationListType	mInfoLocations;

	float				mSavedTimeCountDownStarted;

	SpawnListType				mSpawnList;
	mutable CBaseEntity*		mSpawnEntity;

    // Map voting
    MapVoteListType				mMapVoteList;
	PlayerMapVoteListType		mPlayersVoted;
	PlayerVoteTimeType			mPlayersVoteTime; 

    std::vector<std::string>	mServerVariableList;

	AvHTeamNumber				mCombatAttackingTeamNumber;
};

AvHGamerules* GetGameRules();
void SetGameRules(AvHGamerules* inGameRules);

#endif
