// ZASSERT AND STACKTRACE are an improved assertion system which augments
// the WIN32 assert.h standard _assert.
// (c) 1999 Zachary B. Simpson
// Code may be used and distributed freely as long as all
// changes are noted appropriately.
//
// This is a simple program which test the ZASSERT
// and STACKTRACE funtctions with some arbitrary calls.
// 
// This supplies LEVEL 2 "Template" Code for implementing
// the assert and emailToBugDatabase functions

#ifdef WIN32
	#include "windows.h"
		// Only needed for message box
	#include "winbase.h"
	#include <process.h>
#endif
#include "stdio.h"
#include "stacktrace.h"
#include "zassert.h"

// LEVEL 2 CODE
// The following is template code which can be cut, pasted, 
// and modified appropriately for your application.
//-------------------------------------------------------------------

HWND hRunAssertBox;

int breakpointOnAssert = 0;
	// If this flag is set and it is a debug build then
	// it will issue an int 3 causing a breakpoint in the
	// debugger.  This is very handy for ensuring that
	// the programmer can exmaine memory, etc.
	// default is 0.

int getBreakpointOnAssert()
{
	return breakpointOnAssert;
}

void setBreakpointOnAssert(int inValue)
{
	breakpointOnAssert = inValue;
}

// The following is a template for the function
// which gets called by the runAssertBox
// in the case that the user hits the "email" button
// This is LEVEL 2 "Template" Code
#ifdef WIN32
void emailToBugDatabase( char *msg ) {
	int success = emailMsgTo( msg, "bugs@overmind.org" );
		// Note that you MUST specify the full name of the computer
		// since emailMsgTo does NOT use the MX record.  In other
		// words, you can't specify "zsimpson@eden.com"
	if( !success ) {
		MessageBox(
			NULL,
			"Email to bug database failed.\r\n"
			"Please \"Copy\" the message and send it to the appropriate database.",
			"E-Mail Failed", 
			MB_APPLMODAL|MB_ICONEXCLAMATION
		);
	}
}
#endif

// This is LEVEL 2 "Template" Code
// The following is a template for the function
// which ultimately gets called by the assert macros
// NOTE the use of the ASSERTFUNC macro which makes this PORTABLE
// The declaration is:
//   void _assert( void *msg, void *file, unsigned line )
ASSERTFUNC {
	// Just in case, prevent recursion
	static int recurse = 0;
	if( recurse ) {
		// Under GNU C you may get a warning concerning this
		// return since ASSERTFUNC is declared noreturn.
		// Ignore the warning.
		return;
	}
	recurse = 1;

	// Run a stack trace, skip the assert call (passing 1)
	char *stackMsg = stackTrace(1);

	// Setup and application specific information
	// You should put all code that you want to add to the
	// assert message here.  For example, you might want
	// to include:
	//  * version numbers
	//  * computer name
	//  * user name
	//  * date, time, etc.
	//  * relevant state information (direct draw, CD/HD, etc)
	//  * debug staus, heap check, etc.
	char *appMsg = "ASSERT TEST VERSION 1.0";

	// 
	char buffer[4096]={0,};
	strcpy( buffer, "The system has failed an assertion:\r\nAssert: \"" );
	strcpy( buffer+strlen(buffer), (char *)msg );
	sprintf( buffer+strlen(buffer), "\"  @ %s:%d\r\n", file, line );
	if( appMsg && *appMsg ) {
		sprintf( buffer+strlen(buffer), "%s\r\n", appMsg );
	}
	sprintf( buffer+strlen(buffer), "%s\r\n", stackMsg );

	// Trace the assert out to a file just for good measure.
	FILE *f = fopen( "assert.txt", "w+b" );
	if( f ) {
		fprintf( f, buffer );
		fclose( f );
	}

	// Trace the assert to STDOUT.  Especially important
	// for UNIX since this is the only message you'll see.
	printf( buffer );

	// breakpointOnAssert is true this will force a debugger
	// to stop.  Ideally, it would be possible to determine
	// if we are currently running in a debugger so that this
	// flag would not be necessary, but I have been unable
	// to figure out how to do this.
	//   If you hit this breakpoint and you want to keep
	// running, don't forget that you can simply skip over the
	// exit call by changing the debugger change EIP command.
	#ifdef _DEBUG
		//#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)

		#ifdef WIN32
			if( breakpointOnAssert )
		  	//if(IsDebuggerPresent())
		  	{
				__asm int 3;
		  	}
		#endif
		//#endif
	#endif

	// Create the assert box.  Note that a real implementation
	// should translate all of the stock messages.
	#ifdef WIN32
		hRunAssertBox = (HWND)createAssertBox( buffer, emailToBugDatabase );
		//unsigned long theRC = _beginthread(runAssertBox, 0, hAssertBox);
		//runAssertBox( hAssertBox );
		int theRC = atexit(theExitRunAssertBox);
		//ASSERT(theRC == 0);
	#else
		// Under UNIX, send the email immediately.
		emailMsgTo( buffer, "bugs@overmind.org" );
	#endif

	// And finally, shut down.
	exit(1);
}




//---------------------------------------------------------------------
// The following code is all testing code for
// REGRESSION PURPOSES.
// DO NOT COPY into your application

void freeFunc( int a, char *string ) {
	assert( 0 == 1 );	
		// assert now, we should get a stack trace
		// indicting freeFunc( 0xDEADBEEF, PTR() )
		// and a hex dump of the pointer
}

//struct Boink {
//	int a;
//	int b;
//	
//	void method1( int _a, int _b ) {
//		method2( _a+1, _b, "This is a test" );
//	}
//	void method2( int _a, int _b, char *msg ) {
//		freeFunc( 0xDEADBEEF, msg );
//	}
//};
//
//void main( int argc, char **argv ) {
//	// Setup breakpoint to make debugging easier when in debugger
//	breakpointOnAssert = 1;
//
//	// Call method which is assert...
//	Boink b;
//	b.method1(10,20);
//}

