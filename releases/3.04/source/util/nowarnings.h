#ifndef NS_NOWARNINGS_H
#define NS_NOWARNINGS_H

//
// Syntax help:
//
//#pragma warning( warning-specifier : warning-number-list [,warning-specifier : warning-number-list...] )
//
//#pragma warning( push[ , n ] )
//
//#pragma warning( pop )
//
//Allows selective modification of the behavior of compiler warning messages.
//
//The warning-specifier can be one of the following.
//
//Warning-specifier Meaning 
//once Display the specified message(s) only once. 
//default Apply the default compiler behavior to the specified message(s). 
//1, 2, 3, 4 Apply the given warning level to the specified warning message(s). 
//disable Do not issue the specified warning message(s). 
//error Report the specified warnings as errors. 
//
//
//The warning-number-list can contain any warning numbers. Multiple options can be specified in the same pragma directive as follows:
//
//#pragma warning( disable : 4507 34; once : 4385; error : 164 )
//
//This is functionally equivalent to:
//
//#pragma warning( disable : 4507 34 )  // Disable warning messages
////  4507 and 34.
//#pragma warning( once : 4385 )        // Issue warning 4385
////  only once.
//#pragma warning( error : 164 )        // Report warning 164
////  as an error.

#pragma warning (disable: 4786)
#pragma warning (disable: 4530)
#pragma warning (disable: 4800)

// Signed/float conversions.  Disabled by default in MSVC 7?
#pragma warning (disable: 4018)
#pragma warning (disable: 4244)

// Use before initialization
#pragma warning (disable: 4701)

// This quiets the "C++ exception handler used but unwind semantics not enabled"
// I'm not throwing any exceptions, nor am I catching any. Remove this if it causes problems!  
#pragma warning (disable: 4530)

// Assignment in conditional (is this really classified as level 4?)
#pragma warning (error: 4706)

// Statement has no effect
#pragma warning (error: 4705)

// Comma in array index expression
#pragma warning (error: 4709)

// Non-portable char initializization
#pragma warning (error: 4208)

// Benign typedef redefinition
#pragma warning (error: 4209)

#endif