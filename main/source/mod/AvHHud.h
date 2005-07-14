//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Main NS HUD, also interface to client network messages
//
// $Workfile: AvHHud.h $
// $Date: 2002/10/24 21:29:49 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHud.h,v $
// Revision 1.60  2002/10/24 21:29:49  Flayra
// - Moved help client-side
// - Fixed particle/changelevel crash
// - Reworked marine upgrade drawing
// - Added lots of utility functions for help system (mirrors functions on server)
// - Removed gamma message unless it failed or if maxplayers is 1
// - Fixed alien hive sight crash
// - Show players under reticle while in ready room and spectating
// - Removed ugly/too-prevalent user3 icons
//
// Revision 1.59  2002/10/03 20:24:39  Flayra
// - Changes for "more resources required"
//
// Revision 1.58  2002/10/03 18:54:31  Flayra
// - Allow right-click to cancel building placement
// - Fixed help icons
// - Added a couple utility functions
// - Reworked order notification
// - Reworked blip network messages to avoid hard-coded limit
// - Sent max resources down with current resources
// - Countdown sound no longer prevents other hud sounds
// - Alien trigger sounds
// - New order sounds
// - No longer disable nodes out of our cost range
//
// Revision 1.57  2002/09/25 20:47:22  Flayra
// - Don't draw elements on HUD when dead
// - UI refactoring
// - Split reticle help into help text and reticle text
// - Removed use order
// - Added separate select sound for alien
// - Multiple move sounds
// - Only draw entity build/health status when under reticle (no more scanning around you)
// - Added 3 new sayings
//
// Revision 1.56  2002/09/23 22:18:25  Flayra
// - Added alien build circles
// - Game status changes so particles aren't sent every time
// - Demo playback changes (save restore basic data that HUD already has)
// - New alert sounds
// - Skin support
//
// Revision 1.55  2002/09/09 19:55:24  Flayra
// - Added hive info indicator
// - Fixed bug where reticle tooltip help text wasn't being set until a weapon was selected
// - Fixed release mode bug where tooltips weren't expiring
// - Fixed bug where marine upgrades blinked
// - "No commander" indicator now blinks
//
// Revision 1.54  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.53  2002/08/16 02:37:49  Flayra
// - HUD sounds no longer cut each other off (they won't play instead of cutting off another sound)
// - Tooltip sounds
// - Selection issues
// - Draw rings around buildings that need to be built
// - Removed old overwatch code
//
// Revision 1.52  2002/08/09 01:02:51  Flayra
// - Added hooks for demo playback, removed prediction selection
//
// Revision 1.51  2002/08/02 22:51:28  Flayra
// - Fixed memory overwrite...eek!
//
// Revision 1.50  2002/08/02 21:59:12  Flayra
// - Added reticle help, new tooltip system and much nicer order drawing!  Refactored view model drawing a bit, hoping to make texture blending work for it.
//
// Revision 1.49  2002/07/26 23:05:01  Flayra
// - Generate numerical feedback for damage events
// - Refactoring for more info when looking at something (instead of bad-looking player names only)
//
// Revision 1.48  2002/07/23 17:07:37  Flayra
// - Added visually-smooth energy level, added versatile location code, new hive sight info, refactored to remove extra sprites (128 HUD sprites bug), commander tech help fixes
//
// Revision 1.47  2002/07/08 17:07:56  Flayra
// - Started to add display of marine upgrade sprite, fixed bug where building indicators aren't displayed after a map change, info_location drawing changes, primal scream color tweak, removed old hive drawing code
//
// Revision 1.46  2002/07/01 21:35:05  Flayra
// - Removed lots of outdated sprites and sprite code, added building ranges, fixed ghost building problem (bug #82)
//
// Revision 1.45  2002/06/25 18:03:09  Flayra
// - Added info_locations, removed old weapon help system, added smooth resource swelling, lots of alien UI usability changes, fixed problem with ghost building
//
// Revision 1.44  2002/06/10 19:55:36  Flayra
// - New commander UI (bindable via hotkeys, added REMOVE_SELECTION for when clicking menu options when no players selected)
//
// Revision 1.43  2002/06/03 16:48:45  Flayra
// - Help sprites moved into one animated sprite, select sound volume reduced (now that sound is normalized)
//
// Revision 1.42  2002/05/28 17:48:14  Flayra
// - Minimap refactoring, reinforcement refactoring, new hive sight fixes, recycling support
//
// Revision 1.41  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHHUD_H
#define AVHHUD_H

