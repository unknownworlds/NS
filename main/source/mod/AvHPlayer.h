//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHPlayer.h $
// $Date: 2002/11/22 21:18:33 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPlayer.h,v $
// Revision 1.65  2002/11/22 21:18:33  Flayra
// - Potentially fixed strange Onos collision crash
// - Don't allow player to join team after he's seen another team
// - "lastinv" support
// - Fixed perma-cloak bug after losing last sensory chamber whilst cloaked
// - Started fixing commander PAS problem
// - Fixed readyroom "ghost player" exploit when F4 during REIN
// - Draw damage in debug, never otherwise
//
// Revision 1.64  2002/11/15 04:42:50  Flayra
// - Regenerate now returns true if healing was successful
// - Logging changes and fixes
//
// Revision 1.63  2002/11/13 01:49:08  Flayra
// - Proper death message logging for Psychostats
//
// Revision 1.62  2002/11/12 18:44:54  Flayra
// - Added mp_logdetail support for damage messages
// - Changed the alien ability anti-exploit code to try to co-exist with scripters
//
// Revision 1.61  2002/11/12 02:28:39  Flayra
// - Fixed problems with armor not being updated when armor upgrades completed
// - Aliens now keep same percentage of health and armor when morphing
// - Much better logging, up to standard
// - Don't add enemy buildings to hive sight unless parasited (solves blip spam)
// - Removed draw damage from public build
// - Changes to minimap to less overflows at end of big games
//
// Revision 1.60  2002/10/25 21:48:01  Flayra
// - Added more auth masks
// - Moved mSkin to pev->skin so playerclass could hold what used to be mPlayMode could be used to quickly perform culling, to try to prevent overflowage at the end of large games
//
// Revision 1.59  2002/10/24 21:40:02  Flayra
// - Reworked jetpack effect
// - Authicons update
// - Set builder for alien buildings, so turret kills can credit builder
// - Alien easter eggs
// - Network optimizations after game reset on huge (20-32) player games
// - Allow server ops to disable auth icons
// - Alien energy tweaks
// - Show unbuilt hives in hive sight
// - Move alien energy updating fully into shared code
// - Tried to fix full health ring showing for dead selected players
// - Moved help text client-side
// - Cache info_locations and gamma until map change
// - Skin fixes
//
// Revision 1.58  2002/10/20 21:10:57  Flayra
// - Optimizations
//
// Revision 1.57  2002/10/16 01:05:38  Flayra
// - Sent health as short for big aliens
// - Fixed sayings not triggering commander alerts
// - Now name changes are queued until the next match
// - Added authmask support
// - Egg idle sounds play more frequently, refactored too
// - Fixed preserving model in ready room after game end
// - Profiling of AddToFullPack
// - Fix for falling through lifts when morphing on them (untested)
//
// Revision 1.56  2002/10/03 19:32:06  Flayra
// - Reworked orders completely
// - Send max resources to players
// - Heavy armor sped up slightly
// - Kill players who illegally try to use alien abilities
// - Moved energy to a new variable, send health for aliens
// - Only freeze players during countdown
// - Removed slowdown when taking damage
// - Send blips in two messages, one friendly and one enemy (old bad hack)
//
// Revision 1.55  2002/09/25 20:50:06  Flayra
// - Added 3 new sayings
// - Frame-rate independent updating
// - Don't allow player to kill self while commanding
//
// Revision 1.54  2002/09/23 22:27:52  Flayra
// - Added skin support
// - Added client connected/disconnected hooks for particle system propagation optimizations
// - Removed power armor, added heavy armor
// - Fixed death animations
// - Added hook to see if commander has given an order and to see if he's idle
// - Bound resources for aliens
// - Soldiers asking for ammo and health trigger commander alert
// - Added gestation anims
// - Slowed down Onos movement
// - When cheats are enabled, purchases are free
//
// Revision 1.53  2002/09/09 20:04:53  Flayra
// - Added commander voting
// - Commander score is now the average of the rest of his players (reverted back when he leaves CC)
// - Fixed bug where upgrades were getting removed and then add repeatedly
// - Added multiple skins for marines
// - Play sound when aliens lose an upgrade
// - Changed fov to 90 for all aliens for software compatibility
// - Added hiveinfo drawing
// - Tweaked marine and alien speeds (to compensate for loss of drastic alien fov)
//
// Revision 1.52  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.51  2002/08/16 02:42:57  Flayra
// - New damage types
// - Much more efficient blip calculation (at least it should be, I haven't measured it so who really knows)
// - New way of representing ensnare state for shared code (disabling jumping, jetpacking)
// - Removed old overwatch code
// - Store health in fuser2 for drawing health for commander
// - Swap bile bomb and umbra
//
// Revision 1.50  2002/08/09 01:10:30  Flayra
// - Keep previous model when a game is over and going back to ready room
// - Refactoring for scoreboard
// - Support for "jump" animation
// - Freeze player before game starts
// - Reset score when leaving a team
//
// Revision 1.49  2002/08/02 21:52:17  Flayra
// - Cleaned up jetpacks, made webbing useful again, help messages, added "orderself" cheat, changed gestation messages for new tooltip system, added GetHasAvailableUpgrades() for help system, removed particle template messages, slowed down particle template update rate to reduce overflows (woo!)
//
// Revision 1.48  2002/07/26 23:07:54  Flayra
// - Numerical feedback
// - New artwork for marine, with jetpack as body group (this code doesn't work)
// - Misc. fixes (alien vision mode not being preserved when it should, and staying when it shouldn't)
//
// Revision 1.47  2002/07/23 17:17:57  Flayra
// - Added power armor, added "talking" state for hive sight, changes for new resource model, removed max buildings, create buildings with random orientation, added stimpack, tweaked redemption, added new hive sight info
//
// Revision 1.46  2002/07/08 17:15:39  Flayra
// - Added validation of all impulses (assumes invalid by default, instead of valid), reset players like all other entities, ensnaring fixes, players are temporarily invulnerable when they spawn, preserve health and armor when using command station, fixed up redemption, add hooks for commander voting and going back to the ready room, models can't be changed via the console anymore
//
// Revision 1.45  2002/07/01 21:43:16  Flayra
// - Removed outdated adrenaline concept, added new alien sight mode, primal scream, removed flashlight battery message, fixed morphing problems for level 5 (and others), tweaked marine and alien speeds, fixed triple speed upgrade problem, disabled overwatch, moved player assets out of precache() (wasn't being called reliably), get full energy after a lifeform change, hive sight only draws out of sight now, added scent of fear
//
// Revision 1.44  2002/06/25 18:13:57  Flayra
// - Added death animations, added more general animation support, leaps do damage, general construct effects, new alien inventory system, added charging, info_locations, galloping, new alien building code, better morphing system (prevents getting stuck), more builder error messages, clear deaths on game end, hide health while gestating
//
// Revision 1.43  2002/06/10 20:03:13  Flayra
// - Updated for new minimap (remember killed position).  Updated blood so marines aren't bloody, and aliens emit yellow blood.  Removed slowing when hit (now players fly back when hit though), UpdateBlips() hack (fix when time)
//
// Revision 1.42  2002/05/28 18:03:41  Flayra
// - Refactored size for role code for movement chambers, deal with inventory properly (entity leak), increased web ensnaring effects (put weapon away!), reinforcement refactoring, tweaked speeds so marines are a little slower, and speed upgrade works properly again (now level 1 can generally outrun marines), added recycling support, play destroy egg sound when killed when morphing, new hive sight, EffectivePlayerClassChanged() refactoring
//
// Revision 1.41  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHPLAYER_H
#define AVHPLAYER_H

