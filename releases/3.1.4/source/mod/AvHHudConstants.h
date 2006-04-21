#ifndef AVH_HUD_CONSTANTS_H
#define AVH_HUD_CONSTANTS_H

// Insets for drawing UI
const int					kBaseRightXOffset = 5;
const int					kBaseBottomYOffset = 5;
const int					kZFightingOffset = 5;
const float					kResourceEnergyBarWidth = .06f;
const float					kResourceEnergyBarHeight = .38f;
const float					kAttackOrderZOffset = 15.0f;
const float					kHelpIconDrawSize = 20;	// Size to draw help icons, in world coords

const float                 kPIPWidth  = 0.375;
const float                 kPIPHeight = 0.375;

const float kHiveNormScreenX = .85f;
const float kHiveNormScreenY = 0.0f;
const float kHiveNormScreenWidth = .15f;
const float kHiveNormScreenHeight = .075f;
const float kHiveNormScreenVerticalSpacing = .11f;

// Constants for drawing supporting technology inside hive info sprite

// Percent of kHiveNormScreenWidth that icon starts
const float kHiveTechnologyInsetXScalar = .65f;
const float kHiveTechnologyInsetYScalar = .4f;

// Percent of kHiveNormScreenWidth to use
const float kHiveTechnologyInsetWidthScalar = .24f;
const float kHiveTechnologyInsetHeightScalar = .4f;

// "paralyzed", "webbed", "parasited"
const float kPlayerStatusHorizontalCenteredInset = .5f;
const float kPlayerStatusVerticalInset = .82f;
const float kPlayerStatusStatusSpacing = .02f;

// health armor inset (it is inset by the alien energy indicator for aliens)
const float	kHealthLeftInset = .05f;
const float	kArmorLeftInset = .2f;

// blip size
const float kWorldBlipScale = 100;

// Tooltips, reticle and help message constants
//const float kHelpMessageLeftEdgeInset = .005f;
const float kHelpMessageLeftEdgeInset = .07f;
const float kHelpMessageTopEdgeInset = .75f;

const float kHelpMessageCommanderTopEdgeInset = .6f;
const float kVoiceStatusCommanderTopEdgeInset = .9f;
const float kVoiceStatusCommanderRightEdgeInset = .3f;

const float kVoiceStatusMarineRightEdgeInset = .03f;

const float kHelpMessageAlienTopEdgeInset = .63f;
const float kHelpMessageAlienLeftedgeInset = .055f;

const float kReticleMessageHeight = .05f;
const float kReticleMessageAlienLeftInset = .03f;


const float kToolTipMaxWidth = .25f;
const float kMOTDToolTipMaxWidth = .35f;

const float kReticleMaxWidth = .35f;

const float kToolTipVerticalSpacing = .01f;

// HUD spectator constants
const float kHUDSpectatorDefaultWindowX = .02f;
const float kHUDSpectatorDefaultWindowY = .02f;
const float kHUDSpectatorDefaultWindowWidth = .23f;//240;
const float kHUDSpectatorDefaultWindowHeight = .23f;//180;

const float kHUDSpectatorSoldierWindowX = .79f;
const float kHUDSpectatorSoldierWindowY = .013f;
const float kHUDSpectatorSoldierWindowWidth = .202f;
const float kHUDSpectatorSoldierWindowHeight = .202f;

// UI
#define kSoldierMenu				"SoldierMenu"
#define kSoldierCombatMenu			"SoldierCombatMenu"
#define kAlienMenu					"AlienMenu"
#define kAlienCombatMenu			"AlienCombatMenu"
#define kHierarchy					"Hierarchy"
#define kShowMapHierarchy			"ShowMapHierarchy"
#define kAlienMembrane				"AlienMembrane"
#define kCommanderResourceLabel		"CommanderResources"
#define kMarineResourceLabel		"MarineResources"
#define kCommanderStatusLabel		"CommanderStatus"
#define kGenericProgress			"GenericProgress"
#define kAlienProgress				"AlienProgress"
#define kResearchProgress			"ResearchProgress"
#define kSelectionBox				"SelectionBox"
#define kLeaveCommanderButton		"LogoutButton"
#define kScroller					"Scroller"
#define	kSelectionText				"SelectionText"
#define	kTechHelpText				"TechHelpText"
#define	kPieHelpText				"PieHelpText"
#define kNumUpgradesAvailableText	"NumUpgradesAvailableText"
#define kResearchingLabel			"ResearchingLabel"
#define kResearchBackgroundPanel	"ResearchBackgroundPanel"
#define kActionButtonsComponents	"ActionButtonsComponent"
#define kReinforcementsLabel		"ReinforcementsLabel"
#define kReinforcementsText			"ReinforcementsText"
#define kTopDownHUDTopSpritePanel	"TopDownHUDTop"
#define kTopDownHUDBottomSpritePanel	"TopDownHUDBottom"
#define kPendingImpulseSpecifier	"PendingImpulse%dPanel"
#define kSelectAllImpulsePanel		"SelectAllImpulsePanel"
#define kLastComponent				"TheLastComponent"

#define kDebugCSPServerLabel		"DebugCSPServerInfo"
#define kDebugCSPClientLabel		"DebugCSPClientInfo"

// Particle system editor controls
#define kPSESizeSlider				"PSESizeSlider"
#define kPSESizeLabel				"PSESizeLabel"
#define kPSEScaleSlider				"PSEScaleSlider"
#define kPSEScaleLabel				"PSEScaleLabel"
#define kPSEGenerationRateSlider	"PSEGenerationRateSlider"
#define kPSEGenerationRateLabel		"PSEGenerationRateLabel"
#define kPSEParticleLifetimeSlider	"PSEParticleLifetimeSlider"
#define kPSEParticleLifetimeLabel	"PSEParticleLifetimeLabel"
#define kPSEParticleSystemLifetimeSlider	"PSEParticleSystemLifetimeSlider"
#define kPSEParticleSystemLifetimeLabel		"PSEParticleSystemLifetimeLabel"
#define kPSEMaxParticlesSlider		"PSEMaxParticlesSlider"
#define kPSEMaxParticlesLabel		"PSEMaxParticlesLabel"
#define kPSEDrawModeSlider			"PSEDrawModeSlider"
#define kPSEDrawModeLabel			"PSEDrawModeLabel"

#define PSEGenVelToggleSlider		"PSEGenVelToggleSlider"
#define kPSEGenVelToggleLabel		"PSEGenVelToggleLabel"
#define kPSEGenVelShapeSlider		"PSEGenVelShapeSlider"
#define kPSEGenVelShapeLabel		"PSEGenVelShapeLabel"
#define kPSEGenVelParamNumSlider	"PSEGenVelParamNumSlider"
#define kPSEGenVelParamNumLabel		"PSEGenVelParamNumLabel"
#define kPSEGenVelParamValueSlider	"PSEGenVelParamValueSlider"
#define kPSEGenVelParamValueLabel	"PSEGenVelParamValueLabel"

#endif