#include "ui/UIHud.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHSharedTypes.h"
#include "mod/AvHParticleSystem.h"
#include "common/entity_state.h"
#include "VGUI_ProgressBar.h"
#include "mod/AvHEntityHierarchy.h"
#include "ui/MarqueeComponent.h"
#include "mod/AvHOrder.h"
#include "mod/AvHMessage.h"
#include "mod/AvHAmbientSound.h"
#include "mod/AvHTechTree.h"
#include "mod/AvHVisibleBlipList.h"
#include "mod/AvHMapExtents.h"
#include "mod/AvHSpecials.h"
#include "util/GammaTable.h"
#include "mod/AvHBaseInfoLocation.h"
#include "mod/AvHTooltip.h"
#include "mod/AvHTechSlotManager.h"
#include "mod/AvHHudConstants.h"
#include "mod/AvHOverviewMap.h"

class AvHTeamHierarchy;
class PieMenu;
class FadingImageLabel;
using vgui::Cursor;

struct AnimatedSprite
{
	float mCurrentFrame;
	int mData;
};

class SelectionEffect
{
public:
	SelectionEffect() : mEntIndex(-1), mAngleOffset(0)
				{}
	
	int		mEntIndex;
	int		mAngleOffset;
};

typedef enum
{
	MAIN_MODE,
	EDITPS_MODE,
	SCOREBOARD_MODE
} UIMode;

typedef vector<SelectionEffect>		SelectionListType;

class NumericalInfoEffect
{
public:
			NumericalInfoEffect(float inPosition[3], float inNumber, int inEventType, float inTimeCreated);
	void	GetPosition(float* outPosition) const;
	float	GetNumber() const;
	int		GetEventType() const;
	float	GetTimeCreated() const;
	void	SetPosition(float inPosition[3]);
	
private:
	float	mPosition[3];
	float	mNumber;
	int		mEventType;
	float	mTimeCreated;
};

const int kNumUpgradeLines = 5;

class AvHHud : public UIHud
{
public:
					AvHHud(const string& inFilename, UIFactory* inFactory);
    virtual			~AvHHud(void);

    void            OnActivateSteamUI();
    void            OnDeactivateSteamUI();
    void            OnLostFocus();
    bool            OnKeyEvent(int virtualKey, int scanCode, bool pressed);

	void            AddMiniMapAlert(float x, float y);
	void			AddNumericalInfoMessage(float inOrigin[3], float inNumber, int inEventType);
	void			AddTooltip(const char* inMessageText, bool inIsToolTip = true, float inTooltipWidth = kToolTipMaxWidth);
	bool			AddTooltipOnce(const char* inMessageText, bool inIsToolTip = true);
	void			Cancel();
	
	void			ClearSelection();
	
	void			ClientProcessEntity(struct entity_state_s* inEntity);
	
	void			ComponentJustPainted(Panel* inPanel);

	bool			GetAndClearAlienAbility(AvHMessageID& outMessageID);
	bool			GetAndClearGroupEvent(AvHMessageID& outMessageID);
	bool			GetAndClearTechEvent(AvHMessageID& outMessageID);
	bool			GetLastHotkeySelectionEvent(AvHMessageID& outMessageID);
	void			SetLastHotkeySelectionEvent(AvHMessageID inMessageID);
	
    // Returns true if particles should be rendered in the HUD.
    bool            GetParticlesVisible() const;
    bool			GetSafeForSpriteDrawing() const;

	void			ClearTrackingEntity();
	int				GetTrackingEntity() const;

	bool			GetIsRegionBlockedByUI(float inNormX, float inNormY);
	
