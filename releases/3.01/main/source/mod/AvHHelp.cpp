//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Pop-up context sensitive help ("there's an active hive nearby...destroy it!")
//
// $Workfile: AvHHelp.cpp $
// $Date: 2002/10/24 21:25:58 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHelp.cpp,v $
// Revision 1.17  2002/10/24 21:25:58  Flayra
// - Completely reworked help to be client-side
//
// Revision 1.16  2002/10/03 18:49:57  Flayra
// - Play game objective hints
//
// Revision 1.15  2002/09/25 20:45:02  Flayra
// - Only emit phase gate message when built
// - Removed use order
// - Frame-rate independent updating
//
// Revision 1.14  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.13  2002/08/02 22:51:17  Flayra
// - Don't show pop-up menu help for commander
//
// Revision 1.12  2002/08/02 21:59:55  Flayra
// - Tons of new help messages for new tooltip system
//
// Revision 1.11  2002/07/25 16:57:59  flayra
// - Linux changes
//
// Revision 1.10  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.9  2002/06/25 17:59:39  Flayra
// - Removed help for building gun, renamed arsenal to armory
//
// Revision 1.8  2002/06/03 16:46:02  Flayra
// - Help for arsenal, help for alien building secondary functions (help me, I need to be rewritten!)
//
// Revision 1.7  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.8  2002/05/14 19:24:16  Charlie
//===============================================================================
#include "mod/AvHHud.h"
#include "mod/AvHTitles.h"
#include "mod/AvHBasePlayerWeaponConstants.h"
#include "mod/AvHClientVariables.h"
#include "mod/AvHServerVariables.h"

#include "common/const.h"
#include "common/event_api.h"
#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"
#include "pm_shared/pm_movevars.h"
#include "cl_dll/cl_util.h"

