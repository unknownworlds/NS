#ifndef AVH_DEBUG_UTIL_H
#define AVH_DEBUG_UTIL_H

#include <windows.h>
#include <iostream>

/**
 * Writes the call stack for a context to the stream.
 */
void LogStack(std::ostream& stream, CONTEXT& c, HANDLE hThread);

/**
 * Writes information about an exception to the stream.
 */
void LogException(std::ostream& stream, EXCEPTION_POINTERS* pExp);

/**
 * Sends an e-mail. Line breaks in the message should be specified with a
 * carriage return character followed by a new line character. If the mail
 * could not be sent the function returns false.
 */
bool SendMail(const char* serverName, const char* fromAddress, const char* fromName,
    const char* toAddress, const char* subject, const char* message);

#endif