	//int				GetArmorLevel(void) const;
    int             GetCommanderIndex() const;
	bool			GetHasJetpack() const;
	int				GetHelpIconFrameFromUser3(AvHUser3 inUser3);
	HSPRITE			GetHelpSprite() const;
	bool			GetHasAlienUpgradesAvailable() const;
	bool			GetIsAlien() const;
	bool			GetIsBeingDigested() const;
    bool            GetIsEnsnared() const;
	bool			GetIsDigesting() const;
    bool            GetIsStunned() const;
    bool            GetIsNotInControl() const;
	bool			GetIsInTopDownMode() const;
	bool			GetIsMarine() const;
	bool			GetIsRelevant() const;
	bool			GetIsShowingMap() const;
	AvHPlayMode		GetPlayMode(void) const;
	bool			GetAlienHelpForMessage(int inMessageID, string& outHelpText, int& outPointCost) const;
	bool			GetDoesPlayerHaveOrder() const;
	bool			GetHelpForMessage(int inMessageID, string& outHelpText) const;
	bool			GetInTopDownMode() const;
	bool			GetIsSelecting() const;
	OrderListType	GetOrderList() const;
	AvHOrderType	GetOrderMode() const;
	bool			GetCenterPositionForGroup(int inGroupNumber, vec3_t& outCenterPosition) const;
	bool			GetMouseOneDown() const;
	bool			GetMouseTwoDown() const;
	bool			GetAndClearTopDownScrollAmount(int& outX, int& outY, int& outZ);
	bool			GetAndClearSelectionEvent(vec3_t& outSelectionXY, AvHMessageID& outMessageID);
	EntityListType	GetSelected() const;
	const AvHTechSlotManager&	GetTechSlotManager() const;
	bool			GetTranslatedUser3Name(AvHUser3 inUser3, string& outString) const;
	bool			GetTranslatedUser3Description(AvHUser3 inUser3, bool inFriendly, string& outString) const;
	vec3_t			GetVisualOrigin() const;
	
	AvHMessageID	HotKeyHit(char inChar);
	
    virtual void	Init(void);
	virtual void	PostUIInit(void);
	virtual void	VidInit(void);
	
	bool			GetGameStarted() const;
	int				GetGameTime() const;
	int				GetGameTimeLimit() const;
	int				GetCombatAttackingTeamNumber() const;
	static bool		GetShowingMap();

    static void     PlayStream();
    static void     StopStream();

	bool			GetIsAlive(bool inIncludeSpectating = true) const;
	void			GhostBuildingCallback( struct tempent_s *ent, float frametime, float currenttime);
    void            CancelBuilding();

	void			PlayHUDSound(const char *szSound, float vol, float inSoundLength = 0.0f);
	void			PlayHUDSound(int iSound, float vol, float inSoundLength = 0.0f);
	void			PlayHUDSound(AvHHUDSound inSound);
	
	float			GetHUDExperience() const;
	int				GetHUDExperienceLevel() const;
	float			GetHUDHandicap() const;
	AvHUser3		GetHUDUser3() const;
	AvHPlayMode		GetHUDPlayMode() const;
	AvHTeamNumber	GetHUDTeam() const;
	int				GetHUDUpgrades() const;
	int				GetHUDMaxArmor() const;
	int				GetHUDMaxHealth() const;
	void			GetPrimaryHudColor(int& outR, int& outG, int& outB, bool inIgnoreUpgrades = false, bool gammaCorrect = true) const;
	float			GetTimeOfLastUpdate() const;
	int				GetMenuTechSlots() const;

	//void			GetVisibleBlips(VisibleBlipListType& outBlipList);
	
    virtual int		Redraw( float flTime, int intermission );
	virtual void	ResetComponentsForUser3();
	void			SetSelectingWeaponID(int inWeaponID, int inR = -1, int inG = -1, int inB = -1);
	void			SetTechHelpText(const string& inTechHelpText);
	void			DrawSelectionCircleOnGroundAtPoint(vec3_t inOrigin, int inRadius);
	// tankefugl: 0000988 
	void			DrawBuildHealthEffectsForEntity(int inEntityIndex, float inAlpha = 1.0f);
	// :tankefugl
	void			DrawSelectionAndBuildEffects();
	void			DrawHUDNumber(int inX, int inY, int inFlags, int inNumber);

	int				InitializeDemoPlayback(int inSize, unsigned char* inBuffer);
	int				InitializeDemoPlayback2(int inSize, unsigned char* inBuffer);
	int				InitializeWeaponInfoPlayback(int inSize, unsigned char* inBuffer);
	
    virtual bool    Update(float inCurrentTime, string& outErrorString);
	virtual void	UpdateMusic(float inCurrentTime);

	bool			SlotInput(int inSlot);

	const AvHMapExtents& GetMapExtents();
	float			GetGammaSlope() const;
	string			GetMapName(bool inLocalOnly = false) const;
	int				GetMaxAlienResources() const;
	int				GetNumActiveHives() const;
	void			HideProgressStatus();
	void			SetProgressStatus(float inPercentage);

