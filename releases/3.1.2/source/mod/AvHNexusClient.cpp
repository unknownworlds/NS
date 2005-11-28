#define AVH_NO_NEXUS
#ifdef AVH_NO_NEXUS
	#include <string>
	using std::string;
	#include "AvHNexusClient.h"

	bool AvHNexus::send(const unsigned char* data, const size_t length) { return false; }
	bool AvHNexus::recv(const unsigned char* data, const size_t length) { return false; }

	void AvHNexus::startup(void) {}
	void AvHNexus::shutdown(void) {}

	bool AvHNexus::login(const string& name, const string& password) { return true; }
	bool AvHNexus::logout(void) { return true; }
#else
	#include <NexusClientInterface.h>
	#include "AvHNexusClient.h"
	#include "AvHNexusTunnelToServer.h"
	#include "cl_dll/hud.h"
	#include "cl_dll/cl_util.h"

	string BASE64Encode(const byte_string& input);
	int __MsgFunc_NexusData(const char *pszName, int iSize, void *pbuf);

	bool AvHNexus::send(const unsigned char* data, const size_t length)
	{
		byte_string raw_data(data,length);

		string cmdline("NexusData ");
		cmdline += BASE64Encode(raw_data);
		cmdline += "\n";

		//ugliness due to pfnClientCmd wanting a non-const ptr
		char* ptr = new char[cmdline.length()+1];
		strncpy(ptr,cmdline.c_str(),cmdline.length());
		ptr[cmdline.length()] = '\0';
		gEngfuncs.pfnClientCmd(ptr);
		delete[] ptr;

		return true;
	}

	bool AvHNexus::recv(const unsigned char* data, const size_t length)
	{
		byte_string raw_data(data,length);
		AvHNexus::TunnelToServer::getInstance()->insertMessage(raw_data);
		return true;
	}
	    
	void AvHNexus::startup(void)
	{
		gEngfuncs.pfnHookUserMsg("NexusBytes", __MsgFunc_NexusData);
	//	Nexus::setTunnelToServer(AvHNexus::TunnelToServer::getInstance());
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Incominng message handler
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	int __MsgFunc_NexusData(const char *pszName, int iSize, void *pbuf)
	{
		AvHNexus::recv((unsigned char*)pbuf, iSize);
		return iSize;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Base 64 encoder
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	char Base64EncodeTable[65] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',	// 0- 7
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', // 8-15
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', //16-23
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', //24-31
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //32-39
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v', //40-47
		'w', 'x', 'y', 'z', '0', '1', '2', '3', //48-55
		'4', '5', '6', '7', '8', '9', '+', '/', //56-63
		'=' };									//64 = padding

	//debugged and working properly... do not disturb.
	string BASE64Encode(const byte_string& input)
	{
		string output;
		const byte* data = input.c_str();
		size_t length = input.length();
		int value, value2;

		//handle input in 3 byte blocks
		while( length > 2 )
		{
			value = data[0]; value >>= 2;
			output.push_back(Base64EncodeTable[value]);
			value = data[0]; value2 = data[1];
			value &= 0x03; value <<= 4; 
			value2 &= 0xF0; value2 >>= 4; value |= value2;
			output.push_back(Base64EncodeTable[value]);
			value = data[1]; value2 = data[2];
			value &= 0x0F; value <<= 2; 
			value2 &= 0xC0; value2 >>= 6; value |= value2;
			output.push_back(Base64EncodeTable[value]);
			value = data[2]; value &= 0x3F;
			output.push_back(Base64EncodeTable[value]);
			data += 3; length -= 3;
		}

		//handle remainder
		switch(length)
		{
		case 0: //no remainder to process
			break;
		case 1: //process and pad with two =
			value = data[0]; value >>= 2;
			output.push_back(Base64EncodeTable[value]);
			value = data[0]; value &= 0x03; value <<= 4;
			output.push_back(Base64EncodeTable[value]);
			output.push_back(Base64EncodeTable[64]);
			output.push_back(Base64EncodeTable[64]);
			break;
		case 2: //process and pad with one =
			value = data[0]; value >>= 2;
			output.push_back(Base64EncodeTable[value]);
			value = data[0]; value2 = data[1];
			value &= 0x03; value <<= 4;
			value2 &= 0xF0; value2 >>= 4; value |= value2;
			output.push_back(Base64EncodeTable[value]);
			value = data[1]; value &= 0x0F; value <<= 2; 
			output.push_back(Base64EncodeTable[value]);
			output.push_back(Base64EncodeTable[64]);
			break;
		}

		return output;
	}
#endif