#include "dlls/extdll.h"
#include "dlls/util.h"

#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/trains.h"
#include "dlls/nodes.h"
#include "dlls/weapons.h"
#include "dlls/soundent.h"
#include "dlls/monsters.h"
#include "engine/shake.h"
#include "dlls/decals.h"
#include "dlls/gamerules.h"
#include "mod/AvHConstants.h"
#include "mod/AvHTeam.h"
#include "mod/AvHMessage.h"
#include "mod/AvHSharedTypes.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHEntityHierarchy.h"
#include "mod/AvHOrder.h"
#include "mod/AvHSpecials.h"
#include "common/usercmd.h"
#include "types.h"
#include "mod/AvHTechTree.h"
#include "common/weaponinfo.h"
#include "mod/AvHVisibleBlipList.h"
#include "mod/AvHBaseInfoLocation.h"
#include "mod/AvHCloakable.h"
#include "mod/AvHMessageList.h"
#include "util/Balance.h"

#include <set> //for balance information below...

class AvHPlayer : public CBasePlayer, public AvHCloakable, public BalanceChangeListener
{
public:
	// AvHPlayer stuff
    AvHPlayer();

	void			AddDebugEnemyBlip(float inX, float inY, float inZ);
	void			PrintWeaponListToClient(CBaseEntity *theAvHPlayer);
    virtual void    AddPoints( int score, BOOL bAllowNegativeScore );
	