	AvHVisibleBlipList&	GetEnemyBlipList();
	AvHVisibleBlipList&	GetFriendlyBlipList();

	AvHEntityHierarchy& GetEntityHierarchy();

	int				GetLocalUpgrades() const;
	string			GetNameOfLocation(vec3_t inLocation) const;
	const AvHTechTree& GetTechNodes() const;

	UIMode			GetUIMode() const;
	bool			SwitchUIMode(UIMode inNewMode);
	bool			GetIsCombatMode() const;
    bool			GetIsNSMode() const;
    bool			GetIsScriptedMode() const;
		
	void			HideResearchProgressStatus();
	void			SetResearchProgressStatus(float inPercentage);

	AvHMessageID	GetGhostBuilding() const;
	void			SetGhostBuildingMode(AvHMessageID inGhostBuilding);

	void			SetAlienAbility(AvHMessageID inAlienAbility);
	void			SetRenderingSelectionView(bool inState);
		
	void			SetClientDebugCSP(weapon_data_t* inWeaponData, float inNextPlayerAttack);
	void			SetCurrentWeaponData(int inCurrentWeaponID, bool inEnabled);
	int				GetCurrentWeaponID(void);

	void			DrawTopDownBG();
	void			DrawTranslatedString(int inX, int inY, const char* inStringToTranslate, bool inCentered = false, bool inIgnoreUpgrades = false, bool inTrimExtraInfo = false);
	void			HandleFog();
	void			PostModelRender(char* inModelName);
	void			PreRenderFrame();
	void			PostRenderFrame();
	void			RenderNoZBuffering();

    void            Render();
    void            RenderCommonUI();
    void            RenderMarineUI();
    void            RenderCommanderUI();
    void            RenderAlienUI();
    void            RenderMiniMap(int inX, int inY, int inWidth, int inHeight);

    void            RenderStructureRanges();
    void            RenderStructureRange(vec3_t inOrigin, int inRadius, HSPRITE inSprite, int inRenderMode = kRenderNormal, int inFrame = 0, float inR = 0, float inG = 0.5, float inB = 0, float inAlpha = 1.0f);

    void            DrawWarpedOverlaySprite(int spriteHandle, int numXFrames, int numYFrames,
                        float inWarpXAmount = 0.0f, float inWarpYAmount = 0.0f,
                        float inWarpXSpeed = 0.0f, float inWarpYSpeed = 0.0f);

	void			DrawActionButtons();
	void			DrawHotgroups();
	void			DrawPendingRequests();

	void			SetCurrentUseableEnergyLevel(float inEnergyLevel);
	
	// Network messages
	int				AlienInfo(const char* pszName, int iSize, void* pbuf);
	int				BlipList(const char* pszName, int iSize, void* pbuf);
	int				ClScript(const char *pszName, int iSize, void *pbuf);
	int				Countdown(const char* pszName, int iSize, void* pbuf);
	int				DebugCSP(const char* pszName, int iSize, void* pbuf);
	int				EditPS(const char* pszName, int iSize, void* pbuf);
	int				EntHier(const char *pszName, int iSize, void *pbuf);
	int				Fog(const char* pszName, int iSize, void* pbuf);
	int				ListPS(const char* pszName, int iSize, void* pbuf);
    int				Particles(const char *pszName, int iSize, void *pbuf);
    int				SoundNames(const char *pszName, int iSize, void *pbuf);
	int 			PlayHUDNot(const char* pszName, int iSize, void* pbuf);
	
	int				BalanceVar(const char* pszName, int iSize, void* pbuf);
    int             ServerVar(const char* pszName, int iSize, void* pbuf);

	int				GameStatus(const char* pszName, int iSize, void* pbuf);
	int				MiniMap(const char* pszName, int iSize, void* pbuf);
	int				IssueOrder(const char* pszName, int iSize, void* pbuf);
	int				LUAmsg(const char* pszName, int iSize, void* pbuf);
	int				Progress(const char* pszName, int iSize, void* pbuf);
	int				SetGmma(const char* pszName, int iSize, void* pbuf);
	int				SetSelect(const char* pszName, int iSize, void* pbuf);
	int				SetRequest(const char* pszName, int iSize, void* pbuf);
	int				SetOrder(const char* pszName, int iSize, void* pbuf);
	int				SetupMap(const char* pszName, int iSize, void* pbuf);
	int				SetTopDown(const char* pszName, int iSize, void* pbuf);
	int				SetTech(const char* pszName, int iSize, void* pbuf);
	int				TechSlots(const char* pszName, int iSize, void* pbuf);

