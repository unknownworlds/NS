#ifndef UPPCOMMONINTERFACE_H
#define UPPCOMMONINTERFACE_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//UPP Common Interface 
//
// Declares objects and functions that should be shared between the
// client and server.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "util/nowarnings.h"
#include <algorithm> //for std::binary_function
#include <string>
#include <map>
#include <memory> //for std::auto_ptr
#include <set>
#include "UPPTypes.h"

using std::auto_ptr;
using std::binary_function;
using std::string;
using std::set;
using std::map;

typedef set<string> StringSet;
typedef map<string,string> StringMap;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Forward declaration of UPP Common objects
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace UPP
{
	//Signal transfer objects
	class ConnectSignal;
	class DisconnectSignal;
	class ConnectionSpeedTestSignal;
	class ReportPlayerEnteredSignal;
	class ReportPlayerExitedSignal;
	class ReportPlayerChangingTeamsSignal;
	class ReportPlayerChangingIDsSignal;
	class ReportGameStartedSignal;
	class ReportGameFinishedSignal;
	class ReportGameCancelledSignal;
	class RequestVariablesSignal;
	class RequestAuthGroupsSignal;

	//Client request objects
	class ClientInfo;
	class PlayerInfo;
	class GameInfo;
	class GameResultInfo;
	class TeamInfo;

	//Server return objects
	class ProfileInfo;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Interface function declarations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace UPP
{
	//version information
	int getUPPInterfaceMajorVersion(void);
	int getUPPInterfaceMinorVersion(void);

	//configuration
	bool setConnectionTimeout(const int timeout_milliseconds = DEFAULT_CONNECTIONTIME);
	bool setTransmissionTimeout(const int timeout_milliseconds = DEFAULT_TRANSMISSIONTIME);
	bool setAbortSignalsOnShutdown(const bool abort = false);

	int  getConnectionTimeout(void);
	int  getTransmissionTimeout(void);
	bool getAbortSignalsOnShutdown(void);

	auto_ptr<StringSet> getAvailableProtocols(void);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Utility functors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace UPP
{
	//for use in TeamSet
	struct TeamComparator : public binary_function<const TeamInfo&,const TeamInfo&,bool> 
	{
		bool operator()(const TeamInfo& lhs, const TeamInfo& rhs) const;
	};

	//for use in PlayerSet
	struct PlayerComparator : public binary_function<const PlayerInfo&,const PlayerInfo&,bool> 
	{
		bool operator()(const PlayerInfo& lhs, const PlayerInfo& rhs) const;
	};
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// UPP class storage objects
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef set<UPP::TeamInfo,UPP::TeamComparator> TeamSet;
typedef set<UPP::PlayerInfo,UPP::PlayerComparator> PlayerSet;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class declarations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::ClientInfo
{
	friend class ConnectSignal;
public:
	ClientInfo(void);
	~ClientInfo(void);
	ClientInfo(const ClientInfo& other);
	ClientInfo& operator=(const ClientInfo& other);

	//Property Setters
	void setAddress(const string& address);
	void setPort(int port);
	//Environment is automatically determined internally
	void setRegion(int region);
	void setEngineVersion(const string& version);
	void setGameVersion(const string& version);
	void setName(const string& name);
	void setContact(const string& contact);

	void addPlugin(const string& plugin);
	void removePlugin(const string& plugin);

	//Property Retrieval
	const string getAddress(void) const;
	int getPort(void) const;
	const string getEnvironment(void) const;
	int getRegion(void) const;
	const string getEngineVersion(void) const;
	const string getGameVersion(void) const;
	const string getName(void) const;
	const string getContact(void) const;
	const StringSet getPlugins(void) const;

private:
	friend class UPP::ConnectSignal;
	class Impl;
	ClientInfo(const Impl* data);
	auto_ptr<Impl> pimpl;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::GameInfo
{
public:
	GameInfo(const string& type);
	~GameInfo(void);
	GameInfo(const GameInfo& other);
	GameInfo& operator=(const GameInfo& other);

	//Property Setters
	void setType(const string& type); // e.g. "NS" or "CO"
	void setMapName(const string& mapname); 
	
	void addTeam(const UPP::TeamInfo& team);
	void removeTeam(const UPP::TeamInfo& team);
	
	void addVariable(const string& name, const string& value);
	void removeVariable(const string& name);

	//Property Getters
	const string getType(void) const;
	const string getMapName(void) const;
	const TeamSet getTeams(void) const;
	const StringMap getVariables(void) const;

private:
	friend class UPP::ReportGameStartedSignal;
	class Impl;
	GameInfo(const Impl* data);
	auto_ptr<Impl> pimpl;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::GameResultInfo
{
public:
	GameResultInfo(void);
	~GameResultInfo(void);
	GameResultInfo(const GameResultInfo& other);
	GameResultInfo& operator=(const GameResultInfo& other);

	//Property Setters
	void setWinningTeam(int teamIndex); // e.g. "NS" or "CO"
	
	void addTeam(const UPP::TeamInfo& team);
	void removeTeam(const UPP::TeamInfo& team);

	void addGameStateSetting(const string& name, const string& value);
	void removeGameStateSetting(const string& name);

	void addVariable(const string& name, const string& value);
	void removeVariable(const string& name);

	//Property Getters
	int getWinningTeam(void) const;
	const TeamSet getTeams(void) const;
	const StringMap getVariables(void) const;
	const StringMap getGameState(void) const;

private:
	friend class UPP::ReportGameFinishedSignal;
	class Impl;
	GameResultInfo(const Impl* data);
	auto_ptr<Impl> pimpl;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::TeamInfo
{
public:
	TeamInfo(int index, const string& type);
	~TeamInfo(void);
	TeamInfo(const TeamInfo& other);
	TeamInfo& operator=(const TeamInfo& other);

	//Property Setters
	void setIndex(int teamIndex);
	void setType(const string& type);	// e.g. "alien", "marine", "spectator", "readyroom"
	
	void addPlayer(const UPP::PlayerInfo& player);
	void removePlayer(const UPP::PlayerInfo& player);

	//Property Retrieval
	int getIndex(void) const;
	const string getType(void) const;
	const PlayerSet getPlayers(void) const;

private:
	class Impl;
	TeamInfo(const Impl* data);
	auto_ptr<Impl> pimpl;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::PlayerInfo
{
public:
	PlayerInfo(void);
	~PlayerInfo(void);
	PlayerInfo(const PlayerInfo& other);
	PlayerInfo& operator=(const PlayerInfo& other);

	//Property Setters
	void setNetworkID(const string& networkID);		// STEAM or WON ID
	void setName(const string& name);				// display name in game
	void setClass(const string& className);			// e.g. "skulk", "marine"
	void setScore(int score);
	void setKills(int kills);
	void setDeaths(int deaths);
	void setExperience(float experience);			// CO mode experience amount
	void setTeamIndex(int team);					// current team -- don't change before signaling reportPlayerChangingTeam

	//Property Retrieval
	const string getNetworkID(void) const;
	const string getName(void) const;
	const string getClass(void) const;
	int getScore(void) const;
	int getKills(void) const;
	int getDeaths(void) const;
	float getExperience(void) const;
	int getTeamIndex(void) const;

private:
	friend class UPP::ReportPlayerEnteredSignal;
	friend class UPP::ReportPlayerExitedSignal;
	friend class UPP::ReportPlayerChangingTeamsSignal;
	friend class UPP::ReportPlayerChangingIDsSignal;
	class Impl;
	PlayerInfo(const Impl* data);
	auto_ptr<Impl> pimpl;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//this is a return object, the client can't set properties
class UPP::ProfileInfo
{
public:
	ProfileInfo(void);
	~ProfileInfo(void);
	ProfileInfo(const ProfileInfo& other);
	ProfileInfo& operator=(const ProfileInfo& other);

	//Property Setters
	void setPlayerID(const int playerID);
	void setProfileName(const string& name);
	void setNetworkID(const string& networkID);
	void setAuthorized(const bool authorized);
	void setIconName(const string& iconName);
	void addAuthGroup(const string& authGroup);
	void removeAuthGroup(const string& authGroup);

	//Property Retrieval
	int getPlayerID(void) const;				//UPP ID, returns 0 if player not found
	const string getProfileName(void) const;
	const string getNetworkID(void) const;		//should mirror the id sent by reportPlayerEntered
	bool getAuthorized(void) const;				//can this person play in the game?
	const string getIconName(void) const;
	const StringSet getAuthGroups(void) const;	//set of all current authgroups
private:
	class Impl;
	ProfileInfo(const Impl* data);
	auto_ptr<Impl> pimpl;
};

#endif