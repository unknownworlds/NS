//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: TRDescription.h $
// $Date: 2002/07/26 01:53:49 $
//
//-------------------------------------------------------------------------------
// $Log: TRDescription.h,v $
// Revision 1.6  2002/07/26 01:53:49  Flayra
// - Linux support for FindFirst/FindNext
// - Added document headers
//
//===============================================================================
//
// Reads in a list of tag-value pairs in order.  Every description begins with "start <type> <name>" and ends with "end".
// A line that starts with ', or // is treated as a comment and ignored. There can be multiple descriptions in a file.
// 
// Here are some examples:
//
// start Label ResourceUnits
//      xpos = .5
//      ypos = .4
//      xwidth = .1
//      yheight = .2
//      bgimage = rg
// end
//
// A weapon description might look like:
//
// start Weapon MachineGun
//      rof = 6
//      startingammo = 50
//      damage = 10
//      firesound = sound/weapons/ak47.wav
//      reloadanim = wpn_reload
// end
//
// Something that describes an graphical effect could look like:
//
// ' Weapon smoke and shrapnel
// start ParticleSystem WeaponSmoke
//      ' num particles per second
//      particlerate = 6
//      numinitialparticles = 0
//      particlephysics = smoke
//      smoketexture = wpn_smoke
//      origin = local_player_weapon
// end
//
// ' Finally, some real shrapnel
// start ParticleSystem WeaponShrapnel
//      modelname = models/shrapnel
//      particlerate = 0
//      numinitialparticles = 5
//      particlephysics = fountain
//      initialsound = sound/fx/skrang.wav
// end
// 
// 

#ifndef TRDESCRIPTION_H
#define TRDESCRIPTION_H

#include "types.h"
#include "textrep/TRTag.h"
#include "textrep/TRTagValuePair.h"
#include "util/CString.h"
#include "util/StringVector.h"

class TRDescription
{
public:
    string      GetName(void) const;
                
    bool        GetTagStringList(const TRTag& inTagPrefix, StringVector& outList) const;

    bool        GetTagStringList(const TRTag& inTagPrefix, CStringList& outList) const;

    bool        GetTagValue(const TRTag& inTag, int& outValue) const;
                                        
    bool        GetTagValue(const TRTag& inTag, float& outValue) const;
                                        
    bool        GetTagValue(const TRTag& inTag, string& outValue) const;

    bool        GetTagValue(const TRTag& inTag, CString& outValue) const;
                                        
    bool        GetTagValue(const TRTag& inTag, bool& outValue) const;

    string      GetType(void) const;    
                                        
    bool        SetTagValue(const TRTag& inTag, int inValue);
                                        
    bool        SetTagValue(const TRTag& inTag, float inValue);
                                        
    bool        SetTagValue(const TRTag& inTag, const string& inValue);
                                        
    bool        SetTagValue(const TRTag& inTag, bool inValue);

    // Be cool and nice and define interators so clients can enumurate all data pairs in this description
    typedef     vector<TRTagValuePair>              TRTagValueListType;
    typedef     TRTagValueListType::iterator        iterator;
    typedef     TRTagValueListType::const_iterator  const_iterator;

    iterator    begin(void) 
	{ 
		return this->mTagValueList.begin(); 
	}

    iterator    end(void) 
	{ 
		return this->mTagValueList.end(); 
	}

    const_iterator	begin(void) const 
	{ 
		return this->mTagValueList.begin(); 
	};

    const_iterator	end(void) const 
	{ 
		return this->mTagValueList.end(); 
	}

private:
                // Built by TRFactory only
                friend class TRFactory;

    void        AddPair(const TRTagValuePair& inPair);

    //template <class T>
    //bool        GetTagStringList(const TRTag& inTagPrefix, T& outList) const;

    bool        GetValue(const string& inTagName, string& outString) const;

    bool        SetValue(const string& inTagName, const string& inString);

    void        SetName(const string& inName);

    void        SetType(const  string& inType);

    // The hopefully unique name given this description in the text file it was read from
    string      mName;

    // A string indicating what type of object this represents.  This is used to new the object.
    string      mType;

    // List of TRTagValuePairs representing all the settings for this object.  These settings will
    // be used to configure the object after newing it.
    TRTagValueListType  mTagValueList;

};

typedef vector<TRDescription>   TRDescriptionList;

#endif