    void            GetSpriteForUser3(AvHUser3 inUser3, int& outSprite, int& outFrame, int& outRenderMode);

    int             GetCurrentSquad() const;
    AvHOverviewMap& GetOverviewMap();

    void            ShowCrosshair();
    void            HideCrosshair();

    // This function should be used instead of the global SetCrosshair.
    void            SetCurrentCrosshair(HSPRITE hspr, wrect_t rc, int r, int g, int b);

	static void		ResetGammaAtExit();
	static int		ResetGammaAtExitForOnExit();
	static void		ResetGammaAtExit(int inSig);
 
    void            SetViewport(const int inViewport[4]);
    void            GetViewport(int outViewport[4]) const;

    const AvHFont&  GetSmallFont() const;
	cl_entity_s*	GetVisiblePlayer() const;

    float           GetServerVariableFloat(const char* inName) const;

	// tankefugl:
	void			SetCenterText(const char* inText);
	void			DrawCenterText();
	void			ClearCenterText();
	// :tankefugl

private:

	// tankefugl:
	std::string		mCenterText;
	float			mCenterTextTime;
	// :tankefugl

    bool            GetCommanderLabelText(std::string& outCommanderName) const;

	void			AddCommands();
	void			ClearData();
	void			DisplayCombatUpgradeMenu(bool inVisible);
	void			DrawMouseCursor(int inBaseX, int inBaseY);
	void			DrawOrders();
	void			DrawHelpIcons();
	// tankefugl: 0000971
	void			GetOrderDirection(vec3_t inTarget, int inOrderType);
	void			DrawTeammateOrders();
	// tankefugl: 0000992
	void			DrawDisplayOrder();
	void			SetDisplayOrder(int inOrderType, int inOrderIndex, string inText1, string inText2, string inText3);
	// :tankefugl
	void			DrawHUDStructureNotification();
	void			DrawInfoLocationText();
	void			DrawPlayerNames();
	void			DrawReticleInfo();
	void			DrawToolTips();
	// tankefugl: 0000971 -- added inAlpha
	void			DrawWorldSprite(int inSpriteHandle, int inRenderMode, vec3_t inWorldPosition, int inFrame, float inWorldSize, float inAlpha = 1.0f);
	// :tankefugl
	void			DrawOrderIcon(const AvHOrder& inOrder);
	void			DrawOrderText(const AvHOrder& inOrder);
	int				GetFrameForOrderType(AvHOrderType inOrderType) const;
	void			GetReticleTextDrawingInfo(float& outNormX, float& outNormY, bool& outCentered) const;
	void			DrawTechTreeSprite(AvHMessageID inMessageID, int inPosX, int inPosY, int inWidth, int inHeight, int inFrame);
	int				GetTechTreeSprite(AvHMessageID inMessageID);
	void			GetTooltipDrawingInfo(float& outNormX, float& outNormY) const;
	string			GetRankTitle(bool inShowUnspentLevels = false) const;
	bool			GetShouldDisplayUser3(AvHUser3 inUser3) const;
	void			InitCommanderMode();
	void			InitializeDemoRecording();
	void			InitMenu(const string& inMenuName);
	void			ChangeUpgradeCosts(int inOldMessageID, int inNewMessageID, const char* inText);
	void			ChangeUpgradeCostsForMenu(PieMenu* inMenu, int inOldMessageID, int inNewMessageID, const char* inText);
	void			DisplayMessage(const char* inMessage);
	bool			GetAmbientSoundNameFromIndex(string& outSoundName, int inSoundIndex) const;
	EntityListType	GetDrawPlayerOrders() const;
	bool			GetEntityInfoString(int inEntityID, string& outEntityInfoString, bool& outIsEnemy);
	void			ModifyAmbientSoundEntryIfChanged(bool inSoundOn, int inSoundIndex, int inEntIndex, float inTimeStarted, int inVolume, int inFadeDistance, int inFlags, Vector inOrigin);
	void			ResetTopDownUI();
	bool			SetGamma(float inSlope);
	void			SetReinforcements(int inReinforcements);
	void			SetHelpMessage(const string& inHelpText, bool inForce = false, float inNormX = -1, float inNormY = -1);
	void			SetActionButtonHelpMessage(const string& inHelpText);
	void			SetReticleMessage(const string& inHelpText);
	void			OrderNotification(const AvHOrder& inOrder);
	virtual void	ResetGame(bool inMapChanged = false);
	
