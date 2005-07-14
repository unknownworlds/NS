#ifndef UPPCLIENTINTERFACE_H
#define UPPCLIENTINTERFACE_H

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//UPP Client Interface 
//
//Hides implementation of communication layer between game server 
//(a.k.a. "the client") and UPP server (a.k.a. "the server")
//
//Several details that were previously tracked explicitly by the 
//client (server identity, client timestamps, heartbeat management)
//are now handled behind the scenes.  The client is still responsible
//for collecting player and game data for requests, and still needs 
//to register itself with a call to UPP::connect.  If UPP::connect fails,
//no client identity has been established with the UPP server, and any 
//subsequent requests will automatically fail until a successful call
//to UPP::connect has been made.
//
//The client should call interface functions immediately when the 
//corresponding state change takes place; any batching of calls will
//be the responsibility of the communication layer.  Any server that
//has mp_uplink set to 0 should be considered offline, and changing
//that cvar should trigger the online/offline reporting functions.
//
//If UPP::connect fails, the client can call the function again to 
//retry or notify the user that the link wasn't established.
//
//Ideally, the client should have a centralized player index -> 
//UPP::PlayerInfo object function so that all of the logic for
//generating UPP::PlayerInfo snapshots can be contained in a single
//location.  The ability to build a UPP::TeamInfo object should also
//be encapsulated since that information is required for both 
//UPP::GameInfo and UPP::GameResultInfo objects.  A clean, 
//centralized implementation is less of an issue for the other UPP 
//data objects, each of which is only used from a single location.
//
//Author: Karl Patrick
//Last Modified: 04/09/04
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "UPPCommonInterface.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Forward declaration of UPP Client objects
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace UPP
{
	//Profiling objects
	class PerformanceData;
	class PerformanceSummary; //not implemented

	//Optional callback functions - can also query for result structures
	typedef void (*ProfileInfoCallback)(auto_ptr<ProfileInfo> info);
	typedef void (*AuthGroupCallback)(auto_ptr<const StringSet>);
	typedef void (*VariablesCallback)(auto_ptr<const StringSet>, bool startVars);
	typedef void (*ProfilerCallback)(auto_ptr<PerformanceData> profile);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Interface function declarations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace UPP
{
	//version information
	int getUPPClientLibBuildNumber(void);

	//configuration
	int  getMaxSimultaneousRequests(void);
	bool setServerConnection(const string& connection_string, const string& protocol);
	bool setGeneratePerformanceData(const bool generate_profiler_info = false);	//not implemented
	bool setRunAsynchronously(const bool async = true);

	bool setMaxSimultaneousRequests(const int max_size = 5);
	auto_ptr<string> getConnectionString(void);
	auto_ptr<string> getProtocol(void);
	bool getGeneratePerformanceData(void);										//not implemented
	bool getRunAsynchronously(void);

	//state information
	bool isConnected(void);
	bool isRecordingGame(void);

	//outgoing signal functions (asynchronous, return false if signal definitely won't be sent)
	bool connect(const ClientInfo& info);
	bool disconnect(void);
	bool requestConnectionSpeedTest(void); //always generates profiler info

	bool reportPlayerEntered(const PlayerInfo& player, const bool resolvedID);
	bool reportPlayerChangingIDs(const PlayerInfo& player, const string& newNetworkID, const bool resolvedID);
	bool reportPlayerChangingTeams(const PlayerInfo& player, const int newTeamIndex);
	bool reportPlayerExited(const PlayerInfo& player);

	bool reportGameStarted(const GameInfo& game);
	bool reportGameFinished(const GameResultInfo& game);
	bool reportGameCancelled(void);

	bool requestAuthGroups(void);
	bool requestVariables(void);

	//incoming signal functions (asynchronous, items are sent to callback functions
	//immediately if they are defined, otherwise they are stored until retrieval
	//only the most recently registered callback function is used.  Register NULL to 
	//return to using library-side storage until retrieval

	void registerProfileInfoCallback(ProfileInfoCallback callback);
	void registerAuthGroupCallback(AuthGroupCallback callback);
	void registerVariablesCallback(VariablesCallback callback);
	void registerProfilerCallback(ProfilerCallback callback);

	bool hasProfileInfo(void);
	auto_ptr<ProfileInfo> getProfileInfo(void); //returns auto_ptr<NULL> if no profiles are available

	bool hasPerformanceData(void);
	auto_ptr<PerformanceData> getPerformanceData(void);
	auto_ptr<PerformanceSummary> getPerformanceSummary(void);

	auto_ptr<StringSet> listAuthGroups(void);
	auto_ptr<StringSet> listGameStartVariables(void);
	auto_ptr<StringSet> listGameEndVariables(void);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::PerformanceData
{
public:
	virtual ~PerformanceData(void) {}
	virtual const string getSignalType(void) const = 0;
	virtual long getSignalTimestamp(void) const = 0;
	virtual long getProcessingThreadNumber(void) const = 0;

	virtual const string getConnectionString(void) const = 0;
	virtual const string getConnectionProtocol(void) const = 0;

	virtual int getErrorCode(void) const = 0;
	virtual const string getErrorDescription(void) const = 0;

	virtual long getTimeInQueue(void) const = 0;
	virtual long getTimeInProcessing(void) const = 0;
	virtual long getTimeInProcessingOnServer(void) const = 0;

	virtual long getRequestSize(void) const = 0;
	virtual long getResponseSize(void) const = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class UPP::PerformanceSummary
{
public:
	virtual ~PerformanceSummary(void) {}
	virtual int getNumRequestsCreated(void) const = 0;
	virtual int getNumRequestsSent(void) const = 0;
	virtual int getNumResponsesRecieved(void) const = 0;
	virtual int getNumResponsesSuccessful(void) const = 0;

	//note - timers only account for signals that have recieved
	//responses
	virtual long getTotalTimeInQueue(void) const = 0;
	virtual long getTotalTimeInProcessing(void) const = 0;
	virtual long getTotalTimeInProcessingServer(void) const = 0;

	virtual long getTotalRequestSize(void) const = 0;
	virtual long getTotalResponseSize(void) const = 0;
};

#endif