	virtual void	AwardKill( entvars_t* pevTarget);
	virtual int		BloodColor(void);
	bool			BuildTech(AvHMessageID inBuildID, const Vector& inWorldPos);
	void			ClearBlips();
	void			ClientDisconnected();
	bool			DropItem(const char* inItemName = NULL);
	bool			GroupMessage(AvHMessageID inGroupMessage);
	bool			GetCenterPositionForGroup(int inGroupNumber, float& outX, float& outY);
	
	virtual void	GetAnimationForActivity(int inActivity, char outAnimation[64], bool inGaitSequence = false);

	bool			GetInReadyRoom(void) const;
	AvHPlayer*		GetCommander(void);
	AvHPlayMode		GetPlayMode(bool inIncludeSpectating = false) const;
	bool			GetHasLeftReadyRoom() const;
	bool			GetHasJetpack() const;
	bool			GetHasHeavyArmor() const;
	bool			GetHasAvailableUpgrades() const;
	bool			GetHasPowerArmor() const;
	int				GetHull() const;
	virtual int		GiveAmmo( int iAmount, char *szName, int iMax );
	bool			GetShouldResupplyAmmo();
	float			GetCurrentWeaponAmmoPercentage();
	virtual int		GetMaxWalkSpeed() const;
    int             GetScore() const;
    void            SetScore(int inScore);
	void			PickSkin();
	void			SetSkin(int inSkin);
	bool			GetCanCommand(string& outErrorMessage);
	bool			GetCanReceiveResources() const;

	void			SetPlayMode(AvHPlayMode inPlayMode, bool inForceSpawn = false);
	bool			GetHasBeenSpectator(void) const;
	void			InitializeFromTeam(float inHealthPercentage = 1.0f, float inArmorPercentage = 1.0f);
	bool			GetIsAlien(bool inIncludeSpectating = false) const;
	bool			GetIsMarine(bool inIncludeSpectating = false) const;
	bool			GetIsInTopDownMode(bool inIncludeSpectating = false) const;
	bool			GetIsBeingDigested() const;
	bool			GetIsDigesting() const;
	bool			GetIsEntityInSight(CBaseEntity* inEntity);
	bool			GetIsValidReinforcementFor(AvHTeamNumber inTeam) const;
	AvHTeamNumber	GetTeam(bool inIncludeSpectating = false) const;
	float			GetKilledX() const;
	float			GetKilledY() const;
	AvHTeam*		GetTeamPointer(bool inIncludeSpectating = false) const;
	bool			GetIsAlienSightActive() const;
	
	AvHClassType	GetClassType(void) const;
	
	bool			GetCurrentWeaponCannotHolster() const;
	//Activity		GetDeathActivity(void);
	bool			GetEnemySighted(void) const;
	bool			GetIsFiring(void) const;
	bool			GetIsInOverwatch(void) const;
	bool			GetIsSpectatingTeam(AvHTeamNumber inTeamNumber) const;
	bool			GetIsSpectatingPlayer(int inPlayerNumber) const;
	bool			GiveCombatModeUpgrade(AvHMessageID inMessageID, bool inInstantaneous = false);
	bool			GetHasCombatModeUpgrade(AvHMessageID inMessageID) const;

	bool			GetIsRelevant(bool inIncludeSpectating = false) const;
	bool			GetCanBeAffectedByEnemies() const;
	bool			GetIsSelected(int inEntityIndex) const;
	bool			RemoveSelection(int inEntityIndex);
	void			SetSelection(int inEntityIndex, bool inClearPreviousSelection = true);
	bool			GetIsMetabolizing() const;
	void			SetTimeOfMetabolizeEnd(float inTime);
	virtual float	GetOpacity() const;
	bool			GetIsSpectator() const;
	void			SetIsSpectator();
	float			GetLastTimeInCommandStation() const;
	
	bool			GetIsSpeaking(void) const;
	AvHMessageID	GetLastSaying() const;
	bool			GetOrdersRequested(void) const;
	bool			GetOrderAcknowledged(void) const;
	string			GetPlayerName() const;

	bool			GetHasGivenOrder() const;
	void			SetHasGivenOrder(bool inState);

