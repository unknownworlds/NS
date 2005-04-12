#ifndef UPPTYPES_H
#define UPPTYPES_H

#include <string>
using std::string;

namespace UPP
{
	const static int MAJOR_VERSION_NUMBER = 1;
	const static int MINOR_VERSION_NUMBER = 3;
	
	const int DEFAULT_CONNECTIONTIME = 3000;
	const int DEFAULT_TRANSMISSIONTIME = 10000;

	typedef unsigned char ErrorCode;

	const ErrorCode UPP_OK						= 0x00;
	const ErrorCode ERROR_BAD_PROTOCOL			= 0x10;
	const ErrorCode ERROR_CONNECTION_FAILED		= 0x11;
	const ErrorCode ERROR_CONNECTION_LOST		= 0x12;
	const ErrorCode ERROR_CONNECTION_REJECTED	= 0x13;
	const ErrorCode ERROR_CONNECTION_REDIRECT	= 0x14;

	const ErrorCode ERROR_CLIENT_INVALID_ID		= 0x20;
	const ErrorCode ERROR_CLIENT_INVALID_DATA	= 0x21;
	const ErrorCode ERROR_CLIENT_NOT_FOUND		= 0x22;
	const ErrorCode ERROR_CLIENT_TIMED_OUT		= 0x23;
	const ErrorCode ERROR_CLIENT_UPDATE_FAILED  = 0x24;

	const ErrorCode ERROR_PLAYER_INVALID_ID		= 0x30;
	const ErrorCode ERROR_PLAYER_INVALID_DATA	= 0x31;
	const ErrorCode ERROR_PLAYER_NOT_FOUND		= 0x32;
	//const ErrorCode RESERVED					= 0x33;
	const ErrorCode ERROR_PLAYER_UPDATE_FAILED  = 0x34;

	const ErrorCode ERROR_GAME_INVALID_ID		= 0x40;
	const ErrorCode ERROR_GAME_INVALID_DATA		= 0x41;
	const ErrorCode ERROR_GAME_NOT_FOUND		= 0x42;
	//const ErrorCode RESERVED					= 0x43;
	const ErrorCode ERROR_GAME_UPDATE_FAILED	= 0x44;
}

const string SignalTypeStrings[] =
{
	"Connect", 
	"Disconnect", 
	"ConnectionSpeedTest",
	"ReportPlayerEntrance", 
	"ReportPlayerExit", 
	"ReportPlayerChangeTeams", 
	"ReportPlayerChangeNetworkID",
	"ReportGameStarted",
	"ReportGameCancelled",
	"ReportGameFinished",
	"RequestVariables",
	"RequestAuthGroups"
};

enum SignalType { 
	Connect, 
	Disconnect, 
	ConnectionSpeedTest,
	ReportPlayerEntrance, 
	ReportPlayerExit, 
	ReportPlayerChangeTeams, 
	ReportPlayerChangeNetworkID,
	ReportGameStarted,
	ReportGameCancelled,
	ReportGameFinished,
	RequestVariables,
	RequestAuthGroups,
	NUM_SIGNAL_TYPES
};

enum SignalProtocol {
	NO_SERVER = 0,
	SOAP = 1,
	NUM_PROTOCOLS = 2,
	DEFAULT = 1
};

const string SignalProtocolStrings[] =
{
	"none",
	"soap"
};

#endif