//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: TRDescription.cpp $
// $Date: 2002/07/26 01:53:49 $
//
//-------------------------------------------------------------------------------
// $Log: TRDescription.cpp,v $
// Revision 1.5  2002/07/26 01:53:49  Flayra
// - Linux support for FindFirst/FindNext
// - Added document headers
//
//===============================================================================
#include "TRDescription.h"

// Add a new pair read from the text file to this description.  They can be added in any order.
void TRDescription::AddPair(const TRTagValuePair& inPair)
{ 
    this->mTagValueList.push_back(inPair);
}

// Read a list of tags named prefix and then a number, starting with 1.
// Ie, sound1, sound2, sound3 etc.  It reads in order until it doesn't find a tag, then stops.
bool TRDescription::GetTagStringList(const TRTag& inTagPrefix, StringVector& outList) const
{
    bool theSuccess = false;

    for(int i = 1; ; i++)
    {
        char theNum[4];
        sprintf(theNum, "%d", i);
        string theTagName(inTagPrefix + string(theNum));
        string theTagString;
        
        if(this->GetTagValue(theTagName, theTagString))
        {
            // Successful if we found at least one tag
            outList.push_back(theTagString);
            theSuccess = true;
        }
        else
        {
            break;
        }
    }

    return theSuccess;
}

bool TRDescription::GetTagStringList(const TRTag& inTagPrefix, CStringList& outList) const
{
    bool theSuccess = false;

    for(int i = 1; ; i++)
    {
        char theNum[4];
        sprintf(theNum, "%d", i);
        string theTagName(inTagPrefix + string(theNum));
        string theTagString;

        if(this->GetTagValue(theTagName, theTagString))
        {
            CString theCTagString;
            theCTagString = theTagString;

            // Successful if we found at least one tag
            outList.push_back(theCTagString);
            theSuccess = true;
        }
        else
        {
            break;
        }
    }

    return theSuccess;
}

//template <class T>
//bool TRDescription::GetTagStringList(const TRTag& inTagPrefix, T& outList) const
//{
//    bool theSuccess = false;
//
//    for(int i = 1; ; i++)
//    {
//        char theNum[4];
//        sprintf(theNum, "%d", i);
//        string theTagName(inTagPrefix + string(theNum));
//        string theTagString;
//        
//        string theNodeString;
//        if(this->GetTagValue(theTagName, theTagString))
//        {
//            // Successful if we found at least one tag
//            outList.push_back(theTagString);
//            theSuccess = true;
//        }
//        else
//        {
//            break;
//        }
//    }
//
//    return theSuccess;
//}



// Get the integer associated with the named tag.  Returns false if the tag
// isn't present or the data format couldn't be read.
bool TRDescription::GetTagValue(const TRTag& inTag, int& outValue) const
{
    bool theSuccess = false;
    string theString;

    if(this->GetValue(inTag, theString))
    {
        if(sscanf(theString.c_str(), "%d", &outValue) == 1)
        {
            theSuccess = true;
        }
    }

    return theSuccess;
}

// Get the float associated with the named tag.  Returns false if the tag
// isn't present or the data format couldn't be read.
bool TRDescription::GetTagValue(const TRTag& inTag, float& outValue) const
{
    bool theSuccess = false;
    string theString;

    if(this->GetValue(inTag, theString))
    {
        if(sscanf(theString.c_str(),"%f", &outValue) == 1)
        {
            theSuccess = true;
        }
    }

    return theSuccess;
}
     
// Get the string associated with the named tag.  Returns false if the tag
// isn't present (it can always be read!).
bool TRDescription::GetTagValue(const TRTag& inTag, string& outValue) const
{
    bool theSuccess = false;

    if(this->GetValue(inTag, outValue))
    {
        theSuccess = true;
    }

    return theSuccess;
}

bool TRDescription::GetTagValue(const TRTag& inTag, CString& outValue) const
{
    string  theValue;
    bool    theSuccess = false;

    if(this->GetValue(inTag, theValue))
    {
        outValue = theValue;
        theSuccess = true;
    }

    return theSuccess;
}

// Get the boolean associated with the named tag.  Returns false if the tag
// isn't present or the data format wasn't either "true" or "false".
bool TRDescription::GetTagValue(const TRTag& inTag, bool& outValue) const
{
    bool theSuccess = false;
    string theString;

    if(this->GetValue(inTag, theString))
    {
        if(theString == "false")
        {
            outValue = false;
        } 
        else if(theString == "true")
        {
            outValue = true;
        }
        theSuccess = true;
    }

    return theSuccess;
}

// This is the name of this entity.  It is used to find it by the client code later.
string TRDescription::GetName(void) const
{
	return this->mName;
}

// Indicates what type of object this entity represents and is the keyword used to figure out
// what class to new.
string TRDescription::GetType(void) const
{
	return this->mType;
}


// Internal searching function to get the string value associated with the specified tag name.
// It is used by all the GetTagValue functions.  Returns false if the tag isn't present.
bool TRDescription::GetValue(const string& inTagName, string& outString) const
{
    bool theSuccess = false;
    TRTagValueListType::const_iterator theIter;

    // Lookup tag name in list
    for(theIter = this->mTagValueList.begin(); theIter != this->mTagValueList.end(); theIter++)
    {
        if(theIter->first == inTagName)
        {
            outString = theIter->second;
            theSuccess = true;
            break;
        }
    }

    return theSuccess;
}

void TRDescription::SetName(const string& inName)
{
    this->mName = inName;
}

bool TRDescription::SetTagValue(const TRTag& inTag, int inValue)
{
    bool theSuccess = false;
    char theCharArray[128];

    // Convert inValue to a string
    if(sprintf(theCharArray, "%d", inValue) > 0)
    {
        string theStringValue(theCharArray);

        // Call SetValue, return what it returns
        theSuccess = this->SetValue(inTag, theStringValue);
    }

    return theSuccess;
}

bool TRDescription::SetTagValue(const TRTag& inTag, float inValue)
{
    bool theSuccess = false;
    char theCharArray[128];

    // Convert inValue to a string
    if(sprintf(theCharArray, "%f", inValue) > 0)
    {
        string theStringValue(theCharArray);

        // Call SetValue, return what it returns
        theSuccess = this->SetValue(inTag, theStringValue);
    }

    return theSuccess;
}
     
bool TRDescription::SetTagValue(const TRTag& inTag, const string& inValue)
{
    bool theSuccess = false;

    // Call SetValue, return what it returns
    theSuccess = this->SetValue(inTag, inValue);

    return theSuccess;
}
     
bool TRDescription::SetTagValue(const TRTag& inTag, bool inValue)
{
    bool theSuccess = false;

    // Convert inValue to a string
    string theStringValue;
    if(inValue)
    {
        theStringValue = "true";
    }
    else
    {
        theStringValue = "false";
    }

    // Call SetValue, return what it returns
    theSuccess = this->SetValue(inTag, theStringValue);

    return theSuccess;
}


void TRDescription::SetType(const string& inType)
{
    this->mType = inType;
}

// Internal searching function to set the string for a tag.  Used by all the SetTagValue functions.
bool TRDescription::SetValue(const string& inTagName, const string& inString)
{
    bool theSuccess = false;
    TRTagValueListType::iterator theIter;

    // Lookup tag name in list
    for(theIter = this->mTagValueList.begin(); theIter != this->mTagValueList.end(); theIter++)
    {
        if(theIter->first == inTagName)
        {
            theIter->second = inString;
            theSuccess = true;
            break;
        }
    }

    return theSuccess;
}