	int				GetPointValue(void) const;
	vec3_t			GetVisualOrigin() const;
	int				GetRespawnCost() const;
	
    void            AwardExperienceForObjective(float inHealthChange, AvHMessageID inMessageID);
	float			GetExperience() const;
	void			SetExperience(float inExperience);
	int				GetExperienceLevel() const;

	AvHServerPlayerData* GetServerPlayerData();

	virtual bool	GetHasItem(const char *szName);
	virtual void	GiveNamedItem(const char *szName, bool inSendMessage = false);
	int				GetNumberOfItems();
	void			GiveResources(float inResources);
	float			GetTimeStartedTopDown() const;
	float			GetTimeOfLastConstructUse() const;
	void			SetTimeOfLastConstructUse(float inTime);
	float			GetTimeOfLastSignificantCommanderAction() const;
	void			LogEmitRoleChange();
	void			LogPlayerAction(const char* inActionDescription, const char* inActionData);
	void			LogPlayerActionPlayer(CBasePlayer* inAttackingPlayer, const char* inAction);
	void			LogPlayerAttackedPlayer(CBasePlayer* inAttackingPlayer, const char* inWeaponName, float inDamage);
	void			LogPlayerKilledPlayer(CBasePlayer* inAttackingPlayer, const char* inWeaponName);
		
	void			StartLeap();

	AvHUser3		GetPreviousUser3(bool inIncludeSpectating = false) const;
	AvHUser3		GetUser3(bool inIncludeSpectating = false) const;
	AvHMessageID	GetEvolution(bool inIncludeSpectating = false) const;
	bool			GetSpecialPASOrigin(Vector& outOrigin);
	void			GiveTeamUpgrade(AvHMessageID inUpgrade, bool inGive = true);
	bool			HolsterWeaponToUse();
	void			Kick();
	virtual void	Killed( entvars_t *pevAttacker, int iGib );
	void			NextWeapon();
	void			ResetEntity(void);
    bool            PayPurchaseCost(int inCost);
	void			PlaybackNumericalEvent(int inEventID, int inNumber);
	bool			PlayHUDSound(AvHHUDSound inSound) const;
    bool			PlayHUDSound(AvHHUDSound inSound, float x, float y) const;
	void			PlayHUDStructureNotification(AvHMessageID inMessageID, const Vector& inLocation);
	void			PlayRandomRoleSound(string inSoundListName, int inChannel = CHAN_AUTO, float inVolume = 1.0f);
	void			PlayerConstructUse();
	void			SetCurrentCommand(const struct usercmd_s* inCommand);
	void			SetDebugCSP(weapon_data_t* inWeaponData);
	void			SetPendingCommand(char* inCommand);
	void			TriggerProgressBar(int inEntityID, int inParam);
	float			GetTimeOfLastTeleport() const;
	void			SetTimeOfLastTeleport(float inTime);
	bool			SwitchWeapon(const char* inString);
	virtual void	StartObserver( Vector vecPosition, Vector vecViewAngle );
	virtual void	StartObservingIfNotAlready(void);
	void			StartTopDownMode(void);
	bool			StopTopDownMode(void);
	bool			SetBeingDigestedMode(bool inBeingDigested);

	bool			GetIsCloaked() const;
	bool			GetIsPartiallyCloaked() const;
	void			TriggerUncloak();

	//Nexus interface - replaces all old auth information
	bool			GetIsAuthorized(AvHAuthAction inAction, int inParameter) const;
	bool			GetIsMember(const string& inAuthGroup) const;
	//END Nexus interface

	float			GetTimeLastPlaying() const;

	bool			GetHasSeenTeam(AvHTeamNumber inNumber) const;
	void			SetHasSeenTeam(AvHTeamNumber inNumber);

	float			GetTimeOfLastSporeDamage() const;
	void			SetTimeOfLastSporeDamage(float inTime);

	// Quick ensnare system...add something better?
	bool			GetIsEnsnared() const;
    bool            GetIsAbleToAct() const;

	void			DropAmmo(char *pszAmmoType, int iAmmoAmt, int iMax, int iWeaponID,  Vector vecAngles);
	void			EffectivePlayerClassChanged();
	void			NeedsTeamUpdate();
	void			SendTeamUpdate();

	// Returns true if successful.  Fails if the player is too ensnared already
	bool			SetEnsnareState(bool inState);

	bool			GetIsStunned() const;
	bool            SetIsStunned(bool inState, float inTime = 0.0f);

