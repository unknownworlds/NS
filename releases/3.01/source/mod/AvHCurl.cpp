//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHCurl.cpp $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================

#include "build.h"

#if !defined(USE_UPP) // forget entire file if we're using UPP instead

#include "util/nowarnings.h"
#include "extdll.h"
#include "dlls/util.h"
#include "util/Tokenizer.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#include "curl/include/curl/curl.h"

extern cvar_t							avh_serverops;
extern unsigned int						gTimeLastUpdatedUplink;
extern AuthMaskListType					gAuthMaskList;

extern const char* kSteamIDPending;
extern const char* kSteamIDPrefix;

//const char* kWebStatusURL = "https://www.natural-selection.org/cgi-bin/VictoryStats.pl";
//const char* kWebStatusURL = "http://www.natural-selection.org/cgi-bin/ikonboard/ikonboard.cgi";

void AvHGamerules::PostVictoryStatsToWeb(const string& inFormParams) const
{
//	CURL* curl;
//	CURLcode res;
//	
//	curl = curl_easy_init();
//	if(curl) 
//	{
//		/* First set the URL that is about to receive our POST. This URL can
//		   just as well be a https:// URL if that is what should receive the
//		   data. */
//		curl_easy_setopt(curl, CURLOPT_URL, kWebStatusURL);
//
//		/* Now specify the POST data */
//		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, inFormParams.c_str());
//		
//		/* Perform the request, res will get the return code */
//		res = curl_easy_perform(curl);
//		
//		/* always cleanup */
//		curl_easy_cleanup(curl);
//	}

    //struct soap soap;	// allocate gSOAP runtime environment
    //soap;

/*
    char *sym;
    float q;
    if (argc <= 1)
        sym = "IBM";
    else
        sym = argv[1];
    soap_init(&soap);	// must initialize
    if (soap_call_ns__getQuote(&soap, "http://services.xmethods.net/soap", "", sym, &q) == 0)
        printf("\nCompany - %s    Quote - %f\n", sym, q);
    else
    {
        soap_print_fault(&soap, stderr);
        soap_print_fault_location(&soap, stderr);
    }
*/
}

// This authID could be either a WONID or a STEAMID
int	AvHGamerules::GetAuthenticationMask(const string& inAuthID) const
{
	// Iterate through each mask type, oring it into the mask if id is found
	int theMask = 0;

	if(inAuthID == kSteamIDPending)
	{
		theMask = PLAYERAUTH_PENDING;
	}
	else
	{
		// Error check the incoming authID
		if(AvHSUGetIsValidAuthID(inAuthID))
		{
			for(AuthMaskListType::const_iterator theIter = gAuthMaskList.begin(); theIter != gAuthMaskList.end(); theIter++)
			{
				const AuthIDListType& theAuthIDList = theIter->second;

				for(AuthIDListType::const_iterator theAuthListIter = theAuthIDList.begin(); theAuthListIter != theAuthIDList.end(); theAuthListIter++)
				{
					// Check if incoming authID is equal to this record's WONID _or_ STEAMID.  This is kind of sloppy, but it allows backwards compatibility.
					if((theAuthListIter->first == inAuthID) || (theAuthListIter->second == inAuthID))
					{
						AvHPlayerAuthentication theCurrentAuthMask = theIter->first;
						theMask |= theCurrentAuthMask;
						break;
					}
				}
			}

			// Check if any players are server ops
			const AuthIDListType& theServerOps = this->GetServerOpList();
			for(AuthIDListType::const_iterator theAuthListIter = theServerOps.begin(); theAuthListIter != theServerOps.end(); theAuthListIter++)
			{
				if((inAuthID == theAuthListIter->first) || (inAuthID == theAuthListIter->second))
				{
					theMask |= PLAYERAUTH_SERVEROP;
					break;
				}
			}
		}
	}
	
	return theMask;
}

