//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Constants for all sayings in titles.txt
//
// $Workfile: AvHTitles.h $
// $Date: 2002/10/24 21:43:38 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTitles.h,v $
// Revision 1.35  2002/10/24 21:43:38  Flayra
// - Removed old unused titles
//
// Revision 1.34  2002/10/18 22:23:10  Flayra
// - Regular update
//
// Revision 1.33  2002/10/16 01:08:26  Flayra
// - New HUD titles
//
// Revision 1.32  2002/10/03 19:10:05  Flayra
// - Regular update
//
// Revision 1.31  2002/09/23 22:36:17  Flayra
// - Added "paralyzed" message
//
// Revision 1.30  2002/09/09 20:09:16  Flayra
// - Added constants for commander voting
//
// Revision 1.29  2002/08/16 02:48:49  Flayra
// - Regular update
//
// Revision 1.28  2002/08/02 21:44:16  Flayra
// - Massive help update
//
// Revision 1.27  2002/07/26 23:08:34  Flayra
// - Added numerical feedback
//
// Revision 1.26  2002/07/23 17:33:05  Flayra
// - Regular update
//
// Revision 1.25  2002/07/08 17:20:54  Flayra
// - Regular update
//
// Revision 1.24  2002/06/25 18:22:54  Flayra
// - Regular update
//
// Revision 1.23  2002/06/03 17:01:43  Flayra
// - Regular update (alien building help text, effective player classes for aliens)
//
// Revision 1.22  2002/05/28 18:10:49  Flayra
// - Regular update
//
// Revision 1.21  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_TITLES_H
#define AVH_TITLES_H

// Messages
#define kReadyRoomMessage				"ReadyRoomMessage"
#define kReadyRoomThrottleMessage		"ReadyRoomThrottleMessage"
#define kReinforcementMessage			"ReinforcementMessage"
#define kReinforcingMessage				"ReinforcingMessage"
#define kObserverMessage				"ObserverMessage"
#define kReinforcementComplete			"ReinforcementComplete"
#define kCantSwitchTeamsInTournyMode	"CantSwitchTeamsInTournyMode"
#define kCantSwitchTeamsAfterGameStart	"CantSwitchTeamsAfterGameStart"
#define kTooManyPlayersOnTeam			"TooManyPlayersOnTeam"
#define kCantJoinAfterSpectating		"CantJoinAfterSpectating"
#define kJoinTeamTooFast				"JoinTeamTooFast"
#define kSpectatorsNotAllowed			"SpectatorsNotAllowed"
#define kCanOnlyJoinTeamYouveReinforced "CanOnlyJoinTeamYouveReinforced"
#define kObjectivesVsAliens				"ObjectivesVsAliens"
#define kObjectivesVsMarines			"ObjectivesVsMarines"
#define kTeamTitle						"Team"
#define kMustGestateUp					"MustGestateUp"
#define kMustGestateOnGround			"MustGestateOnGround"
#define kNotWhileDigesting				"NotWhileDigesting"
#define kNoReadyRoomWhileDigested		"NoReadyRoomWhileDigested"
#define kNeedMoreRoomToGestate			"NeedMoreRoomToGestate"
#define kNeedOneHiveToGestate			"NeedOneHiveToGestate"
#define kNeedTwoHivesToGestate			"NeedTwoHivesToGestate"
#define kNeedThreeHivesToGestate		"NeedThreeHivesToGestate"
#define kNeedOneHiveForStructure		"NeedOneHiveForStructure"
#define kNeedTwoHivesForStructure		"NeedTwoHivesForStructure"
#define kNeedThreeHivesForStructure		"NeedThreeHivesForStructure"
#define kNeedsAnotherHiveForStructure	"NeedsAnotherHiveForStructure"
#define kMustBeBuilder					"MustBeBuilder"
#define kUpgradeNotAvailable			"UpgradeNotAvailable"
#define kTooManyStructuresInArea		"TooManyStructuresInArea"
#define kTooManyStructuresOfThisTypeNearby	"TooManyStructuresOfThisTypeNearby"
#define kTooManyWebs					"TooManyWebs"
#define kTooManyWebsNearby				"TooManyWebsNearby"

