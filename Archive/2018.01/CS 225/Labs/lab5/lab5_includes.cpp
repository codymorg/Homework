// lab5_includes.cpp
// -- things you are to assume are already included in your
//    solutions to the problems in lab #5
// cs225 10/18

#include <iostream>
#include <deque>
#include <stack>
#include <queue>
#include <iomanip>
#include <string>
#include <stdexcept>
using namespace std;


struct Proxy {
  const string &str;
  unsigned index;
  Proxy(const string &s, unsigned i) : str(s), index(i) { }
  bool operator==(const char *s);
  operator char() const;
};

struct istring {
  string str;
  istring(const char *s) : str(s) { }
  Proxy operator[](int i)  { return Proxy(str,i); }
};


#include "lab5_solutions.cpp"
// Your code should successfully compile (but not link) with the commands
//    cl /EHsc /c lab5_includes.cpp
// and/or
//    g++ -std=c++11 -c lab5_includes.cpp


