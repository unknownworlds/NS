#ifndef TOKENIZER_H
#define TOKENIZER_H
/**************************************************************
Name:		Tokenizer.h
Author:	KJQ
Desc:		Static class to provide tokenizer services for simple parsers.
Edits:		
Bugs:		
*************************************************************/

#include <util/StringVector.h>
#include <vector>
#include <string>
using std::string;

class Tokenizer
{
public:
    // split separates the line into multiple pieces, discarding delimiters
    // if quotechar is specified it's used to identify quoted strings.
    static int split(const string& input, const string& delimiters, StringVector&, char quoteChar = 0);
    
private:
    Tokenizer();        // prevent anybody from making one
    ~Tokenizer();
};

#endif //TOKENIZER_H