bool AvHHud::ProcessEntityHelp()
{
	bool theDisplayedTooltip = false;

	int theNumCommandersOnTeam = (this->GetCommanderIndex() > 0) ? 1 : 0;
	bool theIsCommander = (this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER);
	bool theIsInTrainingMode = false;

	if(this->GetIsCombatMode())
	{
		// TODO: Add combat mode tooltips
	}
	else if(!theIsInTrainingMode)
	{
		for(EntityListType::iterator theHelpIter = this->mHelpEnts.begin(); (theHelpIter != this->mHelpEnts.end()) && !theDisplayedTooltip ; theHelpIter++)
		{
			bool theEntityIsFriendly = false;
			bool theEntityIsEnemy = false;

			physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(*theHelpIter);
			if(theEntity)
			{
				bool theIsBuildable = GetHasUpgrade(theEntity->iuser4, MASK_BUILDABLE);
				bool theIsBuilt = !theIsBuildable;
					
				int theUser3 = theEntity->iuser3;

				if(theEntity->team == this->GetHUDTeam())
				{
					theEntityIsFriendly = true;
				}
				else if(theEntity->team != 0)
				{
					theEntityIsEnemy = true;
				}
				
				// Handle commander separately
				if(theIsCommander)
				{
				}
				else
				{
					// TODO: Make sure we're facing it?

					// If the team has no commander, and this player is near his team's command station
					if(theUser3 == AVH_USER3_COMMANDER_STATION)
					{
						if(theEntityIsFriendly)
						{
							if(theNumCommandersOnTeam == 0)
							{
								if(this->AddTooltipOnce(kHelpTextCSAttractMode))
								{
									theDisplayedTooltip = true;
									continue;
								}
							}
						}
						else if(this->AddTooltipOnce(kHelpTextAttackNearbyStation))
						{
							theDisplayedTooltip = true;
							continue;
						}
					}
					
					// Check if there are any non fully built buildings on our team (this gets turrets and siege turrets of course)
					if((theUser3 == AVH_USER3_TURRET) || (theUser3 == AVH_USER3_SIEGETURRET))
					{
						if(theIsBuildable && theEntityIsFriendly)
						{
							if(this->AddTooltipOnce(kHelpTextBuildTurret))
							{
								theDisplayedTooltip = true;
								continue;
							}
						}
					}
				
					// Near an armory?  Tell player about getting ammo from it.
					if(theUser3 == AVH_USER3_ARMORY)
					{
						if(!theIsBuildable && theEntityIsFriendly)
						{
							if(this->AddTooltipOnce(kHelpTextArmoryResupply))
							{
								theDisplayedTooltip = true;
								continue;
							}
						}
					}
				
					
					
					if(theUser3 == AVH_USER3_RESTOWER)
					{
						if(!theEntityIsFriendly)
						{
							if(this->AddTooltipOnce(kHelpTextAttackTower))
							{
								theDisplayedTooltip = true;
								continue;
							}
						}
						else if(theIsBuildable)
						{
							this->AddTooltip(kHelpTextBuildTower);
							theDisplayedTooltip = true;
							continue;
						}
						else
						{
							if(this->AddTooltipOnce(kHelpTextExplainTower))
							{
								theDisplayedTooltip = true;
								continue;
							}
						}
					}
					
					// Near a phase gate?
					if((theUser3 == AVH_USER3_PHASEGATE) && theIsBuilt)
					{
						if(this->AddTooltipOnce(kHelpTextPhaseGate))
						{
							theDisplayedTooltip = true;
							continue;
						}
					}
					
					// Hive?
					if((theUser3 == AVH_USER3_HIVE) && (theEntity->team != 0))
					{
						if(theEntityIsFriendly)
						{
						}
						else if(theEntityIsEnemy)
						{
							this->AddTooltip(kHelpTextAttackNearbyHive);
							theDisplayedTooltip = true;
							continue;
						}
					}
				
					// If we're near an unbuilt alien structure, tell the player how to build it
					if((this->GetHUDUser3() == AVH_USER3_ALIEN_PLAYER2) && theIsBuildable && theEntityIsFriendly)
					{
						if(this->AddTooltipOnce(kHelpTextAlienEnergy))
						{
							theDisplayedTooltip = true;
							continue;
						}
					}
				
					// Movement chamber
					if((theUser3 == AVH_USER3_MOVEMENT_CHAMBER) && theIsBuilt)
					{
						if(theEntityIsFriendly)
						{
							this->AddTooltip(kHelpTextFriendlyMovementChamber);
							theDisplayedTooltip = true;
							continue;
						}
						else if(theEntityIsEnemy)
						{
							this->AddTooltip(kHelpTextAttackMovementChamber);
							theDisplayedTooltip = true;
							continue;
						}
					}
				
					// Defensive chamber
					if((theUser3 == AVH_USER3_DEFENSE_CHAMBER) && theIsBuilt)
					{
						if(theEntityIsFriendly)
						{
							this->AddTooltip(kHelpTextFriendlyDefensiveChamber);
							theDisplayedTooltip = true;
							continue;
						}
						else if(theEntityIsEnemy)
						{
							this->AddTooltip(kHelpTextAttackDefensiveChamber);
							theDisplayedTooltip = true;
							continue;
						}
					}
				
					// Sensory chamber
					if((theUser3 == AVH_USER3_SENSORY_CHAMBER) && theIsBuilt)
					{
						if(theEntityIsFriendly)
						{
							this->AddTooltip(kHelpTextFriendlySensoryChamber);
							theDisplayedTooltip = true;
							continue;
						}
						else if(theEntityIsEnemy)
						{
							this->AddTooltip(kHelpTextAttackSensoryChamber);
							theDisplayedTooltip = true;
							continue;
						}
					}
				
					// Offensive chamber
					if((theUser3 == AVH_USER3_OFFENSE_CHAMBER) && theIsBuilt)
					{
						if(theEntityIsFriendly)
						{
							this->AddTooltip(kHelpTextFriendlyOffensiveChamber);
							theDisplayedTooltip = true;
							continue;
						}
						else if(theEntityIsEnemy)
						{
							this->AddTooltip(kHelpTextAttackOffensiveChamber);
							theDisplayedTooltip = true;
							continue;
						}
					}
				
//					if(this->GetIsMarine())
//					{
//						if(theWeldable && theWeldable->GetCanBeWelded() && (theWeldable->GetNormalizedBuildPercentage() < 1.0f))
//						{
//							if(theWeldable->GetWeldOpens())
//							{
//								if(this->AddTooltip(kHelpTextOpenWeldable))
//								{
//									theDisplayedTooltip = true;
//								}
//							}
//							else
//							{
//								if(this->AddTooltip(kHelpTextCloseWeldable))
//								{
//									theDisplayedTooltip = true;
//								}
//							}
//						}
//					}
				}
			}
		}
	}
	return theDisplayedTooltip;
}