    bool			SetCursor(AvHOrderType inOrderType);
    void            GetCursor(HSPRITE& outSprite, int& outFrame);

	void			SetSelectionEffects(EntityListType& inUnitList);
	//void			UpdateSelectionEffects(float inTimePassed);
	void			TraceEntityID(int& outEntityID);
	bool			GetIsMouseInRegion(int inX, int inY, int inWidth, int inHeight);
	void			GetMousePos(int& outX, int& outY) const;

	// Help system
	void			InternalHelpTextThink();
	bool			ProcessAlien();
	bool			ProcessAlienHelp();
	bool			ProcessEntityHelp();
	bool			ProcessGeneralHelp();
	bool			ProcessOrderHelp();
	bool			ProcessWeaponsHelp();
		
	//void			ResetUpgradeCosts();
	//void			ResetUpgradeCostsForMenu(PieMenu* inMenu);
	void			UpdateAlienUI(float inCurrentTime);
	void			UpdateCommonUI();
	void			UpdateDataFromVuser4(float inCurrentTime);
	void			UpdateExploitPrevention();
	void			UpdateMarineUI(float inCurrentTime);
	void			UpdateUpgradeCosts();
	void			UpdateEnableState(PieMenu* inMenu);
	void			UpdateCountdown(float inCurrentTime);
	void			UpdateHierarchy();
	void			UpdateInfoLocation();
	void			UpdatePieMenuControl();
	void			UpdateEntityID(float inCurrentTime);
	void			UpdateTooltips(float inCurrentTime);
	void			UpdateStructureNotification(float inCurrentTime);
	void			UpdateProgressBar();
	void			UpdateDemoRecordPlayback();
	void			UpdateBuildingPlacement();
	void			UpdateResources(float inTimePassed);
	void			UpdateSelection();
	void			UpdateSpectating();
	void			UpdateBuildResearchText();
	void			UpdateHelpText();
	void			UpdateTechNodes();
	void			UpdateAmbientSounds();
	void			UpdateFromEntities(float inCurrentTime);
	void			UpdateViewModelEffects();
	
	int						mResources;
	int						mUser2OfLastResourceMessage;
	int						mMaxResources;
	int						mVisualResources;
	float					mExperience;
	int						mExperienceLevel;
	int						mExperienceLevelLastDrawn;
	int						mExperienceLevelSpent;
	float					mTimeOfLastLevelUp;
	AvHMessageID			mMenuImpulses[kNumUpgradeLines];

	float					mCountDownClock;
	int						mLastTickPlayed;
	int						mNumTicksToPlay;
	float					mTimeOfLastUpdate;
	float					mTimeOfNextHudSound;
    AvHHUDSound             mLastHUDSoundPlayed;
	float					mTimeOfCurrentUpdate;

    AvHOverviewMap          mOverviewMap;

    AvHTeamHierarchy*		mHierarchy;
	AvHTeamHierarchy*		mShowMapHierarchy;
    AvHEntityHierarchy		mEntityHierarchy;

	EntityListType			mSelected;
	EntityListType			mGroups[kNumHotkeyGroups];
	EntityListType			mSelectAllGroup;
	AvHUser3				mGroupTypes[kNumHotkeyGroups];
	AvHAlertType			mGroupAlerts[kNumHotkeyGroups];

	typedef map<AvHMessageID, int>	PendingRequestListType;
	PendingRequestListType	mPendingRequests;

	AvHUser3				mLastUser3;
	AvHTeamNumber			mLastTeamNumber;
	AvHPlayMode				mLastPlayMode;
	