#define kSoldierMessage					"SoldierMessage"
#define kCommanderMessage				"CommanderMessage"
#define kGestationMessage				"GestationMessage"
#define kCocoonMessage					"CocoonMessage"

#define kFriendText						"Friend"
#define kEnemyText						"Enemy"
#define kWeaponHelpText					"Weapon%dHelp"
#define kCostMarker						"Cost"
#define kDamageMarker					"Damage:"
#define kRateOfFireMarker				"Rate of fire:"
#define kSquadIndicator					"SquadIndicator"
#define kRankText						"Rank_%s_%d"
#define kCombatUpgradeLineComplete		"LineComplete"
#define kYouAreAttacking				"YouAreAttacking"
#define kYouAreDefending				"YouAreDefending"
#define kAttacking						"Attacking"
#define kDefending						"Defending"
#define kYouAreNowA						"YouAreNowA"
#define kExclamation					"Exclamation"
#define kChooseAnUpgrade				"ChooseAnUpgrade"

#define kYouReinforcement				"YouAreReinforcements"
#define kNoSpectating					"NoSpectating"
#define kObsInEyePrefix					"ObsInEyePrefix"

#define kReceiveHeavyArmor				"ReceiveHeavyArmor"
#define kReceiveMotionArmor				"ReceiveMotionTrackArmor"
#define kReceiveJetpackArmor			"ReceiveJetpackArmor"
#define kReceiveLifeSupportArmor		"ReceiveLifeSupportArmor"

#define kMarineResourcePrefix			"MarineResourcePrefix"
#define kAlienResourcePrefix			"AlienResourcePrefix"
#define kCarryResourcePrefix			"CarryResourcePrefix"
#define kAlienEnergyDescription			"AlienEnergyDescription"
#define kResourcesTitle					"Resources"
#define kResourcesDepletedTitle			"ResourcesDepleted"
#define kReinforcementsTitle			"Reinforcements"

#define	kNumericalEventResources		"NumericalEventResources"
#define	kNumericalEventHealth			"NumericalEventHealth"
#define	kNumericalEventResourcesDonated	"NumericalEventResourcesDonated"
#define kNumericalEventAmmo				"NumericalEventAmmo"

#define kMenuArmorUpgradeLifeSupport	"#MenuBuyArmorLifeSupport"
#define kMenuArmorUpgradeJetpack		"#MenuBuyArmorJetpack"
#define kMenuArmorUpgradeMotionTrack	"#MenuBuyArmorMotionTrack"
#define kMenuArmorUpgradeFull			"#MenuArmorFull"

#define kMarinePointsAwardedToCommander	"MarineAwardToCommander"
#define kMarinePointsAwardedToLeader	"MarineAwardToLeader"
#define kMarinePointsAwarded			"MarineAward"
#define kAlienPointsAwarded				"AlienAward"

#define kCommander						"Commander"
#define kNoCommander					"NoCommander"
#define kHandicap						"Handicap"

#define kStunned						"Stunned"
#define kDigested						"Digested"
#define kDigestingPlayer				"DigestingPlayer"
#define kParasited						"Parasited"
#define kPrimalScreamed					"PrimalScreamed"
#define kCatalysted                     "Catalysted"
#define kUmbraed						"Umbraed"
#define kWebbed							"Webbed"

#define kWeaponCantBeDropped			"WeaponCantBeDropped"
#define kReceivingLevelData				"ReceivingLevelData"
#define kReceivingLevelData1			"ReceivingLevelData1"
#define kReceivingLevelData2			"ReceivingLevelData2"
#define kReceivingLevelData3			"ReceivingLevelData3"
#define kReceivingLevelData4			"ReceivingLevelData4"

