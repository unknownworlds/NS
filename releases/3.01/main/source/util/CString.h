#ifndef C_STRING_H
#define C_STRING_H

#include <stdio.h>
#include <string>
#include <vector>
using std::string;
using std::vector;

// Include C-style strings for those functions that can't handle it <sigh>
// Half-life keeps pointers to strings and assumes they don't change, meaning
// that STL strings can't be used.  What a hack.
class CString
{
public:

    #define kMaxCStrLen 256

    CString()
    {
        memset(this->mString, 0, kMaxCStrLen);
    }

    operator char*()
    {
        return this->mString;
    }

    operator const char*() const
    {
        return this->mString;
    }

    void operator =(const string& inString)
    {
        sprintf(this->mString, "%s", inString.c_str());
    }

    void operator =(const CString& inString)
    {
        sprintf(this->mString, "%s", inString.mString);
    }

    bool operator ==(const CString& inString) const
    {
        bool theAreEqual = false;

        if(!strcmp(this->mString, inString.mString))
        {
            theAreEqual = true;
        }
        return theAreEqual;
    }

    bool operator ==(const string& inString) const
    {
        bool theAreEqual = false;

        if(!strcmp(this->mString, inString.c_str()))
        {
            theAreEqual = true;
        }
        return theAreEqual;
    }

    int GetMaxLength()
    {
        return kMaxCStrLen;
    }

private:
    char   mString[kMaxCStrLen];
    #undef kMaxCStrLen
};

typedef vector<CString>		CStringList;

#endif
