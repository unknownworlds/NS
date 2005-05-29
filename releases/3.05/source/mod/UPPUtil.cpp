//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: UPPUtil.h $
// $Date: 2002/11/12 22:39:25 $
//
//-------------------------------------------------------------------------------
// $Log: UPPUtil.h,v $
//===============================================================================

#ifdef USE_UPP

#include "build.h"
#include "dlls/extdll.h"
#include "util/STLUtil.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHGamerules.h"
#include "mod/UPPUtil.h"
#include "AvHConstants.h"

// TODO: 
//   Move AuthGroup functionality into own logical section?
//   Move NetworkIDManager class and functionality into another file?
//   Add logic to clear temporary id cache intelligently to prevent dynamic ips from
//     causing collisions across multiple players; this should be a very rare occurance
//     even with the current code.
//   Fix UPPUtil_GetDefaultAuthorization to match Auth system's default behavior.
//   Add cheating death status to Auth mask?
//   Resolve HL version
//   Resolve Plugins
//   Resolve Gamestate - gamestate key-value pairs hold the name of a variable and
//     a concatentated listing of all of the values the variable has held at some point
//     during the round, e.g. ("sv_cheats","0:1") if sv_cheats had been turned on partway 
//     through, then back off... a tracking method needs to be implemented for the 
//     variables, a list of variables to track needs to be finalized, and the code to
//     transform a set of values into the final format needs to be written.
//   Implement unauthorized team join handler

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef map<string,int> AuthGroupMap; //CONSIDER: STLPort's hash map implementation instead