bool AvHHud::ProcessGeneralHelp()
{
	const float kTimeBeforeUpgradeHelp = 500;
	const float kTimeBeforeLifeformHelp = 200;

	int theTimeSinceGameStarted = this->GetGameTime();
	bool theDisplayedTooltip = false;
	AvHTeamNumber theEnemyTeamNumber = (this->GetHUDTeam() == TEAM_ONE) ? TEAM_TWO : TEAM_ONE;
	
	if(!theDisplayedTooltip && this->AddTooltipOnce(kHelpTextPopupMenu))
	{
		theDisplayedTooltip = true;
	}
//	else if(theEnemyTeam && (theEnemyTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && this->AddTooltipOnce(kObjectivesVsAliens))
//	{
//		theDisplayedTooltip = true;
//	}
//	else if(theEnemyTeam && (theEnemyTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE) && this->AddTooltipOnce(kObjectivesVsMarines))
//	{
//		theDisplayedTooltip = true;
//	}
	// Don't sent these messages until the game has been going for a bit...don't want to overwhelm players
	else if(!theDisplayedTooltip && this->GetIsAlien() && (theTimeSinceGameStarted > kTimeBeforeLifeformHelp) && this->AddTooltipOnce(kHelpTextBuyLifeform))
	{
		theDisplayedTooltip = true;
	}
	//else if(!theDisplayedTooltip && this->GetIsAlien() && (theTimeSinceGameStarted > kTimeBeforeUpgradeHelp) && this->AddTooltipOnce(kHelpTextBuyUpgrade))
	//{
	//	theDisplayedTooltip = true;
	//}
	else if(this->GetHasJetpack() && !this->GetIsInTopDownMode() && this->AddTooltipOnce(kHelpTextJetpacks))
	{
		theDisplayedTooltip = true;
	}
	else if(this->GetIsAlien() && this->AddTooltipOnce(kHelpTextAlienPopupMenu))
	{
		theDisplayedTooltip = true;
	}
	else if(this->GetIsMarine() && !this->GetIsInTopDownMode() && this->AddTooltipOnce(kHelpTextMarinePopupMenu))
	{
		theDisplayedTooltip = true;
	}
	else if(this->AddTooltipOnce(kHelpTextDisableHelp))
	{
		theDisplayedTooltip = true;
	}
	// Else if we have an order
	else if(this->GetDoesPlayerHaveOrder())
	{
		if(this->AddTooltipOnce(kHelpTextOrder))
		{
			theDisplayedTooltip = true;
		}
	}
	
	return theDisplayedTooltip;
}

bool AvHHud::ProcessWeaponsHelp()
{
	bool theDisplayedTooltip = false;

	if((this->mCurrentWeaponID >= 0) && this->mCurrentWeaponEnabled)
	{
		if(!theDisplayedTooltip && (this->mCurrentWeaponID == AVH_WEAPON_BITE))
		{
			if(this->AddTooltipOnce(kHelpTextBite))
			{
				theDisplayedTooltip = true;
			}
		}
		if(!theDisplayedTooltip && (this->mCurrentWeaponID == AVH_WEAPON_WEBSPINNER))
		{
			if(this->AddTooltipOnce(kHelpTextWeb))
			{
				theDisplayedTooltip = true;
			}
		}
		if(!theDisplayedTooltip && (this->mCurrentWeaponID == AVH_WEAPON_SPORES))
		{
			if(this->AddTooltipOnce(kHelpTextSpores))
			{
				theDisplayedTooltip = true;
			}
		}
	}

	return theDisplayedTooltip;
}