	bool					mSelectionJustChanged;
	bool					mMouseOneDown;
	bool					mMouseTwoDown;
	int						mMouseOneStartX;
	int						mMouseOneStartY;
	bool					mLeftMouseStarted;
	bool					mLeftMouseEnded;
	bool					mPlacingBuilding;
	int						mMouseTwoStartX;
	int						mMouseTwoStartY;
	bool					mRightMouseStarted;
	bool					mRightMouseEnded;
	vec3_t					mMouseWorldPosition;
	vec3_t					mLeftMouseWorldStart;
	vec3_t					mLeftMouseWorldEnd;
	vec3_t					mRightMouseWorldStart;
	vec3_t					mRightMouseWorldEnd;
	MarqueeComponent*		mSelectionBox;
	int						mMouseCursorX;
	int						mMouseCursorY;
	string					mPieMenuControl;

	OrderListType			mOrders;
	//AvHOrderType			mOrderMode;

	// tankefugl: 0000971
	map< int, TeammateOrderType >	mTeammateOrder;
	// tankefugl: 0000992
	float					mDisplayOrderTime;
	int						mDisplayOrderType;
	int						mDisplayOrderDirection;
	int						mDisplayOrderIndex;
	string					mDisplayOrderText1;
	string					mDisplayOrderText2;
	string					mDisplayOrderText3;
	int						mCurrentOrderTarget;
	int						mCurrentOrderType;
	float					mCurrentOrderTime;
	// :tankefugl
	AvHMessageID			mTechEvent;
	AvHMessageID			mAlienAbility;
	AvHMessageID			mGroupEvent;
	AvHMessageID			mLastHotkeySelectionEvent;
	int						mTrackingEntity;

	AvHAlienUpgradeListType	mUpgrades;
	AvHAlienUpgradeCategory mCurrentUpgradeCategory[ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE];
	int						mNumUpgradesAvailable;

	Label*					mCommanderResourceLabel;
	Label*					mOverwatchRangeLabel;
	ProgressBar*			mGenericProgressBar;
	ProgressBar*			mResearchProgressBar;
	ProgressBar*			mAlienProgressBar;
	Label*					mResearchLabel;
	string					mPreviousHelpText;
	float					mTimeLastHelpTextChanged;

	int						mSelectedNodeResourceCost;
	float					mCurrentUseableEnergyLevel;
	float					mVisualEnergyLevel;
	int						mUser2OfLastEnergyLevel;

	static void				PlayRandomSongHook();
	static void				ShowMap();
	static void				HideMap();

	float					mTimeLastOverwatchPulse;
	bool					mInTopDownMode;
	int						mNumLocalSelectEvents;
	AvHMapMode				mMapMode;

	static GammaTable		sPregameGammaTable;
	static GammaTable		sGameGammaTable;
	float					mDesiredGammaSlope;
	
	typedef vector<AvHAmbientSound>	AmbientSoundListType;
	AmbientSoundListType	mAmbientSounds;

	AvHTechTree				mTechNodes;

	string					mMapName;
	AvHMapExtents			mMapExtents;

	// If this isn't MESSAGE_NULL, it means we are trying to build a building of this tech
	AvHMessageID			mGhostBuilding;
	AvHMessageID			mValidatedBuilding;
	bool					mCreatedGhost;
	bool					mCancelBuilding;
	bool					mCurrentGhostIsValid;
	vec3_t					mNormBuildLocation;
	vec3_t					mGhostWorldLocation;

    // Added by mmcguire.
    int                     mSelectionBoxX1;
    int                     mSelectionBoxY1;
    int                     mSelectionBoxX2;
    int                     mSelectionBoxY2;
    bool                    mSelectionBoxVisible;

	Vector					mCommanderPAS;

	StringList				mSoundNameList;

	AvHVisibleBlipList		mEnemyBlips;
	AvHVisibleBlipList		mFriendlyBlips;
	
	bool					mMarineUIDrawUI;
	HSPRITE					mMarineUIJetpackSprite;

	HSPRITE					mAlienUIEnergySprite;

	HSPRITE					mMembraneSprite;
	HSPRITE					mDigestingSprite;
	HSPRITE					mBackgroundSprite;
	HSPRITE					mTopDownTopSprite;
	HSPRITE					mTopDownBottomSprite;
	HSPRITE					mMarineTopSprite;
	HSPRITE					mLogoutSprite;
	HSPRITE					mCommandButtonSprite;
	HSPRITE					mCommandStatusSprite;
	HSPRITE					mSelectAllSprite;

	HSPRITE					mMarineOrderIndicator;
	HSPRITE					mMarineUpgradesSprite;

