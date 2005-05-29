//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: NetworkMeter.cpp $
// $Date: 2002/11/15 19:09:23 $
//
//-------------------------------------------------------------------------------
// $Log: NetworkMeter.cpp,v $
// Revision 1.9  2002/11/15 19:09:23  Flayra
// - Reworked network metering to be easily toggleable
//
// Revision 1.8  2002/10/24 21:44:01  Flayra
// - Size was being calculated wrong (harmless, but gave wrong number in network log)
//
// Revision 1.7  2002/10/20 16:36:54  Flayra
// - Allow network metering to be compiled in or out easily
//
// Revision 1.6  2002/08/02 21:44:50  Flayra
// - Added code to enable/disable network metering, but there are problems when it's used
//
// Revision 1.5  2002/07/10 14:46:46  Flayra
// - More debug info to the log to track down overflows
//
// Revision 1.4  2002/07/08 17:22:56  Flayra
// - Allow disabling of network metering by setting rate to -1
//
// Revision 1.3  2002/07/01 21:20:51  Flayra
// - Added logging code to track down overflows
//
// Revision 1.2  2002/05/23 02:32:40  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.2  2002/05/01 02:34:41  Charlie
//===============================================================================
#include "mod/NetworkMeter.h"
#include "build.h"

////////////////////
// Hook functions //
////////////////////

void NetworkMeterMessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed)
{
	if(CVAR_GET_FLOAT("mp_networkdebug") > 0) 
	{
		char theDebugString[512];
		sprintf(theDebugString, "MessageBegin(%d, %d...)\n", msg_dest, msg_type);
		ALERT(at_logged, theDebugString); 
	}
	
#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->MessageBegin(msg_dest, msg_type, pOrigin, ed);
#else
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
#endif
}

void MESSAGE_END()
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->MessageEnd();
	#else
	(*g_engfuncs.pfnMessageEnd)();
	#endif
}

void WRITE_BYTE(int inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteByte(inData);
	#else
	(*g_engfuncs.pfnWriteByte)(inData);
	#endif
}

void WRITE_CHAR(int inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteChar(inData);
	#else
	(*g_engfuncs.pfnWriteChar)(inData);
	#endif
}

void WRITE_SHORT(int inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteShort(inData);
	#else
	(*g_engfuncs.pfnWriteShort)(inData);
	#endif
}

void WRITE_LONG(int inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteLong(inData);
	#else
	(*g_engfuncs.pfnWriteLong)(inData);
	#endif
}

void WRITE_ANGLE(float inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteAngle(inData);
	#else
	(*g_engfuncs.pfnWriteAngle)(inData);
	#endif
}

void WRITE_COORD(float inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteCoord(inData);
	#else
	(*g_engfuncs.pfnWriteCoord)(inData);
	#endif
}

void WRITE_STRING(const char* inData)
{
	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->WriteString(inData);
	#else
	(*g_engfuncs.pfnWriteString)(inData);
	#endif
}

int REG_USER_MSG(const char* inMessageName, int inMessageSize)
{
	int theMessageID = (*g_engfuncs.pfnRegUserMsg)(inMessageName, inMessageSize);

	#ifdef USE_NETWORK_METERING
	NetworkMeter::Instance()->AddMessage(inMessageName, theMessageID);
	#endif

	return theMessageID;
}


//////////////////
// Network data //
//////////////////
NetworkData::NetworkData()
{
	this->mDataType = NETWORK_DATA_TYPE_INVALID;
	this->mIntData = 0;
	this->mFloatData = 0.0f;
	this->mStringData = "";
}

// Return size of data, in bytes
int NetworkData::GetDataSize()
{
	int theDataSize = 0;

	switch(this->mDataType)
	{
	case NETWORK_DATA_TYPE_INVALID:
		ASSERT(false);
		break;

	case NETWORK_DATA_TYPE_BYTE:
	case NETWORK_DATA_TYPE_CHAR:
		theDataSize = 1;
		break;

	case NETWORK_DATA_TYPE_SHORT:
	case NETWORK_DATA_TYPE_COORD:
		theDataSize = 2;
		break;

	case NETWORK_DATA_TYPE_LONG:
		theDataSize = 4;
		break;

	case NETWORK_DATA_TYPE_ANGLE:
		theDataSize = 4;
		break;
		
	case NETWORK_DATA_TYPE_STRING:
		//ASSERT(this->mStringData != "");
		theDataSize = 1*this->mStringData.length();
		break;
	}

	return theDataSize;
}

