//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: Checksum.cpp $
// $Date: 2002/05/23 04:03:11 $
//
//-------------------------------------------------------------------------------
// $Log: Checksum.cpp,v $
// Revision 1.1  2002/05/23 04:03:11  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/Checksum.h"
#include "util/STLUtil.h"

const string kFormattedStringDelimiter = " -> ";
const string kFormattedStringEOL = "\n";

ChecksumEntry::ChecksumEntry(void)
{ 
	this->mChecksum = 0; 
}

uint32 ChecksumEntry::GetChecksum() const
{ 
	return this->mChecksum; 
}

const string& ChecksumEntry::GetDescription() const
{ 
	return this->mChecksumDescription; 
}

string ChecksumEntry::GetFormattedString() const
{ 
	string theFormattedString = this->mChecksumDescription;
	theFormattedString += kFormattedStringDelimiter;
	theFormattedString += MakeStringFromInt(this->mChecksum);
	theFormattedString += kFormattedStringEOL;

	return theFormattedString; 
}


void ChecksumEntry::SetChecksum(uint32 inChecksum)
{ 
	this->mChecksum = inChecksum; 
}

void ChecksumEntry::SetDescription(const string& inDescription)
{ 
	this->mChecksumDescription = inDescription; 
}

bool ChecksumEntry::SetFromFormattedString(const string& inFormattedString)
{
	bool theSuccess = false;

	size_t theEOL = inFormattedString.find(kFormattedStringEOL);
	if(theEOL != std::string::npos)
	{
		string theNonDelimitedString = inFormattedString.substr(0, theEOL);
		
		// Parse out delimiter
		size_t theSplit = theNonDelimitedString.find(kFormattedStringDelimiter);
		if(theSplit != std::string::npos)
		{
			// Everything before it is description
			string theDescription = theNonDelimitedString.substr(0, theSplit-1);
		
			// Everything after is checksum
			string theChecksumString = theNonDelimitedString.substr(theSplit + kFormattedStringDelimiter.size());
		
			this->mChecksumDescription = theDescription;
			this->mChecksum = MakeIntFromString(theChecksumString);
		
			theSuccess = true;
		}
	}
	
	return theSuccess;
}


bool ChecksumEntry::Compare(const ChecksumEntry& inChecksumEntry, string& theErrorString) const
{
	bool theReturnCode = false;

	if(inChecksumEntry.mChecksumDescription != this->mChecksumDescription)
	{
		sprintf(theErrorString, "ChecksumEntry::Compare failed, the tags don't match: %s != %s\n", inChecksumEntry.mChecksumDescription.c_str(), this->mChecksumDescription.c_str());
	}
	else if(inChecksumEntry.mChecksum != this->mChecksum)
	{
		sprintf(theErrorString, "ChecksumEntry::Compare failed, checksums don't match: %u != %u (%s)\n", inChecksumEntry.mChecksum, this->mChecksum, inChecksumEntry.mChecksumDescription.c_str());
	}
	else
	{
		theReturnCode = true;
	}
	
	return theReturnCode;
}

Checksum::Checksum(bool inVerboseMode)
{
	this->mIsVerboseMode = inVerboseMode;
}

void Checksum::AddChecksum(const string& inInfoString, uint32 inChecksum)
{
	ChecksumEntry theChecksumEntry;

	// Add a new tagged checksum or just add the new checksum number to our total depending on verbose mode
	if(this->mIsVerboseMode)
	{
		// Initialize new checksum entry
		theChecksumEntry.SetDescription(inInfoString);
		theChecksumEntry.SetChecksum(inChecksum);
		
		// Add our new entry
		this->mChecksumList.push_back(theChecksumEntry);
	}
	else
	{
		this->AddChecksum(inChecksum);
	}
}

void Checksum::AddFloatChecksum(const string& inInfoString, float inChecksum)
{
	// Convert float to uint32
	uint32 theUint32Checksum = (uint32)(inChecksum*1000.0f);
	this->AddChecksum(inInfoString, theUint32Checksum);
}


void Checksum::AddChecksum(uint32 inChecksum)
{
	ChecksumList::iterator		theIterator;
	ChecksumEntry				theNewEntry;

	// Make sure there is at least one checksum already, add one otherwise
	if(this->mChecksumList.size() == 0)
	{
		// Add new checksum with no description
		this->mChecksumList.push_back(theNewEntry);
	}

	// Get iterator to last checksum
	theIterator = this->mChecksumList.end() - 1;

	// Add inChecksum to the existing value
	theIterator->SetChecksum(theIterator->GetChecksum() + inChecksum);
}

