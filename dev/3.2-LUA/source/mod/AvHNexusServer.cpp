#ifdef AVH_PLAYTEST_BUILD
	#define AVH_NO_NEXUS
#endif

#ifdef AVH_NO_NEXUS
	#include <string>
	using std::string;
	#include "AvHNexusServer.h"

	bool AvHNexus::send(entvars_t* const pev, const unsigned char* data, const unsigned int length) { return false; }
	bool AvHNexus::recv(entvars_t* const pev, const char* data, const unsigned int length) { return false; }
	string AvHNexus::getNetworkID(const edict_t* edict) { return ""; }
	void AvHNexus::handleUnauthorizedJoinTeamAttempt(const edict_t* edict, const unsigned char team_index) {}
	void AvHNexus::performSpeedTest(void) {}
	void AvHNexus::processResponses(void) {}
	void AvHNexus::setGeneratePerformanceData(const edict_t* edict, const bool generate) {}
	bool AvHNexus::getGeneratePerformanceData(void) { return false; }
	bool AvHNexus::isRecordingGame(void) { return false; }
	void AvHNexus::cancelGame(void) {}
	void AvHNexus::finishGame(void) {}
	void AvHNexus::startGame(void) {}
	void AvHNexus::startup(void) {}
	void AvHNexus::shutdown(void) {}