void NetworkData::Execute()
{
	switch(this->mDataType)
	{
	case NETWORK_DATA_TYPE_INVALID:
		ASSERT(false);
		break;
		
	case NETWORK_DATA_TYPE_BYTE:
		(*g_engfuncs.pfnWriteByte)(this->mIntData);
		break;

	case NETWORK_DATA_TYPE_CHAR:
		(*g_engfuncs.pfnWriteChar)(this->mIntData);
		break;
		
	case NETWORK_DATA_TYPE_SHORT:
		(*g_engfuncs.pfnWriteShort)(this->mIntData);
		break;
		
	case NETWORK_DATA_TYPE_LONG:
		(*g_engfuncs.pfnWriteLong)(this->mIntData);
		break;
		
	case NETWORK_DATA_TYPE_ANGLE:
		(*g_engfuncs.pfnWriteAngle)(this->mFloatData);
		break;

	case NETWORK_DATA_TYPE_COORD:
		(*g_engfuncs.pfnWriteCoord)(this->mFloatData);
		break;
		
	case NETWORK_DATA_TYPE_STRING:
		//ASSERT(this->mStringData != "");
		(*g_engfuncs.pfnWriteString)(this->mStringData.c_str());
		break;
	}
}

void NetworkData::SetTypeByte(int inData)
{
	this->mDataType = NETWORK_DATA_TYPE_BYTE;
	this->mIntData = inData;
}

void NetworkData::SetTypeChar(int inData)
{
	this->mDataType = NETWORK_DATA_TYPE_CHAR;
	this->mIntData = inData;
}

void NetworkData::SetTypeShort(int inData)
{
	this->mDataType = NETWORK_DATA_TYPE_SHORT;
	this->mIntData = inData;
}

void NetworkData::SetTypeLong(int inData)
{
	this->mDataType = NETWORK_DATA_TYPE_LONG;
	this->mIntData = inData;
}

void NetworkData::SetTypeAngle(float inData)
{
	this->mDataType = NETWORK_DATA_TYPE_ANGLE;
	this->mFloatData = inData;
}

void NetworkData::SetTypeCoord(float inData)
{
	this->mDataType = NETWORK_DATA_TYPE_COORD;
	this->mFloatData = inData;
}

void NetworkData::SetTypeString(const char* inData)
{
	this->mDataType = NETWORK_DATA_TYPE_STRING;
	this->mStringData = inData;
}



////////////////////
// NetworkMessage //
////////////////////
NetworkMessage::NetworkMessage()
{
	this->Clear();
}

void NetworkMessage::Clear()
{
	this->mMessageDest = -1;
	this->mMessageType = -1;
	this->mEdict = NULL;
	this->mOrigin[0] = this->mOrigin[1] = this->mOrigin[2] = 0.0f;
	this->mMessageData.clear();
	this->mMessagePending = false;
	this->mMessageSent = false;
	this->mTimeMessageSent = -1;
}

void NetworkMessage::MessageBegin(int inMessageDest, int inMessageType, const float* inOrigin, edict_t* inEd)
{
	this->Clear();

	// Set message header data
	this->mMessageDest = inMessageDest;
	this->mMessageType = inMessageType;
	if(inOrigin)
	{
		this->mOrigin[0] = inOrigin[0];
		this->mOrigin[1] = inOrigin[1];
		this->mOrigin[2] = inOrigin[2];
	}
	this->mEdict = inEd;

	this->mMessagePending = true;
}

void NetworkMessage::AddData(const NetworkData& inData)
{
	ASSERT(this->mMessagePending);

	this->mMessageData.push_back(inData);
}

