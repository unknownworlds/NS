// ZASSERT AND STACKTRACE are an improved assertion system which augments
// the WIN32 assert.h standard _assert.
// (c) 1999 Zachary B. Simpson
// Code may be used and distributed freely as long as all
// changes are noted appropriately.
#ifndef ZASSERT_H
#define ZASSERT_H

#include "assert.h"

extern "C"
{
	int getBreakpointOnAssert();
	void setBreakpointOnAssert(int inValue);
	void freeFunc(int a, char *string);
}

#ifdef WIN32
void *createAssertBox(
	char *msg, 
	void (*emailButtonFuncPtr)(char *) = NULL,
	char *titleText = NULL,
	char *okButtonText = NULL,
	char *emailButtonText = NULL,
	char *copyButtonText = NULL,
	char *editAreaText = NULL,
	void *parentBox = NULL
);
	// This creates a window which displays the msg "msg".
	// It creates three buttons:
	// OK: Closes the box
	// Email: Emails the message to the email address (if specified)
	// Copy: Copies the text to the window's clipboard.
	// Parameters:
	//   msg:
	//      message to display.  Likely to be the output of stack trace
	//      with other information such as version number, etc.
	//   emailButtonFuncPtr:
	//      Pointer to func to call when the email button is pressed.
	//      Passes a pointer to the complete assert string.
	//   titleText:
	//      Message to display in on title bar of assert box.  If NULL, prints "Assertion Failed".
	//      Isolated so that it may be translated easily.
	//   okButtonText:
	//      Message to display in "OK" button.  If NULL, prints "OK".
	//      Isolated so that it may be translated easily.
	//   emailButtonText:
	//      Message to display in "email" button.  If NULL, prints "Email".
	//      Isolated so that it may be translated easily.
	//   copyButtonText:
	//      Message to display in "Copy" button.  If NULL, prints "Copy".
	//      Isolated so that it may be translated easily.
	//   editAreaText:
	//      Message didplayerd in the edit area.  If NULL, prints
	//       "Please enter description of what you were
	//       doing at the time this assertion occured here."
	//   parentBox:
	//      What the parent of this box is.  If null, it is a top level wnd
	//      Note, thus "void *" is really an HWND.  I do not declare it as such
	//      to avoid extraneous inclusion of windows.h
	// Return:
	//   Returns an HWND to this box.  Should be passed to runAssertBox.
	//   These two calls are seperated from one another so that the 
	//   caller may center the assert box or perform other deisred manipulations
	//   before it is shown.

void theExitRunAssertBox(void);

void runAssertBox( void *hWnd );
	// Runs a modal message loop on the assert box created by createAssertBox
	// This is separated so that the programmer may manipulate the assert
	// window, such as centering it, before it is displayed.
#endif //WIN32


int emailMsgTo( char *msg, char *emailAddress );
	// A tool which uses SMTP to send a mail message "msg"
	// to emailAddress.  emailAddress must be an internet
	// address.
	// Returns:
	//   0 if failed
	//   1 if success



// Assert.h is defined to call the following function
// You should implement this function in your application
// to do the tings you want.  See the LEVEL 2 template code
// in zassert_template.cpp

// This code is somewhat complicated by the portability issues.
// WIN32 and POSIX (not surprisingly) seem to difer on the 
// function declarations for the call from the assert macro.
// So, the ASSERTFUNC macro defines function in a portable way
#ifdef WIN32
	#define ASSERTFUNC _CRTIMP void __cdecl _assert( void *msg, void *file, unsigned line )
#else
	#define ASSERTFUNC __dead void __assert __P((const char *msg,const char *file,int line)) 
#endif

// Now declare the assert function as external
#ifdef WIN32
extern ASSERTFUNC;
#endif


// The assert in assert.h is wrapped in a #ifndef _DEBUG
// block which means that they will not assert in non-debug 
// builds.  Unfortunatly, this is not always the desired behavior.
// Thus, the following macro "aasert" is NOT in such a wrapper and will
// thus *always* assert in any build.

#define aassert(exp) (void)( (exp) || (_assert(#exp, __FILE__, __LINE__), 0) )

#endif
