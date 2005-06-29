// -*- C++ -*- Time-stamp: <04/01/16 21:32:26 ptr>


#include <string>

#if defined(_STLP_PTHREADS) || defined(_STLP_WIN32THREADS)

#ifdef _STLP_PTHREADS
# include <pthread.h>
#endif

#ifdef _STLP_WIN32THREADS
# include <windows.h>
#endif

#ifdef MAIN
#define string_mt_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif

string func( const string& par )
{
  string tmp( par );

  return tmp;
}

#if defined (_STLP_PTHREADS)
void *f( void * )
#elif defined (_STLP_WIN32THREADS)
DWORD __stdcall f (void *)
#endif
{
  string s( "qyweyuewunfkHBUKGYUGL,wehbYGUW^(@T@H!BALWD:h^&@#*@(#:JKHWJ:CND" );

  for ( int i = 0; i < 2000000; ++i ) {
    string sx = func( s );
  }

  return 0;
}

int string_mt_test( int, char ** )
{
  const int nth = 2;
#if defined(_STLP_PTHREADS)
  pthread_t t[nth];

  for ( int i = 0; i < nth; ++i ) {
    pthread_create( &t[i], 0, f, 0 );
  }

  for ( int i = 0; i < nth; ++i ) {
    pthread_join( t[i], 0 );
  }
#endif // _STLP_PTHREADS

#if defined (_STLP_WIN32THREADS)
  HANDLE t[nth];

  int i; // VC6 not support in-loop scope of cycle var
  for ( i = 0; i < nth; ++i ) {
    t[i] = CreateThread(NULL, 0, f, 0, 0, NULL);
  }

  for ( i = 0; i < nth; ++i ) {
    WaitForSingleObject(t[i], INFINITE);
  }
#endif

  return 0;
}
#else // !_STLP_PTHREADS && !_STLP_WIN32THREADS

int string_mt_test( int, char ** )
{
  return -1;
}

#endif // _STLP_PTHREADS || _STLP_WIN32THREADS