#define kVoteCast						"VoteCast"
#define kVoteStarted					"VoteStarted"
#define kVoteIllegal					"VoteIllegal"
#define kAlreadyVoted					"AlreadyVoted"
#define	kVoteFailed						"VoteFailed"
#define kVoteSucceeded					"VoteSucceeded"
#define kVoteCancelled					"VoteCancelled"
#define kBannedFromCommand				"BannedFromCommand"

#define kTeamNotAvailable				"TeamNotAvailable"
#define kAlreadyOnTeam					"AlreadyOnTeam"
#define kGameBegun						"GameBegun"
#define kJoinSoon						"YouCanJoinSoon"
#define kTeamOneWon						"TeamOneWon"
#define kTeamTwoWon						"TeamTwoWon"
#define kGameDraw						"GameDraw"

#define kRankOnlyBeforeGameStart		"RankBeforeGameStart"
#define kDefectOnlyAfterGameStart		"DefectAfterGameStart"
#define	kDefectMessageToLeader			"DefectMessageToLeader"
#define kGameWontStart					"GameWontStart"
#define kGameWontStartUntilReady		"GameWontStartUntilReady"
#define kGameStarting					"GameStarting"

#define	kInvalidOrderGiven				"InvalidOrderGiven"

#define kBuildingPrefix					"Building"
#define kResearchingPrefix				"Researching"
#define kEnergyPrefix					"Energy"

#define kEditingParticleSystem			"EditingParticleSystem"
#define kNoParticleSystem				"NoParticleSystem"

#define kCommandStationForOtherTeam		"CommandStationOtherTeam"
#define kCommandStationInUse			"CommandStationInUse"
#define kWeaponPreventingCommandStation	"WeaponPreventingCommandStation"
#define kCommandStationDestroyed		"CommandStationDestroyed"
#define kCommandStationWaitTime			"CommandStationWaitTime"
#define kJetpackEnergyHUDText			"JetpackEnergyHUDText"

#define kHelpTextDisableHelp			"HelpTextDisableHelp"

// Marine help text
#define kHelpTextCSAttractMode			"HelpTextCSAttractMode"
#define kHelpTextArmoryResupply			"HelpTextArmoryResupply"
#define kHelpTextAttackNearbyStation	"HelpTextAttackNearbyStation"
#define kHelpTextBuildTurret			"HelpTextBuildTurret"
#define kHelpTextBuildTower				"HelpTextBuildTower"
#define kHelpTextExplainTower			"HelpTextExplainTower"
#define kHelpTextAttackNearbyHive		"HelpTextAttackNearbyHive"

// Alien chambers
#define kHelpTextFriendlyMovementChamber	"HelpTextFriendlyMovementChamber"
#define kHelpTextAttackMovementChamber		"HelpTextAttackMovementChamber"

#define kHelpTextFriendlyOffensiveChamber	"HelpTextFriendlyOffensiveChamber"
#define kHelpTextAttackOffensiveChamber		"HelpTextAttackOffensiveChamber"

#define kHelpTextFriendlyDefensiveChamber	"HelpTextFriendlyDefensiveChamber"
#define kHelpTextAttackDefensiveChamber		"HelpTextAttackDefensiveChamber"

#define kHelpTextFriendlySensoryChamber		"HelpTextFriendlySensoryChamber"
#define kHelpTextAttackSensoryChamber		"HelpTextAttackSensoryChamber"

#define kHelpTextOpenWeldable			"HelpTextOpenWeldable"
#define kHelpTextCloseWeldable			"HelpTextCloseWeldable"
#define kHelpTextAttackHive				"HelpTextAttackHive"
#define kHelpTextHiveBlocked			"HelpTextOtherHiveBlocked"
#define kHelpTextOtherHiveBuilding		"HelpTextOtherHiveBuilding"
#define kHelpTextEmptyHiveNotNearby		"HelpTextEmptyHiveNotNearby"
#define	kHelpTextPhaseGate				"HelpTextPhaseGate"
#define kHelpTextOrder					"HelpTextOrder"