    bool            GetIsCatalysted() const;
    void            SetIsCatalysted(bool inState, float inTime = 0.0f);

	bool			Energize(float inEnergyAmount);
	bool			Heal(float inAmount, bool inPlaySound = true);
	bool			Regenerate(float inRegenerationAmount, bool inPlaySound = true);
	bool			GetIsScreaming();
	void			StartScreaming();

	//void			UpgradeArmorLevel(void);

	// CBasePlayer stuff
	virtual void	ImpulseCommands( void );
	virtual void	ItemPostFrame(void);
	virtual void	Jump( void );
	virtual void	ObserverModeIllegal();
	virtual void	PackDeadPlayerItems(void);
	virtual void	PreThink( void );
	virtual void	Spawn( void );
	//virtual BOOL	SwitchWeapon( CBasePlayerItem* pWeapon );
	virtual void    TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
    virtual int		TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	//virtual const char*	TeamID( void );
	virtual void	UpdateClientData( void );

	virtual float	GetCloakTime() const;
    virtual int     GetExperienceLevelsSpent() const;
    virtual void    SetExperienceLevelsSpent(int inSpentLevels);

    virtual string  GetNetworkID() const;
    virtual void    SetNetworkID(string& inNetworkID);

	virtual string	GetNetworkAddress() const;
	virtual void	SetNetworkAddress(string& inNetworkAddress);

	virtual bool	GetIsTemporarilyInvulnerable() const;
	AvHTechTree&	GetCombatNodes();
	MessageIDListType& GetPurchasedCombatUpgrades();
	bool			Redeem();
    
	bool            GetCanBeResupplied() const;
    bool			Resupply(bool inGiveHealth = false);

	void			SetDesiredNetName(string inDesiredNetName);
	void			SetDesiredRoomType(int inRoomType, bool inForceUpdate = false);

    // Gets origin of the player after changing the iuser3.
    void            GetNewOrigin(AvHUser3 inNewUser3, bool inDucking, vec3_t& outOrigin) const;

	float			GetResources(bool inIncludeSpectating = false) const;
    void			SetUser3(AvHUser3 inUser3, bool inForceChange = false, bool inGiveWeapons = true);
    void			SetResources(float inResources, bool inPlaySound = false);

	// Send messages to player's screen
	bool			SendMessage(const char* pMessage, bool inIsToolTip = false);
	bool			SendMessageOnce(const char* pMessage, bool inIsToolTip = false);
	bool			SendMessageNextThink(const char* pMessage);

	virtual int		GetEffectivePlayerClass();

	void			BecomePod();
	void			SetModelFromState();

	int				GetDigestee() const;
	void			SetDigestee(int inPlayerID);
	void			StartDigestion(int inDigestee);
	void			StopDigestion(bool inDigested);
	
	void			ProcessEntityBlip(CBaseEntity* inEntity);

	void			SetPosition(const Vector& inNewPosition);
	virtual char*	GetPlayerModelKeyName();
	void			GetSize(Vector& outMinSize, Vector& outMaxSize) const;
	void			SetSizeForUser3();
	
    void            GetViewForUser3(AvHUser3 inUser3, bool inIsDucking, float& outFOV, float& outOffset) const;
    void			SetViewForUser3();
	void			SetWeaponsForUser3();

	bool			RunClientScript(const string& inScriptName);
	
	void			TriggerFog(int inFogEntity, float inFogExpireTime);

	void			UpdateInventoryEnabledState(int inNumActiveHives, bool inForceUpdate = false);

    virtual bool    GetCanUseWeapon() const;

    void            PropagateServerVariables();

	bool			GetUsedKilled()					{ return mUsedKilled; }
	void			SetUsedKilled(bool bKilled )	{ mUsedKilled = bKilled; }
	void			ClearOrders() { mClientOrders.clear(); }