	// tankefugl: 0000971
	HSPRITE					mTeammateOrderSprite;
	// :tankefugl
	typedef map<int, int>			SpriteListType;
	SpriteListType					mActionButtonSprites;
	//SpriteListType					mHelpSprites;
	int						mHelpSprite;

	typedef vector< pair<vec3_t, int> >		HelpIconListType;
	HelpIconListType						mHelpIcons;
	EntityListType							mHelpEnts;

	EntityListType							mBuildingEffectsEntityList;
	float									mTimeOfLastEntityUpdate;

	HSPRITE					mAlienUIUpgrades;
	HSPRITE					mAlienUIUpgradeCategories;

	HSPRITE					mAlienBuildSprite;
	HSPRITE					mMarineBuildSprite;

	HSPRITE					mAlienHealthSprite;
	HSPRITE					mMarineHealthSprite;

	HSPRITE					mHealthEffectsSprite;
	HSPRITE					mBuildCircleSprite;
	//HSPRITE					mSiegeTurretSprite;
	SelectionListType		mSelectionEffects;


	//HSPRITE					mMappingTechSprite;

	HSPRITE					mHiveInfoSprite;
	HSPRITE					mHiveHealthSprite;
	HSPRITE					mOrderSprite;
	HSPRITE					mCursorSprite;
	HSPRITE					mMarineCursor;
	HSPRITE					mAlienCursor;
	HSPRITE					mAlienLifeformsCursor;
	int						mCurrentCursorFrame;

	int						mProgressBarEntityIndex;
	int						mProgressBarParam;

	bool					mFogActive;
	vec3_t					mFogColor;
	float					mFogStart;
	float					mFogEnd;

	AvHBaseInfoLocationListType		mInfoLocationList;
	string							mLocationText;

	string					mReticleInfoText;
	//int						mReticleInfoColorR;
	//int						mReticleInfoColorG;
	//int						mReticleInfoColorB;
	//float					mReticleInfoColorA;
	//int						mReticleInfoScreenX;
	//int						mReticleInfoScreenY;
	int						mSelectingWeaponID;
	string					mTechHelpText;
	AvHMessageID			mSelectingNodeID;

	struct tempent_s*		mLastGhostBuilding;

	typedef vector<NumericalInfoEffect>		NumericalInfoEffectListType;
	NumericalInfoEffectListType				mNumericalInfoEffects;

	AvHTooltip								mHelpMessage;
	AvHTooltip								mReticleMessage;
	AvHTooltip								mTopDownPlayerNameMessage;
	AvHTooltip								mTopDownActionButtonHelp;
	AvHTooltip								mCombatUpgradeMenu;
	bool									mDrawCombatUpgradeMenu;

	typedef vector<AvHTooltip>				AvHTooltipListType;
	AvHTooltipListType						mTooltips;

	HiveInfoListType		mHiveInfoList;

	bool					mRecordingLastFrame;

	float					mTimeOfLastHelpText;
	StringList				mDisplayedToolTipList;

	int						mCurrentWeaponID;
	bool					mCurrentWeaponEnabled;

	AvHTechSlotManager		mTechSlotManager;
	UIMode					mCurrentUIMode;

	int						mMenuTechSlots;
	int						mCurrentSquad;
	int						mBlinkingAlertType;

	float					mGameTime;
	int						mTimeLimit;
	int						mCombatAttackingTeamNumber;
	bool					mGameStarted;
	bool					mGameEnded;

	AvHTeamNumber			mLastTeamSpectated;

	typedef struct
	{
		AvHMessageID	mStructureID;
		float			mTime;
		int				mPlayerIndex;
		Vector			mLocation;
	} HUDNotificationType;

	typedef vector< HUDNotificationType >	StructureHUDNotificationListType;
	StructureHUDNotificationListType		mStructureNotificationList;

    int                     mCrosshairShowCount;
    HSPRITE                 mCrosshairSprite;
    wrect_t                 mCrosshairRect;
    int                     mCrosshairR;
    int                     mCrosshairG;
    int                     mCrosshairB;

	int		                mViewport[4];		// the viewport coordinates x ,y , width, height
	int		                mSpecialViewport[4];		// the viewport coordinates x ,y , width, height

    bool                    mSteamUIActive;

    typedef std::map<std::string, std::string> ServerVariableMapType;
    ServerVariableMapType   mServerVariableMap;

    static bool             sShowMap;

};

#endif