void NetworkMessage::Execute(int theNumMessagesQueued)
{
	if(CVAR_GET_FLOAT("mp_networkdebug") > 0) 
	{
		string theMessageName = NetworkMeter::Instance()->LookupMessageID(this->mMessageType);
		string theMessageDest = "-";
		if(this->mEdict)
		{
			theMessageDest = STRING(VARS(this->mEdict)->netname);
		}
		UTIL_LogPrintf("NetworkMessage::Execute(\"%s\", \"%s\", %d bytes, %d queued, time: %f)\n", theMessageDest.c_str(), theMessageName.c_str(), this->GetDataSize(), theNumMessagesQueued, gpGlobals->time);
	}
	
	// Start the message
	(*g_engfuncs.pfnMessageBegin)(this->mMessageDest, this->mMessageType, this->mOrigin, this->mEdict);
	
	// Loop through the data
	for(NetworkMessageDataList::iterator theIter = this->mMessageData.begin(); theIter != this->mMessageData.end(); theIter++)
	{
		// Send each
		theIter->Execute();
	}

	// End the message
	(*g_engfuncs.pfnMessageEnd)();
	
	// Mark message as sent
	this->SetMessageSent();
}

void NetworkMessage::MessageEnd()
{
	ASSERT(this->mMessagePending);

	this->mMessagePending = false;
}

int	NetworkMessage::GetDataSize()
{
	// Estimated amount for header.  TODO: Ask Yahn to find out exactly?
	const int kNumBytesForMessageHeader = 4;

	int theNumBytes = kNumBytesForMessageHeader;

	for(NetworkMessageDataList::iterator theIter = this->mMessageData.begin(); theIter != this->mMessageData.end(); theIter++)
	{
		theNumBytes += theIter->GetDataSize();
	}

	return theNumBytes;
}

bool NetworkMessage::GetMessagePending() const
{
	return this->mMessagePending;
}

bool NetworkMessage::GetMessageSent() const
{
	return this->mMessageSent;
}

void NetworkMessage::SetMessageSent()
{
	this->mMessageSent = true;
	this->mTimeMessageSent = gpGlobals->time;
}

float NetworkMessage::GetTimeMessageSent() const
{
	return this->mTimeMessageSent;
}


////////////////////////
// PlayerNetworkMeter //
////////////////////////
PlayerNetworkMeter::PlayerNetworkMeter()
{
	this->mBytesSentInPastSecond = 0;
	this->mBytesPerSecond = 0;
}

void PlayerNetworkMeter::SetBufferAmount(int inBytesPerSecond)
{
	this->mBytesPerSecond = inBytesPerSecond;
}

void PlayerNetworkMeter::MessageBegin(int inMessageDest, int inMessageType, const float* inOrigin, edict_t* inEd)
{
	// Make sure we're not in the middle of sending a message
	ASSERT(!this->mMessage.GetMessagePending());
	
	// Start new message
	this->mMessage.MessageBegin(inMessageDest, inMessageType, inOrigin, inEd);
}

void PlayerNetworkMeter::MessageEnd()
{
	ASSERT(this->mMessage.GetMessagePending());
	
	this->mMessage.MessageEnd();
	
	// Add message to the (end of) list
	this->mMessageList.push_back(this->mMessage);
	
	// Clear message
	this->mMessage.Clear();
}

void PlayerNetworkMeter::WriteByte(int inByte)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeByte(inByte);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::WriteChar(int inChar)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeChar(inChar);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::WriteShort(int inShort)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeShort(inShort);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::WriteLong(int inLong)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeLong(inLong);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::WriteAngle(float inAngle)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeAngle(inAngle);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::WriteCoord(float inCoord)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeCoord(inCoord);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::WriteString(const char* inString)
{
	ASSERT(this->mMessage.GetMessagePending());
	
	NetworkData theNetworkData;
	theNetworkData.SetTypeString(inString);
	this->mMessage.AddData(theNetworkData);
}

