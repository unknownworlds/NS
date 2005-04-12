//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: Checksum.h $
// $Date: 2002/05/23 04:03:11 $
//
//-------------------------------------------------------------------------------
// $Log: Checksum.h,v $
// Revision 1.1  2002/05/23 04:03:11  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "types.h"

class ChecksumEntry
{
public:
					ChecksumEntry(void);

	uint32			GetChecksum() const;

	const string&	GetDescription() const;

	string			GetFormattedString() const;
	
	void			SetChecksum(uint32 inChecksum);

	void			SetDescription(const string& inDescription);

	bool			SetFromFormattedString(const string& inFormattedString);

private:
	friend class Checksum;

	bool			Compare(const ChecksumEntry& inChecksumEntry, string& outStringToAppendTo) const;

	string			mChecksumDescription;

	uint32			mChecksum;

};

class Checksum
{
public:


							Checksum(bool inVerboseMode = true);
	virtual					~Checksum(void) { }

	void					AddChecksum(const string& inInfoString, uint32 inChecksum);

	void					AddFloatChecksum(const string& inInfoString, float inChecksum);

	bool					Compare(const Checksum& inChecksum, StringList& outErrors) const;

	bool					GetIsVerboseMode(void) const;

	uint32					GetTotalChecksum(void) const;

	void					PrintReport(void) const;

	bool					ReadFromFile(const char* inFilename);

	bool					SaveToFile(const char* inFilename) const;

protected:
	void					AddChecksum(uint32 inChecksum);

	typedef					vector<ChecksumEntry>	ChecksumList;

	ChecksumList			mChecksumList;

	bool					mIsVerboseMode;

};

#endif