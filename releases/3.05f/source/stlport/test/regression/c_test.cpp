#include <string>
#include <iostream>
#include <fstream>
#include <list>

#ifndef STLPORT
//#error You Did not get the STLport include files!
#endif

using namespace std;

main (int argc, char *argv[])
{
#ifdef STLPORT
  string s2 = "STLport included";
  cout << s2 << endl;
#else
  string s2 = "STLport NOT included!";
  cout << s2 << endl;
#endif

  string s = "hello";
  cout << s << endl;
  ofstream fstr("testfile");
  fstr << s << endl;


  list<int> L;
  for (int i = 0; i < 10; ++i) {
    L.push_back(i);
  }
  for (list<int>::const_reverse_iterator cri = L.rbegin();
       cri != (list<int>::const_reverse_iterator) L.rend(); ++cri) {
    cout << *cri << endl;
  }

#ifdef STLPORT
  string s3 = "PASSED";
  if (strstr(argv[0], "nostlport"))
    s3 = "FAILED";
  cout << s3 << endl;
#else
  string s3 = "PASSED";
  if (!strstr(argv[0], "nostlport"))
    s3 = "FAILED";
  cout << s3 << endl;
#endif

}