void PlayerNetworkMeter::ProcessQueuedMessages()
{
	#ifdef USE_NETWORK_METERING
	
	//char theMessage[256];
	//sprintf(theMessage, "PlayerNetworkMeter::ProcessQueuedMessages(): %d messages\n", this->mMessageList.size());
	//ALERT(at_logged, "%s", theMessage);

	// Run through our list of network messages
	for(NetworkMessageList::iterator theIter = this->mMessageList.begin(); theIter != this->mMessageList.end(); )
	{
		// See how much data it requires
		int theMessageSize = theIter->GetDataSize();

		// For each, if message hasn't been sent
		if(!theIter->GetMessageSent())
		{
			bool theForceSend = (this->mBytesPerSecond < 0);
			int theBudget = this->mBytesPerSecond - this->mBytesSentInPastSecond;

			// This can be less then 0 when the variable is changed mid-game
			if((theBudget >= 0) || theForceSend)
			{
				// If we can afford to send this message
				if((theMessageSize <= theBudget) || theForceSend)
				{
					// Execute the message
					theIter->Execute(this->mMessageList.size());
				
					// Increment num bytes we've sent in past second
					this->mBytesSentInPastSecond += theMessageSize;
				
					// Continue processing
					theIter++;
				}
				else
				{
					// If not, stop looping
					break;
				}
			}
			else
			{
				// If not, stop looping
				break;
			}
		}
		else
		{
			// if time message sent was over a second ago
			float theMessageSendTime = theIter->GetTimeMessageSent();
			if(theMessageSendTime < (gpGlobals->time - 1.0f))
			{
				// get back the amount of bytes from it
				this->mBytesSentInPastSecond -= theMessageSize;

				// pop it off the list
				theIter = this->mMessageList.erase(theIter);
			}
			else
			{
				// Continue processing
				theIter++;
			}
		}
	}
	#endif
}


///////////////////
// Network meter //
///////////////////
NetworkMeter* NetworkMeter::sSingletonNetworkMeter = NULL;

// Singleton static accessor (see "Design Patterns")
NetworkMeter* NetworkMeter::Instance()
{
	if(!sSingletonNetworkMeter)
	{
		sSingletonNetworkMeter = new NetworkMeter();
	}

	// Out of memory condition
	ASSERT(sSingletonNetworkMeter);

	return sSingletonNetworkMeter;
}

NetworkMeter::NetworkMeter()
{
	this->mCurrentEntity = NULL;
	this->mBytesPerSecond = 0;
}

void NetworkMeter::SetBufferAmount(int inBytesPerSecond)
{
	this->mBytesPerSecond = inBytesPerSecond;
}

void NetworkMeter::AddMessage(const char* inMessageName, int inMessageID)
{
	this->mNetworkMessageTypes[inMessageID] = string(inMessageName);
}

string NetworkMeter::LookupMessageID(int inMessageID)
{
	string theMessageName = this->mNetworkMessageTypes[inMessageID];
	if(theMessageName == "")
	{
		theMessageName = "Unknown";
	}
	return theMessageName;
}


void NetworkMeter::MessageBegin(int inMessageDest, int inMessageType, const float* inOrigin, edict_t* inEd)
{
	this->mCurrentEntity = inEd;
	this->GetPlayerNetworkMeter(this->mCurrentEntity).MessageBegin(inMessageDest, inMessageType, inOrigin, inEd);
}

void NetworkMeter::MessageEnd()
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).MessageEnd();
}

void NetworkMeter::WriteByte(int inByte)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteByte(inByte);
}

void NetworkMeter::WriteChar(int inChar)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteChar(inChar);
}

void NetworkMeter::WriteShort(int inShort)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteShort(inShort);
}

void NetworkMeter::WriteLong(int inLong)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteLong(inLong);
}

void NetworkMeter::WriteAngle(float inAngle)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteAngle(inAngle);
}

void NetworkMeter::WriteCoord(float inCoord)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteCoord(inCoord);
}

void NetworkMeter::WriteString(const char* inString)
{
	this->GetPlayerNetworkMeter(this->mCurrentEntity).WriteString(inString);
}

void NetworkMeter::ProcessQueuedMessages()
{
	// Iterate through all player network meters
	for(PlayerNetworkMeterMap::iterator theIter = this->mPlayerList.begin(); theIter != this->mPlayerList.end(); theIter++)
	{
		theIter->second.SetBufferAmount(this->mBytesPerSecond);
		theIter->second.ProcessQueuedMessages();
	}
}

PlayerNetworkMeter&	NetworkMeter::GetPlayerNetworkMeter(edict_t* inEd)
{
	// Look up edict in map.  If it doesn't exist, this will create a new entry
	return this->mPlayerList[inEd];
}
