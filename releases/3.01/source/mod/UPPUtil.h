//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Centralized functions that build and return UPP data objects
//
// $Workfile: UPPUtil.h $
// $Date: 2002/11/12 22:39:25 $
//
//-------------------------------------------------------------------------------
// $Log: UPPUtil.h,v $
//===============================================================================

#ifdef USE_UPP
#ifndef UPP_UTIL
#define UPP_UTIL

#include "mod/AvHEntities.h"
#include "upp/common/UPPClientInterface.h"

//UPP Object construction routines
UPP::GameInfo UPPUtil_GetGameInfo(void);
UPP::GameResultInfo UPPUtil_GetGameResultInfo(const int WinningTeam);
UPP::PlayerInfo UPPUtil_GetPlayerInfo(const AvHPlayer* inPlayer);
UPP::ClientInfo UPPUtil_GetClientInfo(void);

//AvHGamerules functionality 
void UPPUtil_ProcessResponses(void);
void UPPUtil_ConnectPlayer(edict_t* inEntity, const char* inName, const char* inAddress);
void UPPUtil_InitializePlayer(CBasePlayer* inPlayer);
void UPPUtil_HandleUnauthorizedJoinTeamAttempt(AvHPlayer* inPlayer, AvHTeam inTeamNumber);
void UPPUtil_Connect(void);
void UPPUtil_Disconnect(void);

//AvHPlayer defaults for initialization
bool UPPUtil_GetDefaultAuthorization(void);
int UPPUtil_GetDefaultAuthMask(void);
string UPPUtil_GetDefaultScoreboardIcon(void);

//NetworkID functionality
string UPPUtil_GenerateNetworkID(edict_t* inEntity, const char* inAddress);
string UPPUtil_GetNetworkID(const edict_t* inEntity);
string UPPUtil_GetNetworkAddress(const edict_t* inEntity);
bool UPPUtil_GetPlayerIsResolved(edict_t* inEntity);
bool UPPUtil_ResolvePlayer(AvHPlayer* inPlayer);
void UPPUtil_StoreProfileInfo(UPP::ProfileInfo* info);
UPP::ProfileInfo* UPPUtil_GetProfileInfo(const string& inNetworkID);

#endif
#endif