bool UPPUtil_GetDefaultAuthorization(void)
{
	bool theDefaultAuthorization = true;

	#ifdef AVH_PLAYTEST_BUILD
	theDefaultAuthorization = false;
	#endif

	return theDefaultAuthorization;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int UPPUtil_GetDefaultAuthMask(void)
{
	return PLAYERAUTH_NONE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string UPPUtil_GetDefaultScoreboardIcon(void)
{
	return "";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

map<string,int> UPPUtil_CreateAuthGroupMap(void)
{
	AuthGroupMap theMap;
	
	theMap.insert(AuthGroupMap::value_type("betaop",PLAYERAUTH_BETASERVEROP));
	theMap.insert(AuthGroupMap::value_type("const",PLAYERAUTH_CONTRIBUTOR));
	theMap.insert(AuthGroupMap::value_type("dev",PLAYERAUTH_DEVELOPER));
	theMap.insert(AuthGroupMap::value_type("guide",PLAYERAUTH_GUIDE));
	theMap.insert(AuthGroupMap::value_type("op",PLAYERAUTH_SERVEROP));
	theMap.insert(AuthGroupMap::value_type("pt",PLAYERAUTH_PLAYTESTER));
	theMap.insert(AuthGroupMap::value_type("vet",PLAYERAUTH_VETERAN));

	return theMap;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int UPPUtil_GetAuthMask(const StringSet inAuthGroups)
{
	int theAuthMask = PLAYERAUTH_NONE;

	//step 1 - convert auth groups into mask
	if(!inAuthGroups.empty())
	{
		static AuthGroupMap theMap = UPPUtil_CreateAuthGroupMap();
		AuthGroupMap::iterator theMapEntry;

		StringSet::const_iterator end = inAuthGroups.end();
		for(StringSet::const_iterator i = inAuthGroups.begin(); i != end; ++i)
		{
			theMapEntry = theMap.find(*i);
			if(theMapEntry != theMap.end())
			{
				theAuthMask |= theMapEntry->second;
			}
		}
	}
	return theAuthMask;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_ProcessProfileInfo(auto_ptr<UPP::ProfileInfo> inProfileInfo)
{
	UPP::ProfileInfo* theProfileInfo = inProfileInfo.release();

	UPPUtil_StoreProfileInfo(theProfileInfo);
	if(UPP::getGeneratePerformanceData()) //report if we're profiling
	{
		AvHSUPrintDevMessage(string("UPP ProfileInfo Received for ") + theProfileInfo->getNetworkID() + "\n",true);
	}


	AvHPlayer* thePlayer = NULL;
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		if(!theEntity->GetNetworkID().compare(theProfileInfo->getNetworkID()))
		{
			thePlayer = theEntity;
			break;
		}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	if(thePlayer)
	{
		UPPUtil_InitializePlayer(thePlayer);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_ProcessPerformanceData(auto_ptr<UPP::PerformanceData> data)
{
	string theMessage("UPP Signal Returned: ");
	theMessage += data->getSignalType() + " ";
	theMessage += MakeStringFromInt(data->getTimeInQueue()) + "/";
	theMessage += MakeStringFromInt(data->getTimeInProcessing()-data->getTimeInProcessingOnServer()) + "/";
	theMessage += MakeStringFromInt(data->getTimeInProcessingOnServer()) + " ms; ";
	theMessage += MakeStringFromInt(data->getRequestSize()) + "/";
	theMessage += MakeStringFromInt(data->getResponseSize()) + " bytes; ";
	theMessage += MakeStringFromInt(data->getErrorCode()) + " ";
	theMessage += data->getErrorDescription() + " \n";
	AvHSUPrintDevMessage(theMessage,true);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TeamSet UPPUtil_BuildTeamInfoList(void)
{
    TeamSet theTeamInfoList;

	UPP::TeamInfo readyroom(TEAM_IND,"readyroom");
	UPP::TeamInfo marines(TEAM_ONE,"marines");
	UPP::TeamInfo aliens(TEAM_TWO,"aliens");
	UPP::TeamInfo spectators(TEAM_SPECT,"spectators");

	UPP::PlayerInfo player;

	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		player = UPPUtil_GetPlayerInfo(theEntity);
		switch(player.getTeamIndex())
		{
		case TEAM_ONE:
			marines.addPlayer(player);
			break;
		case TEAM_TWO:
			aliens.addPlayer(player);
			break;
		case TEAM_IND:
			readyroom.addPlayer(player);
			break;
		case TEAM_SPECT:
			spectators.addPlayer(player);
			break;
		}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

	theTeamInfoList.insert(readyroom);
	theTeamInfoList.insert(marines);
	theTeamInfoList.insert(aliens);
	theTeamInfoList.insert(spectators);

    // Return them
    return theTeamInfoList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

StringMap UPPUtil_BuildServerVariableList(bool GameEndVariables)
{
	StringMap theVariableList;

	auto_ptr<const StringSet> varsToUse = GameEndVariables ? UPP::listGameEndVariables() : UPP::listGameStartVariables();

	string theKey;
	string theValue;

	const char* theRawValue;

	StringSet::const_iterator end = varsToUse->end();
	for(StringSet::const_iterator i = varsToUse->begin(); i != end; ++i)
	{
		theKey.assign(*i);
		theRawValue = CVAR_GET_STRING(theKey.c_str());
		if(theRawValue)
		{
			theValue = theRawValue;
		}
		else
		{
			theValue.clear();
		}
		theVariableList.insert(StringMap::value_type(theKey,theValue));
	}

    return theVariableList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::GameInfo UPPUtil_GetGameInfo(void)
{
    string theGameType;
    switch(GetGameRules()->GetMapMode())
    {
    case MAP_MODE_CO:
        theGameType.assign("CO");
        break;
    case MAP_MODE_NS:
        theGameType.assign("NS");
        break;
    }

	UPP::GameInfo game(theGameType);

    // Map name
    string theMapName = STRING(gpGlobals->mapname);
    game.setMapName(theMapName);

    // Add team infos
    TeamSet theTeamInfoList = UPPUtil_BuildTeamInfoList();
	TeamSet::iterator end = theTeamInfoList.end();
    for(TeamSet::iterator theTeamIter = theTeamInfoList.begin(); theTeamIter != end; ++theTeamIter)
    {
        game.addTeam(*theTeamIter);
    }

    StringMap theVariableValueList = UPPUtil_BuildServerVariableList(false);
	StringMap::iterator SMend = theVariableValueList.end();
    for(StringMap::iterator theIter = theVariableValueList.begin(); theIter != SMend; ++theIter)
    {
       game.addVariable(theIter->first, theIter->second);
    }

	return game;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::GameResultInfo UPPUtil_GetGameResultInfo(const int WinningTeam)
{
	UPP::GameResultInfo result;

	result.setWinningTeam(WinningTeam);

    // Add team infos
    TeamSet theTeamInfoList = UPPUtil_BuildTeamInfoList();
	TeamSet::iterator end = theTeamInfoList.end();
    for(TeamSet::iterator theTeamIter = theTeamInfoList.begin(); theTeamIter != end; ++theTeamIter)
    {
        result.addTeam(*theTeamIter);
    }

	StringMap theVariableValueList = UPPUtil_BuildServerVariableList(true);
	StringMap::iterator SMend = theVariableValueList.end();
    for(StringMap::iterator theIter = theVariableValueList.begin(); theIter != SMend; ++theIter)
    {
       result.addVariable(theIter->first, theIter->second);
    }

	return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::PlayerInfo UPPUtil_GetConnectingPlayerInfo(edict_t* edict, const char* name, const char* address)
{
	UPP::PlayerInfo player;

    player.setNetworkID(UPPUtil_GenerateNetworkID(edict,address));
    player.setName(string(name));
    player.setScore(0);
    player.setKills(0);
    player.setExperience(0.0f);
    player.setDeaths(0);
	player.setClass("none");
	player.setTeamIndex(TEAM_IND);

	return player;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::PlayerInfo UPPUtil_GetPlayerInfo(const AvHPlayer* inPlayer)
{
	UPP::PlayerInfo player;

    string theName = STRING(inPlayer->pev->netname);
    player.setName(theName);
    player.setNetworkID(inPlayer->GetNetworkID());

    player.setScore(inPlayer->GetScore());
    player.setKills(inPlayer->pev->frags);
    player.setExperience(inPlayer->GetExperience());
    player.setDeaths(inPlayer->m_iDeaths);

	switch(inPlayer->GetPlayMode())
	{
	case PLAYMODE_READYROOM:
		player.setClass("none");
		player.setTeamIndex(TEAM_IND);
		break;
	case PLAYMODE_OBSERVER:
		player.setClass("spectator");
		player.setTeamIndex(TEAM_SPECT);
		break;
	default:
	    const char* theUser3Name = AvHSHUGetClassNameFromUser3(inPlayer->GetUser3());
	    player.setClass(string(theUser3Name));
		player.setTeamIndex(inPlayer->GetTeam());
		break;
	}

	return player;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::ClientInfo UPPUtil_GetClientInfo(void)
{
	UPP::ClientInfo client;

	//Name
	const char* theCStrServerName = CVAR_GET_STRING("hostname");
	client.setName(theCStrServerName ? string(theCStrServerName) : "Unspecified Server Name");

	//IP Address & Port
	string theAddress("?.?.?.?");
	int thePort = 0;

	const char* theCStrServerAddress = CVAR_GET_STRING("net_address");
	if(theCStrServerAddress)
	{
		theAddress.assign(theCStrServerAddress);
		int theColonIndex = theAddress.find(':');
		if(theColonIndex != string::npos)
		{
			thePort = MakeIntFromString(theAddress.substr(theColonIndex+1));
			theAddress.resize(theColonIndex);
		}
		else
		{
			const char* theCStrPort = CVAR_GET_STRING("port");
			if(theCStrPort)
			{
				thePort = MakeIntFromString(string(theCStrPort));
			}
		}
	}

	client.setAddress(theAddress);
	client.setPort(thePort);

    // TODO: Plug-ins

    // Region
    const char* theCStrRegion = CVAR_GET_STRING("sv_region");
	client.setRegion(theCStrRegion ? MakeIntFromString(string(theCStrRegion)) : 255);

    // TODO: Engine version
    string theEngineVersion = "1.1.1.0"; // WON: 1.1.1.0, STEAM: 1.1.2.0
    client.setEngineVersion(theEngineVersion);

    // Game version
    string theGameVersion = AvHSUGetGameVersionString();
    client.setGameVersion(theGameVersion);

    // Contact
    const char* theCStrContact = CVAR_GET_STRING("sv_contact");
    client.setContact(theCStrContact ? string(theCStrContact) : "");

	return client;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_ConnectPlayer(edict_t* inEntity, const char* inName, const char* inAddress)
{
	UPP::PlayerInfo thePlayerInfo = UPPUtil_GetConnectingPlayerInfo(inEntity,inName,inAddress);
	bool theResolved = UPPUtil_GetPlayerIsResolved(inEntity);
	UPP::reportPlayerEntered(thePlayerInfo,theResolved);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_ResetPlayer(AvHPlayer* inPlayer)
{
	inPlayer->SetAuthenticationMask(UPPUtil_GetDefaultAuthMask());
	inPlayer->SetAuthorized(UPPUtil_GetDefaultAuthorization());
	inPlayer->SetScoreboardIconName(UPPUtil_GetDefaultScoreboardIcon());
	inPlayer->EffectivePlayerClassChanged();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_HandleUnauthorizedJoinTeamAttempt(AvHPlayer* inPlayer, AvHTeam inTeamNumber)
{

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_InitializePlayer(CBasePlayer* inPlayer)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	if(thePlayer)
	{
		thePlayer->SetNetworkAddress(UPPUtil_GetNetworkAddress(inPlayer->edict()));
		thePlayer->SetNetworkID(UPPUtil_GetNetworkID(inPlayer->edict()));
		UPPUtil_ResetPlayer(thePlayer);
		if(UPP::isConnected())
		{
			UPP::ProfileInfo* theProfileInfo = UPPUtil_GetProfileInfo(thePlayer->GetNetworkID());
			if(theProfileInfo)
			{
				thePlayer->SetAuthorized(theProfileInfo->getAuthorized());
				thePlayer->SetAuthenticationMask(UPPUtil_GetAuthMask(theProfileInfo->getAuthGroups()));
				thePlayer->SetScoreboardIconName(theProfileInfo->getIconName());
				thePlayer->EffectivePlayerClassChanged();
			}
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_ProcessResponses(void)
{
	//process ProfileInfos
	while(UPP::hasProfileInfo())
	{
		UPPUtil_ProcessProfileInfo(UPP::getProfileInfo());
	}

	//process PerformanceData
	while(UPP::hasPerformanceData())
	{
		UPPUtil_ProcessPerformanceData(UPP::getPerformanceData());
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_Disconnect(void) //actual function registered with atexit
{
	UPP::disconnect();
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		UPPUtil_ResetPlayer(theEntity);
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_RegisterShutdown(void)
{
	static bool registered = false;
	if(!registered)
	{
		atexit(UPPUtil_Disconnect);
		registered = true;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_Connect(void)
{
	UPP::setServerConnection("https://24.5.210.1/upp_server-dev/index.php","soap");
	UPP::connect(UPPUtil_GetClientInfo());
	UPP::requestAuthGroups();
	UPP::requestVariables();
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		UPP::reportPlayerEntered(UPPUtil_GetPlayerInfo(theEntity),UPPUtil_GetPlayerIsResolved(theEntity->edict()));
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	UPPUtil_RegisterShutdown();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Network ID management functions for use with UPP
// CONSIDER: this class isn't multithreading safe - do we need it to be?
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//CONSIDER: using hash_map instead
typedef map<const edict_t*,string> NetworkIDMap; 
typedef map<const edict_t*,string> AddressMap;   //key = edict, value = address
typedef map<string,UPP::ProfileInfo*> ProfileInfoMap;
typedef map<string,string> TemporaryIDMap; //key = address, value = id

class NetworkIDManager
{
public:
	const static string STEAM_ID_PENDING;
	const static string STEAM_ID_LAN;
	const static string WON_ID_LAN;
	const static string UNRESOLVED_PREFIX;

	static NetworkIDManager* GetInstance(void);
	~NetworkIDManager(void);
	const string GenerateID(edict_t* inEntity, const char* inAddress);
	const string RetrieveID(const edict_t* inEntity) const;
	const string RetrieveAddress(const edict_t* inEntity) const;
	UPP::ProfileInfo* GetProfileInfo(const string& inNetworkID) const;
	void SetProfileInfo(const string& inNetworkID, UPP::ProfileInfo* info);
	bool GetIsResolved(const edict_t* inEntity);
	bool UpdateID(edict_t* inEntity);
	bool ClearForLevelReset(void);

private:
	auto_ptr<string> GenerateResolvedID(edict_t* inEntity);
	NetworkIDManager(void);
	NetworkIDMap mNetworkIDs;
	AddressMap mAddresses;
	ProfileInfoMap mProfiles;
	TemporaryIDMap mUnresolvedIDs;
	int mNextTempIDNumber;
};

const string NetworkIDManager::STEAM_ID_PENDING("STEAM_ID_PENDING");
const string NetworkIDManager::STEAM_ID_LAN("STEAM_ID_LAN");
const string NetworkIDManager::WON_ID_LAN("-1");
const string NetworkIDManager::UNRESOLVED_PREFIX("UNKNOWN_");

NetworkIDManager::NetworkIDManager(void) : mNextTempIDNumber(1000) {}
NetworkIDManager::~NetworkIDManager(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

NetworkIDManager* NetworkIDManager::GetInstance(void)
{
	static auto_ptr<NetworkIDManager> ptr(new NetworkIDManager());
	return ptr.get();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool NetworkIDManager::ClearForLevelReset(void)
{
	this->mNetworkIDs.clear();
	this->mAddresses.clear();
	this->mProfiles.clear();
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

auto_ptr<string> NetworkIDManager::GenerateResolvedID(edict_t* inEntity)
{
	//(1) attempt to resolve it using STEAM
	auto_ptr<string> theNetworkID(new string(g_engfuncs.pfnGetPlayerAuthId(inEntity)));	
	if(!theNetworkID->compare(STEAM_ID_PENDING) || !theNetworkID->compare(STEAM_ID_LAN))
	{
		//(2) attempt to resolve it using WON
		theNetworkID->assign(MakeStringFromInt(g_engfuncs.pfnGetPlayerWONId(inEntity)));
		if(!theNetworkID->compare(WON_ID_LAN))
		{
			//(3) couldn't resolve it, return NULL
			theNetworkID.reset();
		}
	}
	return theNetworkID;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const string NetworkIDManager::GenerateID(edict_t* inEntity, const char* inAddress)
{
	string theAddress(inAddress);

	//(1) attempt to resolve it
	auto_ptr<string> theNetworkID = this->GenerateResolvedID(inEntity);
	if(theNetworkID.get() == NULL)
	{
		//(2) attempt to retrieve temporary id matching the address - it's probably
		//    a reconnection from the last map change and the same person...
		TemporaryIDMap::iterator theEntry = this->mUnresolvedIDs.find(inAddress);
		if(theEntry != this->mUnresolvedIDs.end())
		{
			theNetworkID.reset(new string(theEntry->second));
		}
		else
		{
			//(3) generate new temporary id and store in temporary id storage
			string theTempID(UNRESOLVED_PREFIX + MakeStringFromInt(this->mNextTempIDNumber));
			++this->mNextTempIDNumber;
			this->mUnresolvedIDs.insert(TemporaryIDMap::value_type(inAddress,theTempID));
			theNetworkID.reset(new string(theTempID));
		}
	}
	this->mNetworkIDs.insert(NetworkIDMap::value_type(inEntity,*theNetworkID.get()));
	this->mAddresses.insert(AddressMap::value_type(inEntity,theAddress));
	return string(*theNetworkID.get());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const string NetworkIDManager::RetrieveID(const edict_t* inEntity) const
{
	NetworkIDMap::const_iterator theEntry = this->mNetworkIDs.find(inEntity);
	return theEntry->second;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const string NetworkIDManager::RetrieveAddress(const edict_t* inEntity) const
{
	AddressMap::const_iterator theEntry = this->mAddresses.find(inEntity);
	return theEntry->second;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool NetworkIDManager::GetIsResolved(const edict_t* inEntity)
{
	return this->RetrieveID(inEntity).compare(0,UNRESOLVED_PREFIX.size(),UNRESOLVED_PREFIX);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool NetworkIDManager::UpdateID(edict_t* inEntity)
{
	auto_ptr<string> theNewID(NULL);
	if(!this->GetIsResolved(inEntity))
	{
		theNewID = this->GenerateResolvedID(inEntity);
		if(theNewID.get())
		{
			//remove this edict from the temporary list
			AddressMap::iterator theEntry = this->mAddresses.find(inEntity);
			this->mUnresolvedIDs.erase(theEntry->second);
		}
	}
	return theNewID.get();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::ProfileInfo* NetworkIDManager::GetProfileInfo(const string& inNetworkID) const
{
	UPP::ProfileInfo* theProfile = NULL;
	ProfileInfoMap::const_iterator i = this->mProfiles.find(inNetworkID);
	if(i != this->mProfiles.end())
	{ 
		theProfile = i->second; 
	}
	return theProfile;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void NetworkIDManager::SetProfileInfo(const string& inNetworkID, UPP::ProfileInfo* info)
{
	this->mProfiles.insert(ProfileInfoMap::value_type(inNetworkID,info));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string UPPUtil_GenerateNetworkID(edict_t* inEntity, const char* inAddress)
{ 
	return NetworkIDManager::GetInstance()->GenerateID(inEntity,inAddress); 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string UPPUtil_GetNetworkID(const edict_t* inEntity)
{
	return NetworkIDManager::GetInstance()->RetrieveID(inEntity);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

string UPPUtil_GetNetworkAddress(const edict_t* inEntity)
{
	return NetworkIDManager::GetInstance()->RetrieveAddress(inEntity);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool UPPUtil_GetPlayerIsResolved(edict_t* inEntity)
{ 
	return NetworkIDManager::GetInstance()->GetIsResolved(inEntity);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool UPPUtil_GetPlayerIsResolved(const AvHPlayer* inPlayer)
{ 
	return NetworkIDManager::GetInstance()->GetIsResolved(const_cast<AvHPlayer*>(inPlayer)->edict());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

UPP::ProfileInfo* UPPUtil_GetProfileInfo(const string& inNetworkID)
{
	return NetworkIDManager::GetInstance()->GetProfileInfo(inNetworkID);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool UPPUtil_ResolvePlayer(AvHPlayer* inPlayer)
{
	bool theUpdated = NetworkIDManager::GetInstance()->UpdateID(inPlayer->edict());
	if(theUpdated)
	{
		string theNewID = NetworkIDManager::GetInstance()->RetrieveID(inPlayer->edict());
		UPP::reportPlayerChangingIDs(UPPUtil_GetPlayerInfo(inPlayer),theNewID,true);
		inPlayer->SetNetworkID(theNewID);
	}
	return theUpdated;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UPPUtil_StoreProfileInfo(UPP::ProfileInfo* info)
{
	NetworkIDManager::GetInstance()->SetProfileInfo(info->getNetworkID(),info);
}

#endif