#else
	#include <NexusServerInterface.h>
	#include "AvHNexusServer.h"
	#include "AvHNexusTunnelToClient.h"

	#include "NetworkMeter.h"
	extern int g_msgNexusBytes;

	byte_string BASE64Decode(const string& input);
	Nexus::ServerInfo createServerInfo(void);

	//note: we place this here so that we have the possibility of giving out AvHNetworkMessages.cpp
	bool AvHNexus::send(entvars_t* const pev, const unsigned char* data, const unsigned int length)
	{
		if( !pev ) { return false; }
		MESSAGE_BEGIN( MSG_ONE, g_msgNexusBytes, NULL, pev );
			for( int counter = 0; counter < length; counter++ )
			{ WRITE_BYTE(data[counter]); }
		MESSAGE_END();
		return true;
	}

	bool AvHNexus::recv(entvars_t* const pev, const char* data, const unsigned int length)
	{
		string base64_message(data,length);
		byte_string message = BASE64Decode(base64_message);
		Nexus::ClientID client = OFFSET(pev);
		TunnelToClient::getInstance()->insertMessage(client,message);
		return true;
	}

	inline CBaseEntity* getEntity(const edict_t* edict)
	{
		return edict == NULL ? NULL : (CBaseEntity*)edict->pvPrivateData;
	}

	string AvHNexus::getNetworkID(const edict_t* edict)
	{
		//TODO: resolve this!
		return "";
	}

	void AvHNexus::handleUnauthorizedJoinTeamAttempt(const edict_t* edict, const unsigned char team_index)
	{
		//TODO: teleport ready room player to spawn to prevent blocked doorway
	}

	void AvHNexus::performSpeedTest(void)
	{
		//TODO: check permission to do this before allowing it
		Nexus::testConnectionSpeed();
	}

	void AvHNexus::processResponses(void)
	{
		//TODO: do something with the responses as we consume them!
		Nexus::ClientID id;
		while((id = Nexus::hasHandleInfo()) != 0)
		{
			Nexus::getHandleInfo(id);
		}
		while((id = Nexus::hasHistoryInfo()) != 0)
		{
			Nexus::getHistoryInfo(id);
		}
		while(Nexus::hasErrorReport())
		{
			Nexus::getErrorReport();
		}
		while(Nexus::hasPerformanceData())
		{
			Nexus::getPerformanceData();
		}
	}

	void AvHNexus::setGeneratePerformanceData(const edict_t* edict, const bool generate)
	{
		//TODO: check permission to do this before allowing it
		Nexus::setGeneratePerformanceData(generate);
		CBaseEntity* player = getEntity(edict);
		if(player && AvHNexus::getGeneratePerformanceData())
		{
			UTIL_SayText("Nexus Profiling is now ON", player);
		}
		else
		{
			UTIL_SayText("Nexus Profiling is now OFF", player);
		}
	}

	bool AvHNexus::getGeneratePerformanceData(void)
	{
		return Nexus::getGeneratePerformanceData();
	}

	bool AvHNexus::isRecordingGame(void)
	{
		return Nexus::isRecordingEvent();
	}

	void AvHNexus::cancelGame(void) 
	{
		Nexus::reportEventCancelled(); 
	}

	void AvHNexus::finishGame(void) 
	{ 
		Nexus::EventResultInfo info;
		Nexus::reportEventFinished(info); 
	}

	void AvHNexus::startGame(void) 
	{ 
		Nexus::EventInfo info;
		Nexus::reportEventStarted(info); 
	}

	void AvHNexus::startup(void)
	{
		Nexus::setTunnelToClient(TunnelToClient::getInstance());
		Nexus::setProductName("Natural Selection-temp product");
		Nexus::connect(createServerInfo());
	}

	void AvHNexus::shutdown(void)
	{
		Nexus::disconnect();
	}

	Nexus::ServerInfo(createServerInfo())
	{
		Nexus::ServerInfo info;
		info.setName( string( CVAR_GET_STRING( "hostname" ) ) );
		info.setProduct( Nexus::getProductName() );
		return info;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base 64 decoder
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	byte Base64DecodeTable[256] = {
		0,  0,  0,  0,  0,  0,  0,  0, //  0-  7
		0,  0,  0,  0,  0,  0,  0,  0, //  8- 15
		0,  0,  0,  0,  0,  0,  0,  0, // 16- 23
		0,  0,  0,  0,  0,  0,  0,  0, // 24- 31
		0,  0,  0,  0,  0,  0,  0,  0, // 32- 39
		0,  0,  0, 62,  0,  0,  0, 63, // 40- 47 ( 43 = '+', 47 = '/')
		52, 53, 54, 55, 56, 57, 58, 59, // 48- 55 ( 48- 55 = 0-7)
		60, 61,  0,  0,  0,  0,  0,  0, // 56- 63 ( 56- 57 = 8-9)
		0,  0,  1,  2,  3,  4,  5,  6, // 64- 71 ( 65- 71 = A-G)
		7,  8,  9, 10, 11, 12, 13, 14, // 72- 79 ( 72- 79 = H-O)
		15, 16, 17, 18, 19, 20, 21, 22, // 80- 87 ( 80- 87 = P-W)
		23, 24, 25,  0,  0,  0,  0,  0, // 88- 95 ( 88- 90 = X-Z)
		0, 26, 27, 28, 29, 30, 31, 32, // 96 003 ( 97 003 = a-g)
		33, 34, 35, 36, 37, 38, 39, 40, //104 011 (104-111 = h-o)
		41, 42, 43, 44, 45, 46, 47, 48, //112 019 (112-119 = p-w)
		49, 50, 51,  0,  0,  0,  0,  0, //120 027 (120-122 = x-z)
		0,  0,  0,  0,  0,  0,  0,  0, //128 035
		0,  0,  0,  0,  0,  0,  0,  0, //136 043
		0,  0,  0,  0,  0,  0,  0,  0, //144 051
		0,  0,  0,  0,  0,  0,  0,  0, //152 059
		0,  0,  0,  0,  0,  0,  0,  0, //160 067
		0,  0,  0,  0,  0,  0,  0,  0, //168 075
		0,  0,  0,  0,  0,  0,  0,  0, //176 083
		0,  0,  0,  0,  0,  0,  0,  0, //184 091
		0,  0,  0,  0,  0,  0,  0,  0, //192 099
		0,  0,  0,  0,  0,  0,  0,  0, //200-207
		0,  0,  0,  0,  0,  0,  0,  0, //208-215
		0,  0,  0,  0,  0,  0,  0,  0, //216-223
		0,  0,  0,  0,  0,  0,  0,  0, //224-231
		0,  0,  0,  0,  0,  0,  0,  0, //232-239
		0,  0,  0,  0,  0,  0,  0,  0, //240-247
		0,  0,  0,  0,  0,  0,  0,  0  //248-255
	};

	//debugged and working properly... do not disturb.
	byte_string BASE64Decode(const string& input)
	{
		byte_string output;
		const byte* data = (const byte*)input.c_str();
		size_t length = input.length();
		byte value, value2;

		while( length > 0 )
		{
			value = Base64DecodeTable[data[0]];
			value2 = Base64DecodeTable[data[1]];
			value <<= 2;
			value2 &= 0x30;	value2 >>= 4; value |= value2;
			output.push_back(value);
			if( data[2] != '=' )
			{
				value = Base64DecodeTable[data[1]];
				value2 = Base64DecodeTable[data[2]];
				value &= 0x0F; value <<= 4;
				value2 &= 0x3C; value2 >>= 2; value |= value2;
				output.push_back(value);
			}
			if( data[3] != '=' )
			{
				value = Base64DecodeTable[data[2]];
				value &= 0x03; value <<= 6;
				value |= Base64DecodeTable[data[3]];
				output.push_back(value);
			}
			data += 4; length -= 4;
		}
		return output;
	}
#endif