bool AvHHud::ProcessAlienHelp()
{
	bool theDisplayedTooltip = false;

	// Have we explained resources?
	if(this->AddTooltipOnce(kHelpTextAlienWeapons))
	{
		theDisplayedTooltip = true;
	}
	else if(this->AddTooltipOnce(kHelpTextAlienResources))
	{
		theDisplayedTooltip = true;
	}
	// Have we explained energy?
	else if(this->AddTooltipOnce(kHelpTextAlienEnergy))
	{
		theDisplayedTooltip = true;
	}
	// Do we have upgrades pending?
	else if(this->GetHasAlienUpgradesAvailable())
	{
		if(this->AddTooltipOnce(kHelpTextAlienPendingUpgrades))
		{
			theDisplayedTooltip = true;
		}
	}
	else if(this->AddTooltipOnce(kHelpTextAlienHiveSight))
	{
		theDisplayedTooltip = true;
	}
	else if(this->AddTooltipOnce(kHelpTextAlienVisionMode))
	{
		theDisplayedTooltip = true;
	}
	
	if(!theDisplayedTooltip)
	{
		switch(this->GetHUDUser3())
		{
		// If we're a level 1 alien and we're holding crouch, tell them they can wall-walk
		case AVH_USER3_ALIEN_PLAYER1:
			if(this->AddTooltipOnce(kHelpTextWallwalking))
			{
				theDisplayedTooltip = true;
				break;
			}
			// TODO: More level 1 help here
			break;
		
		case AVH_USER3_ALIEN_PLAYER2:
			if(this->AddTooltipOnce(kHelpTextBuilder))
			{
				theDisplayedTooltip = true;
				break;
			}
			if(this->AddTooltipOnce(kHelpTextAlienBuildStructure))
			{
				theDisplayedTooltip = true;
				break;
			}
			break;
		
		case AVH_USER3_ALIEN_PLAYER3:
			// If level 3 jumps, tell player about flying
			if(this->AddTooltipOnce(kHelpTextFlight))
			{
				theDisplayedTooltip = true;
				break;
			}
			// TODO: More level 3 help here
			break;
			
		case AVH_USER3_ALIEN_PLAYER4:
			break;
			
		case AVH_USER3_ALIEN_PLAYER5:
			break;
		}
	}
	
	return theDisplayedTooltip;
}

bool AvHHud::ProcessOrderHelp()
{
	bool theDisplayedTooltip = false;
	
//	vec3_t theOrderPosition;
//	int theEntIndex = -1;
//	AvHUser3 theUser3 = AVH_USER3_NONE;
//
//	AvHOrderType theOrder = AvHGetDefaultOrderType(this->GetTeam(), this->GetVisualOrigin(), this->mMouseWorldPos, theEntIndex, theOrderPosition, theUser3);
//	if(theOrder == ORDERTYPET_BUILD)
//	{
//		if(this->AddTooltipOnce(kHelpTextCommanderBuild))
//		{
//			theDisplayedTooltip = true;
//		}
//	}
////	else if(theOrder == ORDERTYPEL_USE)
////	{
////		if(this->AddTooltipOnce(kHelpTextCommanderUse))
////		{
////			theDisplayedTooltip = true;
////		}
////	}
//	else if(theOrder == ORDERTYPET_ATTACK)
//	{
//		if(this->AddTooltipOnce(kHelpTextCommanderAttack))
//		{
//			theDisplayedTooltip = true;
//		}
//	}
//	else if(theOrder == ORDERTYPET_WELD)
//	{
//		if(this->AddTooltipOnce(kHelpTextCommanderWeld))
//		{
//			theDisplayedTooltip = true;
//		}
//	}
//	else if(theOrder == ORDERTYPET_GET)
//	{
//		if(this->AddTooltipOnce(kHelpTextCommanderGet))
//		{
//			theDisplayedTooltip = true;
//		}
//	}
	
	return theDisplayedTooltip;
}