void AvHGamerules::AddAuthStatus(AvHPlayerAuthentication inAuthMask, const string& inWONID, const string& inSteamID)
{
//	int theData = 0;

	// Not sure why this is needed, but it seems to ("warning, decorated name length exceeded)
	#pragma warning (disable: 4503)

	AuthIDListType& theAuthList = gAuthMaskList[inAuthMask];

	bool theFoundEntry = false;

	for(AuthIDListType::iterator theIter = theAuthList.begin(); theIter != theAuthList.end(); theIter++)
	{
        // Allow duplicate WONids but not SteamIDs
		if(inSteamID == theIter->second)
		{
			theFoundEntry = true;
			break;
		}
	}

	if(!theFoundEntry)
	{
		// Add entry for this id
		theAuthList.push_back( make_pair(inWONID, inSteamID) );
	}
}

struct MemoryStruct {
	char *memory;
	size_t size;
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	register int realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;
	
	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

bool TagToAuthMask(const string& inTag, AvHPlayerAuthentication& outMask)
{
	bool theSuccess = false;

	if(inTag == "dev")
	{
		outMask = PLAYERAUTH_DEVELOPER;
		theSuccess = true;
	}
	else if(inTag == "guide")
	{
		outMask = PLAYERAUTH_GUIDE;
		theSuccess = true;
	}
	else if(inTag == "op")
	{
		outMask = PLAYERAUTH_SERVEROP;
		theSuccess = true;
	}
	else if(inTag == "pt")
	{
		outMask = PLAYERAUTH_PLAYTESTER;
		theSuccess = true;
	}
	else if(inTag == "const")
	{
		outMask = PLAYERAUTH_CONTRIBUTOR;
		theSuccess = true;
	}
	else if(inTag == "vet")
	{
		outMask = PLAYERAUTH_VETERAN;
		theSuccess = true;
	}
	else if(inTag == "betaop")
	{
		outMask = PLAYERAUTH_BETASERVEROP;
		theSuccess = true;
	}

	return theSuccess;
}

// Build string in obfuscated procedural way so it can't be scanned for and hacked.  Build URL using IP instead of domain to 
// make sure proxy can't change it either.
string BuildAuthenticationURL()
{
	// "http://www.natural-selection.org/auth.php";
	// (backwards-compatibly for pre-2.1 servers use: (http://www.natural-selection.org/auth/)
	string kAuthURL = "h";

	kAuthURL += "t";
	kAuthURL += "t";
	kAuthURL += "p";
	//kAuthURL += "s";
	kAuthURL += ":";
	kAuthURL += "/";
	kAuthURL += "/";
	kAuthURL += "w";
	kAuthURL += "w";
	kAuthURL += "w";
	kAuthURL += ".";
	kAuthURL += "n";
	kAuthURL += "a";
	kAuthURL += "t";
	kAuthURL += "u";
	kAuthURL += "r";
	kAuthURL += "a";
	kAuthURL += "l";
	kAuthURL += "-";
	kAuthURL += "s";
	kAuthURL += "e";
	kAuthURL += "l";
	kAuthURL += "e";
	kAuthURL += "c";
	kAuthURL += "t";
	kAuthURL += "i";
	kAuthURL += "o";
	kAuthURL += "n";
	kAuthURL += ".";
	kAuthURL += "o";
	kAuthURL += "r";
	kAuthURL += "g";
	kAuthURL += "/";
	kAuthURL += "a";
	kAuthURL += "u";
	kAuthURL += "t";
	kAuthURL += "h";
	kAuthURL += "/";
	kAuthURL += "a";
	kAuthURL += "u";
	kAuthURL += "t";
	kAuthURL += "h";
	kAuthURL += ".";
	kAuthURL += "p";
	kAuthURL += "h";
	kAuthURL += "p";

	return kAuthURL;
}

string BuildVersionURL()
{
	// "http://207.44.144.68/auth.txt";
	string kAuthURL = "h";
	
	kAuthURL += "t";
	kAuthURL += "t";
	kAuthURL += "p";
	//kAuthURL += "s";
	kAuthURL += ":";
	kAuthURL += "/";
	kAuthURL += "/";
	kAuthURL += "w";
	kAuthURL += "w";
	kAuthURL += "w";
	kAuthURL += ".";
	kAuthURL += "n";
	kAuthURL += "a";
	kAuthURL += "t";
	kAuthURL += "u";
	kAuthURL += "r";
	kAuthURL += "a";
	kAuthURL += "l";
	kAuthURL += "-";
	kAuthURL += "s";
	kAuthURL += "e";
	kAuthURL += "l";
	kAuthURL += "e";
	kAuthURL += "c";
	kAuthURL += "t";
	kAuthURL += "i";
	kAuthURL += "o";
	kAuthURL += "n";
	kAuthURL += ".";
	kAuthURL += "o";
	kAuthURL += "r";
	kAuthURL += "g";
	kAuthURL += "/";
	kAuthURL += "a";
	kAuthURL += "u";
	kAuthURL += "t";
	kAuthURL += "h";
	kAuthURL += "/";
	kAuthURL += "v";
	kAuthURL += "e";
	kAuthURL += "r";
	kAuthURL += "s";
	kAuthURL += "i";
	kAuthURL += "o";
	kAuthURL += "n";
	kAuthURL += ".";
	kAuthURL += "t";
	kAuthURL += "x";
	kAuthURL += "t";
	
	return kAuthURL;
}

string BuildUserPassword()
{
	string theUserPassword;

	// auth:mnbv5tgb
	theUserPassword += "a";
	theUserPassword += "u";
	theUserPassword += "t";
	theUserPassword += "h";
	theUserPassword += ":";
	theUserPassword += "m";
	theUserPassword += "n";
	theUserPassword += "b";
	theUserPassword += "v";
	theUserPassword += "5";
	theUserPassword += "t";
	theUserPassword += "g";
	theUserPassword += "b";
	
	return theUserPassword;
}

CURLcode PopulateChunkFromURL(const string& inURL, MemoryStruct& outChunk)
{
	const int kCurlTimeout = 5;

	// init the curl session
	CURL* theCurlHandle = curl_easy_init();
	
	// specify URL to get
	curl_easy_setopt(theCurlHandle, CURLOPT_URL, inURL.c_str());
	
	// send all data to this function
	curl_easy_setopt(theCurlHandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	// Ignore header and progress so there isn't extra stuff at the beginning of the data returned
	//curl_easy_setopt(theCurlHandle, CURLOPT_HEADER, 0);
	//curl_easy_setopt(theCurlHandle, CURLOPT_NOPROGRESS, 1);
    //curl_easy_setopt(theCurlHandle, CURLOPT_VERBOSE, 0);
    //curl_easy_setopt(theCurlHandle, CURLOPT_NOBODY, 1);

	// timeout (3 seconds)
	curl_easy_setopt(theCurlHandle, CURLOPT_CONNECTTIMEOUT, kCurlTimeout);
	curl_easy_setopt(theCurlHandle, CURLOPT_TIMEOUT, kCurlTimeout);
	curl_easy_setopt(theCurlHandle, CURLOPT_NOSIGNAL, true);
	// CURLOPT_LOW_SPEED_LIMIT?
	// CURLOPT_LOW_SPEED_TIME?

	// Specify the user name and password
	string kUserPassword = BuildUserPassword();
	curl_easy_setopt(theCurlHandle, CURLOPT_USERPWD, kUserPassword.c_str());
	
	// we pass our 'theChunk' struct to the callback function
	outChunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	outChunk.size = 0;    /* no data at this point */
	curl_easy_setopt(theCurlHandle, CURLOPT_FILE, (void *)&outChunk);
	
	// get it!
	CURLcode theCode = curl_easy_perform(theCurlHandle);
	
	// cleanup curl stuff
	curl_easy_cleanup(theCurlHandle);
   
	return theCode;
}

void AvHGamerules::InitializeAuthentication()
{
	bool theSuccess = false;

	ALERT(at_console, "\tReading authentication data...");

    struct MemoryStruct theChunk;
	CURLcode theCode = PopulateChunkFromURL(BuildAuthenticationURL(), theChunk);
	
	// Now parse data and add users for each mask
	if((theCode == CURLE_OK) && theChunk.memory)
	{
        // Clear existing authentication data, only after lookup succeeds
        gAuthMaskList.clear();

		string theString(theChunk.memory);
		string theDelimiters("\r\n");
		StringVector theLines;
		Tokenizer::split(theString, theDelimiters, theLines);
		
		// Run through each line
		int theNumConstellationMembers = 0;
		for(StringVector::const_iterator theIter = theLines.begin(); theIter != theLines.end(); theIter++)
		{
			// If it's not empty and not a comment
			char theFirstChar = (*theIter)[0];
			if((theFirstChar != '/') && (theFirstChar != '#'))
			{
				// Split up tag and number
				StringVector theTokens;
				if(Tokenizer::split(*theIter, " ", theTokens) == 3)
				{
					// Translate tag to auth mask
					string theTag = theTokens[0];
					string theWONID = theTokens[1];
					string theSteamID = theTokens[2];

                    // Upper-case prefix
                    UppercaseString(theSteamID);

                    // Make sure it starts with "STEAM_X:Y"
					if(strncmp(theSteamID.c_str(), kSteamIDPrefix, strlen(kSteamIDPrefix)))
                    {
                        string theNewSteamID = kSteamIDPrefix + theSteamID;
                        theSteamID = theNewSteamID;
                    }

					// Add auth status
					AvHPlayerAuthentication theMask = PLAYERAUTH_NONE;
					if(TagToAuthMask(theTag, theMask))
					{
						// Count Constellation members fyi
						if(theMask & PLAYERAUTH_CONTRIBUTOR)
						{
							theNumConstellationMembers++;
						}

						if((theWONID != "") || (theSteamID != ""))
						{
#ifdef DEBUG
							char theMessage[512];
							sprintf(theMessage, "	Adding auth mask: %s %s %s\n", theTag.c_str(), theWONID.c_str(), theSteamID.c_str());
							ALERT(at_logged, theMessage);
#endif
							
							this->AddAuthStatus(theMask, theWONID, theSteamID);
							theSuccess = true;
						}
					}
				}
			}
		}

		// Breakpoint to see how many Constellation members there are (don't even think of printing or logging)
		int a = 0;
	}
    else
    {
        int a = 0;
    }

	// Now build server op list
	this->mServerOpList.clear();
		
	// Parse contents server op variable
	string theServerOpsString(avh_serverops.string);
	
	// Tokenize string
	StringVector theServerOpsStrings;
	Tokenizer::split(theServerOpsString, ";", theServerOpsStrings);
	
	// For each in list, add as an int to our list
	for(StringVector::const_iterator theIter = theServerOpsStrings.begin(); theIter != theServerOpsStrings.end(); theIter++)
	{
		// Add whatever the put in this line as both the WONID and SteamID
		string theCurrentAuthID = *theIter;
		this->mServerOpList.push_back( make_pair(theCurrentAuthID, theCurrentAuthID));
	}
	
	if(theSuccess)
	{
		ALERT(at_console, "success.\n");
	}
	else
	{
		ALERT(at_console, "failure.\n");
	}
	gTimeLastUpdatedUplink = AvHSUTimeGetTime();
}

void AvHGamerules::DisplayVersioning()
{
	struct MemoryStruct theChunk;
	CURLcode theCode = PopulateChunkFromURL(BuildVersionURL(), theChunk);

	// Now parse data and add users for each mask
	if((theCode == CURLE_OK) && theChunk.memory)
	{
		string theString(theChunk.memory, theChunk.size);

		string theDelimiters("\r\n");
		StringVector theLines;
		Tokenizer::split(theString, theDelimiters, theLines);

		if(theLines.size() > 0)
		{
			string theCurrentVersion(theLines[0]);
			string theCurrentGameVersion = AvHSUGetGameVersionString();

			char theMessage[1024];
			if(theCurrentGameVersion != theCurrentVersion)
			{
				sprintf(theMessage, "\tServer out of date.  NS version %s is now available!\n", theCurrentVersion.c_str());
				ALERT(at_console, theMessage);
			}
			else
			{
				sprintf(theMessage, "\tServer version is up to date.\n");
				ALERT(at_console, theMessage);
			}
		}
	}
}

#endif //!defined(USE_UPP)