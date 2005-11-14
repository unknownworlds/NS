//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: TRFactory.h $
// $Date: 2002/08/16 02:28:25 $
//
//-------------------------------------------------------------------------------
// $Log: TRFactory.h,v $
// Revision 1.5  2002/08/16 02:28:25  Flayra
// - Added document headers
//
//===============================================================================
#ifndef TRFACTORY_H
#define TRFACTORY_H

#include "textrep/TRDescription.h"
#include "string.h"

// Build a list of text description from a file.  The filename is relative to the mod's base directory.  
// This exists outside of TRDescription because it is conceptually different, but is closely coupled with it.
class TRFactory
{
public:
    // Read all the descriptions from the file
    static  bool    ReadDescriptions(const string& inRelativePathFilename, TRDescriptionList& outDescriptionList);
    static  bool    WriteDescriptions(const string& inRelativePathFilename, const TRDescriptionList& inDescriptionList, const string& inHeader);

private:
    static  bool    ReadDescription(fstream& infile, TRDescription& outDescription);
    static  bool    WriteDescription(fstream& outfile, const TRDescription& inDescription);

	static  bool	charIsWhiteSpace(char inChar);
    static  void    trimWhitespace(string& inString);
    static  bool    lineIsAComment(const string& inString);
    static  bool    readAndTrimNextLine(istream& inStream, string& outLine);
    static  bool    readStartBlockLine(const string& inString, TRDescription& outDescription);
    static  bool    readEndBlockLine(const string& inString);
    static  bool    readTagAndValueLine(const string& inString, TRDescription& outDescription);

};

#endif