#define kHelpTextAlienPopupMenu			"HelpTextAlienPopupMenu"
#define kHelpTextMarinePopupMenu		"HelpTextMarinePopupMenu"

#define kHelpTextCommanderUseable		"HelpTextCommanderUseable"
#define kHelpTextResourceDropoff		"HelpTextResourceDropoff"
#define kHelpTextJetpacks				"HelpTextJetpacks"
#define kHelpTextBite					"HelpTextBite"
#define kHelpTextWallwalking			"HelpTextWallwalking"
#define kHelpTextBuilder				"HelpTextBuilder"
#define kHelpTextWeb					"HelpTextWeb"
#define	kHelpTextEggLayer				"HelpTextEggLayer"
#define kHelpTextFlight					"HelpTextFlight"
#define kHelpTextSpores					"HelpTextSpores"
#define kHelpTextAlienResource			"HelpTextAlienResource"
#define kHelpTextPopupMenu				"HelpTextPopupMenu"

// General alien help
#define kHelpTextBuyUpgrade				"HelpTextBuyUpgrade"
#define kHelpTextBuyLifeform			"HelpTextBuyLifeform"
#define kHelpTextAlienWeapons			"HelpTextAlienWeapons"
#define kHelpTextAlienResources			"HelpTextAlienResources"
#define kHelpTextAlienEnergy			"HelpTextAlienEnergy"
#define kHelpTextAlienPendingUpgrades	"HelpTextAlienPendingUpgrades"
#define kHelpTextAlienBuildStructure	"HelpTextAlienBuildStructure"
#define kHelpTextAlienHiveSight			"HelpTextAlienHiveSight"
#define kHelpTextAlienVisionMode		"HelpTextAlienVisionMode"

#define kHelpTextAlienUnderAttack		"HelpTextAlienUnderAttack"

#define	kHelpTextCommanderGiveOrders	"HelpTextCommanderGiveOrders"
#define kHelpTextCommanderSelectPlayers	"HelpTextCommanderSelectPlayers"
#define kHelpTextCommanderBuild			"HelpTextCommanderBuild"
#define kHelpTextCommanderUse			"HelpTextCommanderUse"
#define kHelpTextCommanderAttack		"HelpTextCommanderAttack"
#define kHelpTextCommanderWeld			"HelpTextCommanderWeld"
#define kHelpTextCommanderGet			"HelpTextCommanderGet"
#define kHelpTextCommanderUnderAttack	"HelpTextCommanderUnderAttack"
#define kHelpTextCommanderResearchComplete	"HelpTextCommanderResearchComplete"
#define kHelpTextCommanderUpgradeComplete	"HelpTextCommanderUpgradeComplete"
#define kHelpTextCommanderLogout		"HelpTextCommanderLogout"

// Alien help text
#define kHelpTextAlienCommandMenu		"HelpTextAlienCommandMenu"
#define kHelpTextAttackTower			"HelpTextAttackTower"

// Scoreboard
#define kClassCommander					"#ClassCommander"
#define kClassHeavyMarine				"#ClassHeavyMarine"
#define kClassJetpackMarine				"#ClassJetpackMarine"
#define kClassDead						"#ClassDead"
#define kClassDigesting					"#ClassDigesting"
#define kClassReinforcing				"#ClassReinforcing"
#define kClassReinforcingComplete		"#ClassReinforcingComplete"
#define kClassLevel1					"#ClassLevel1"
#define kClassLevel2					"#ClassLevel2"
#define kClassLevel3					"#ClassLevel3"
#define kClassLevel4					"#ClassLevel4"
#define kClassLevel5					"#ClassLevel5"
#define kClassGestating					"#ClassGestating"

// Timelimit
#define kGameTime						"GameTime"
#define kElapsed						"Elapsed"
#define kRemaining						"Remaining"
#define kTimeLimit						"TimeLimit"

#endif