	// tankefugl: 0000953 
	bool			JoinTeamCooledDown(float inCoolDownTime);
	// tankefugl
private:
	void				AcquireOverwatchTarget();
	bool				AttemptToBuildAlienStructure(AvHMessageID inMessageID);
	void				ClearRoleAbilities();
	void				ClearUserVariables();
	CBaseEntity*		CreateAndDrop(const char* inItemName);
	void				DeployCurrent();
	bool				ExecuteAlienMorphMessage(AvHMessageID inMessageID, bool inInstantaneous);
    bool				ExecuteCombatMessage(AvHMessageID inMessageID, bool& outIsAvailable, bool inForce = false);
	bool				ExecuteMessage(AvHMessageID inMessageID, bool inInstantaneous = false, bool inForce = false);
	float				GetAlienAdjustedEventVolume() const;
	bool				GetCanGestate(AvHMessageID inMessageID, string& outErrorMessage);
	bool				GetDoesCurrentStateStopOverwatch() const;
	bool				QueryEnemySighted(CBaseEntity* inEntity);
	bool				GetHasActiveAlienWeaponWithImpulse(AvHMessageID inMessageID) const;
	bool				GetRandomGameStartedTick(float inApproximateFrameRate);
	bool				GetPurchaseAllowed(AvHMessageID inUpgrade, int& outCost, string* outErrorMessage = NULL) const;
	int					GetRelevantWeight(void) const;
	int					GetRelevantWeightForWeapon(AvHBasePlayerWeapon* inWeapon) const;
	void				GetSpeeds(int& outBaseSpeed, int& outUnemcumberedSpeed) const;
	void				GiveCombatUpgradesOnSpawn();
	bool				GiveOrderToSelection(AvHOrderType inOrder, Vector inNormRay);
	void				GiveOrderToSelection(AvHOrder& inOrder);
	void				GiveUpgrade(AvHMessageID inUpgrade);
	void				HandleOverwatch(void);
	void				HandleResearch();
	void				HolsterCurrent();
	void				InitializeTechNodes();
	void				HandleTopDownInput();
	void				ProcessEvolution();
	void				ProcessCombatDeath();
	void				ProcessResourceAdjustment(AvHMessageID inMessageID);
	void				ResetBehavior(bool inRemoveFromTeam);
	void				ResetGameNewMap();
	void				ResetPlayerPVS();
	void				SetCombatNodes(const AvHTechTree& inTechNodes);
	void				SetLifeformCombatNodesAvailable(bool inAvailable);
	void				ValidateClientMoveEvents();
	
	void				InternalAlienThink();
	void				InternalAlienUpgradesThink();
	void				InternalAlienUpgradesCloakingThink();
	void				InternalAlienUpgradesPheromonesThink();
	void				InternalAlienUpgradesRegenerationThink();
	void				InternalCommanderThink();
	void				InternalBoundResources();
	void				InternalCommonThink();
	void				InternalCombatThink();
	void				InternalDigestionThink();
	void				InternalEnemySightedPreThink();
	void				InternalFogThink();
	void				InternalHUDThink();
	void				InternalMarineThink();
	void				InternalPreThink();
	void				InternalProgressBarThink();
	void				InternalSpeakingThink();

	void EXPORT			PlayerTouch(CBaseEntity* inOther);
	
	bool				PlaySaying(AvHMessageID inMessageID);				
	bool				ProcessClickEvent(bool inLMB, float inWorldX, float inWorldY);
	void				ProcessTechUpgrade(AvHMessageID inMessageID);
	void				Research(AvHMessageID inMessageID, int inEntityIndex);
	void				Evolve(AvHMessageID inMessageID, bool inInstantaneous = false);
    void				PurchaseCombatUpgrade(AvHMessageID inMessageID);
	void				RecalculateSpeed(void);
	void				ReloadWeapon(void);
    void                RemoveCombatUpgrade(AvHMessageID inMessageID);
    void                RemoveCombatUpgradesPremptedBy(AvHMessageID inMessageID);
	void				ResetOverwatch();
	void				RevertHealthArmorPercentages();
	void				SaveHealthArmorPercentages();
	void				SetMoveTypeForUser3();
	//bool				SpawnReinforcements(void);
	void				TurnOverwatchTowardsTarget(CBaseEntity* theTarget);
	void				TurnOffOverwatch();
	void				TurnOnOverwatch();
	void				UpdateAmbientSounds();

	//void				UpdateArmor();
	void				UpdateAlienUI();
	void				UpdateBlips();
	void				UpdateDebugCSP();
	void				UpdateEffectiveClassAndTeam();
	void				UpdateEntityHierarchy();
	void				UpdateExperienceLevelsSpent();
	void				UpdateFirst();
	void				UpdateFog();
	void				UpdateGamma();
	void				UpdateInfoLocations();
	void				UpdateParticleTemplates();
	void				UpdateOrders();
	void				UpdateOverwatch();
	void				UpdatePendingClientScripts();
	void				UpdateProgressBar();
	void				UpdateVUser4();
	void				UpdateSetSelect();
	void				UpdateSpawnScreenFade();
	void				UpdateSoundNames();
	void				UpdateTechNodes();
	void				UpdateTopDownMode();