void AvHHud::UpdateHelpText()
{
	const float kHelpTextInterval = 20.0f;
	bool theDisplayedTooltip = false;

	bool theIsCommander = (this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER);
	bool theIsAlien = this->GetIsAlien();

	bool theAutoHelpEnabled = gEngfuncs.pfnGetCvarFloat(kvAutoHelp);

	if(theAutoHelpEnabled)
	{
		// Has it been a little bit since we sent the last help message?
		if((this->mTimeOfLastHelpText == -1) || (this->mTimeOfLastHelpText + kHelpTextInterval < gEngfuncs.GetClientTime()))
		{
			if(this->GetIsRelevant() && this->GetGameStarted() && !this->GetIsCombatMode())
			{
				// Process general help first
				theDisplayedTooltip = this->ProcessGeneralHelp();
				if(!theDisplayedTooltip)
				{
					theDisplayedTooltip = this->ProcessWeaponsHelp();
					if(!theDisplayedTooltip)
					{
						// Search around us
						theDisplayedTooltip = this->ProcessEntityHelp();
						
						// If there is nothing around us to trigger something
						if(!theDisplayedTooltip)
						{
							// If we are alien and a friendly hive is being attacked
							//kHelpTextHiveAttacked
							
//							AvHAlertType theAlertType;
//							float theTimeOfLastAlert = theTeam->GetLastAlertTime(-1, &theAlertType);
//							if(theTimeOfLastAlert > 0)
//							{
//								if(theIsAlien && (theAlertType == ALERT_UNDER_ATTACK))
//								{
//									if(this->AddTooltipOnce(kHelpTextAlienUnderAttack))
//									{
//										theDisplayedTooltip = true;
//									}
//								}
//								else if(theIsCommander)
//								{
//									if(theAlertType == ALERT_UNDER_ATTACK)
//									{
//										if(this->AddTooltipOnce(kHelpTextCommanderUnderAttack))
//										{
//											theDisplayedTooltip = true;
//										}
//									}
//								}
//								else if(theAlertType == ALERT_RESEARCH_COMPLETE)
//								{
//									if(this->AddTooltipOnce(kHelpTextCommanderResearchComplete))
//									{
//										theDisplayedTooltip = true;
//									}
//								}
//								else if(theAlertType == ALERT_UPGRADE_COMPLETE)
//								{
//									if(this->AddTooltipOnce(kHelpTextCommanderUpgradeComplete))
//									{
//										theDisplayedTooltip = true;
//									}
//								}
//							}
						}
						
						if(!theDisplayedTooltip)
						{
							// If the player has never used the command menu
							//kHelpTextMarineCommandMenu
							//kHelpTextAlienCommandMenu
							switch(this->GetHUDUser3())
							{
							case AVH_USER3_MARINE_PLAYER:
								//if(this->AddTooltipOnce(kHelpTextPopupMenu))
								//{
								//	theDisplayedTooltip = true;
								//}
								break;
								
							case AVH_USER3_COMMANDER_PLAYER:
								if(this->mSelected.size() > 0)
								{
									if(this->AddTooltipOnce(kHelpTextCommanderGiveOrders))
									{
										theDisplayedTooltip = true;
										break;
									}
								}
								else
								{
									if(this->AddTooltipOnce(kHelpTextCommanderSelectPlayers))
									{
										theDisplayedTooltip = true;
										break;
									}
									else if(this->AddTooltipOnce(kHelpTextCommanderLogout))
									{
										theDisplayedTooltip = true;
										break;
									}
								}
							
								theDisplayedTooltip = this->ProcessOrderHelp();
								break;
							
							case AVH_USER3_ALIEN_PLAYER1:
							case AVH_USER3_ALIEN_PLAYER2:
							case AVH_USER3_ALIEN_PLAYER3:
							case AVH_USER3_ALIEN_PLAYER4:
							case AVH_USER3_ALIEN_PLAYER5:
								theDisplayedTooltip = this->ProcessAlienHelp();
								break;
							
							}
						}
					}
				}
			}
		
			if(theDisplayedTooltip)
			{
				this->mTimeOfLastHelpText = gEngfuncs.GetClientTime();
			}
		}
	}
}

