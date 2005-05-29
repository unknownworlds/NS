//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: NetworkMeter.h $
// $Date: 2002/11/15 19:09:23 $
//
//-------------------------------------------------------------------------------
// $Log: NetworkMeter.h,v $
// Revision 1.5  2002/11/15 19:09:23  Flayra
// - Reworked network metering to be easily toggleable
//
// Revision 1.4  2002/07/10 14:46:46  Flayra
// - More debug info to the log to track down overflows
//
// Revision 1.3  2002/07/01 21:20:51  Flayra
// - Added logging code to track down overflows
//
// Revision 1.2  2002/05/23 02:32:40  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.2  2002/05/01 02:34:41  Charlie
//===============================================================================
#ifndef NETWORK_METER_H
#define NETWORK_METER_H

#include "dlls/extdll.h"
#include "util.h"
#include "types.h"
#include "build.h"

//	NetworkMeter: Main manager, creates and updates NetworkMeterPlayers
//		list of NetworkMeterPlayers: manages network messages for a player
//			list of NetworkMessages: contains message begin info, plus ordered list of network data
//				list of NetworkData: network data type (created by WRITE_BYTE(), WRITE_STRING(), etc.)

typedef enum
{
	NETWORK_DATA_TYPE_INVALID,
	NETWORK_DATA_TYPE_BYTE,
	NETWORK_DATA_TYPE_CHAR,
	NETWORK_DATA_TYPE_SHORT,
	NETWORK_DATA_TYPE_LONG,
	NETWORK_DATA_TYPE_ANGLE,
	NETWORK_DATA_TYPE_COORD,
	NETWORK_DATA_TYPE_STRING
} NetworkDataType;

class NetworkData
{
public:
					NetworkData();

	// Size of data in bytes
	int				GetDataSize();

	void			Execute();

	void			SetTypeByte(int inData);
	void			SetTypeChar(int inData);
	void			SetTypeShort(int inData);
	void			SetTypeLong(int inData);
	void			SetTypeAngle(float inData);
	void			SetTypeCoord(float inData);
	void			SetTypeString(const char* inData);

private:
	NetworkDataType	mDataType;
	int				mIntData;
	float			mFloatData;
	string			mStringData;
};



class NetworkMessage
{
public:
					NetworkMessage();

	void			Clear();
					
	void			MessageBegin(int inMessageDest, int inMessageType, const float* inOrigin, edict_t* inEd);

	void			AddData(const NetworkData& inData);

	void			Execute(int theNumMessagesQueued);
	
	void			MessageEnd();

	// Size of data in bytes
	int				GetDataSize();

	bool			GetMessagePending() const;

	bool			GetMessageSent() const;
	void			SetMessageSent();

	float			GetTimeMessageSent() const;
					
private:			

	int				mMessageDest;
	int				mMessageType;
	float			mOrigin[3];
	edict_t*		mEdict;

	typedef vector<NetworkData>	NetworkMessageDataList;
	NetworkMessageDataList		mMessageData;

	bool						mMessagePending;
	bool						mMessageSent;
	float						mTimeMessageSent;
	
};

class PlayerNetworkMeter
{
public:
			PlayerNetworkMeter();

	void	SetBufferAmount(int inBytesPerSecond);
			
	void	MessageBegin(int inMessageDest, int inMessageType, const float* inOrigin, edict_t* inEd);
	void	MessageEnd();
			
	void	WriteByte(int inByte);
	void	WriteChar(int inChar);
	void	WriteShort(int inShort);
	void	WriteLong(int inShort);
	void	WriteAngle(float inCoord);
	void	WriteCoord(float inCoord);
	void	WriteString(const char* inString);
	
	void	ProcessQueuedMessages();
	
private:
	typedef vector<NetworkMessage>	NetworkMessageList;
	NetworkMessageList				mMessageList;
	
	NetworkMessage					mMessage;
	int								mBytesSentInPastSecond;
	int								mBytesPerSecond;
	
};

class NetworkMeter
{
public:
	static	NetworkMeter*	Instance();
	
	void	SetBufferAmount(int inBytesPerSecond);

	void	AddMessage(const char* inMessageName, int inMessageID);
	string	LookupMessageID(int inMessageID);

	void	MessageBegin(int inMessageDest, int inMessageType, const float* inOrigin, edict_t* inEd);
	void	MessageEnd();
	
	void	WriteByte(int inByte);
	void	WriteChar(int inChar);
	void	WriteShort(int inShort);
	void	WriteLong(int inLong);
	void	WriteAngle(float inAngle);
	void	WriteCoord(float inCoord);
	void	WriteString(const char* inString);
	
	void	ProcessQueuedMessages();
	
private:
	static	NetworkMeter*	sSingletonNetworkMeter;
	NetworkMeter();
	
	PlayerNetworkMeter&	GetPlayerNetworkMeter(edict_t* inEd);
	
	// Keep list of queued network messages per edict_t
	int			mBytesPerSecond;
	
	edict_t*	mCurrentEntity;
	
	typedef map<edict_t*, PlayerNetworkMeter>	PlayerNetworkMeterMap;
	PlayerNetworkMeterMap						mPlayerList;
	
	typedef map<int, string>	NetworkMessageListType;
	NetworkMessageListType		mNetworkMessageTypes;
};

#endif