	void				Init();

	//BalanceChangeListener implementation
	void				InitBalanceVariables(void);
	void				UpdateBalanceVariables(void);
	bool				shouldNotify(const string& name, const BalanceValueType type) const;
	void				balanceCleared(void) const;
	void				balanceValueInserted(const string& name, const float value) const;
	void				balanceValueInserted(const string& name, const int value) const;
	void				balanceValueInserted(const string& name, const string& value) const;
	void				balanceValueChanged(const string& name, const float old_value, const float new_value) const;
	void				balanceValueChanged(const string& name, const int old_value, const int new_value) const;
	void				balanceValueChanged(const string& name, const string& old_value, const string& default_value) const;
	void				balanceValueRemoved(const string& name, const float old_value) const;
	void				balanceValueRemoved(const string& name, const int old_value) const;
	void				balanceValueRemoved(const string& name, const string& old_value) const;

    float				mResources;

	bool				mFirstUpdate;
	bool				mNewMap;

	bool				mHasSeenTeamA;
	bool				mHasSeenTeamB;

	string				mQueuedThinkMessage;

	bool				mClientInOverwatch;
	bool				mInOverwatch;
	bool				mOverwatchEnabled;
	int					mOverwatchTarget;
	float				mTimeOfLastOverwatchPreventingAction;
	float				mTimeLastSeenOverwatchTarget;
	Vector				mOverwatchFacing;
	bool				mOverwatchFiredThisThink;

	// tankefugl: 0000953
	float				mTimeLastJoinTeam;
	// tankefugl

	// alien upgrades
	float				mTimeOfLastRegeneration;
	float				mTimeOfLastPheromone;

	float				mTimeOfLastUse;

	float				mTimeLeapEnd;

	Vector				mPositionBeforeTopDown;
	Vector				mAnglesBeforeTopDown;
	Vector				mViewAnglesBeforeTopDown;
	Vector				mViewOfsBeforeTopDown;
	string				mAnimExtensionBeforeTopDown;
	bool				mClientInTopDownMode;
	bool				mInTopDownMode;
	int					mTimeStartedTopDown;
	
	float				mTimeOfLastSaying;
	AvHMessageID		mLastSaying;
	bool				mIsSpeaking;
	bool				mOrdersRequested;
	bool				mOrderAcknowledged;
	float				mTimeOfLastEnemySighting;
	bool				mEnemySighted;

	bool				mTriggerUncloak;

	bool				mHasLeftReadyRoom;
	bool				mHasBeenSpectator;
	char*				mPendingCommand;

	struct usercmd_s	mCurrentCommand;
	bool				mAttackOneDown;
	bool				mAttackTwoDown;
	Vector				mAttackOnePressedWorldPos;
	Vector				mAttackTwoPressedWorldPos;
	Vector				mMouseWorldPos;
	bool				mPlacingBuilding;

	EntityListType		mSelected;
	EntityListType		mClientSelected;

	EntityInfo			mTrackingEntity;
	EntityInfo			mClientTrackingEntity;

	EntityListType		mClientGroups[kNumHotkeyGroups];
	AvHAlertType		mClientGroupAlerts[kNumHotkeyGroups];
	int					mClientRequests[kNumRequestTypes];
	EntityListType		mClientSelectAllGroup;

	OrderListType		mClientOrders;

    AvHEntityHierarchy	mClientEntityHierarchy;

	// Research nodes for commander
	AvHTechTree			mClientTechNodes;
	MessageIDListType	mClientTechDelta;
	AvHTechSlotListType	mClientTechSlotList;

	AvHMessageID		mClientResearchingTech;

	float				mClientGamma;

	StringList			mSentMessageList;

	AvHVisibleBlipList	mFriendlyBlips;
	AvHVisibleBlipList	mEnemyBlips;

	AvHVisibleBlipList	mClientFriendlyBlips;
	AvHVisibleBlipList	mClientEnemyBlips;
	
	int					mClientCommander;

	Vector				mSpecialPASOrigin;
	Vector				mClientSpecialPASOrigin;
	float				mTimeOfLastPASUpdate;

	bool				mClientIsAlien;

	bool				mAlienSightActive;
	
	float				mTimeOfLastTeleport;
    float               mTimeOfLastRedeem;

	float				mJetpackEnergy;
	bool				mJetpacking;

