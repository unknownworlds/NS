#include "util/nowarnings.h"
#include "util/Tokenizer.h"

int Tokenizer::split(const string& input, const string& delimiters, StringVector& result, char quoteChar)
{
    int subStart = 0;
    int subEnd = 0;
    int length = 0;
    
    result.clear();   // erase result before we start
    
    while (true)
    {
        if ((subStart = input.find_first_not_of(delimiters, subEnd)) == string::npos)
            break;
        
        // if we're handling quotes
        if (quoteChar && (input[subStart] == quoteChar))
        {
            ++subStart;                   // skip the open quote
            // have to turn quoteChar into a string for find_first_of
            if ((subEnd = input.find_first_of(string(1, quoteChar), subStart))
                == string::npos)
            {
                subEnd = input.size();
                length = subEnd - subStart;
            }
            else
            {
                // subStart has already been incremented to skip the open quote;
                // subEnd excludes the close quote; so we do NOT have to tweak the
                // difference to get a count of the characters between them.
                length = subEnd - subStart;
                ++subEnd;                   // now skip the close quote
            }
        }
        else
        {
            if ((subEnd = input.find_first_of(delimiters, subStart)) == string::npos)
                subEnd = input.size();
            length = subEnd - subStart;
        }
        result.push_back(input.substr(subStart, length));
    }
    return result.size();
}