bool Checksum::Compare(const Checksum& inChecksum, StringList& outErrors) const
{
	ChecksumList::const_iterator	theIncomingIter = inChecksum.mChecksumList.begin();
	ChecksumList::const_iterator	theSourceIter = this->mChecksumList.begin();

	string							theErrorString;

	int32							theChecksumCount = 0;

	bool							theReturnCode = true;

	// Only try compare if both checksums are in the same mode 
	if(this->GetIsVerboseMode() == inChecksum.GetIsVerboseMode())
	{
		if(this->mChecksumList.size() == inChecksum.mChecksumList.size())
		{
			for( ; theSourceIter != this->mChecksumList.end(); theSourceIter++, theIncomingIter++)
			{
				if(!(theSourceIter->Compare(*theIncomingIter, theErrorString)))
				{
					outErrors.push_back(theErrorString);
					theReturnCode = false;
				}
				theChecksumCount++;
			}
		}
		else
		{
			sprintf(theErrorString, "Checksum::Compare(): Checksum sizes don't match.  Source size is %d and other size is %d.\n", this->mChecksumList.size(), inChecksum.mChecksumList.size());
			outErrors.push_back(theErrorString);
			theReturnCode = false;
		}
	}
	else
	{
		sprintf(theErrorString, "Checksum::Compare(): One checksum is in verbose mode, the other isn't.\n");
		outErrors.push_back(theErrorString);
		theReturnCode = false;
	}

	return theReturnCode;
}

bool Checksum::GetIsVerboseMode(void) const
{
	return this->mIsVerboseMode;
}

uint32 Checksum::GetTotalChecksum(void) const
{
	ChecksumList::const_iterator	theSourceIter = this->mChecksumList.begin();
	uint32							theTotalChecksum = 0;
	
	for( ; theSourceIter != this->mChecksumList.end(); theSourceIter++)
	{
		theTotalChecksum += theSourceIter->mChecksum;
	}
	
	return theTotalChecksum;
}

void Checksum::PrintReport(void) const
{
	//	ChecksumList::const_iterator	theSourceIter = this->mChecksumList.begin();
	//
	//	::ESReport("Checksum::PrintReport(): begin.\n");
	//	for( ; theSourceIter != this->mChecksumList.end(); theSourceIter++)
	//	{
	//		::ESReport("  %s, %u\r\n", (const char*)(theSourceIter->mChecksumDescription), theSourceIter->mChecksum);
	//	}
	//	::ESReport("Checksum::PrintReport(): complete.\n");
}

bool Checksum::ReadFromFile(const char* inFilename) 
{
	bool theSuccess = false;

	this->mChecksumList.clear();
	
	ifstream theStream(inFilename);
	if(theStream.is_open())
	{
		string theStartString;
		theStream >> theStartString;

		// Read in num checksums
		int theNumChecksums = 0;
		theStream >> theNumChecksums;
		
		// Read in pairs of checksums
		for(int i = 0; i < theNumChecksums; i++)
		{
			string theFormattedString;
			theStream >> theFormattedString;

			ChecksumEntry theNewEntry;
			theNewEntry.SetFromFormattedString(theFormattedString);
			this->mChecksumList.push_back(theNewEntry);
		}
		
		string theEndString;
		theStream >> theEndString;
		
		theStream.close();
	}

	return theSuccess;
}

bool Checksum::SaveToFile(const char* inFilename) const
{
	bool theSuccess = false;

	// open file for writing
	ofstream theStream(inFilename);
	if(theStream.is_open())
	{
		string theStartString("CHECKSUM-REPORT:\r\n");
		theStream << theStartString;

		int theNumChecksums = (int)this->mChecksumList.size();
		theStream << theNumChecksums;

		// dump everything to it
		for(ChecksumList::const_iterator theIter = this->mChecksumList.begin(); theIter != this->mChecksumList.end(); theIter++)
		{
			string theFormattedDescription = theIter->GetFormattedString();
			theStream << theFormattedDescription;
		}
		
		string theEndString("END.\r\n");
		theStream << theEndString;

		theStream.close();

		theSuccess = true;
	}

//	FILE* theFile = fopen(inFilename, "wt");
//	if(theFile)
//	{
//		string theStartString("Checksum report:\r\n");
//		fwrite(theStartString.c_str(), theStartString.length(), 1, theFile);
//		
//		// dump everything to it
//		for(ChecksumList::const_iterator theIter = this->mChecksumList.begin(); theIter != this->mChecksumList.end(); theIter++)
//		{
//			const string& theFormattedString = theIter->GetFormattedString();
//			fwrite(theFormattedString.c_str(), theFormattedString.length(), 1, theFile);
//			fwrite("\r\n", 2, 1, theFile);
//		}
//
//		string theEndString("Checksum report complete.\r\n");		
//		fwrite(theEndString.c_str(), theEndString.length(), 1, theFile);
//		
//		// close it
//		int theCloseRC = fclose(theFile);
//		if(theCloseRC == 0)
//		{
//			theSuccess = true;
//		}
//	}

	//	ChecksumList::const_iterator	theSourceIter = this->mChecksumList.begin();
	//
	//	::ESReport("Checksum::PrintReport(): begin.\n");
	//	for( ; theSourceIter != this->mChecksumList.end(); theSourceIter++)
	//	{
	//		::ESReport("  %s, %u\r\n", (const char*)(theSourceIter->mChecksumDescription), theSourceIter->mChecksum);
	//	}
	//	::ESReport("Checksum::PrintReport(): complete.\n");
	
	return theSuccess;
}