	weapon_data_t		mClientDebugCSPInfo;
	weapon_data_t		mDebugCSPInfo;
	float				mClientNextAttack;

	float				mTimeToBeUnensnared;
	float				mLastTimeEnsnared;
	int					mMaxWalkSpeed;
	int					mMaxGallopSpeed;
	vec3_t				mLastGallopViewDirection;

	float				mTimeToBeFreeToMove;
    float               mTimeToEndCatalyst;

	float				mLastTimeInCommandStation;
	float				mLastTimeCheckedRedemption;
	float				mLastTimeRedemptionTriggered;

	float				mLastTimeStartedPlaying;

	float				mTimeOfLastHelpText;
	bool				mLastHelpWasGeneral;

	float				mLastPowerArmorThink;
	float				mLastInventoryThink;

	int					mNumHives;

	float				mTimeLastPlaying;

	float				mTimeGestationStarted;

	AvHUser3			mPreviousUser3;
    float               mSavedJetpackEnergy;

	AvHMessageID		mEvolution;
	float				mHealthPercentBefore;
	float				mArmorPercentBefore;

	StringList			mClientSoundNames;

	bool				mIsScreaming;
	float				mTimeStartedScream;

	float				mKilledX, mKilledY;

	int					mNumParticleTemplatesSent;
	float				mTimeOfLastParticleTemplateSending;

	int					mClientProgressBarEntityIndex;
	int					mProgressBarEntityIndex;
	int					mProgressBarParam;
	float				mTimeProgressBarTriggered;

	float				mTimeOfLastFogTrigger;
	float				mFogExpireTime;
	int					mCurrentFogEntity;
	int					mClientCurrentFogEntity;

	AvHBaseInfoLocationListType	mClientInfoLocations;

	HiveInfoListType	mClientHiveInfo;

	AvHAlienUpgradeListType		mClientUpgrades;

	StringList			mPendingClientScripts;

	bool				mHasGivenOrder;
	float				mTimeOfLastSignificantCommanderAction;

	int					mPreThinkTicks;
	float				mPreThinkFrameRate;

	string				mDesiredNetName;

	int					mClientMenuTechSlots;

	float				mTimeOfLastClassAndTeamUpdate;
	bool				mEffectivePlayerClassChanged;
	bool				mNeedsTeamUpdate;
	bool				mSendTeamUpdate;
	bool				mSendSpawnScreenFade;

	int					mDigestee;
	float				mTimeOfLastDigestDamage;
	float				mTimeOfLastCombatThink;

	int					mDesiredRoomType;
	int					mClientDesiredRoomType;

	float				mTimeOfLastConstructUseAnimation;
	float				mTimeOfLastConstructUse;

	float				mTimeOfLastResupply;

	float				mTimeOfMetabolizeEnd;

	AvHMessageID		mLastSelectEvent;
	Vector				mPositionBeforeLastGotoGroup;

	float				mTimeOfLastSporeDamage;
	float				mTimeOfLastTouchDamage;

	string				mLastMessageSent;

	float				mExperience;
	float				mClientPercentToNextLevel;
	int					mExperienceLevelsSpent;
	int					mClientExperienceLevelsSpent;

	MessageIDListType	mPurchasedCombatUpgrades;
    MessageIDListType	mGiveCombatUpgrades;

	AvHTechTree			mCombatNodes;

    int                 mScore;
    int                 mSavedCombatFrags;

	string				mNetworkAddress;
    int                 mLastModelIndex;
    
    string              mNetworkID;
	
    struct ServerVariable
    {
        std::string mName;
        std::string mLastValueSent;
    };

    std::vector<ServerVariable> mServerVariableList;

	bool				mUsedKilled;

	//TODO: remove this system from AvHPlayer and create an 
	// explicit balance forwarding class registered to each
	// client instead.  This functionality is tangential to
	// AvHPlayer's role as a game entity and AvHPlayer has
	// far too much responsibility (included in over 60 source
	// files?!?).  Other functionality should also be examined
	// and refactored if appropriate.
	mutable bool					mBalanceMessagePending; //are we in the middle of a set of changes?
	mutable std::set<string>		mBalanceRemovalList;
	mutable BalanceFloatCollection	mBalanceMapFloats;
	mutable BalanceIntCollection	mBalanceMapInts;
	mutable BalanceStringCollection	mBalanceMapStrings;
	float							mNextBalanceVarUpdate;
};

typedef vector<AvHPlayer*>	PlayerListType;

#endif

